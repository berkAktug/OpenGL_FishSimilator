#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <vector>

#include <stdio.h>
#include <cstring>

// Include GLEW 
#include <GL/glew.h>

// Include GLM for 3D mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "Game.h"

class GameObject
{
public:
	GameObject();
	~GameObject();

	void loadObj(std::string path);

	glm::mat4 getModelMatrix();

	void setModelMatrix(glm::mat4 modelMatrix);

	void draw();

	GLuint getVAO_ID();
	int getVerticesSize();

private:
	bool loadOBJFromFile(const char *, std::vector<glm::vec3>&, std::vector<glm::vec2>&, std::vector<glm::vec3>&);
	GLuint bufferBinder(std::vector<glm::vec3> vect, int shader_buffer_pos);
	GLuint bufferBinder(std::vector<glm::vec2> vect, int shader_buffer_pos);

	GLuint VAO_ID;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	glm::mat4 ModelMatrix;
};

