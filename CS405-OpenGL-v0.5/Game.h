#pragma once

// Include string
#include <string>

// Include vectors
#include <vector>

// Include GLEW 
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM for 3D mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Include objLoader
#include "Common/objloader.hpp"

// Include shaders
#include "Common/shader.hpp"

// Include texture
#include "Common/texture.hpp"

#include "GameObject.h"

class Game
{
public:

	static Game* getGameInstance();

	void setShaders(std::string pathVertexShader, std::string pathFragmentShader);
	void setBackgroundColor(float red, float green, float blue, float alpha);

	GLuint getID_Matrix_Model();
	GLuint getID_Matrix_MVP();
	GLuint getID_Matrix_View();

	GLuint getID_Program();

	GLuint getTexture();
	GLuint getID_Texture();

	void setTexture_DDS(std::string path);
	void setTexture_BMP_custom(std::string path);


	glm::mat4 getMatrix_Projection();
	glm::mat4 getMatrix_View();
	glm::mat4 getMatrix_Model();

	void run();

	void computeMatricesFromInputs();

private:
	// Private constructor so that no objects can be created.
	Game();
	~Game();

	bool Init();

	static Game* gameInstance;
	GLFWwindow * window;

	int windowSize[2] = { 1024, 768 };
	GLuint ProgramID;
	GLuint MVPMatrixID;
	GLuint ViewMatrixID;
	GLuint ModelMatrixID;
	GLuint LightID;

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
	glm::mat4 ModelMatrix;

	glm::vec3 position;
	float horizintalAngle;
	float verticalAngle;
	float initialFoV;

	float speed;
	float mouseSpeed;


	////
	GLuint TextureID;
	GLuint Texture;
};
