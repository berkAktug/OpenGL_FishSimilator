#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "model.h"
#include "Object.h"
#include "Enums.h"

class GameObject
{
public:
	GameObject(const std::string &path);
	~GameObject();

	void update();
	
	void loadObject(std::string path);

	void scaleObject(glm::vec3 scale);

	void rotateObject(glm::vec3 rotate);

	void printObject();

	//void setShaderKey(std::string keyShader);

	void setObjectType(ObjectType objectType);

	void setDirection(Directions dir);

private:
	Model *model;
	Object *cage;
	PhysicEngine *physics;

	ObjectType objectType;

	//std::string keyShader;

	void _move();
	void _move(glm::vec3 vec);

	void _draw(Shader shader);
};

GameObject::GameObject(const std::string &path) : physics()
{
	model = &Model(path);
	//cage = 
}

GameObject::~GameObject()
{
}

void GameObject::scaleObject(glm::vec3 scale)
{

}

void GameObject::printObject()
{
	model->printPosition();
	model->printModel();
	cage->printBox();
}

#endif