#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Enums.h"
#include "values.h"

#include "CollusionAlgo.h"


static BoxAABB gameBoundry = BoxAABB(Point(GAMEBOUNDRY_X, GAMEBOUNDRY_Y, GAMEBOUNDRY_Z), 
									 Point(-GAMEBOUNDRY_X, -GAMEBOUNDRY_Y, -GAMEBOUNDRY_Z));

class Object
{
public:
	Object() : isMoveable(true), frameCounter(0), vel(VECTOR_ZERO), _scaleFactor(glm::vec3(1.0f)),
		movementType(MovementType::Normal), lastFrame(0.0f)
	{}
	
	void update(float currentFrame);

	bool isMoving();

	void move();
	void turnTowards(Directions dir);
	void moveTo(glm::vec3 point);

	BoxAABB getCollusionBox();
	void setCollusionBox(BoxAABB box);

	void setupCollusionBox(glm::vec3 vector);

	void printBox();

	void doCollusion(Object & other, CollusionDetectionType type);

	glm::vec3 getCenter();
	void placeTo(glm::vec3 pos);
	void placeRandomly();

	void stopMovement();
	bool canMove();
	void setMoveablity(bool mobility);

	glm::vec3 getVelocity();
	void setVelocity(glm::vec3 velocity);

	glm::vec3 getAcc();
	void setAcc(glm::vec3 acc);

	void scale(glm::vec3 scale);

	MovementType getMovementType();
	void setMovementType(MovementType movementType);

	glm::mat4 getModelMatrix();

	void setupCenter();

	void tempMove(glm::vec3 vec);

private:
	BoxAABB box;

	glm::mat4 _modelMatrix;

	float deltaTime;
	float lastFrame;

	int randDirection;
	int frameCounter;

	MovementType movementType;

	bool isMoveable;


	//////////////////////
	glm::vec3 upVector;
	glm::vec3 rightVector;
	glm::vec3 frontVector;

	glm::vec3 position;
	//////////////////////

	glm::vec3 _scaleFactor;

	glm::vec3 vel;
	glm::vec3 acc;


	void _moveRandom();
	void _moveModelTo(const glm::vec3 &vector);
	void _moveModel(const glm::vec3 &vector);

	
	void _applyFriction(int accPos, int velPos, const glm::vec3 & friction);
	void _setAndApplyFriction();
	void _adjustVelocityByLimit(int pos);
	void _checkVelocityLimit();
	void _stopVelocityIfRequired(int pos);
	void _adjustVelocity();

	void _checkBoxBoundry();

	//bool _checkCollusionAABB(const Object &other);
	
	void _solveSurfaceCollusion();
	bool _checkIsInGameField();
	void _surfaceBounce();
	bool _isCommonCollusionSolutionEnough(Object & other);
	void _solveCollusionBox(Object & other);
};

BoxAABB Object::getCollusionBox()
{
	return this->box;
}

void Object::setCollusionBox(BoxAABB box)
{
	this->box = box;
}

void Object::scale(glm::vec3 scale)
{
	_scaleFactor /= scale;

	double depth = box.max.x - box.min.x;
	double height = box.max.x - box.min.y;
	double width = box.max.x - box.min.z;

	double diff_depth  = (depth  * scale.x) - depth;
	double diff_height = (height * scale.y) - height;
	double diff_width  = (width  * scale.z) - width;

	box.max.x += diff_depth / 2;
	box.min.x -= diff_depth / 2;

	box.max.z += diff_width / 2;
	box.min.z -= diff_width / 2;

	box.max.y += diff_height / 2;
	box.min.y -= diff_height / 2;

	_checkBoxBoundry();


	_modelMatrix = glm::scale(_modelMatrix, scale);
}

