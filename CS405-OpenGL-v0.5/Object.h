#ifndef OBJECT_H
#define OBJECT_H

#include "Enums.h"
#include "values.h"

#include "CollusionAlgo.h"


static BoxAABB gameBoundry = BoxAABB(Point(GAMEBOUNDRY_X, GAMEBOUNDRY_Y, GAMEBOUNDRY_Z), 
									 Point(-GAMEBOUNDRY_X, -GAMEBOUNDRY_Y, -GAMEBOUNDRY_Z));

class Object
{
public:
	Object(Point max, Point min) 
		: frameCounter(0), movementType(MovementType::Normal), lastFrame(0.0f)//, physics()
	{
		box.max = max;
		box.min = min;
		
		physics = new PhysicEngine();
	}
	
	void init();

	void update(float currentFrame);

	bool isMoving();

	void move();
	void turnTowards(Directions dir);
	void moveTo(glm::vec3 point);

	BoxAABB getCollusionBox();
	void setCollusionBox(BoxAABB box);

	void setupCollusionBox(glm::vec3 vector);

	void printBox();

	void doCollusion(Object & other);

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

private:
	BoxAABB box;

	PhysicEngine *physics;

	glm::mat4 _modelMatrix;

	float deltaTime;
	float lastFrame;

	int randDirection;
	int frameCounter;

	MovementType movementType;

	void _moveRandom();
	void _moveModelTo(const glm::vec3 &vector);
	void _moveModel(const glm::vec3 &vector);

	void _checkBoxBoundry();

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
	physics->setScaleFactor(physics->getScaleFactor()/scale);

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
		//physics->accelerateTowards(glm::vec3(0.0f, ACCELERATE_RATE, 0.0f));
		physics->setAcceleration(glm::vec3(0.0f, ACCELERATE_RATE, 0.0f));
		break;
	case Directions::DOWN:
		//physics->accelerateTowards(glm::vec3(0.0f, -ACCELERATE_RATE, 0.0f));
		physics->setAcceleration(glm::vec3(0.0f, -ACCELERATE_RATE, 0.0f));
		break;
	case Directions::LEFT:
		//physics->accelerateTowards(glm::vec3(ACCELERATE_RATE, 0.0f, 0.0f));
		physics->setAcceleration(glm::vec3(ACCELERATE_RATE, 0.0f, 0.0f));
		break;
	case Directions::RIGHT:
		//physics->accelerateTowards(glm::vec3(-ACCELERATE_RATE, 0.0f, 0.0f));
		physics->setAcceleration(glm::vec3(-ACCELERATE_RATE, 0.0f, 0.0f));
		break;
	case Directions::FORWARD:
		//physics->accelerateTowards(glm::vec3(0.0f, 0.0f, ACCELERATE_RATE));
		physics->setAcceleration(glm::vec3(0.0f, 0.0f, ACCELERATE_RATE));
		break;
	case Directions::BACKWARD:
		//physics->accelerateTowards(glm::vec3(0.0f, 0.0f, -ACCELERATE_RATE));
		physics->setAcceleration(glm::vec3(0.0f, 0.0f, -ACCELERATE_RATE));
		break;
	default:
		break;
	}
	//vel += acc;
	physics->setVelocity(physics->getVelocity() + physics->getAcceleration());
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
	
	box.move(physics->getAcceleration());

	if (movementType == MovementType::Normal)
	{
		physics->applyMotion(physics->getAcceleration());

		_moveModel(physics->getAcceleration());
	}
	else if (movementType == MovementType::Random)
	{
		_moveRandom();
		_moveModel(physics->getAcceleration());
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

void Object::doCollusion(Object &other)
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

	if (CollusionAlgo::checkCollusionAABB(this->box, other.box))
	{
		std::cout << "Collusion Detected AABB Box" << std::endl;
		box.print();
		other.box.print();

		_solveCollusionBox(other);
	}
}

glm::vec3 Object::getCenter()
{
	return physics->getCenter();
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
	physics->stopMotion();
}

bool Object::canMove()
{
	return physics->canMove();
}

void Object::setMoveablity(bool mobility)
{
	physics->setMoveablity(mobility);
}

glm::vec3 Object::getVelocity()
{
	return physics->getVelocity();
}

void Object::setVelocity(glm::vec3 velocity)
{
	if (!this->canMove())
	{
		physics->stopMotion();
		return;
	}
	physics->setVelocity(velocity);
}

bool Object::isMoving()
{
	return physics->isMoving();
}

glm::vec3 Object::getAcc()
{
	return physics->getAcceleration();
}

void Object::setAcc(glm::vec3 acc)
{
	physics->setAcceleration(acc);
}

void Object::init()
{
	setupCenter();
}

