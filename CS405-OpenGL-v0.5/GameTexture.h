#pragma once

#include <string>
#include <iostream>

//#include <GL/glew.h>
#include "Include/glad/glad.h"

#include "stb_image.cpp"

class GameTexture
{
public:
	GameTexture(std::string path);

	GLuint getID();

	void setPath(std::string path);
	std::string getPath();

	void use();
private:
	GLuint loadTexture(std::string path);

	GLuint ID;

	std::string path;
};

