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

#include "StringTable.h"
#include "Enums.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, Model &model);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(std::vector<std::string> faces);

void moveTogether(Directions dir, Model &model);

void processInputForObject(GLFWwindow *window, Model &model);
unsigned int skyboxInit();


// Initial camera settings.
Camera camera(glm::vec3(0.0f, 2.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLfloat box_vertices[] = {
	-10.0f , -10.0f, 0.0f, 0.0f, 1.0f,
	-10.0f , 10.0f, 0.0f, 0.0f, 0.0f,
   10.0f , 10.0f, 0.0f, 1.0f, 0.0f,
   10.0f , -10.0f, 0.0f, 1.0f, 1.0f
};

auto distRange = glm::vec3(5.0f);

bool isGodMod = false;

bool checkDistance(Model& model)
{
	double dist = std::sqrt( pow(camera.getPosition().x - model.getPosition().x,2)
						   + pow(camera.getPosition().y - model.getPosition().y,2)
						   + pow(camera.getPosition().z - model.getPosition().z,2));

	//std::cout << dist << std::endl;
	if (dist < 25.0)
	{
		return true;
	}
	return false;
}

std::vector<Model*> modelList;

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

	ResourceManager::LoadShader(FILE_SHADER_VERTEX_STANDARD_OBJECT.c_str(),
		FILE_SHADER_FRAGMENT_STANDART_OBJECT.c_str(), nullptr, KEY_SHADER_OBJECT);

	// Initialize Skybox
	// --------------------------------------
	unsigned int skyboxTexture = skyboxInit();

	// shader configuration
	// --------------------
	ResourceManager::GetShader(KEY_SHADER_OBJECT).use();

	ResourceManager::GetShader(KEY_SHADER_SKYBOX).use().setInt("skybox", 0);

	// Create Game models
	// ---------------------------------------
	auto cyborgModel = Model(FILE_OBJECT_CYBORG);
	auto soldierModel = Model(FILE_OBJECT_NANOSUIT);

	auto coinModel = Model(FILE_OBJECT_COIN);
	auto coinModel2 = Model(FILE_OBJECT_COIN);
	auto coinModel3 = Model(FILE_OBJECT_COIN);
	auto coinModel4 = Model(FILE_OBJECT_COIN);
	//auto coinModel5 = Model(FILE_OBJECT_COIN);
	//auto coinModel6 = Model(FILE_OBJECT_COIN);
	//auto coinModel7 = Model(FILE_OBJECT_COIN);
	//auto coinModel8 = Model(FILE_OBJECT_COIN);

	auto hpPanelModel = Model(FILE_OBJECT_HP);
	auto scorePanelModel = Model(FILE_OBJECT_SCORE);
	auto hungerPanelModel = Model(FILE_OBJECT_HUNGER);
		
	// Set Unique Id to Models
	// ----------------------
	cyborgModel.setID(0);
	soldierModel.setID(1);
	coinModel.setID(3);
	   
	// Scale models 
	// -------------------
	auto scaleCyborg = glm::vec3(0.5f, 0.5f, 0.5f);
	auto scaleSoldier = glm::vec3(0.2f, 0.2f, 0.2f);
	auto scaleCoin = glm::vec3(0.01f, 0.01f, 0.01f);
	
	auto scaleHpPanel = glm::vec3(0.1f, 0.9f, 0.2f);
	auto scaleScore = glm::vec3(0.1f, 0.9f, 0.2f);
	auto HungerScore = glm::vec3(0.1f, 0.9f, 0.2f);
	
	
	// Debug Printer
	// -------------------------
	//cyborgModel.printPosition();
	//soldierModel.printPosition();
	coinModel.printPosition();

	// Apply Scale operation to models
	// -----------------------------------
	cyborgModel.ScaleModel(scaleCyborg);
	soldierModel.ScaleModel(scaleSoldier);
	
	coinModel.ScaleModel(scaleCoin);
	coinModel2.ScaleModel(scaleCoin);
	coinModel3.ScaleModel(scaleCoin);
	coinModel4.ScaleModel(scaleCoin);
	//coinModel5.ScaleModel(scaleCoin);
	//coinModel6.ScaleModel(scaleCoin);
	//coinModel7.ScaleModel(scaleCoin);
	//coinModel8.ScaleModel(scaleCoin);

	hpPanelModel.ScaleModel(scaleHpPanel);
	scorePanelModel.ScaleModel(scaleScore);
	hungerPanelModel.ScaleModel(HungerScore);


	// Debug Printer
	// -----------------------------
	cyborgModel.printPosition();
	//soldierModel.printPosition();
	//coinModel.printPosition();


	// Add models to an list to ease cycling
	// --------------------------------------
	modelList.push_back(&cyborgModel);
	modelList.push_back(&soldierModel);
	modelList.push_back(&coinModel);

	Model coinList[4] = {
		coinModel,
		coinModel2,
		coinModel3,
		coinModel4,
		//coinModel5,
		//coinModel6,
		//coinModel7,
		//coinModel8
	};

	// Set object movement types. Default is Normal
	// ---------------------------------------------
	soldierModel.setMovementType(MovementType::Random);

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
			//printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
			nbFrames = 0;
		}

		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----

		// render
		// ------
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		ResourceManager::GetShader(KEY_SHADER_OBJECT).use();
		
		for (int i = 0; i < 4; i++)
		{
			// TODO IMPLEMENT THIS
			cyborgModel.doCollusionCoin(coinList[i]);
		}
		cyborgModel.doCollusion(soldierModel);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("projection", projection);
		ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("view", view);
		
		processInput(window, cyborgModel);
		
		processInputForObject(window, cyborgModel);

		//// render the loaded model
		//for (int i =0; i< modelList.size(); i++)
		//{
		//	ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", modelList[i]->getModelMatrix());
		//	modelList[i]->update(ResourceManager::GetShader(KEY_SHADER_OBJECT));
		//}

		//if (checkDistance(cyborgModel))
		{
			ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", cyborgModel.getModelMatrix());
			cyborgModel.update(ResourceManager::GetShader(KEY_SHADER_OBJECT));
		}
		//if (checkDistance(soldierModel))
		{
			ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", soldierModel.getModelMatrix());
			soldierModel.update(ResourceManager::GetShader(KEY_SHADER_OBJECT));
		}
		//if (checkDistance(coinModel))
		{
			ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", coinModel.getModelMatrix());
			coinModel.update(ResourceManager::GetShader(KEY_SHADER_OBJECT));
		}
		for (int i = 0; i < 4; i++)
		{
			ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", coinList[i].getModelMatrix());
			coinList[i].update(ResourceManager::GetShader(KEY_SHADER_OBJECT));
		}


		////////////////////////////////////////////////////-----------------------------
		if (nbFrames < 1000 && VAR_TOTAL_LIVES!=0) 
		{
			// view/projection transformations
			glm::mat4 ORTHOprojection = glm::orthoLH(-10, 10, -10, 10, -10, 10);
			
			ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("projection", glm::mat4x4(1));
			ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("view", glm::mat4x4(1));

			glDepthFunc(GL_ALWAYS);
			
			for (float i = 0; i < VAR_TOTAL_LIVES; i++) {
				ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", { 1.0f,0.0f,0.0f,0.0f,//x
																				 0.0f,1.0f,0.0f,0.0f,//y
																				 0.0f,0.0f,0.0f,0.0f,//z
																				 -7.50f,7.50f-i,0.0f,8.0f });

				hpPanelModel.update(ResourceManager::GetShader(KEY_SHADER_OBJECT));
			}

			int tempscore = VAR_TOTAL_SCORE;
			float i=0;
			while(  5 <= tempscore ) {

				ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", { 1.0f,0.0f,0.0f,0.0f,//x
																				 0.0f,1.0f,0.0f,0.0f,//y
																				 0.0f,0.0f,0.0f,0.0f,//z
																				 6.0f - i,6.10f ,0.0f,6.5f });
				i++;
				tempscore -= 5;
				scorePanelModel.update(ResourceManager::GetShader(KEY_SHADER_OBJECT));
			}
			float j = 0;
			while (0 < tempscore) {
				
				ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", { 1.0f,0.0f,0.0f,0.0f,//x
																				 0.0f,1.0f,0.0f,0.0f,//y
																				 0.0f,0.0f,0.0f,0.0f,//z
																				 7.50f - j,6.40f ,0.0f,8.0f });
				
				j++;
				tempscore -= 1;
				scorePanelModel.update(ResourceManager::GetShader(KEY_SHADER_OBJECT));
			}
			
			if (VAR_HUNGER < 10) {
				ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", { 10 - VAR_HUNGER,0.0f,0.0f,0.0f,//x
																				 0.0f,0.5f,0.0f,0.0f,//y
																				 0.0f,0.0f,0.0f,0.0f,//z
																				 0.0f,-7.50f,0.0f,8.0f });
				VAR_HUNGER = VAR_HUNGER +( 0.000001*SCR_WIDTH);
				hungerPanelModel.update(ResourceManager::GetShader(KEY_SHADER_OBJECT));
			}
			else if( VAR_TOTAL_LIVES > 0)
			{
				VAR_HUNGER = 0;
				VAR_TOTAL_LIVES--;
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
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
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
void processInput(GLFWwindow *window, Model &model)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		moveTogether(Directions::FORWARD, model);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		moveTogether(Directions::BACKWARD, model);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		moveTogether(Directions::LEFT, model);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		moveTogether(Directions::RIGHT, model);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		moveTogether(Directions::UP, model);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		moveTogether(Directions::DOWN, model);
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		isGodMod = true;
	}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	{
		isGodMod = false;
	}
}

void moveTogether(Directions dir, Model& model)
{
	camera.ProcessKeyboard(dir, deltaTime);
	//if (isGodMod)
	//	model.tempMove(dir);
}

void processInputForObject(GLFWwindow * window, Model &model)
{
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		model.turnTowards(Directions::FORWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		model.turnTowards(Directions::BACKWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		model.turnTowards(Directions::LEFT);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		model.turnTowards(Directions::RIGHT);
	}
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
	{
		model.turnTowards(Directions::UP);
	}
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
	{
		model.turnTowards(Directions::DOWN);
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		std::cout << std::endl;
		model.printPosition();
	}
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
	{
		// Clear the console.
		std::cout << "\x1B[2J\x1B[H";
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




