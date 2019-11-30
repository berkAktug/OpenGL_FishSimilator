#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>

#include "Enums.h"
#include "values.h"

struct BoxAABB
{
	Point max;
	Point min;
	glm::vec3 center;

	BoxAABB() : max(), min()
	{}

	BoxAABB(Point max, Point min)
		: max(max), min(min), center()
	{
		center.x = (max.x - min.x)/2;
		center.y = (max.y - min.y)/2;
		center.z = (max.z - min.z)/2;
	}

	void move(const glm::vec3 &vec)
	{
		max.x += vec.x;
		min.x += vec.x;

		max.y += vec.y;
		min.y += vec.y;

		max.z += vec.z;
		min.z += vec.z;
	}

	bool intersect(BoxAABB b) {
		return  (min.x <= b.max.x && max.x >= b.min.x) &&
			(min.y <= b.max.y && max.y >= b.min.y) &&
			(min.z <= b.max.z && max.z >= b.min.z);
	}

	bool isPointInsideAABB(Point point) {
		return (point.x >= min.x && point.x <= max.x) &&
			(point.y >= min.y && point.y <= max.y) &&
			(point.z >= min.z && point.z <= max.z);
	}

	void Print()
	{
		std::cout << " max_X: " << max.x
			<< " min_X: " << min.x
			<< " max_Y: " << max.y
			<< " min_Y: " << min.y
			<< " max_Z: " << max.z
			<< " min_Z: " << min.z << std::endl;
	}
};


static BoxAABB gameBoundry = BoxAABB(Point(GAMEBOUNDRY_X, GAMEBOUNDRY_Y, GAMEBOUNDRY_Z),
	Point(-GAMEBOUNDRY_X, -GAMEBOUNDRY_Y, -GAMEBOUNDRY_Z));

class PhysicEngine {

public:
	PhysicEngine(Point max, Point min) : _deltaTime(0.0f), _isMoveable(true), 
		_vel(VECTOR_ZERO) //, _scaleFactor(glm::vec3(1.0f))
	{
		_box = BoxAABB(max, min);

		_SetCenter(max, min);
		//_box.max = max;
		//_box.min = min;
	}

	glm::vec3 GetVelocity();

	void DoCollusion(PhysicEngine *other);

	void AccelerateTowards(glm::vec3 acc);

	glm::vec3 GetCenter();

	void StopMotion();
	void ReverseMotion();

	void Apply(float delta_time);

	bool IsMoving();
	bool CanMove();
	void SetMoveablity(bool isMoveable);

	void Scale(glm::vec3 scale);

	void Print();

	void Move(glm::vec3 scale_factor);

	void MoveTo(glm::vec3 point);

private:
	BoxAABB _box;

	/* Time since last frame */
	float _deltaTime;
	
	/* Can object Move? */
	bool _isMoveable;

	/* Velocity */
	glm::vec3 _vel;

	/* Acceleration */
	glm::vec3 _acc;

	void _SetCenter(glm::vec3 center);
	void _SetCenter(Point max, Point min);

	void _CheckLimitAcceleration();
	void _ApplyFriction();
	void _CheckBoxBoundry();
	void _SolveSurfaceCollusion();
	bool _CheckIsInGameField();
	void _SolveCollusionBox(PhysicEngine & other);
};

glm::vec3 PhysicEngine::GetVelocity()
{
	return this->_vel;
}

void PhysicEngine::DoCollusion(PhysicEngine *other)
{
	if (!_CheckIsInGameField())
	{
		// lhs object is not within game field.
		std::cout << "Object1 is not in gamefield. Returning them to the mother base. Over." << std::endl;
		_SolveSurfaceCollusion();
	}
	if (!other->_CheckIsInGameField())
	{
		// rhs object is not within game field.
		std::cout << "Object2 is not in gamefield. Returning them to the mother base. Over." << std::endl;
		_SolveSurfaceCollusion();
	}


	if ((this->_box.intersect(other->_box)) || other->_box.intersect(this->_box))
	{
		std::cout << "Collusion Detected AABB Box" << std::endl;
		_box.Print();
		other->_box.Print();

		_SolveCollusionBox(*other);
	}
}

void PhysicEngine::AccelerateTowards(glm::vec3 acceleration)
{
	this->_acc += acceleration;
}

void PhysicEngine::StopMotion()
{
	_vel = VECTOR_ZERO;
}

void PhysicEngine::ReverseMotion()
{
	this->_vel = -this->_vel;
}

