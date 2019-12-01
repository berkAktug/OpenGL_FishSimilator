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

	void Move(const glm::vec3 &vec)
	{
		max.x += vec.x;
		min.x += vec.x;

		max.y += vec.y;
		min.y += vec.y;

		max.z += vec.z;
		min.z += vec.z;

		center.x += vec.x;
		center.y += vec.y;
		center.z += vec.z;
	}

	bool Intersect(BoxAABB b) {
		return  (min.x <= b.max.x && max.x >= b.min.x) &&
			(min.y <= b.max.y && max.y >= b.min.y) &&
			(min.z <= b.max.z && max.z >= b.min.z);
	}

	bool IsInsideBoxBoxAABB(const BoxAABB & biggerBox)
	{
		auto is_inside_x = false;
		auto is_inside_y = false;
		auto is_inside_z = false;

		auto is_inside_all = true;

		if (biggerBox.max.x > this->max.x && biggerBox.min.x < this->min.x) 
		{
			is_inside_x = true;
		}

		if (biggerBox.max.y > this->max.y && biggerBox.min.y < this->min.y)
		{
			is_inside_y = true;
		}

		if (biggerBox.max.z > this->max.z && biggerBox.min.z < this->min.z)
		{
			is_inside_z = true;
		}

		is_inside_all = is_inside_x && is_inside_y && is_inside_z;

		return is_inside_all;
	}

	bool IsPointInsideAABB(Point point) {
		return (point.x >= min.x && point.x <= max.x) &&
			(point.y >= min.y && point.y <= max.y) &&
			(point.z >= min.z && point.z <= max.z);
	}

	bool IsPointInsideAABB(glm::vec3 point) {
		return (point.x >= min.x && point.x <= max.x) &&
			(point.y >= min.y && point.y <= max.y) &&
			(point.z >= min.z && point.z <= max.z);
	}

	void Print()
	{
		std::cout
			<< "max_X: " << max.x
			<< " max Y: " << max.y
			<< " max Z: " << max.z << std::endl
			<< "min X: " << min.x
			<< " min Y: " << min.y
			<< " min Z: " << min.z << std::endl
			<< "Center X: " << center.x
			<< " Center Y: " << center.y
			<< " Center Z: " << center.z << std::endl;
	}
};


static BoxAABB gameBoundry = BoxAABB(Point(GAMEBOUNDRY_X, GAMEBOUNDRY_Y, GAMEBOUNDRY_Z),
	Point(-GAMEBOUNDRY_X, -GAMEBOUNDRY_Y, -GAMEBOUNDRY_Z));

class PhysicEngine {

public:
	PhysicEngine(Point max, Point min) : _deltaTime(0.0f), _isMoveable(true), 
		_vel(VECTOR_ZERO), _acc(VECTOR_ZERO) 
	{
		_cage = BoxAABB(max, min);
		_AdjustCenter();
	}

	glm::vec3 GetVelocity();

	void DoBoundryCollusion();

	void DoCollusion(PhysicEngine *other);

	bool CheckCollusion(PhysicEngine *other);

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
	BoxAABB _cage;

	/* Time since last frame */
	float _deltaTime;
	
	/* Can object Move? */
	bool _isMoveable;

	/* Velocity */
	glm::vec3 _vel;

	/* Acceleration */
	glm::vec3 _acc;

