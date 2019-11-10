#include "Game.h"

/* Null,  beacuse instance will be initialized on demand.*/
Game* Game::gameInstance = 0;

bool Game::Init() {

	if (this->gameInstance != 0) {
		fprintf(stderr, "Game is already initialized\n");
		getchar();
		return false;
	}

	glewExperimental = true; // Needed for core profile

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return false;
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
		return false;
	}

	glfwMakeContextCurrent(this->window);

	// Initialize GLEW
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return false;
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

	return true;
}

void Game::setShaders(std::string pathVertexShader, std::string pathFragmentShader) {
	this->ProgramID = LoadShaders(pathVertexShader.c_str(), pathFragmentShader.c_str());

	this->MVPMatrixID	= glGetUniformLocation(this->ProgramID, "MVP");
	this->ViewMatrixID	= glGetUniformLocation(this->ProgramID, "V");
	this->ModelMatrixID = glGetUniformLocation(this->ProgramID, "M");
}

void Game::setBackgroundColor(float red, float green, float blue, float alpha = 0.0f) {
	glClearColor(red, green, blue, alpha);
}

GLuint Game::getID_Matrix_Model() {
	return this->ModelMatrixID;
}

GLuint Game::getID_Matrix_View() {
	return this->ViewMatrixID;
}

GLuint Game::getID_Matrix_MVP() {
	return this->MVPMatrixID;
}

GLuint Game::getID_Program() {
	return this->ProgramID;
}

void Game::setTexture_DDS(std::string path)
{
	this->Texture = loadDDS(path.c_str());
	this->TextureID = glGetUniformLocation(this->getID_Program(), "myTextureSampler");

}

void Game::setTexture_BMP_custom(std::string path)
{
	this->Texture = loadBMP_custom(path.c_str());
	this->TextureID = glGetUniformLocation(this->getID_Program(), "myTextureSampler");

	glUseProgram(this->getID_Program());
	this->LightID = glGetUniformLocation(this->getID_Program(), "LightPosition_worldspace");
}

GLuint Game::getTexture()
{
	return this->Texture;
}

GLuint Game::getID_Texture()
{
	return this->TextureID;
}

Game* Game::getGameInstance()
{
	if (gameInstance == 0)
	{
		gameInstance = new Game();
	}

	return gameInstance;
}

void Game::run()
{
	//GameObject objSuzanne = GameObject("suzanne.obj");
	GameObject objCube = GameObject("cube.obj");

	glUseProgram(this->getID_Program());
	this->LightID = glGetUniformLocation(this->getID_Program(), "LightPosition_worldspace");

	do {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(this->ProgramID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();

		glUniformMatrix4fv(this->ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->Texture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(this->TextureID, 0);

		//objSuzanne.draw();
		objCube.draw();

		// Swap buffers
		glfwSwapBuffers(Game::window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed.
	while (glfwGetKey(Game::window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(Game::window) == 0);

}

void Game::computeMatricesFromInputs()
{
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Compute new orientation
	horizintalAngle += mouseSpeed * float(1024 / 2 - xpos);
	verticalAngle += mouseSpeed * float(768 / 2 - ypos);

	//Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizintalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizintalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizintalAngle - 3.14f / 2.0f),
		0,
		cos(horizintalAngle - 3.14f / 2.0f)
	);

	//Up vector
	glm::vec3 up = glm::cross(right, direction);

	// Move Forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += direction * deltaTime * speed;
	}

	// Move Backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
	}

	// Strafe Right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += right * deltaTime * speed;
	}

	// Strafe Left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= right * deltaTime * speed;
	}

	// Ascend up
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		position += up * deltaTime * speed;
	}

	// Decend down
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		position -= up * deltaTime * speed;
	}

	float FoV = initialFoV; // -5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,			// Camera is here
		position + direction, // and looks here : at the same position, plug "direction"
		up					// Head is up (set to 0,-1,0 to look upside-down)
	);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;

}

Game::Game()
{
	this->Init();
}


Game::~Game()
{

}

glm::mat4 Game::getMatrix_Model() 
{
	//return ModelMatrix;
	return glm::mat4(1.0); // for now.
}

glm::mat4 Game::getMatrix_Projection()
{
	return ProjectionMatrix;
}

glm::mat4 Game::getMatrix_View()
{
	return ViewMatrix;
}
