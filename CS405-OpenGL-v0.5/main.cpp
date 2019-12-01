#include "Include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ResourceManager.h"

#include "camera.h"
#include "model.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(std::vector<std::string> faces);
void drawText(const char *text, int length, int x, int y);
// PFfft.
void processInputForObject(GLFWwindow *window, Model &model, double deltaDistance);
// PFfft. x2
unsigned int skyboxInit();

// STRING TABLE
// -----------------------------
std::string WINDOW_TITLE = "Fish In the sea";

std::string KEY_SHADER_SKYBOX	= "SKYBOX_SHADER";
std::string KEY_SHADER_OBJECT	= "OBJECT_SHADER";
std::string KEY_TEXTURE_MARBLE	= "TEXTURE_MARBLE";

std::string FILE_TEXTURE_MARBLE			= "./Resource/textures/marble.jpg";

std::string FILE_SHADER_FRAGMENT_SKYBOX = "./Resource/shaders/skybox.fs";
std::string FILE_SHADER_VERTEX_SKYBOX	= "./Resource/shaders/skybox.vs";

std::string FILE_SHADER_FRAGMENT_STANDART_OBJECT = "./Resource/shaders/model_loading.fs";
std::string FILE_SHADER_VERTEX_STANDARD_OBJECT	 = "./Resource/shaders/model_loading.vs";
//"./Resource/objects/cube/cube.obj"
//

std::string FILE_OBJECT_CYBORG			= "./Resource/objects/cyborg/cyborg.obj";
std::string FILE_OBJECT_NANOSUIT		= "./Resource/objects/nanosuit/nanosuit.obj";
std::string FILE_OBJECT_FLATPLANE		= "./Resource/objects/flat-plane/flat-plane.obj";
std::string FILE_OBJECT_COIN			= "./Resource/objects/coin/coin.fbx";

std::string FILE_TEXTURE_SKYBOX_RIGHT	= "./Resource/textures/skybox/right.jpg";
std::string FILE_TEXTURE_SKYBOX_LEFT	= "./Resource/textures/skybox/left.jpg";
std::string FILE_TEXTURE_SKYBOX_FRONT	= "./Resource/textures/skybox/front.jpg";
std::string FILE_TEXTURE_SKYBOX_BACK	= "./Resource/textures/skybox/back.jpg";
std::string FILE_TEXTURE_SKYBOX_TOP		= "./Resource/textures/skybox/top.jpg";
std::string FILE_TEXTURE_SKYBOX_BOTTOM	= "./Resource/textures/skybox/bottom.jpg";
// ------------------------------


// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


// PFTT
unsigned int skyboxVAO, skyboxVBO;












int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, WINDOW_TITLE.c_str(), NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	ResourceManager::LoadShader(FILE_SHADER_VERTEX_SKYBOX.c_str(), 
		FILE_SHADER_FRAGMENT_SKYBOX.c_str(), nullptr, KEY_SHADER_SKYBOX);

	auto cyborgModel = Model(FILE_OBJECT_CYBORG);
	auto soldierModel = Model(FILE_OBJECT_NANOSUIT);
	auto flatCubeModel = Model(FILE_OBJECT_FLATPLANE);
	auto coinModel = Model(FILE_OBJECT_COIN);


	ResourceManager::LoadShader(FILE_SHADER_VERTEX_STANDARD_OBJECT.c_str(), 
		FILE_SHADER_FRAGMENT_STANDART_OBJECT.c_str(), nullptr, KEY_SHADER_OBJECT);
	
	// Init skybox, its not good practise.
	unsigned int cubemapTexture = skyboxInit();

	// shader configuration
	// --------------------
	ResourceManager::GetShader(KEY_SHADER_OBJECT).use();

	ResourceManager::GetShader(KEY_SHADER_SKYBOX).use().setInt("skybox", 0);

	// cyborg model matrix
	// -------------------
	auto scaleCyborg = glm::vec3(0.5f, 0.5f, 0.5f);
	auto scaleSoldier	= glm::vec3(0.2f, 0.2f, 0.2f);
	auto scaleFlatPlane = glm::vec3(20.0f, 0.001f, 20.0f);
	auto scaleCoin		= glm::vec3(0.01f, 0.01f, 0.01f);
	
	coinModel.move(2.0f, M_UP);
	coinModel.move(2.0f, M_RIGHT);
