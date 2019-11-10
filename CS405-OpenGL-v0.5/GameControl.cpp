#include "GameControl.h"

using namespace glm;


GameControl::GameControl()
{
}

void GameControl::init() 
{
	// Initial position : on +Z
	this->position = glm::vec3(0, 0, 5);
	// Initial horizontal angle : toward -Z
	this->horizintalAngle = 3.14f;
	// Initial vertical angle : none
	this->verticalAngle = 0.0f;
	// Initial Flied of View
	this->initialFoV = 45.0f;

	this->speed = 1.0f; // 3 unit / second
	this->mouseSpeed = 0.005f;
}

GameControl & GameControl::getIntance()
{
	// TODO: insert return statement here
	static GameControl gamecontrol;
	return gamecontrol;
}

GameControl::~GameControl()
{
}

glm::mat4 GameControl::getProjectionMatrix() 
{
	return ProjectionMatrix;
}

glm::mat4 GameControl::getViewMatrix() 
{
	return ViewMatrix;
}

void GameControl::computeMatricesFromInputs() 
{
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(Game::getInstance().getWindow(), &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(Game::getInstance().getWindow(), 1024 / 2, 768 / 2);

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
	if (glfwGetKey(Game::getInstance().getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
		position += direction * deltaTime * speed;
	}

	// Move Backward
	if (glfwGetKey(Game::getInstance().getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
	}

	// Strafe Right
	if (glfwGetKey(Game::getInstance().getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
		position += right * deltaTime * speed;
	}

	// Strafe Left
	if (glfwGetKey(Game::getInstance().getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
		position -= right * deltaTime * speed;
	}

	// Ascend up
	if (glfwGetKey(Game::getInstance().getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS) {
		position += up * deltaTime * speed;
	}

	// Decend down
	if (glfwGetKey(Game::getInstance().getWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
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

