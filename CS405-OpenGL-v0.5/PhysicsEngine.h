#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>

#include "values.h"


class PhysicEngine {

public:
	PhysicEngine() : _isMoveable(true), 
		_velocity(VECTOR_ZERO), _accel(VECTOR_ZERO) 
	{}

	glm::vec3 GetVelocity();

	void AccelerateTowards(glm::vec3 acc);

	void StopMotion();
	void ReverseMotion() { _velocity = -_velocity; }

	glm::vec3 Apply(float delta_time);

	bool CanMove() { return _isMoveable; }
	bool IsMoving() { return _velocity != VECTOR_ZERO; }
	
	void DisableMovement() { _isMoveable = false; }
	void EnableMovmeent() { _isMoveable = true; }

	void PrintPhysics();

private:
	
	/* Can object MoveCollider? */
	bool _isMoveable;

	/* Velocity */
	glm::vec3 _velocity;

	/* Acceleration */
	glm::vec3 _accel;

	void _CheckLimitAcceleration();
	void _ApplyFriction();
};

glm::vec3 PhysicEngine::GetVelocity()
{
	return this->_velocity;
}

void PhysicEngine::AccelerateTowards(glm::vec3 acceleration)
{
	this->_accel += acceleration;
}

void PhysicEngine::StopMotion()
{
	_velocity = VECTOR_ZERO;
	_accel = VECTOR_ZERO;
}

glm::vec3 PhysicEngine::Apply(float delta_time)
{
	_velocity += _accel * delta_time;

	glm::vec3 dt_distance = _velocity * delta_time;

	_CheckLimitAcceleration();

	_ApplyFriction();

	return dt_distance;
}

void PhysicEngine::PrintPhysics()
{
	std::cout << "\nObject's" <<
		" velocity is on x:" << _velocity.x << " on y:" << _velocity.y << " on z:" << _velocity.z <<
		" accel is on x:" << _accel.x << " on y:" << _accel.y << " on z:" << _accel.z << std::endl;
}

void PhysicEngine::_CheckLimitAcceleration()
{
	if ((_accel.x > HIGHEST_ACCELERATION && _accel.x >= 0))
	{
		_accel.x = HIGHEST_ACCELERATION;
	}
	else if((_accel.x < -HIGHEST_ACCELERATION && _accel.x <= 0))
	{
		_accel.x = -HIGHEST_ACCELERATION;
	}

	if ((_accel.y > HIGHEST_ACCELERATION && _accel.y >= 0))
	{
		_accel.y = HIGHEST_ACCELERATION;
	}
	else if((_accel.y < -HIGHEST_ACCELERATION && _accel.y <= 0))
	{
		_accel.y = -HIGHEST_ACCELERATION;
	}
	
	if ((_accel.z > HIGHEST_ACCELERATION && _accel.z >= 0))
	{
		_accel.z = HIGHEST_ACCELERATION;
	}
	else if ((_accel.z < -HIGHEST_ACCELERATION && _accel.z <= 0))
	{
		_accel.z = -HIGHEST_ACCELERATION;
	}
}

void PhysicEngine::_ApplyFriction()
{
	// simulate friction
	_accel *= 0.90f;
	if (((_accel.x < LOWEST_ACCELERATION && _accel.x >= 0) || (_accel.x > -LOWEST_ACCELERATION && _accel.x <= 0)) &&
		((_accel.y < LOWEST_ACCELERATION && _accel.y >= 0) || (_accel.y > -LOWEST_ACCELERATION && _accel.y <= 0)) &&
		((_accel.z < LOWEST_ACCELERATION && _accel.z >= 0) || (_accel.z > -LOWEST_ACCELERATION && _accel.z <= 0)))
	{
		StopMotion();
	}
}

#endif // !PHYSICS_H
