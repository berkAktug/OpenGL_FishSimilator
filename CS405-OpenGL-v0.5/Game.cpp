#include "Game.h"

Game::Game() : Shader("", "")
{
}

Game::~Game()
{
}

void Game::init()
{
	//Initialise GLFW
	glewExperimental = true; // Needed for core profile

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		exit(1);
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // For OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MAcOS happy; probably will not need this.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

	// Open a window and create its OpenGL context
	this->window = glfwCreateWindow(windowSize[0], windowSize[1], "OpenGL C++ Window", nullptr, nullptr);
	if (this->window == nullptr) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version instead");
		getchar();
		glfwTerminate();
		exit(1);
	}

	glfwMakeContextCurrent(this->window);

	// Initialize GLEW
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		exit(1);
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(this->window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue Background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(this->window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited movement
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(this->window, windowSize[0] / 2, windowSize[1] / 2);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one.
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
}

glm::mat4 Game::getVPMatrix() {
	return this->VPMatrix;
}

void Game::run()
{
	GameControl::getIntance().init();

	//auto r_int = rand();

	//auto random_loc_transform = glm::translate(glm::mat4(1), glm::vec3((r_int % 5), 0.0f, 0.0f));
	//gameobjects[1].setModelMatrix(random_loc_transform);

	int SIZE = gameobjects.size();
	do {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		//glUseProgram(Game::getInstance().getProgramID());
		Shader.use();


		// Compute the MVP matrix from keyboard and mouse input
		GameControl::getIntance().computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = GameControl::getIntance().getProjectionMatrix();
		glm::mat4 ViewMatrix = GameControl::getIntance().getViewMatrix();

		this->VPMatrix = ProjectionMatrix * ViewMatrix;
		
		// Send our transformation to the currently bound shader,
		glUniformMatrix4fv(Game::getInstance().getViewMatrixID(), 1, GL_FALSE, &ViewMatrix[0][0]);

		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glUniform3f(Game::getInstance().getLightID(), lightPos.x, lightPos.y, lightPos.z);

		// Draw 
		for (int i = 0; i < SIZE; ++i) 
		{
			gameobjects[i].Draw(Shader);
		}

		// Swap buffers
		glfwSwapBuffers(Game::getInstance().getWindow());
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed.
	while (glfwGetKey(Game::getInstance().getWindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(Game::getInstance().getWindow()) == 0);

}

GLFWwindow* Game::getWindow() 
{
	return this->window;
}

void Game::cleanUp()
{
	//glDeleteBuffers(1, &vertexbuffer);
	//glDeleteBuffers(1, &uvbuffer);
	//glDeleteBuffers(1, &normalbuffer);
	//glDeleteProgram(programID);
	//glDeleteTextures(1, &Texture);
	//glDeleteVertexArrays(1, &vers.VAO_ID);
	glfwTerminate();
}

Game& Game::getInstance()
{
	static Game game;
	return game;
}

void Game::setShader(GameShader shader)
{
	this->ViewMatrixID = glGetUniformLocation(shader.ID, "V");
	this->MVPMatrixID = glGetUniformLocation(shader.ID, "MVP");
	this->ModelMatrixID = glGetUniformLocation(shader.ID, "M");

	// Get a handle for our "LightrPosition" uniform
	shader.use();

	this->LightID = glGetUniformLocation(shader.ID, "LightPosition_worldspace");
	this->TextureID = glGetUniformLocation(shader.ID, "myTextureSampler");
}

void Game::addObject(GameModel model)
{
	gameobjects.push_back(model);
}

GLuint Game::getMVPMatrixID()
{
	return this->MVPMatrixID;
}

GLuint Game::getViewMatrixID()
{
	return this->ViewMatrixID;
}

GLuint Game::getModelMatrixID()
{
	return this->ModelMatrixID;
}

GLuint Game::getTextureID()
{
	return this->TextureID;
}

GLuint Game::getLightID()
{
	return this->LightID;
}