//	&&&texture i kalp yap 
	//	renderlarken project'on matr'z' orthogonal yap '
		//	projectıon view identity
	cyborgModel.ScaleModel(scaleCyborg);
	soldierModel.ScaleModel(scaleSoldier);
	flatCubeModel.ScaleModel(scaleFlatPlane);
	coinModel.ScaleModel(scaleCoin);

	cyborgModel.move(1.0f, M_UP);
	soldierModel.move(1.75f, M_DOWN);
	//GalpModel.move(1.0f, M_UP);
	// frame counter for random movement
	// ---------------

	std::string FILE_OBJECT_help_here2 = "./Resource/objects/Help_here2/press_here.obj";

	std::string FILE_OBJECT_help_here = "./Resource/objects/Help_here/press_here.obj";
	std::string FILE_OBJECT_Galp = "./Resource/objects/Galp/Galp.obj";
	std::string FILE_OBJECT_Score = "./Resource/objects/Score/Score.obj";
	std::string FILE_OBJECT_Hunger = "./Resource/objects/Hunger/Hunger.obj";

	auto Help_Model_mini = Model(FILE_OBJECT_help_here2);

	auto Help_Model=Model(FILE_OBJECT_help_here);
	auto GalpModel = Model(FILE_OBJECT_Galp);
	auto ScoreModel = Model(FILE_OBJECT_Score);
	auto HungerModel = Model(FILE_OBJECT_Hunger);
	
	auto Helpscale = glm::vec3(0.1f, 0.9f, 0.2f);
	auto scaleGalp = glm::vec3(0.1f, 0.9f, 0.2f);
	auto scaleScore = glm::vec3(0.1f, 0.9f, 0.2f);
	auto HungerScore = glm::vec3(0.1f, 0.9f, 0.2f);

	Help_Model.ScaleModel(Helpscale);
	GalpModel.ScaleModel(scaleGalp);
	ScoreModel.ScaleModel(scaleScore);
	HungerModel.ScaleModel(HungerScore);
	Help_Model_mini.ScaleModel(scaleGalp);


	int lives=3;
	int score = 17;
	float hunger=0;

	int randDistance;

		double lastTime = glfwGetTime();
		int nbFrames = 0;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
			// Measure speed
			double currentTime = glfwGetTime();
			nbFrames++;
			if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1sec ago
				// printf and reset
				printf("%f ms/frame\n", 1000.0 / double(nbFrames));
				//nbFrames = 0;
				lastTime += 1.0;
				nbFrames = 0;
			}





		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		randDistance = rand() % 2;
		randDistance *= 5;

		double deltaDistance = randDistance * deltaTime;

		// input
		// -----
		processInput(window);
		processInputForObject(window, cyborgModel, deltaDistance);

		// render
		// ------
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		//soldiershader.use();
		ResourceManager::GetShader(KEY_SHADER_OBJECT).use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("projection", projection);
		ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("view", view);

		//// render the loaded model

		cyborgModel.moveRandom(deltaDistance);
		ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", cyborgModel.GetModelMatrix());
		cyborgModel.Draw(ResourceManager::GetShader(KEY_SHADER_OBJECT));

		ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", soldierModel.GetModelMatrix());
		soldierModel.Draw(ResourceManager::GetShader(KEY_SHADER_OBJECT));

		ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", flatCubeModel.GetModelMatrix());
		flatCubeModel.Draw(ResourceManager::GetShader(KEY_SHADER_OBJECT));

		ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", coinModel.GetModelMatrix());
		coinModel.Draw(ResourceManager::GetShader(KEY_SHADER_OBJECT));
		////////////////////////////////////////////////////-----------------------------
		if (nbFrames < 1000 && lives!=0) {
			// view/projection transformations
			glm::mat4 ORTHOprojection = glm::orthoLH(-10, 10, -10, 10, -10, 10);



			ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("projection", glm::mat4x4(1));
			ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("view", glm::mat4x4(1));

			glDepthFunc(GL_ALWAYS);
			
			for (float i = 0; i < lives; i++) {
				ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", { 1.0f,0.0f,0.0f,0.0f,//x
																				 0.0f,1.0f,0.0f,0.0f,//y
																				 0.0f,0.0f,0.0f,0.0f,//z
																				 -7.50f,7.50f-i,0.0f,8.0f });

				GalpModel.Draw(ResourceManager::GetShader(KEY_SHADER_OBJECT));
			}

			int tempscore = score;
			float i=0;
			while(  5<=tempscore ) {

				ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", { 1.0f,0.0f,0.0f,0.0f,//x
																				 0.0f,1.0f,0.0f,0.0f,//y
																				 0.0f,0.0f,0.0f,0.0f,//z
																				 6.0f - i,6.10f ,0.0f,6.5f });
				i++;
				tempscore -= 5;
				ScoreModel.Draw(ResourceManager::GetShader(KEY_SHADER_OBJECT));
			}
			float j = 0;
			while (0 < tempscore) {
				
				ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", { 1.0f,0.0f,0.0f,0.0f,//x
																				 0.0f,1.0f,0.0f,0.0f,//y
																				 0.0f,0.0f,0.0f,0.0f,//z
																				 7.50f - j,6.40f ,0.0f,8.0f });
				
				j++;
				tempscore -= 1;
				ScoreModel.Draw(ResourceManager::GetShader(KEY_SHADER_OBJECT));
			}

			
			if (hunger < 10) {
				ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", { 10 - hunger,0.0f,0.0f,0.0f,//x
																				 0.0f,0.5f,0.0f,0.0f,//y
																				 0.0f,0.0f,0.0f,0.0f,//z
																				 0.0f,-7.50f,0.0f,8.0f });
				hunger = hunger +( 0.000001*SCR_WIDTH);
				HungerModel.Draw(ResourceManager::GetShader(KEY_SHADER_OBJECT));
			}
			else {
				if (lives > 0) {
					hunger = 0;
					lives--;
				}

			}
			bool help_open = 0;
			if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
			{
				help_open = !help_open;
			}
			if (!help_open) {
				ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", { 1.0f,0.0f,0.0f,0.0f,//x
																				 0.0f,1.0f,0.0f,0.0f,//y
																				 0.0f,0.0f,0.0f,0.0f,//z
																				 0.0f,7.50f ,0.0f,8.0f });

				Help_Model_mini.Draw(ResourceManager::GetShader(KEY_SHADER_OBJECT));
			}
			if (help_open) {
				ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", { 1.0f,0.0f,0.0f,0.0f,//x
																				 0.0f,1.f,0.0f,0.0f,//y
																				 0.0f,0.0f,1.0f,0.0f,//z
																				 0.0f,0.0f,0.0f,1.0f });
				
				Help_Model.Draw(ResourceManager::GetShader(KEY_SHADER_OBJECT));
			}





		}





		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		ResourceManager::GetShader(KEY_SHADER_SKYBOX).use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		ResourceManager::GetShader(KEY_SHADER_SKYBOX).setMat4("view", view);
		ResourceManager::GetShader(KEY_SHADER_SKYBOX).setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	//glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	//glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &skyboxVAO);

	glfwTerminate();
	return 0;
}

unsigned int skyboxInit()
{
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	// skybox VAO
	//unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	std::vector<std::string> faces
	{
		FILE_TEXTURE_SKYBOX_RIGHT,
		FILE_TEXTURE_SKYBOX_LEFT,
		FILE_TEXTURE_SKYBOX_TOP,
		FILE_TEXTURE_SKYBOX_BOTTOM,
		FILE_TEXTURE_SKYBOX_FRONT,
		FILE_TEXTURE_SKYBOX_BACK
	};
	return loadCubemap(faces);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(DOWN, deltaTime);
	}
}

void processInputForObject(GLFWwindow * window, Model &model, double deltaDistance)
{
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
	{
		model.move(deltaDistance, M_FORWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		model.move(deltaDistance, M_BACKWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	{
		model.move(deltaDistance, M_LEFT);
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		model.move(deltaDistance, M_RIGHT);
	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}




