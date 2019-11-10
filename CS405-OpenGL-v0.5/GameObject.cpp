#include "GameObject.h"

GameObject::GameObject()
{
}


GameObject::~GameObject()
{
}

void GameObject::loadObj(std::string path)
{
	// Cube
	bool res = loadOBJFromFile(path.c_str(), vertices, uvs, normals);

	// Initialize VAO -> Vertex Array Object
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Initialize Vertex buffer
	GLuint vertebuffer	= bufferBinder(vertices, 0);
	// Initialize UV buffer
	GLuint uvbuffer		= bufferBinder(uvs, 1);
	// Initialize Normal buffer
	GLuint normalbuffer = bufferBinder(normals, 2);

	//////////
	this->ModelMatrix = glm::mat4(1.0);
	//////////

	glBindVertexArray(0);
	VAO_ID = VertexArrayID;
}

glm::mat4 GameObject::getModelMatrix() {
	return this->ModelMatrix;
}

void GameObject::setModelMatrix(glm::mat4 modelMatrix) {
	this->ModelMatrix = modelMatrix;
}

void GameObject::draw()
{
	glm::mat4 MVP = Game::getInstance().getVPMatrix() * ModelMatrix;
	glUniformMatrix4fv(Game::getInstance().getMVPMatrixID(), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(Game::getInstance().getModelMatrixID(), 1, GL_FALSE, &ModelMatrix[0][0]);
	glBindVertexArray(this->VAO_ID);
	glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
}

GLuint GameObject::getVAO_ID()
{
	return this->VAO_ID;
}

int GameObject::getVerticesSize()
{
	return this->vertices.size();
}

GLuint GameObject::bufferBinder(std::vector<glm::vec2> vect, int shader_buffer_pos) {
	// This will identify our buffer
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, vect.size() * sizeof(glm::vec2), &vect[0], GL_STATIC_DRAW);

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
	// This will identify our  buffer
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, vect.size() * sizeof(glm::vec3), &vect[0], GL_STATIC_DRAW);

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

bool GameObject::loadOBJFromFile(const char * path,
			std::vector<glm::vec3> & out_vertices,
			std::vector<glm::vec2> & out_uvs,
			std::vector<glm::vec3> & out_normals) 
{
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertrexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	FILE * file;
	errno_t err = fopen_s(&file, path, "r");
	if (err != 0) {
		printf("Impossible to open the file !\n");
		return false;
	}

	while (true) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		// Note that this check makes the assumption the file will be not be longer than 128 characters. 
		// which is very silly/optimistic..
		// TODO: implement new check for this.
		if (res == EOF) {
			break; //END OF FILE. END OF THE ROAD.
		}
		// else : parse lineHeader
		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file,"%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0],
																	  &vertexIndex[1], &uvIndex[1], &normalIndex[1],
																	  &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser : (Try exporting with other options)\n)");
				return false;
			}
			// Looping this somehow breaks the content.
			vertrexIndices.push_back(vertexIndex[0]);
			vertrexIndices.push_back(vertexIndex[1]);
			vertrexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertrexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertrexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes from their index.
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);
	}

	fclose(file);
	return true;
}