void PhysicEngine::Apply(float delta_time)
{
	this->_deltaTime = delta_time;

	//position += velocity * _deltaTime;
	// f(x)   = position
	// f'(x)  = velocity
	// f''(x) = acceleration

	_box.center += _vel * delta_time;

	_vel += _acc * delta_time;

	_CheckLimitAcceleration();

	_ApplyFriction();
}

glm::vec3 PhysicEngine::GetCenter()
{
	return _box.center;
}

void PhysicEngine::_SetCenter(glm::vec3 center)
{
	auto diff_x = (_box.max.x - _box.min.x)/2;
	auto diff_y = (_box.max.x - _box.min.y)/2;
	auto diff_z = (_box.max.x - _box.min.y)/2;

	_box.center = center;

	_box.max.x = _box.center.x + diff_x;
	_box.min.x = _box.center.x - diff_x;

	_box.max.y = _box.center.y + diff_y;
	_box.max.y = _box.center.y - diff_y;

	_box.max.z = _box.center.z + diff_z;
	_box.max.z = _box.center.z - diff_z;
}

void PhysicEngine::_SetCenter(Point max, Point min)
{
	auto center_x = (_box.max.x + _box.min.x) / 2;
	auto center_y = (_box.max.x + _box.min.y) / 2;
	auto center_z = (_box.max.x + _box.min.y) / 2;

	_box.center = glm::vec3(center_x, center_y, center_z);
}

bool PhysicEngine::IsMoving()
{
	return _vel != VECTOR_ZERO;
}

bool PhysicEngine::CanMove()
{
	return _isMoveable;
}

void PhysicEngine::SetMoveablity(bool isMoveable)
{
	this->_isMoveable = isMoveable;
}

void PhysicEngine::Scale(glm::vec3 scale)
{
	double depth = _box.max.x - _box.min.x;
	double height = _box.max.x - _box.min.y;
	double width = _box.max.x - _box.min.z;

	double diff_depth = (depth  * scale.x) - depth;
	double diff_height = (height * scale.y) - height;
	double diff_width = (width  * scale.z) - width;

	_box.max.x += diff_depth / 2;
	_box.min.x -= diff_depth / 2;

	_box.max.z += diff_width / 2;
	_box.min.z -= diff_width / 2;

	_box.max.y += diff_height / 2;
	_box.min.y -= diff_height / 2;

	_CheckBoxBoundry();
}

void PhysicEngine::MoveTo(glm::vec3 point)
{
	_CheckBoxBoundry();

	double dist_x = _box.max.x - _box.min.x;
	_box.max.x = point.x + dist_x / 2;
	_box.min.x = point.x - dist_x / 2;

	double dist_y = _box.max.y - _box.min.y;
	_box.max.y = point.y + dist_y / 2;
	_box.min.y = point.y - dist_y / 2;

	double dist_z = _box.max.z - _box.min.z;
	_box.max.z = point.z + dist_z / 2;
	_box.min.z = point.z - dist_z / 2;

	_SetCenter(_box.max, _box.min);
}

void PhysicEngine::Move(glm::vec3 scale_factor)
{
	_box.move(this->_vel * scale_factor);
}

void PhysicEngine::Print()
{
	_box.Print();
}

void PhysicEngine::_CheckLimitAcceleration()
{
	if ((_acc.x > HIGHEST_ACCELERATION && _acc.x >= 0))
	{
		_acc.x = HIGHEST_ACCELERATION;
	}
	else if((_acc.x < -HIGHEST_ACCELERATION && _acc.x <= 0))
	{
		_acc.x = -HIGHEST_ACCELERATION;
	}

	if ((_acc.y > HIGHEST_ACCELERATION && _acc.y >= 0))
	{
		_acc.y = HIGHEST_ACCELERATION;
	}
	else if((_acc.y < -HIGHEST_ACCELERATION && _acc.y <= 0))
	{
		_acc.y = -HIGHEST_ACCELERATION;
	}
	
	if ((_acc.z > HIGHEST_ACCELERATION && _acc.z >= 0))
	{
		_acc.z = HIGHEST_ACCELERATION;
	}
	else if ((_acc.z < -HIGHEST_ACCELERATION && _acc.z <= 0))
	{
		_acc.z = -HIGHEST_ACCELERATION;
	}
}

