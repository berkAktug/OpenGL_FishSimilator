#ifndef MODEL_H
#define MODEL_H

#include "Include/glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.cpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"

#include "GameObject.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

struct boundingboxDimension {
	double max_x;
	double max_y;
	double max_z;

	double min_y;
	double min_z;
	double min_x;

	boundingboxDimension() : max_x(1), max_y(1), max_z(1), min_x(0), min_y(0), min_z(0)
	{
	}
};


enum MoveDirections {
	M_UP,
	M_DOWN,
	M_LEFT,
	M_RIGHT,
	M_FORWARD,
	M_BACKWARD
};

class Model
{
public:
	/*  Model Data */
	std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrection;
	int ID;

	/*  Functions   */
	// constructor, expects a filepath to a 3D model.
	Model(std::string const &path, bool gamma = false) : gammaCorrection(gamma)
	{
		Point point = getRandomPoint();
		boxDimensions = boundingboxDimension();
			
		loadModel(path);
		
		ID = rand() % 1000;

		ObjectBound dimensions = ObjectBound(
			abs(boxDimensions.max_y - boxDimensions.min_y),
			abs(boxDimensions.max_z - boxDimensions.min_z),
			abs(boxDimensions.max_x - boxDimensions.min_x)
		);

		this->modelObject = GameObject(point, dimensions);

		// Set model matrix to initial value;
		this->modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(point.x, point.y, point.z));