	void _AdjustCenter();

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

void PhysicEngine::DoBoundryCollusion()
{
	if (!this->_CheckIsInGameField())
	{
		// lhs object is not within game field.
		std::cout << "Object is not in gamefield. Returning them to the mother base. Over." << std::endl;
		std::cout << "Before function coordinates are: ";
		Print();

		_SolveSurfaceCollusion();
	}
}

void PhysicEngine::DoCollusion(PhysicEngine *other)
{
	if(CheckCollusion(other))
	{
		std::cout << "Collusion Detected AABB Box" << std::endl;
		_cage.Print();
		other->_cage.Print();

		_SolveCollusionBox(*other);
	}
}

bool PhysicEngine::CheckCollusion(PhysicEngine * other)
{
	//return (_cage.Intersect(other->_cage) || other->_cage.Intersect(_cage));
	return (_cage.IsPointInsideAABB(other->_cage.center) || other->_cage.IsPointInsideAABB(_cage.center));
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

	_cage.center += _vel * delta_time;

	_vel += _acc * delta_time;

	_CheckLimitAcceleration();

	_ApplyFriction();
}

glm::vec3 PhysicEngine::GetCenter()
{
	return _cage.center;
}

inline void PhysicEngine::_AdjustCenter()
{
	auto center_x = _cage.min.x + (_cage.max.x - _cage.min.x) / 2;
	auto center_y = _cage.min.y + (_cage.max.y - _cage.min.y) / 2;
	auto center_z = _cage.min.z + (_cage.max.z - _cage.min.z) / 2;

	_cage.center = glm::vec3(center_x, center_y, center_z);
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
	double depth = _cage.max.x - _cage.min.x;
	double height = _cage.max.y - _cage.min.y;
	double width = _cage.max.z - _cage.min.z;

	double diff_depth = (depth  * scale.x) - depth;
	double diff_height = (height * scale.y) - height;
	double diff_width = (width  * scale.z) - width;

	_cage.max.x += diff_depth / 2;
	_cage.min.x -= diff_depth / 2;

	_cage.max.z += diff_width / 2;
	_cage.min.z -= diff_width / 2;

	_cage.max.y += diff_height / 2;
	_cage.min.y -= diff_height / 2;

	_CheckBoxBoundry();
}

void PhysicEngine::MoveTo(glm::vec3 point)
{
	_CheckBoxBoundry();

	double dist_x = _cage.max.x - _cage.min.x;
	double dist_y = _cage.max.y - _cage.min.y;
	double dist_z = _cage.max.z - _cage.min.z;

	_cage.max.x = point.x + dist_x / 2;
	_cage.min.x = point.x - dist_x / 2;

	_cage.max.y = point.y + dist_y / 2;
	_cage.min.y = point.y - dist_y / 2;

	_cage.max.z = point.z + dist_z / 2;
	_cage.min.z = point.z - dist_z / 2;

	_AdjustCenter();
}

void PhysicEngine::Move(glm::vec3 scale_factor)
{
	_cage.Move(this->_vel * scale_factor);
	_AdjustCenter();
}

void PhysicEngine::Print()
{
	_cage.Print();
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
	if (_cage.max.x < _cage.min.x)
	{
		std::swap(_cage.max.x, _cage.min.x);
	}
	if (_cage.max.y < _cage.min.y)
	{
		std::swap(_cage.max.y, _cage.min.y);
	}
	if (_cage.max.z < _cage.min.z)
	{
		std::swap(_cage.max.z, _cage.min.z);
	}
}

void PhysicEngine::_SolveSurfaceCollusion()
{
	auto dist_x = _cage.center.x ;
	auto dist_y = _cage.center.y ;
	auto dist_z = _cage.center.z ;

	if (_cage.max.x > gameBoundry.max.x)
	{
		dist_x = gameBoundry.max.x - _cage.max.x;
	}
	else if (_cage.min.x < gameBoundry.min.x)
	{
		dist_x = _cage.min.x - gameBoundry.min.x;
	}

	if (_cage.max.y > gameBoundry.max.y)
	{
		dist_y = gameBoundry.max.y - _cage.max.y;
	}
	else if (_cage.min.y < gameBoundry.min.y)
	{
		dist_y = _cage.min.y - gameBoundry.min.y;
	}

	if (_cage.max.z > gameBoundry.max.z)
	{
		dist_z = gameBoundry.max.z - _cage.max.z;
	}
	else if (_cage.min.z < gameBoundry.min.z)
	{
		dist_z = _cage.min.z - gameBoundry.min.z;
	}

	MoveTo(glm::vec3(dist_x, dist_y, dist_z));
}

bool PhysicEngine::_CheckIsInGameField()
{
	if (_cage.IsInsideBoxBoxAABB(gameBoundry))
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
			if (_cage.max.x > other._cage.max.x && _cage.min.x < other._cage.min.x)
				// case 1
			else if (other._cage.max.x > _cage.min.x && _cage.min.x > other._cage.min.x)
				// case 2
			else if (_cage.max.x > other._cage.min.x && other._cage.min.x > _cage.max.x)
				// case 3
			else if (other._cage.max.x > _cage.max.x && _cage.min.x > other._cage.min.x)
				// case 4
		*/
		// Optimized version.
		if (other._cage.max.x > _cage.min.x && other._cage.max.x < _cage.max.x)
		{
			// Covers case 1 && 2
			dist_x = other._cage.max.x - _cage.min.x;
		}
		else if (_cage.max.x > other._cage.min.x && _cage.max.x < other._cage.max.x)
		{
			// Covers case 3 && 4
			dist_x = _cage.max.x - other._cage.min.x;
		}
		dist_x = dist_x / 2;


		if (other._cage.max.y > _cage.min.y && other._cage.max.y < _cage.max.y)
		{
			// Covers case 1 && 2
			dist_y = other._cage.max.y - _cage.min.y;
		}
		else if (_cage.max.y > other._cage.min.y && _cage.max.y < other._cage.max.y)
		{
			// Covers case 3 && 4
			dist_y = _cage.max.y - other._cage.min.y;
		}
		dist_y = dist_y / 2;

		if (other._cage.max.z > _cage.min.z && other._cage.max.z < _cage.max.z)
		{
			// Covers case 1 && 2
			dist_z = other._cage.max.z - _cage.min.z;
		}
		else if (_cage.max.z > other._cage.min.z && _cage.max.z < other._cage.max.z)
		{
			// Covers case 3 && 4
			dist_z = _cage.max.z - other._cage.min.z;
		}
		dist_z = dist_z / 2;
		
		auto push_vec = glm::vec3(dist_x, dist_y, dist_z);
		this->_vel = push_vec;
		other._vel = push_vec;

		this->_acc -= this->_acc;
		other._acc -= other._acc;
	}
	else
	{
		this->_vel -= this->_vel;
		other._vel -= other._vel;
	}
}

#endif // !PHYSICS_H
