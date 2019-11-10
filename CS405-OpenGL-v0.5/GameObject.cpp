#include "GameObject.h"

GameObject::GameObject()
{
}

GameObject::GameObject(std::string path)
{
	this->vertices = std::vector<glm::vec3>();
	this->uvs		= std::vector<glm::vec2>();
	this->normals = std::vector<glm::vec3>();

	bool res = loadOBJ(path.c_str(), this->vertices, this->uvs, this->normals);

	// Initialize VAO -> Vertex Array Object
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Initialize vertex buffer
	this->vertexbuffer	= bufferBinder(vertices, 0);
	// Initialize uv buffer
	this->uvbuffer		= bufferBinder(uvs, 1);
	// Initialize Normal buffer
	this->normalbuffer	= bufferBinder(normals, 2);

	glBindVertexArray(0);
	this->VAO_ID = VertexArrayID;
}

GameObject::~GameObject()
{
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteVertexArrays(1, &this->VAO_ID);
}

GLuint GameObject::getVerticesSize()
{
	return this->vertices.size();
}

GLuint GameObject::getVAO_ID() 
{
	return this->VAO_ID;
}

void GameObject::draw()
{
	Game *gameInstance = Game::getGameInstance();

	glm::mat4 MVP = gameInstance->getMatrix_Projection() * gameInstance->getMatrix_View() * gameInstance->getMatrix_Model();

	glUniformMatrix4fv(gameInstance->getID_Matrix_MVP(), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(gameInstance->getID_Matrix_Model(), 1, GL_FALSE, &gameInstance->getMatrix_Model()[0][0]);

	glBindVertexArray(this->VAO_ID);

	glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
}


GLuint GameObject::bufferBinder(std::vector<glm::vec2> vect, int shader_buffer_pos) {
	// This will identify our vertex buffer
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, vect.size() * sizeof(glm::vec2), &vect[0], GL_STATIC_DRAW);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(shader_buffer_pos);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexAttribPointer(
		shader_buffer_pos,		// attribute. No particular reason for 0, but must match the Layout in the shader.
		2,						// size
		GL_FLOAT,				// type
		GL_FALSE,				// (is) normalized?
		0,						// stride
		(void*)0				// array buffer offset
	);
	return buffer;
}

GLuint GameObject::bufferBinder(std::vector<glm::vec3> vect, int shader_buffer_pos) {
	// This will identify our vertex buffer
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, vect.size() * sizeof(glm::vec3), &vect[0], GL_STATIC_DRAW);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(shader_buffer_pos);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexAttribPointer(
		shader_buffer_pos,		// attribute. No particular reason for 0, but must match the Layout in the shader.
		3,						// size
		GL_FLOAT,				// type
		GL_FALSE,				// (is) normalized?
		0,						// stride
		(void*)0				// array buffer offset
	);
	return buffer;
}