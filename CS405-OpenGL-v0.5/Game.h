#ifndef GAME_H
#define GAME_H
#include <stdio.h>
#include <string>

// Include GLEW 
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

// We are introducing GameObject class to compiler so it does not 
// freak out with circular reference problem.
class GameModel;

#include "GameControl.h"
//#include "GameTexture.h"
#include "GameModel.h"
#include "GameShader.h"

class Game
{
public:
	static Game &getInstance();

	void init();

	glm::mat4 getVPMatrix();

	void run();

	void setShader(GameShader Shader);

	void addObject(GameModel model);
	
	GLuint getMVPMatrixID();
	GLuint getViewMatrixID();
	GLuint getModelMatrixID();
	
	GLuint getTextureID();
	GLuint getLightID();

	GLFWwindow* getWindow();

	void cleanUp();

	~Game();
private:

	int windowSize[2] = { 1024, 768 };

	GLuint MVPMatrixID;
	GLuint ViewMatrixID;
	GLuint ModelMatrixID;
	GLuint TextureID;
	GLuint LightID;

	glm::mat4 VPMatrix;

	GameShader  Shader; // This is temporary solution. Need to allow multiple textures

	std::vector<GameModel> gameobjects;

	GLFWwindow* window;

	Game();
};

#endif