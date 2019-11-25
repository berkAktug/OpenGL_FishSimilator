#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Enums.h"
#include "values.h"

#include "CollusionAlgo.h"


class GameObject
{
public:
	GameObject() : isMoveable(true), frameCounter(0), vel(VECTOR_ZERO), movementType(MovementType::Normal), lastFrame(0.0f)
	{}

	BoxAABB getCollusionBox()
	{
		return this->box;
	}

	void setCollusionBox(BoxAABB box)
	{
		this->box = box;
	}

	void scale(glm::vec3 scale)
	{
		scaleFactor = scale;

		double depth  = box.max.x - box.min.x;
		double height = box.max.x - box.min.y;
		double width  = box.max.x - box.min.z;
		
		double diff_depth  = (depth  * scale.x) - depth;
		double diff_height = (height * scale.y) - height;
		double diff_width  = (width  * scale.z) - width;

		box.max.x += diff_depth / 2;
		box.min.x -= diff_depth / 2;

		box.max.z += diff_width / 2;
		box.min.z -= diff_width / 2;

		box.max.y += diff_height / 2;
		box.min.y -= diff_height / 2;

		setupCollusionSphere();
	}

	void move(Directions dir)
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
	}

#pragma optimize( "", off )
	void move()
	{
		if (getMoveablity())
		{
			box.max.x += vel.x;
			box.min.x += vel.x;

			//if (box.min.y < 0.0)
			//{
			//	vel.y = -box.min.y;
			//}
			//else if(box.min.y + vel.y < 0.0)
			//{
			//	vel.y = 0;
			//}
			box.max.y += vel.y;
			box.min.y += vel.y;

			box.max.z += vel.z;
			box.min.z += vel.z;

		}
	}
