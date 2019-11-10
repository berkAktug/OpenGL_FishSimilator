#include "GameShader.h"

GameShader::GameShader()
{
}

GameShader::~GameShader()
{
}

GLuint GameShader::getProgramID() 
{
	return this->programID;
}

std::string GameShader::readShader(const char* shader_file_path) 
{
	std::string ShaderCode;
	std::ifstream ShaderStream(shader_file_path, std::ios::in);
	if (ShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << ShaderStream.rdbuf();
		ShaderCode = sstr.str();
		ShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", shader_file_path);
		getchar();
		return 0;
	}
	return ShaderCode;
}

void GameShader::checkShader(GLuint shaderID) 
{
	GLint Result = GL_FALSE;
	int InfoLogLength;

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(shaderID, InfoLogLength, NULL, &ShaderErrorMessage[0]);
		printf("%s\n", &ShaderErrorMessage[0]);
	}
}

void GameShader::compileShader(const char* shader_file_path, std::string shaderCode, GLuint ShaderID)
{
	printf("Compiling shader : %s\n", shader_file_path);
	char const * VertexSourcePointer = shaderCode.c_str();
	glShaderSource(ShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(ShaderID);
}


GLuint GameShader::LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode = readShader(vertex_file_path);
	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode = readShader(fragment_file_path);
	// Compile Vertex Shader
	this->compileShader(vertex_file_path, VertexShaderCode, VertexShaderID);
	// Check Vertex Shader
	checkShader(VertexShaderID);
	// Compile Fragment Shader
	this->compileShader(fragment_file_path, FragmentShaderCode, FragmentShaderID);
	// Check Fragment Shader
	checkShader(FragmentShaderID);

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	this->programID = ProgramID;

	return ProgramID;
}