#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
//#include <fstream>

#include <GL/glew.h>

#include <GLFW/glfw3.h>


class GameTexture
{
public:
	GameTexture();
	~GameTexture();

	GLuint loadBMP_custom(const char * imagepath);
	GLuint loadDDS(const char * imagepath);

	GLuint getTexture();
private:
	GLuint TextureID;
};