void Object::turnTowards(Directions dir)
{
	switch (dir)
	{
	case Directions::UP:
		acc += glm::vec3(0.0f, ACCELERATE_RATE, 0.0f);
		break;
	case Directions::DOWN:
		acc += glm::vec3(0.0f, -ACCELERATE_RATE, 0.0f);
		break;
	case Directions::LEFT:
		acc += glm::vec3(ACCELERATE_RATE, 0.0f, 0.0f);
		break;
	case Directions::RIGHT:
		acc += glm::vec3(-ACCELERATE_RATE, 0.0f, 0.0f);
		break;
	case Directions::FORWARD:
		acc += glm::vec3(0.0f, 0.0f, ACCELERATE_RATE);
		break;
	case Directions::BACKWARD:
		acc += glm::vec3(0.0f, 0.0f, -ACCELERATE_RATE);
		break;
	default:
		break;
	}
	vel += acc;
}

void Object::moveTo(glm::vec3 point)
{
	_checkBoxBoundry();

	double distx = box.max.x - box.min.x;
	box.max.x = point.x + distx / 2;
	box.min.x = point.x - distx / 2;

	double disty = box.max.y - box.min.y;
	box.max.y = point.y + disty / 2;
	box.min.y = point.y - disty / 2;

	double distz = box.max.z - box.min.z;
	box.max.z = point.z + distz / 2;
	box.min.z = point.z - distz / 2;
}

void Object::move()
{
	if (! canMove())
	{
		return;
	}
	
	box.move(acc);

	if (movementType == MovementType::Normal)
	{
		position += acc;

		_moveModel(acc);
	}
	else if (movementType == MovementType::Random)
	{
		_moveRandom();
		_moveModel(acc);
	}
}

void Object::setupCollusionBox(glm::vec3 vector)
{
	if (box.min.x > vector.x) { box.min.x = vector.x; }
	if (box.min.y > vector.y) { box.min.y = vector.x; }
	if (box.min.z > vector.z) { box.min.z = vector.z; }

	if (box.max.x < vector.x) { box.max.x = vector.x; }
	if (box.max.y < vector.y) { box.max.y = vector.x; }
	if (box.max.z < vector.z) { box.max.z = vector.z; }
}

void Object::printBox()
{
	box.print();
}

void Object::doCollusion(Object &other, CollusionDetectionType type)
{

	if (! _checkIsInGameField())
	{
		// lhs object is not within game field.
		std::cout << "Object1 is not in gamefield. Returning them to the mother base. Over." << std::endl;
		_solveSurfaceCollusion();
	}
	if(! other._checkIsInGameField())
	{
		// rhs object is not within game field.
		std::cout << "Object2 is not in gamefield. Returning them to the mother base. Over." << std::endl;
		_solveSurfaceCollusion();
	}

	switch (type)
	{
	case CollusionSimple:
		//if (CollusionAlgo::checkCollusionSimple(this->box, other.box))
		//{
		//	std::cout << "Collusion Detected simple Box" << std::endl;
		//	box.print();
		//	other.box.print();

		//	_solveCollusionBox(other);
		//}
		break;
	case CollusionAABB:
		//std::cout << "AABB Box" << std::endl;
		//if (_checkCollusionAABB(other))
		if (CollusionAlgo::checkCollusionAABB(this->box, other.box))
		{
			std::cout << "Collusion Detected AABB Box" << std::endl;
			box.print();
			other.box.print();

			_solveCollusionBox(other);
		}
		break;
	default:
		break;
	}
}

glm::vec3 Object::getCenter()
{
	return position;
}

void Object::placeTo(glm::vec3 pos)
{
	moveTo(pos);
	_moveModel(pos);
}

void Object::placeRandomly()
{
	double randx = rand() % 7;
	double randy = rand() % 7;
	double randz = rand() % 7;

	int randsignx = rand() % 2;
	int randsignz = rand() % 2;

	if (randsignx == 0) randx *= -1.0;
	if (randsignz == 0) randz *= -1.0;

	auto pos = glm::vec3(randx, randy, randz);

	moveTo(pos);

	this->_moveModelTo(pos);
}

void Object::stopMovement()
{
	this->vel = VECTOR_ZERO;
}

bool Object::canMove()
{
	return this->isMoveable;
}

void Object::setMoveablity(bool mobility)
{
	this->isMoveable = mobility;
}

glm::vec3 Object::getVelocity()
{
	return this->vel;
}

void Object::setVelocity(glm::vec3 velocity)
{
	if (!this->canMove())
	{
		vel = VECTOR_ZERO;
		acc = VECTOR_ZERO;
		return;
	}
	this->vel = velocity;
}