		//this->modelMatrix = glm::scale(modelMatrix, glm::vec3(dimensions.width, dimensions.height, dimensions.width));
		frameCounter = 0;
	}


	void setFloor()
	{
		auto pos = modelObject.getCenter();
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-pos.x, -pos.y, -pos.z));
		modelObject.setFloor();
	}

	void printPosition()
	{
		std::cout << "Object "<< ID <<" is at~~"
			<< " x: " << modelObject.getCenter().x
			<< " y: " << modelObject.getCenter().y
			<< " z: " << modelObject.getCenter().z
			<< " height: " << modelObject.getBoundry().height
			<< " width: " << modelObject.getBoundry().width
			<< " depth: " << modelObject.getBoundry().depth << std::endl;
	}

	bool isFloorObj()
	{
		return modelObject.getIsFloor();
	}

	void doCollision(Model &other)
	{
		if (this->ID == other.ID)
		{
			return;
		}
		if (modelObject.doCollision(other.modelObject))
		{
			std::cout << "Collision between OBJ1: " << ID << " And OBJ2: " << other.ID << std::endl;
			printPosition();
			other.printPosition();
			if (this->isFloorObj())
			{
				// :)
				auto escape_velocity = 5.5f;
				other.move(escape_velocity, M_UP);
			}
			else if (other.isFloorObj())
			{
				auto escape_velocity = 5.5f;
				this->move(escape_velocity, M_UP);
			}
			else
			{
				auto lpos = this->modelObject.getCenter();
				auto rpos = other.modelObject.getCenter();

				auto lspeed = glm::vec3(rpos.x - lpos.x, rpos.y - lpos.y, rpos.z - lpos.z);
				auto rspeed = -lspeed;

				this->move(-lspeed);
				other.move(-rspeed);
			}
		}
	}

	// draws the model, and thus all its meshes
	void Draw(Shader shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

	void ScaleModel(glm::vec3 scaleVector)
	{
		modelObject.scaleBoundry(scaleVector);
		modelMatrix = glm::scale(modelMatrix, scaleVector);	// it's a bit too big for our scene, so scale it down
	}

	void move(glm::vec3 speed)
	{
		//printPosition();
		modelMatrix = glm::translate(modelMatrix, speed);
		modelObject.setSpeed(speed);
		modelObject.move();
	}

	void move(double deltaDistance, MoveDirections dir)
	{
		//printPosition();
		glm::vec3 tmp_vel;
		switch (dir)
		{
		case M_UP:
			// Move UP
			tmp_vel = glm::vec3(0.0f, deltaDistance, 0.0f);
			modelObject.setSpeed(tmp_vel);
			modelObject.move();
			modelMatrix = glm::translate(modelMatrix, tmp_vel);
			break;
		case M_DOWN:
			// Move DOWN
			tmp_vel = glm::vec3(0.0f, -deltaDistance, 0.0f);
			modelObject.setSpeed(tmp_vel);
			modelObject.move();
			modelMatrix = glm::translate(modelMatrix, tmp_vel);
			break;
		case M_LEFT:
			// Move LEFT
			tmp_vel = glm::vec3(deltaDistance, 0.0f, 0.0f);
			modelObject.setSpeed(tmp_vel);
			modelObject.move();
			modelMatrix = glm::translate(modelMatrix, tmp_vel);
			break;
		case M_RIGHT:
			// Move RIGHT
			tmp_vel = glm::vec3(-deltaDistance, 0.0f, 0.0f);
			modelObject.setSpeed(tmp_vel);
			modelObject.move();
			modelMatrix = glm::translate(modelMatrix, tmp_vel);
			break;
		case M_FORWARD:
			// Move FORWARD
			tmp_vel = glm::vec3(0.0f, 0.0f, deltaDistance);
			modelObject.setSpeed(tmp_vel);
			modelObject.move();
			modelMatrix = glm::translate(modelMatrix, tmp_vel);
			break;
		case M_BACKWARD:
			// Move BACKWARD
			tmp_vel = glm::vec3(0.0f, 0.0f, -deltaDistance);
			modelObject.setSpeed(tmp_vel);
			modelObject.move();
			modelMatrix = glm::translate(modelMatrix, tmp_vel);
			break;
		default:
			break;
		}
		//modelObject.move(tmp_vel);
		//modelMatrix = glm::translate(modelMatrix, tmp_vel);
	}

	void moveRandom(double deltaDistance)
	{
		if (frameCounter % 30 == 0)
		{
			frameCounter = 0;
			randDirection = rand() % 5;
		}
		frameCounter++;			

		switch (randDirection)
		{
		case 0: // move up
			move(deltaDistance, M_UP);
			break;
		case 1:// move down
			move(deltaDistance, M_DOWN);
			break;
		case 2:// move left
			move(deltaDistance, M_LEFT);
			break;
		case 3:// move right
			move(deltaDistance, M_RIGHT);
			break;
		case 4:// move forward
			move(deltaDistance, M_FORWARD);
			break;
		case 5:// move backward
			move(deltaDistance, M_BACKWARD);
			break;
		default:
			break;
		}
	}

	const glm::mat4 GetModelMatrix() const
	{
		return modelMatrix;
	}

private:
	/*  Model Data  */
	glm::mat4 modelMatrix;
	int randDirection;
	int frameCounter;

	GameObject modelObject;

	boundingboxDimension boxDimensions;


	/*  Functions   */
	Point getRandomPoint()
	{
		auto randx = rand() % 3;
		auto randy = rand() % 3;
		auto randz = rand() % 3;

		auto randsignx = rand() % 2;
		auto randsignz = rand() % 2;

		if (randsignx == 0) randsignx *= -1;
		if (randsignz == 0) randsignz *= -1;

		randx *= randsignx;
		randz *= randsignz;

		return Point(randx, randy, randz);
	}

	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(std::string const &path)
	{
		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
			return;
		}
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		// process ASSIMP's root node recursively
		processNode(scene->mRootNode, scene);
	}

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode *node, const aiScene *scene)
	{
		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}

	}

	Mesh processMesh(aiMesh *mesh, const aiScene *scene)
	{
		// data to fill
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		// Walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			// positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			//////////////////////////////////////
			if (boxDimensions.min_x > vector.x) { boxDimensions.min_x = vector.x; }
			if (boxDimensions.min_y > vector.y) { boxDimensions.min_y = vector.x; }
			if (boxDimensions.min_z > vector.z) { boxDimensions.min_z = vector.z; }

			if (boxDimensions.max_x < vector.x) { boxDimensions.max_x = vector.x; }
			if (boxDimensions.max_y < vector.y) { boxDimensions.max_y = vector.x; }
			if (boxDimensions.max_z < vector.z) { boxDimensions.max_z = vector.z; }
			//////////////////////////////////////
			// normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
			vertices.push_back(vertex);
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// process materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN

		// 1. diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		// return a mesh object created from the extracted mesh data
		return Mesh(vertices, indices, textures);
	}

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
	{
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // if texture hasn't been loaded already, load it
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}
};


unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
	std::string filename = std::string(path);
	//filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// Operator Overload: Is Equal checks.
inline bool operator==(const Model& lhs, const Model& rhs)
{
	if (lhs.directory == rhs.directory)
	{
		if (lhs.GetModelMatrix() == rhs.GetModelMatrix())
		{
			return true;
		}
	}
	return false;
}
inline bool operator!=(const Model& lhs, const Model& rhs) { return !(lhs == rhs); }

#endif
