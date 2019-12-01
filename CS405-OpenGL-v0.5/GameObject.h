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

	void SetID(int id) { _ID = id; }

	void PrintModelMinMax();
	  
	glm::mat4 GetModelMatrix() { return _model->GetModelMatrix(); }
	void SetModelMatrix(glm::mat4 matrix) { _model->SetModelMatrix(matrix); }

	glm::vec3 GetPosition() { return _rigidBody->GetCenter(); }

	void Print();

	void AccelerateTowards(Directions dir);

	void DoBoundryCollusion();

	void DoCollusion(GameObject *other);

	bool CheckCollusion(GameObject *other);

	void EnableRender() { _renderOn = true; }
	void DisableRender() { _renderOn = false; }
	bool getShouldRender() { return _renderOn; }

private:
	/*  Object Data  */
	Model *_model;
	PhysicEngine *_rigidBody;

	MovementType _movementType;

	ObjectType _objectType;

	int _ID;
	int _lastdirection;
	int _frameCounter;
	float _deltaTime;

	bool _renderOn;

	glm::vec3 _scaleFactor;

	/* Functions */
	void _Move();
	void _MoveTo(glm::vec3 vec);

	glm::vec3 _CalculateUpDownVector();

	glm::vec3 _CalculateRandomVector();
};

GameObject::GameObject(const std::string &path, ObjectType objectType) 
	: _objectType(objectType), _ID(rand() % 1000), _scaleFactor(1.0f), _renderOn(true)
{
	std::cout << "\n~~~~~~~~~ ID : "<< _ID <<"~~~~~~~~~~~~~~ Type:"<< objectType <<"~~~~~~~~~~~~~~~~\n";
	_model = new Model(path);
	std::cout << "Model Matrix \n";
	_model->Print();
	std::cout << "Model inital values:\n";
	std::cout << "Max X:" << _model->GetInitialMax().x << " Y:" << _model->GetInitialMax().y << " Z:" << _model->GetInitialMax().z << std::endl;
	std::cout << "Min X:" << _model->GetInitialMin().x << " Y:" << _model->GetInitialMin().y << " Z:" << _model->GetInitialMin().z << std::endl;
	
	_rigidBody = new PhysicEngine(_model->GetInitialMax(), _model->GetInitialMin());
	std::cout << "Object Cage \n";
	_rigidBody->Print();

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
		_rigidBody->DisableMovement();
		_movementType = MovementType::NONE;
		break;
	case OnScreenPanel:
		_rigidBody->DisableMovement();
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

	_rigidBody->Scale(scale);
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
	_rigidBody->Print();
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
		_rigidBody->AccelerateTowards(VECTOR_UP);
		break;
	case DOWN:
		_rigidBody->AccelerateTowards(-VECTOR_UP);
		break;
	case RIGHT:
		_rigidBody->AccelerateTowards(-VECTOR_LEFT);
		break;
	case LEFT:
		_rigidBody->AccelerateTowards(VECTOR_LEFT);
		break;
	case FORWARD:
		_rigidBody->AccelerateTowards(VECTOR_FORWARD);
		break;
	case BACKWARD:
		_rigidBody->AccelerateTowards(-VECTOR_FORWARD);
		break;
	default:
		break;
	}
}

void GameObject::DoBoundryCollusion()
{
	_rigidBody->DoBoundryCollusion();
	_model->MoveTo(_rigidBody->GetCenter(), _scaleFactor);
	//_MoveTo(_rigidBody->GetCenter());
}

void GameObject::DoCollusion(GameObject *other)
{
	if (this->_ID == other->_ID)
	{
		return;
	}
	//std::cout << "Starting collusion " <<_objectType <<" obj1: " << this->_ID 
	//	<< " &&  " << other->_objectType << " obj2: " << other->_ID << std::endl;

	_rigidBody->DoCollusion(other->_rigidBody);
}

bool GameObject::CheckCollusion(GameObject * other)
{
	bool is_collusion = _rigidBody->CheckCollusion(other->_rigidBody);

	if (is_collusion)
	{
		std::cout << "has collided.\n";
		this->Print();
		other->Print();
	}
	return is_collusion;
}

glm::vec3 GameObject::_CalculateRandomVector()
{
	if (_frameCounter % 30 == 0)
	{
		_frameCounter = 0;
		_lastdirection = rand() % 6;

		_rigidBody->StopMotion();
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
	if (!_rigidBody->CanMove() || _objectType == ObjectType::OnScreenPanel || _objectType == ObjectType::Envoirmental)
	{
		return;
	}
	
	switch (_movementType)
	{
	case Random:
		_rigidBody->AccelerateTowards(_CalculateRandomVector());
		_rigidBody->Apply(_deltaTime);
		break;
	case Normal:
		_rigidBody->Apply(_deltaTime);
		break;
	case UpDown:
		_rigidBody->AccelerateTowards(_CalculateUpDownVector());
		_rigidBody->Apply(_deltaTime);
		break;
	case NONE:
		_rigidBody->StopMotion();
		break;
	default:
		_rigidBody->StopMotion();
		break;
	}
	_rigidBody->Move(_scaleFactor);
	_model->Move(_rigidBody->GetVelocity());
}

void GameObject::_MoveTo(glm::vec3 vec)
{
	_rigidBody->MoveTo(vec * _scaleFactor);
	_model->MoveTo(vec, _scaleFactor);
}

glm::vec3 GameObject::_CalculateUpDownVector()
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