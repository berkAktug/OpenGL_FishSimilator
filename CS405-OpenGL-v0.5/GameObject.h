#pragma once
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

#include "Game.h"

class GameObject {
public:		
	GameObject();
	GameObject(std::string path);
	~GameObject();

	void draw();

	GLuint getVAO_ID();
	GLuint getVerticesSize();

private:
	GLuint bufferBinder(std::vector<glm::vec3> vect, int shader_buffer_pos);
	GLuint bufferBinder(std::vector<glm::vec2> vect, int shader_buffer_pos);

	GLuint ModelMatrixID;
	GLuint VAO_ID;
	
	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;
	
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	float speed;
	float maxSpeed;
	
	bool isDead;
};



