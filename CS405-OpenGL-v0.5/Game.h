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
class GameObject;

#include "GameControl.h"
#include "GameTexture.h"
#include "GameObject.h"
#include "GameShader.h"

class Game
{
public:
	static Game &getInstance();

	void init();

	glm::mat4 getVPMatrix();

	void run();

	void setShader(const char*vertexshaderpath, const char*fragmentshaderpath);

	void setTexture_DDS(std::string imagepath);

	void addObject(std::string objectpath);

	//GameObject getNextObject(); // This one is tricky.
	//bool isEqual(GameObject leftobject, GameObject rightobject); // Not sure if i need it

	GLuint getProgramID();
	
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

	GLuint ProgramID;
	GLuint MVPMatrixID;
	GLuint ViewMatrixID;
	GLuint ModelMatrixID;
	GLuint TextureID;
	GLuint LightID;

	glm::mat4 VPMatrix;

	GameTexture Texture; // This is temporary solution. Need to allow multiple textures
	GameShader  Shader; // This is temporary solution. Need to allow multiple textures

	std::vector<GameObject> gameobjects;

	GLFWwindow* window;

	Game();
};

#endif