#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "model.h"
#include "Enums.h"
#include "PhysicsEngine.h"
#include "Transform.h"
#include "Collider.h"

class GameObject
{
public:
	Transform transform;
	Model *model;
	PhysicEngine *physics;
	Collider *collider;

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

	glm::vec3 GetPosition() { return collider->GetCenter(); }

	void PrintObject();

	void AccelerateTowards(Directions dir);

	void DoBoundryCollusion();

	void DoCollusion(GameObject *other);

	bool CheckCollusion(GameObject *other);

	void EnableRender() { _renderOn = true; }
	void DisableRender() { _renderOn = false; }
	bool ShouldRender() { return _renderOn && !_isAlive; }

private:
	/*  Object Data  */
	MovementType _movementType;

	ObjectType _objectType;

	int _ID;
	int _lastdirection;
	int _frameCounter;
	float _deltaTime;

	bool _renderOn;
	bool _isAlive;

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
	model = new Model(path);
	std::cout << "Model Matrix \n";
	model->PrintModel();
	std::cout << "Model inital values:\n";
	std::cout << "Max X:" << model->GetInitialMax().x << " Y:" << model->GetInitialMax().y << " Z:" << model->GetInitialMax().z << std::endl;
	std::cout << "Min X:" << model->GetInitialMin().x << " Y:" << model->GetInitialMin().y << " Z:" << model->GetInitialMin().z << std::endl;
	
	physics = new PhysicEngine();

	collider = new Collider(model->GetInitialMax(), model->GetInitialMin());
	std::cout << "Object Cage \n";
	physics->PrintPhysics();

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
		physics->DisableMovement();
		_movementType = MovementType::NONE;
		break;
	case OnScreenPanel:
		physics->DisableMovement();
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
	_deltaTime = delta_time;
	if (ShouldRender()) {
		_Move();
	}
}

void GameObject::Draw(Shader shader)
{
	if (ShouldRender()) {
		model->Draw(shader);
	}
}

void GameObject::Draw(std::string shader_key)
{
	ResourceManager::GetShader(shader_key).setMat4("model", model->GetModelMatrix());

	Draw(ResourceManager::GetShader(shader_key));
}

void GameObject::ScaleObject(glm::vec3 scale)
{
	_scaleFactor = scale;
	
	collider->ScaleCollider(scale);
	model->ScaleModel(scale);
}

void GameObject::PlaceRandomly()
{
	auto random_position_vector = Point::getRandomPointVector();

	_MoveTo(glm::vec3(0.0f));

	_MoveTo(random_position_vector);

	std::cout << "\nAfter Random Placement\n~~~~~~~~~~~~~~~~~~" << std::endl;
	PrintObject();
	//PrintModelMinMax();
}

void GameObject::PrintModelMinMax()
{
	std::cout << "Object " << _ID << " Model Matrix ~~~~~" << std::endl;
	std::cout
		<< "Model max x: " << model->GetInitialMax().x
		<< " Model max y: " << model->GetInitialMax().y
		<< " Model max z: " << model->GetInitialMax().z << std::endl
		<< "Model min x: " << model->GetInitialMin().x
		<< " Model min y: " << model->GetInitialMin().y
		<< " Model min z: " << model->GetInitialMin().z << std::endl << std::endl;
}

void GameObject::PrintObject()
{
	std::cout << "Object " << _ID << " is at~~" << std::endl;
	physics->PrintPhysics();
	collider->PrintCollider();
	model->PrintModel();
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
		physics->AccelerateTowards(VECTOR_UP);
		break;
	case DOWN:
		physics->AccelerateTowards(-VECTOR_UP);
		break;
	case RIGHT:
		physics->AccelerateTowards(-VECTOR_LEFT);
		break;
	case LEFT:
		physics->AccelerateTowards(VECTOR_LEFT);
		break;
	case FORWARD:
		physics->AccelerateTowards(VECTOR_FORWARD);
		break;
	case BACKWARD:
		physics->AccelerateTowards(-VECTOR_FORWARD);
		break;
	default:
		break;
	}
}

void GameObject::DoBoundryCollusion()
{
	collider->DoBoundryCollusion();
	model->MoveModelTo(collider->GetCenter(), _scaleFactor);
	//_MoveTo(physics->GetCenter());
}

void GameObject::DoCollusion(GameObject *other)
{
	if (this->_ID == other->_ID)
	{
		return;
	}
	//std::cout << "Starting collusion " <<_objectType <<" obj1: " << this->_ID 
	//	<< " &&  " << other->_objectType << " obj2: " << other->_ID << std::endl;

	collider->DoCollusion(other->collider);
}

bool GameObject::CheckCollusion(GameObject * other)
{
	bool is_collusion = collider->CheckCollusion(other->collider);

	if (is_collusion)
	{
		std::cout << "has collided.\n";
		this->PrintObject();
		other->PrintObject();
	}
	return is_collusion;
}

glm::vec3 GameObject::_CalculateRandomVector()
{
	if (_frameCounter % 30 == 0)
	{
		_frameCounter = 0;
		_lastdirection = rand() % 6;

		physics->StopMotion();
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
	if (!physics->CanMove() || _objectType == ObjectType::OnScreenPanel || _objectType == ObjectType::Envoirmental)
	{
		return;
	}
	
	//auto dt_distance = VECTOR_ZERO;
	glm::vec3 dt_distance;
	switch (_movementType)
	{
	case Random:
		physics->AccelerateTowards(_CalculateRandomVector());
		dt_distance = physics->Apply(_deltaTime);
		break;
	case Normal:
		dt_distance = physics->Apply(_deltaTime);
		break;
	case UpDown:
		physics->AccelerateTowards(_CalculateUpDownVector());
		dt_distance = physics->Apply(_deltaTime);
		break;
	case NONE:
		physics->StopMotion();
		break;
	default:
		physics->StopMotion();
		break;
	}

	collider->MoveCollider(dt_distance);
	model->MoveModel(dt_distance);
}

void GameObject::_MoveTo(glm::vec3 vec)
{
	collider->MoveColliderTo(vec * _scaleFactor);
	model->MoveModelTo(vec, _scaleFactor);
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