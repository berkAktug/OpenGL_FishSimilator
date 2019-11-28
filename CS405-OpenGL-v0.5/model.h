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

#include "Object.h"

#include "Enums.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model
{
public:
	/*  Model Data */
	std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrection;

	/*  Functions   */
	// constructor, expects a filepath to a 3D model.
	Model(std::string const &path, bool gamma = false) : gammaCorrection(gamma), cage(Object())
	{
		_loadModel(path);

		cage.setupCenter();
		ID = rand() % 1000;

		// Set model to random place;
		cage.placeRandomly();
	}

	void setID(int id);

	void addTexture(std::string path, std::string type);

	void printModel();

	void printPosition();

	void doCollusion(Model &other);

	MovementType getRandomMovmeent();

	void setMovementType(MovementType movementType);

	void ScaleModel(glm::vec3 scaleVector);

	void update(Shader shader);

	void turnTowards(Directions dir);

	glm::mat4 getModelMatrix();

	glm::vec3 getPosition();

	void setPosition(glm::vec3 pos);

	void tempMove(glm::vec3 vec);

private:
	/*  Model Data  */
	int ID;

	Object cage;

	/*  Functions   */
	void _Draw(Shader shader);

	void _loadModel(std::string const & path);

	void _processNode(aiNode * node, const aiScene * scene);

	Mesh _processMesh(aiMesh * mesh, const aiScene * scene);

	std::vector<Texture> _loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName);
};

void Model::setID(int id)
{
	this->ID = id;
}

void Model::addTexture(std::string path, std::string type = "texture_diffuse")
{
	// THIS ASSUMES THERE ARE NO DELETE OPERATIONS IN THE VECTOR
	auto temp_id = textures_loaded.size() + 1;
	Texture temp;
	temp.id = temp_id;
	temp.path = path;
	temp.type = type;
	textures_loaded.push_back(temp);
}

void Model::printModel()
{
	std::cout << "Object " << ID << " is at~~" << std::endl;
	glm::mat4 pMat4 = cage.getModelMatrix();  // your matrix

	double dArray[16] = { 0.0 };

	const float *pSource = (const float*)glm::value_ptr(pMat4);
	for (int i = 0; i < 16; ++i)
		dArray[i] = pSource[i];

	std::cout << " [0]: " << dArray[0] << " [1]: " << dArray[1] << " [2]: " << dArray[2] << " [3]: " << dArray[3] << std::endl;
	std::cout << " [4]: " << dArray[4] << " [5]: " << dArray[5] << " [6]: " << dArray[6] << " [7]: " << dArray[7] << std::endl;
	std::cout << " [8]: " << dArray[8] << " [9]: " << dArray[9] << " [10]: " << dArray[10] << " [11]: " << dArray[11] << std::endl;
	std::cout << " [12]: " << dArray[12] << " [13]: " << dArray[13] << " [14]: " << dArray[14] << " [15]: " << dArray[15] << std::endl;
	std::cout << std::endl;
}

void Model::printPosition()
{
	std::cout << "Object " << ID << " is at~~";
	cage.printBox();
}

void Model::doCollusion(Model &other)
{
	if (this->ID == other.ID)
	{
		return;
	}
	cage.doCollusion(other.cage, CollusionDetectionType::CollusionAABB);
}

MovementType Model::getRandomMovmeent()
{
	return cage.getMovementType();
}

void Model::setMovementType(MovementType movementType)
{
	cage.setMovementType(movementType);
}

void Model::ScaleModel(glm::vec3 scaleVector)
{
	cage.scale(scaleVector);
}

void Model::update(Shader shader)
{
	float currentFrame = glfwGetTime();

	cage.update(currentFrame);
	//if (cage.getCenter().x < 10 )
	//{

		_Draw(shader);

	//}
	//cage.update(currentFrame);

	//_Draw(shader);
}

void Model::turnTowards(Directions dir)
{
	cage.turnTowards(dir);
}

inline glm::mat4 Model::getModelMatrix()
{
	return cage.getModelMatrix();
}

glm::vec3 Model::getPosition()
{
	return cage.getCenter();
}

inline void Model::setPosition(glm::vec3 pos)
{
	cage.placeTo(pos);
}

void Model::tempMove(glm::vec3 vec)
{
	cage.tempMove(vec);
}

// draws the model, and thus all its meshes
void Model::_Draw(Shader shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shader);
	}
}

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::_loadModel(std::string const &path)
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
	_processNode(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::_processNode(aiNode *node, const aiScene *scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(_processMesh(mesh, scene));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		_processNode(node->mChildren[i], scene);
	}

}

Mesh Model::_processMesh(aiMesh *mesh, const aiScene *scene)
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

		cage.setupCollusionBox(vector);

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
	std::vector<Texture> diffuseMaps = _loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<Texture> specularMaps = _loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = _loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = _loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	// return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures);
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::vector<Texture> Model::_loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
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

#endif