void Object::update(float currentFrame)
{
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;


	if (!canMove())
	{
		physics->stopMotion();
		return;
	}

	physics->applyPhysics(deltaTime);
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
	glm::vec3 pos;
	pos.x = (box.max.x + box.min.x) / 2;
	pos.y = (box.max.y + box.min.y) / 2;
	pos.z = (box.max.z + box.min.z) / 2;
	physics->setCenter(pos);
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
		physics->accelerateTowards(glm::vec3(0.0f, ACCELERATE_RATE, 0.0f));
		break;
	case 1://Directions::DOWN:
		physics->accelerateTowards(glm::vec3(0.0f, -ACCELERATE_RATE, 0.0f));
		break;
	case 2://Directions::LEFT:
		physics->accelerateTowards(glm::vec3(ACCELERATE_RATE, 0.0f, 0.0f));
		break;
	case 3://Directions::RIGHT:
		physics->accelerateTowards(glm::vec3(-ACCELERATE_RATE, 0.0f, 0.0f));
		break;
	case 4://Directions::FORWARD:
		physics->accelerateTowards(glm::vec3(0.0f, 0.0f, ACCELERATE_RATE));
		break;
	case 5://Directions::BACKWARD:
		physics->accelerateTowards(glm::vec3(0.0f, 0.0f, -ACCELERATE_RATE));
		break;
	default:
		break;
	}
}

inline void Object::_moveModel(const glm::vec3 &vector)
{
	_modelMatrix = glm::translate(_modelMatrix, vector * physics->getScaleFactor());
}

inline void Object::_moveModelTo(const glm::vec3 & vector)
{
	_modelMatrix = glm::translate(glm::mat4(1.0f), vector * physics->getScaleFactor());
}


//inline void Object::_applyFriction(int accPos, int velPos, const glm::vec3 &friction)
//{
	//vel[velPos] > 0 ? acc -= friction : acc += friction;

	//if (acc[accPos] < 0 && acc[accPos] < -FRICTION_LIMIT)
	//{
	//	acc[accPos] = -FRICTION_LIMIT;
	//}
	//else if (acc[accPos] > 0 && acc[accPos] > FRICTION_LIMIT)
	//{
	//	acc[accPos] = FRICTION_LIMIT;
	//}
//}
	
//void Object::_setAndApplyFriction()
//{
	//if (vel.x != 0)
	//{
	//	auto friction = glm::vec3(FRICTION_COEFFICIENT * deltaTime, 0.0f, 0.0f);

	//	_applyFriction(0, 0, friction);
	//}
	//if (vel.y != 0)
	//{
	//	auto friction = glm::vec3(0.0f, FRICTION_COEFFICIENT * deltaTime, 0.0f);

	//	_applyFriction(1, 1, friction);
	//}
	//if (vel.z != 0)
	//{
	//	auto friction = glm::vec3(0.0f, 0.0f, FRICTION_COEFFICIENT * deltaTime);

	//	_applyFriction(2, 2, friction);
	//}
//}
	
//inline void Object::_adjustVelocityByLimit(int pos)
//{
//	if (vel[pos] > SPEED_LIMIT)
//	{
//		vel[pos] = SPEED_LIMIT;
//	}
//	else if (vel[pos]< -SPEED_LIMIT)
//	{
//		vel[pos] = -SPEED_LIMIT;
//	}
//}
//
//inline void Object::_checkVelocityLimit()
//{
//	vel += acc;
//
//	_adjustVelocityByLimit(0);
//	_adjustVelocityByLimit(1);
//	_adjustVelocityByLimit(2);
//}
//
//inline void Object::_stopVelocityIfRequired(int pos)
//{
//	if ((vel[pos]< STOP_SPEED && vel[pos] >= 0) || (vel[pos] > -STOP_SPEED && vel[pos] <= 0))
//	{
//		vel[pos] = 0.0;
//		acc[pos]= 0.0;
//	}
//}
//
//inline void Object::_adjustVelocity()
//{
//	_checkVelocityLimit();
//
//	_stopVelocityIfRequired(0);
//	_stopVelocityIfRequired(1);
//	_stopVelocityIfRequired(2);
//}

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
		physics->setVelocity(VECTOR_Y_POSITIVE);
	}
	else
	{
		physics->reverseMotion();
		//vel = -vel;
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
				physics->setVelocity(push_vec);
				//this->vel = push_vec;
			}
			if (other.canMove())
			{
				other.physics->setVelocity(-push_vec);
				//other.vel = - push_vec;
			}
		}
		else
		{
			physics->reverseMotion();
			other.physics->reverseMotion();
			//this->vel = -this->vel;
			//other.vel = -other.vel;
		}
	}
}

#endif