bool Object::isMoving()
{
	return vel != VECTOR_ZERO;
}

glm::vec3 Object::getAcc()
{
	return this->acc;
}

void Object::setAcc(glm::vec3 acc)
{
	this->acc = acc;
}

void Object::update(float currentFrame)
{
	//float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;


	if (!canMove())
	{
		// Stop motion. object is not able to move.
		vel = VECTOR_ZERO;
		acc = VECTOR_ZERO;
		return;
	}

	_setAndApplyFriction();
	_adjustVelocity();
	move();
}

MovementType Object::getMovementType()
{
	return movementType;
}

void Object::setMovementType(MovementType movementType)
{
	this->movementType = movementType;
}

inline glm::mat4 Object::getModelMatrix()
{
	return _modelMatrix;
}

void Object::setupCenter()
{
	position.x = (box.max.x + box.min.x) / 2;
	position.y = (box.max.y + box.min.y) / 2;
	position.z = (box.max.z + box.min.z) / 2;
}

void Object::tempMove(glm::vec3 vec)
{
	acc = vec;
	_moveModel(vec);
}

void Object::_moveRandom()
{
	if (frameCounter % 30 == 0)
	{
		frameCounter = 0;
		randDirection = rand() % 6;
	}
	frameCounter++;

	switch (randDirection)
	{
	case 0://Directions::UP:
		acc += glm::vec3(0.0f, ACCELERATE_RATE, 0.0f);
		break;
	case 1://Directions::DOWN:
		acc += glm::vec3(0.0f, -ACCELERATE_RATE, 0.0f);
		break;
	case 2://Directions::LEFT:
		acc += glm::vec3(ACCELERATE_RATE, 0.0f, 0.0f);
		break;
	case 3://Directions::RIGHT:
		acc += glm::vec3(-ACCELERATE_RATE, 0.0f, 0.0f);
		break;
	case 4://Directions::FORWARD:
		acc += glm::vec3(0.0f, 0.0f, ACCELERATE_RATE);
		break;
	case 5://Directions::BACKWARD:
		acc += glm::vec3(0.0f, 0.0f, -ACCELERATE_RATE);
		break;
	default:
		break;
	}
}

inline void Object::_moveModel(const glm::vec3 &vector)
{
	_modelMatrix = glm::translate(_modelMatrix, vector * _scaleFactor);
}

inline void Object::_moveModelTo(const glm::vec3 & vector)
{
	_modelMatrix = glm::translate(glm::mat4(1.0f), vector * _scaleFactor);
}


inline void Object::_applyFriction(int accPos, int velPos, const glm::vec3 &friction)
{
	vel[velPos] > 0 ? acc -= friction : acc += friction;

	if (acc[accPos] < 0 && acc[accPos] < -FRICTION_LIMIT)
	{
		acc[accPos] = -FRICTION_LIMIT;
	}
	else if (acc[accPos] > 0 && acc[accPos] > FRICTION_LIMIT)
	{
		acc[accPos] = FRICTION_LIMIT;
	}
}
	
void Object::_setAndApplyFriction()
{
	if (vel.x != 0)
	{
		auto friction = glm::vec3(FRICTION_COEFFICIENT * deltaTime, 0.0f, 0.0f);

		_applyFriction(0, 0, friction);
	}
	if (vel.y != 0)
	{
		auto friction = glm::vec3(0.0f, FRICTION_COEFFICIENT * deltaTime, 0.0f);

		_applyFriction(1, 1, friction);
	}
	if (vel.z != 0)
	{
		auto friction = glm::vec3(0.0f, 0.0f, FRICTION_COEFFICIENT * deltaTime);

		_applyFriction(2, 2, friction);
	}
}
	
inline void Object::_adjustVelocityByLimit(int pos)
{
	if (vel[pos] > SPEED_LIMIT)
	{
		vel[pos] = SPEED_LIMIT;
	}
	else if (vel[pos]< -SPEED_LIMIT)
	{
		vel[pos] = -SPEED_LIMIT;
	}
}