#pragma optimize( "", on)
	void setupCollusionSphere()
	{
		_setupCenter();
	}

	void setupCollusionBox(glm::vec3 vector)
	{
		if (box.min.x > vector.x) { box.min.x = vector.x; }
		if (box.min.y > vector.y) { box.min.y = vector.x; }
		if (box.min.z > vector.z) { box.min.z = vector.z; }

		if (box.max.x < vector.x) { box.max.x = vector.x; }
		if (box.max.y < vector.y) { box.max.y = vector.x; }
		if (box.max.z < vector.z) { box.max.z = vector.z; }
	}

	void printBox() 
	{
		box.print();
	}
		   
	void doCollusion(GameObject &other, CollusionDetectionType type)
	{
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

	Point getCenter()
	{
		return center;
	}

	Point getRandomPoint()
	{
		double randx = rand() % 7;
		double randy = rand() % 7;
		double randz = rand() % 7;

		int randsignx = rand() % 2;
		int randsignz = rand() % 2;

		if (randsignx == 0) randx *= -1.0;
		if (randsignz == 0) randz *= -1.0;

		return Point(randx, randy, randz);
	}
	
	glm::vec3 setFloor()
	{
		setMoveablity(false);

		box.max.y =  FLOOR_DIMENSION_Y;
		box.min.y = -FLOOR_DIMENSION_Y;

		box.max.z =  FLOOR_DIMENSION_Z;
		box.min.z = -FLOOR_DIMENSION_Z;

		box.max.x =  FLOOR_DIMENSION_X;
		box.min.x = -FLOOR_DIMENSION_X;

		setupCollusionSphere();

		return glm::vec3(FLOOR_DIMENSION_X, FLOOR_DIMENSION_Y, FLOOR_DIMENSION_Z);
	}
	
	void stopMovement()
	{
		this->vel = VECTOR_ZERO;
	}

	bool getMoveablity()
	{
		return this->isMoveable;
	}

	void setMoveablity(bool mobility)
	{
		this->isMoveable = mobility;
	}

	glm::vec3 getVelocity()
	{
		return this->vel;
	}

	void setVelocity(glm::vec3 velocity)
	{
		if (!this->getMoveablity())
		{
			vel = VECTOR_ZERO;
			acc = VECTOR_ZERO;
			return;
		}
		this->vel = velocity;
	}

	bool isMoving()
	{
		return vel != VECTOR_ZERO;
	}

	glm::vec3 getAcc()
	{
		return this->acc;
	}

	void setAcc(glm::vec3 acc)
	{
		this->acc = acc;
	}

	void update(float currentFrame)
	{
		//float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		if (!getMoveablity())
		{
			// Do nothing. object is not able to move.
			vel = VECTOR_ZERO;
			acc = VECTOR_ZERO;
		}
		else
		{
			if (getMovementType() == MovementType::Random)
			{
				_moveRandom();
			}

			_setAndApplyFriction();
			_adjustVelocity();
		}
	}

	MovementType getMovementType()
	{
		return movementType;
	}

	void setMovementType(MovementType movementType)
	{
		this->movementType = movementType;
	}

	//void rotate(glm::vec3 rotate) 
	//{
	//}

private:
	BoxAABB box;
	//SphereAABB sphere;
	Point center;

	float deltaTime;
	float lastFrame;

	int randDirection;
	int frameCounter;

	MovementType movementType;

	bool isMoveable;

	glm::vec3 scaleFactor;
	   
	glm::vec3 vel;
	glm::vec3 acc;

	void _setupCenter()
	{
		center.x = (box.max.x + box.min.x) / 2;
		center.y = (box.max.y + box.min.y) / 2;
		center.z = (box.max.z + box.min.z) / 2;

	}

	void _moveRandom()
	{
		if (frameCounter % 30 == 0)
		{
			frameCounter = 0;
			randDirection = rand() % 6;
		}
		frameCounter++;

		switch (randDirection)
		{
		case 0: // move up
			move(Directions::UP);
			break;
		case 1:// move down
			move(Directions::DOWN);
			break;
		case 2:// move left
			move(Directions::LEFT);
			break;
		case 3:// move right
			move(Directions::RIGHT);
			break;
		case 4:// move forward
			move(Directions::FORWARD);
			break;
		case 5:// move backward
			move(Directions::BACKWARD);
			break;
		default:
			break;
		}
	}
	
	inline void _applyFriction(int accPos, int velPos, const glm::vec3 &friction)
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
	
	void _setAndApplyFriction()
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
	
	inline void _adjustVelocityByLimit(int pos)
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

	inline void _checkVelocityLimit()
	{
		vel += acc;

		_adjustVelocityByLimit(0);
		_adjustVelocityByLimit(1);
		_adjustVelocityByLimit(2);
	}

	inline void _stopVelocityIfRequired(int pos)
	{
		if ((vel[pos]< STOP_SPEED && vel[pos] >= 0) || (vel[pos] > -STOP_SPEED && vel[pos] <= 0))
		{
			vel[pos] = 0.0;
			acc[pos]= 0.0;
		}
	}

	inline void _adjustVelocity()
	{
		_checkVelocityLimit();

		_stopVelocityIfRequired(0);
		_stopVelocityIfRequired(1);
		_stopVelocityIfRequired(2);
	}

	inline void _surfaceBounce()
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

	bool _isCommonCollusionSolutionEnough(GameObject &other)
	{
		if (!this->getMoveablity() && !other.getMoveablity())
		{
			std::cout << "Neither can move.";
			// Error case! Problem occoured while loading.
			// TODO: IMPLEMENT A LOGIC FOR THIS CASE
			return true;
		}
		if (!this->getMoveablity())
		{
			std::cout << "This cannot move.";
			other._surfaceBounce();
			return true;
		}
		else if (!other.getMoveablity())
		{
			std::cout << "Other cannot move.";
			this->_surfaceBounce();
			return true;
		}
		return false;
	}
	
	void _solveCollusionBox(GameObject &other)
	{
		//if (!_isCommonCollusionSolutionEnough(other))
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
				
				if (this->getMoveablity())
				{
					this->vel = push_vec;
				}
				if (other.getMoveablity())
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
};

#endif