void PhysicEngine::_ApplyFriction()
{
	// simulate friction
	_acc *= 0.90f;
	if (((_acc.x < LOWEST_ACCELERATION && _acc.x >= 0) || (_acc.x > -LOWEST_ACCELERATION && _acc.x <= 0)) &&
		((_acc.y < LOWEST_ACCELERATION && _acc.y >= 0) || (_acc.y > -LOWEST_ACCELERATION && _acc.y <= 0)) &&
		((_acc.z < LOWEST_ACCELERATION && _acc.z >= 0) || (_acc.z > -LOWEST_ACCELERATION && _acc.z <= 0)))
	{
		_acc = VECTOR_ZERO;
		// Temporary solution.
		_vel = VECTOR_ZERO;
	}
}


void PhysicEngine::_CheckBoxBoundry()
{
	if (_box.max.x < _box.min.x)
	{
		std::swap(_box.max.x, _box.min.x);
	}
	if (_box.max.y < _box.min.y)
	{
		std::swap(_box.max.y, _box.min.y);
	}
	if (_box.max.z < _box.min.z)
	{
		std::swap(_box.max.z, _box.min.z);
	}
}

void PhysicEngine::_SolveSurfaceCollusion()
{
	if (!gameBoundry.isPointInsideAABB(_box.max))
	{
		auto posx = _box.max.x <= gameBoundry.max.x ? _box.max.x : gameBoundry.max.x - _box.max.x;
		auto posy = _box.max.y <= gameBoundry.max.y ? _box.max.y : gameBoundry.max.y - _box.max.y;
		auto posz = _box.max.z <= gameBoundry.max.z ? _box.max.z : gameBoundry.max.z - _box.max.z;

		MoveTo(glm::vec3(posx, posy, posz));
	}
	if (!gameBoundry.isPointInsideAABB(_box.min))
	{
		auto posx = _box.min.x >= gameBoundry.min.x ? _box.min.x : gameBoundry.min.x + _box.min.x;
		auto posy = _box.min.y >= gameBoundry.min.y ? _box.min.y : gameBoundry.min.y + _box.min.y;
		auto posz = _box.min.z >= gameBoundry.min.z ? _box.min.z : gameBoundry.min.z + _box.min.z;

		MoveTo(glm::vec3(posx, posy, posz));
	}
}

bool PhysicEngine::_CheckIsInGameField()
{
	if (gameBoundry.isPointInsideAABB(_box.max) && gameBoundry.isPointInsideAABB(_box.min))
	{
		return true;
	}
	return false;
}

void PhysicEngine::_SolveCollusionBox(PhysicEngine &other)
{
	if (this->IsMoving() && other.IsMoving())
	{
		double dist_x = 0;
		double dist_y = 0;
		double dist_z = 0;

		/*
			Case 1: This object has engulfed Other object.
			Case 2: This object is on top the of Other object.
			Case 3: This object is under the Other object.
			Case 4: This object has been engulfed by Other object.
			if (_box.max.x > other._box.max.x && _box.min.x < other._box.min.x)
				// case 1
			else if (other._box.max.x > _box.min.x && _box.min.x > other._box.min.x)
				// case 2
			else if (_box.max.x > other._box.min.x && other._box.min.x > _box.max.x)
				// case 3
			else if (other._box.max.x > _box.max.x && _box.min.x > other._box.min.x)
				// case 4
		*/
		// Optimized version.
		if (other._box.max.x > _box.min.x && other._box.max.x < _box.max.x)
		{
			// Covers case 1 && 2
			dist_x = other._box.max.x - _box.min.x;
		}
		else if (_box.max.x > other._box.min.x && _box.max.x < other._box.max.x)
		{
			// Covers case 3 && 4
			dist_x = _box.max.x - other._box.min.x;
		}
		dist_x = dist_x / 2;


		if (other._box.max.y > _box.min.y && other._box.max.y < _box.max.y)
		{
			// Covers case 1 && 2
			dist_y = other._box.max.y - _box.min.y;
		}
		else if (_box.max.y > other._box.min.y && _box.max.y < other._box.max.y)
		{
			// Covers case 3 && 4
			dist_y = _box.max.y - other._box.min.y;
		}
		dist_y = dist_y / 2;

		if (other._box.max.z > _box.min.z && other._box.max.z < _box.max.z)
		{
			// Covers case 1 && 2
			dist_z = other._box.max.z - _box.min.z;
		}
		else if (_box.max.z > other._box.min.z && _box.max.z < other._box.max.z)
		{
			// Covers case 3 && 4
			dist_z = _box.max.z - other._box.min.z;
		}
		dist_z = dist_z / 2;

		auto push_vec = glm::vec3(dist_x, dist_y, dist_z);
		this->_vel = push_vec;
		other._vel = push_vec;
	}
	else
	{
		this->_vel -= this->_vel;
		other._vel -= other._vel;
	}
}


#endif // !PHYSICS_H
