#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

class GameShader
{
public:
	GameShader();
	~GameShader();

	GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
	GLuint getProgramID();
	
private:
	GLuint programID;

	void checkShader(GLuint shaderID);
	void compileShader(const char * shader_file_path, std::string shaderCode, GLuint ShaderID);
	std::string readShader(const char * shader_file_path);
};

