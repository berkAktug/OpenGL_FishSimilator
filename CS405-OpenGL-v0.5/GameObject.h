#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "model.h"
//#include "Object.h"
#include "Enums.h"
#include "PhysicsEngine.h"

class GameObject
{
public:
	GameObject(const std::string &path, ObjectType objectType);
	~GameObject();

	void Update(std::string shader_key);
	
	void Update(Shader shader);

	void ScaleObject(glm::vec3 scale);

	//void RotateObject(glm::vec3 rotate);

	void SetID(int id);

	void Print();

	void AccelerateTowards(Directions dir);

	void doCollusion(GameObject *other);

private:
	/*  Object Data  */
	Model *_model;
	PhysicEngine *_physics;

	MovementType _movementType;

	ObjectType _objectType;

	int _ID;
	int _lastdirection;
	int _frameCounter;
	float _deltaTime;
	float _lastTime;

	glm::vec3 _scaleFactor;

	/* Functions */
	void _Move();
	void _MoveTo(glm::vec3 vec);

	glm::vec3 _GetUpDownDirectionVector();

	glm::vec3 _GetRandomDirectionVector();
};

GameObject::GameObject(const std::string &path, ObjectType objectType) 
	: _objectType(objectType), _ID(rand() % 1000), _scaleFactor(1.0f)
{
	_model = new Model(path);
	_physics = new PhysicEngine(_model->GetMax(), _model->GetMin());
	
	auto rand_point_vec = Point::getRandomPointVector();

	switch (objectType)
	{
	case Player:
		_MoveTo(rand_point_vec);
		_movementType = MovementType::Normal;
		break;
	case Enemy:
		_MoveTo(rand_point_vec);
		_movementType = MovementType::Random;
		break;
	case Coin:
		_MoveTo(rand_point_vec);
		_lastdirection = 1;
		_movementType = MovementType::UpDown;
		break;
	case Envoirmental:
		_physics->SetMoveablity(false);
		_movementType = MovementType::NONE;
		break;
	case OnScreenPanel:
		_physics->SetMoveablity(false);
		_movementType = MovementType::NONE;
		break;
	default:
		break;
	}

}

GameObject::~GameObject()
{

}

void GameObject::Update(Shader shader)
{
	float currentFrame = glfwGetTime();
	this->_deltaTime = currentFrame - this->_lastTime;
	this->_lastTime = currentFrame;


	_Move();

	_model->Draw(shader);
}

void GameObject::Update(std::string shader_key)
{
	ResourceManager::GetShader(shader_key).setMat4("model", _model->GetModelMatrix());

	Update(ResourceManager::GetShader(shader_key));
	//_Move();

	//_model->Draw(ResourceManager::GetShader(shader_key));
}

void GameObject::ScaleObject(glm::vec3 scale)
{
	_scaleFactor /= scale;

	_physics->Scale(scale);
	_model->ScaleModel(scale);
}

void GameObject::SetID(int id)
{
	_ID = id;
}

void GameObject::Print()
{
	std::cout << "Object " << _ID << " is at~~" << std::endl;
	_physics->Print();
	_model->Print();
	std::cout
		<< "Model max x: " << _model->GetMax().x
		<< " Model max y: " << _model->GetMax().y
		<< " Model max z: " << _model->GetMax().z << std::endl
		<< "Model min x: " << _model->GetMin().x
		<< " Model min y: " << _model->GetMin().y
		<< " Model min z: " << _model->GetMin().z << std::endl;
}

void GameObject::AccelerateTowards(Directions dir)
{
	if (!_movementType == MovementType::Normal) 
	{
		return;
	}

	switch (dir)
	{
	case UP:
		_physics->AccelerateTowards(VECTOR_UP);
		break;
	case DOWN:
		_physics->AccelerateTowards(-VECTOR_UP);
		break;
	case LEFT:
		_physics->AccelerateTowards(-VECTOR_RIGHT);
		break;
	case RIGHT:
		_physics->AccelerateTowards(VECTOR_RIGHT);
		break;
	case FORWARD:
		_physics->AccelerateTowards(VECTOR_FORWARD);
		break;
	case BACKWARD:
		_physics->AccelerateTowards(-VECTOR_FORWARD);
		break;
	default:
		break;
	}
}

void GameObject::doCollusion(GameObject *other)
{
	if (this->_ID == other->_ID)
	{
		return;
	}

	_physics->DoCollusion(other->_physics);
}

glm::vec3 GameObject::_GetRandomDirectionVector()
{
	if (_frameCounter % 30 == 0)
	{
		_frameCounter = 0;
		_lastdirection = rand() % 6;

		_physics->StopMotion();
	}
	_frameCounter++;

	switch (_lastdirection)
	{
	case 0://Directions::UP:
		return VECTOR_UP;
	case 1://Directions::DOWN:
		return -VECTOR_UP;
	case 2://Directions::RIGHT:
		return VECTOR_RIGHT;
	case 3://Directions::LEFT:
		return -VECTOR_RIGHT;
	case 4://Directions::FORWARD:
		return VECTOR_FORWARD;
	case 5://Directions::BACKWARD:
		return -VECTOR_FORWARD;
	default:
		return VECTOR_ZERO;
	}
}

void GameObject::_Move()
{
	if (!_physics->CanMove() || _objectType == ObjectType::OnScreenPanel || _objectType == ObjectType::Envoirmental)
	{
		return;
	}
	
	switch (_movementType)
	{
	case Random:
		_physics->AccelerateTowards(_GetRandomDirectionVector());
		_physics->Apply(_deltaTime);
		break;
	case Normal:
		_physics->Apply(_deltaTime);
		break;
	case UpDown:
		_physics->AccelerateTowards(_GetUpDownDirectionVector());
		_physics->Apply(_deltaTime);
		break;
	case NONE:
		_physics->StopMotion();
		break;
	default:
		_physics->StopMotion();
		break;
	}
	_physics->Move(_scaleFactor);
	//_model->Move(_physics->GetVelocity() * _physics->GetScaleFactor());
	_model->Move(_physics->GetVelocity() * _scaleFactor);
}

void GameObject::_MoveTo(glm::vec3 vec)
{
	//auto scale_factor = _physics->GetScaleFactor();
	_physics->MoveTo(vec * _scaleFactor);
	_model->MoveTo(vec * _scaleFactor);
}

glm::vec3 GameObject::_GetUpDownDirectionVector()
{
	if (_frameCounter % 60 == 0)
	{
		_frameCounter = 0;
		
		_lastdirection *= -1;
	}
	_frameCounter++;

	return VECTOR_UP * (float)_lastdirection;
}

#endif