inline void Object::_checkVelocityLimit()
{
	vel += acc;

	_adjustVelocityByLimit(0);
	_adjustVelocityByLimit(1);
	_adjustVelocityByLimit(2);
}

inline void Object::_stopVelocityIfRequired(int pos)
{
	if ((vel[pos]< STOP_SPEED && vel[pos] >= 0) || (vel[pos] > -STOP_SPEED && vel[pos] <= 0))
	{
		vel[pos] = 0.0;
		acc[pos]= 0.0;
	}
}

inline void Object::_adjustVelocity()
{
	_checkVelocityLimit();

	_stopVelocityIfRequired(0);
	_stopVelocityIfRequired(1);
	_stopVelocityIfRequired(2);
}

inline void Object::_checkBoxBoundry()
{
	if (box.max.x < box.min.x)
	{
		std::swap(box.max.x, box.min.x);
	}
	if (box.max.y < box.min.y)
	{
		std::swap(box.max.y, box.min.y);
	}
	if (box.max.z < box.min.z)
	{
		std::swap(box.max.z, box.min.z);
	}
}

//inline bool Object::_checkCollusionAABB(const Object & other)
//{
//	return (box.intersect(other.box) || other.box.intersect(box));
//}

void Object::_solveSurfaceCollusion()
{
	if (!gameBoundry.isPointInsideAABB(box.max))
	{
		auto posx = box.max.x <= gameBoundry.max.x ? box.max.x : gameBoundry.max.x - box.max.x;
		auto posy = box.max.y <= gameBoundry.max.y ? box.max.y : gameBoundry.max.y - box.max.y;
		auto posz = box.max.z <= gameBoundry.max.z ? box.max.z : gameBoundry.max.z - box.max.z;

		moveTo(glm::vec3(posx, posy, posz));
	}
	if (! gameBoundry.isPointInsideAABB(box.min))
	{
		auto posx = box.min.x >= gameBoundry.min.x ? box.min.x : gameBoundry.min.x + box.min.x;
		auto posy = box.min.y >= gameBoundry.min.y ? box.min.y : gameBoundry.min.y + box.min.y;
		auto posz = box.min.z >= gameBoundry.min.z ? box.min.z : gameBoundry.min.z + box.min.z;

		moveTo(glm::vec3(posx, posy, posz));
	}
}

bool Object::_checkIsInGameField()
{
	if (gameBoundry.isPointInsideAABB(box.max) && gameBoundry.isPointInsideAABB(box.min))
	{
		return true;
	}
	return false;
}

inline void Object::_surfaceBounce()
{
	if (!isMoving())
	{
		vel = VECTOR_Y_POSITIVE;
	}
	else
	{
		vel = -vel;
	}
}

bool Object::_isCommonCollusionSolutionEnough(Object &other)
{
	if (!this->canMove() && !other.canMove())
	{
		std::cout << "Neither can move.";
		// Error case! Problem occoured while loading.
		// TODO: IMPLEMENT A LOGIC FOR THIS CASE
		return true;
	}
	if (!this->canMove())
	{
		std::cout << "This cannot move.";
		other._surfaceBounce();
		return true;
	}
	else if (!other.canMove())
	{
		std::cout << "Other cannot move.";
		this->_surfaceBounce();
		return true;
	}
	return false;
}
	
void Object::_solveCollusionBox(Object &other)
{
	if (!_isCommonCollusionSolutionEnough(other))
	{
		if (! this->isMoving() && ! other.isMoving())
		{
			auto Tx = box.max.x - box.min.x;
			auto Ty = box.max.y - box.min.y;
			auto Tz = box.max.z - box.min.z;

			auto Ox = other.box.max.x - other.box.min.x;
			auto Oy = other.box.max.y - other.box.min.y;
			auto Oz = other.box.max.z - other.box.min.z;
			
			auto push_vec = glm::vec3(Ox - Tx, Oy - Ty, Oz - Tz);
				
			if (this->canMove())
			{
				this->vel = push_vec;
			}
			if (other.canMove())
			{
				other.vel = - push_vec;
			}
		}
		else
		{
			this->vel = -this->vel;
			other.vel = -other.vel;
		}
	}
}

#endif