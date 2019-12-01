#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "model.h"
#include "Enums.h"
#include "PhysicsEngine.h"

class GameObject
{
public:
	GameObject(const std::string &path, ObjectType objectType);
	~GameObject();

	void Update(const float & delta_time);

	void Draw(Shader shader);

	void Draw(std::string shader_key);

	void ScaleObject(glm::vec3 scale);

	//void RotateObject(glm::vec3 rotate);

	void PlaceRandomly();

	void SetID(int id);

	void PrintModelMinMax();
	  
	glm::mat4 GetModelMatrix()
	{
		return _model->GetModelMatrix();
	}

	void Print();

	void AccelerateTowards(Directions dir);

	void DoBoundryCollusion();

	void DoCollusion(GameObject *other);

	bool CheckCollusion(GameObject *other);

	void setShouldRender(bool shouldRender);
	bool getShouldRender();

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

	bool _shouldRender;

	glm::vec3 _scaleFactor;

	/* Functions */
	void _Move();
	void _MoveTo(glm::vec3 vec);

	glm::vec3 _GetUpDownDirectionVector();

	glm::vec3 _GetRandomDirectionVector();
};

GameObject::GameObject(const std::string &path, ObjectType objectType) 
	: _objectType(objectType), _ID(rand() % 1000), _scaleFactor(1.0f), _shouldRender(true)
{
	_model = new Model(path);
	//std::cout << "Model Matrix \n";
	//_model->Print();
	
	_physics = new PhysicEngine(_model->GetInitialMax(), _model->GetInitialMin());
	std::cout << "Object Cage \n";
	_physics->Print();

	switch (objectType)
	{
	case Player:
		_movementType = MovementType::Normal;
		break;
	case Enemy:
		_movementType = MovementType::Random;
		break;
	case Coin:
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

void GameObject::Update(const float &delta_time)
{
	this->_deltaTime = delta_time;

	_Move();
}

void GameObject::Draw(Shader shader)
{
	_model->Draw(shader);
}

void GameObject::Draw(std::string shader_key)
{
	ResourceManager::GetShader(shader_key).setMat4("model", _model->GetModelMatrix());

	Draw(ResourceManager::GetShader(shader_key));
}

void GameObject::ScaleObject(glm::vec3 scale)
{
	_scaleFactor = scale;

	_physics->Scale(scale);
	_model->ScaleModel(scale);
}

void GameObject::PlaceRandomly()
{
	auto random_position_vector = Point::getRandomPointVector();

	_MoveTo(glm::vec3(0.0f));

	_MoveTo(random_position_vector);

	std::cout << "\nAfter Random Placement\n~~~~~~~~~~~~~~~~~~" << std::endl;
	Print();
	//PrintModelMinMax();
}

void GameObject::SetID(int id)
{
	_ID = id;
}

void GameObject::PrintModelMinMax()
{
	std::cout << "Object " << _ID << " Model Matrix ~~~~~" << std::endl;
	std::cout
		<< "Model max x: " << _model->GetInitialMax().x
		<< " Model max y: " << _model->GetInitialMax().y
		<< " Model max z: " << _model->GetInitialMax().z << std::endl
		<< "Model min x: " << _model->GetInitialMin().x
		<< " Model min y: " << _model->GetInitialMin().y
		<< " Model min z: " << _model->GetInitialMin().z << std::endl << std::endl;
}

void GameObject::Print()
{
	std::cout << "Object " << _ID << " is at~~" << std::endl;
	_physics->Print();
	_model->Print();
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
	case RIGHT:
		_physics->AccelerateTowards(-VECTOR_LEFT);
		break;
	case LEFT:
		_physics->AccelerateTowards(VECTOR_LEFT);
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

void GameObject::DoBoundryCollusion()
{
	_physics->DoBoundryCollusion();
	_model->MoveTo(_physics->GetCenter(), _scaleFactor);
	//_MoveTo(_physics->GetCenter());
}

void GameObject::DoCollusion(GameObject *other)
{
	if (this->_ID == other->_ID)
	{
		return;
	}
	//std::cout << "Starting collusion " <<_objectType <<" obj1: " << this->_ID 
	//	<< " &&  " << other->_objectType << " obj2: " << other->_ID << std::endl;

	_physics->DoCollusion(other->_physics);
}

bool GameObject::CheckCollusion(GameObject * other)
{
	bool is_collusion = _physics->CheckCollusion(other->_physics);

	if (is_collusion)
	{
		std::cout << "has collided.\n";
		this->Print();
		other->Print();
	}
	return is_collusion;
}

void GameObject::setShouldRender(bool shouldRender)
{
	this->_shouldRender = shouldRender;
}

bool GameObject::getShouldRender()
{
	return _shouldRender;
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
	case 2://Directions::LEFT:
		return VECTOR_LEFT;
	case 3://Directions::RIGHT:
		return -VECTOR_LEFT;
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
	_model->Move(_physics->GetVelocity() * _scaleFactor);
}

void GameObject::_MoveTo(glm::vec3 vec)
{
	_physics->MoveTo(vec * _scaleFactor);
	_model->MoveTo(vec * _scaleFactor, _scaleFactor);
}

glm::vec3 GameObject::_GetUpDownDirectionVector()
{
	if (_frameCounter % 30 == 0)
	{
		_frameCounter = 0;
		
		_lastdirection *= -1;
	}
	_frameCounter++;

	return VECTOR_UP * (float)_lastdirection * 0.5f;
}

#endif