#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>
#include <iostream>

#include "Enums.h"
#include "values.h"

struct Point {
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;

	Point() : x(), y(), z()
	{}

	Point(double x, double y, double z) : x(x), y(y), z(z)
	{}

	const double operator[](const int idx) const
	{
		if (idx == 0) return x;
		if (idx == 1) return y;
		if (idx == 2) return z;
		//if (idx == 3) return w;
		assert(0);
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
};

struct SphereAABB {

	SphereAABB() : c(), radius() {}

	SphereAABB(const Point & center, const double radius)
		: c(center), radius(radius)
	{}

	Point c;	// Center Point
	double radius;

	bool intersect(SphereAABB rhs)
	{
		auto distance = sqrt((c.x - rhs.c.x) * (c.x - rhs.c.x) +
			(c.y - rhs.c.y) * (c.y - rhs.c.y) +
			(c.z - rhs.c.z) * (c.z - rhs.c.z));
		return distance < (radius + rhs.radius);
	}

	bool isPointInside(Point point) {
		// we are using multiplications because is faster than calling Math.pow
		auto distance = sqrt((point.x - c.x) * (point.x - c.x) +
			(point.y - c.y) * (point.y - c.y) +
			(point.z - c.z) * (point.z - c.z));
		return distance < radius;
	}

	void print()
	{
		std::cout << " center X: " << c.x
			<< " center Y: " << c.y
			<< " center Z: " << c.z
			<< " Radius: " << radius << std::endl;
	}

};

struct BoxAABB
{
	Point max;
	Point min;

	BoxAABB() : max(), min()
	{}

	BoxAABB(Point max, Point min)
		: max(max), min(min)
	{}

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

	void print()
	{
		std::cout << " max_X: " << max.x
			<< " min_X: " << min.x
			<< " max_Y: " << max.y
			<< " min_Y: " << min.y
			<< " max_Z: " << max.z
			<< " min_Z: " << min.z << std::endl;
	}
};


class PhysicEngine {

public:
	PhysicEngine() : deltaTime(0.0), isMoveable(true), scaleFactor(), vel(VECTOR_ZERO), acc(VECTOR_ZERO)
	{}

	void setVelocity(glm::vec3 vel);
	glm::vec3 getVelocity();

	void setAcceleration(glm::vec3 acc);
	glm::vec3 getAcceleration();
	void accelerateTowards(glm::vec3 acc);


	void setScaleFactor(glm::vec3 scale);
	glm::vec3 getScaleFactor();

	void stopMotion();
	void reverseMotion();

	void applyPhysics(float deltaTime);

	void applyMotion(SphereAABB &cage);
	void applyMotion(BoxAABB &cage);
	
	bool isMoving();
	bool canMove();
	void setMoveablity(bool isMoveable);

	MovementType getMovementType();
	void setMovementType(MovementType movementType);

private:

	/* Time since last frame */
	float deltaTime;
	
	/* Can object move? */
	bool isMoveable;

	/* Scale factor */
	glm::vec3 scaleFactor;

	/* Velocity */
	glm::vec3 vel;

	/* Acceleration */
	glm::vec3 acc;

	/* Movement Tpye */
	MovementType movementType;

	/* Functions */
	void _prepareFriction();
	void _adjustVelocityByLimit(int pos);
	void _checkVelocityLimit();
	void _checkAndStopPartialVelocity(int pos);
	void _applyFriction(int accPos, int velPos, const glm::vec3 & friction);

	void _adjustVelocity();
};

void PhysicEngine::setVelocity(glm::vec3 vel)
{
	this->vel = vel;
}

glm::vec3 PhysicEngine::getVelocity()
{
	return this->vel;
}

void PhysicEngine::stopMotion()
{
	vel = VECTOR_ZERO;
	acc = VECTOR_ZERO;
}

inline void PhysicEngine::reverseMotion()
{
	this->vel = -this->vel;
	this->acc = -this->acc;
}

void PhysicEngine::applyPhysics(float deltaTime)
{
	this->deltaTime = deltaTime;

	_prepareFriction();
	_adjustVelocity();
}

void PhysicEngine::applyMotion(SphereAABB &cage)
{
	if (canMove())
	{
		cage.c.x += vel.x;
		cage.c.y += vel.y;
		cage.c.z += vel.z;
	}
}

void PhysicEngine::applyMotion(BoxAABB &cage)
{
	if (canMove())
	{
		cage.max.x += vel.x;
		cage.min.x += vel.x;
		cage.max.y += vel.y;
		cage.min.y += vel.y;
		cage.max.z += vel.z;
		cage.min.z += vel.z;
	}
}

void PhysicEngine::setAcceleration(glm::vec3 acc)
{
	this->acc = acc;
}

glm::vec3 PhysicEngine::getAcceleration()
{
	return this->acc;
}

void PhysicEngine::accelerateTowards(glm::vec3 acc)
{
	acc += acc;
}

void PhysicEngine::setScaleFactor(glm::vec3 scale)
{
	scaleFactor = scale;
}

inline glm::vec3 PhysicEngine::getScaleFactor()
{
	return scaleFactor;
}

bool PhysicEngine::isMoving()
{
	return vel != VECTOR_ZERO;
}

bool PhysicEngine::canMove()
{
	return isMoveable;
}

void PhysicEngine::setMoveablity(bool isMoveable)
{
	this->isMoveable = isMoveable;
}

inline MovementType PhysicEngine::getMovementType()
{
	return movementType;
}

inline void PhysicEngine::setMovementType(MovementType movementType)
{
	this->movementType = movementType;
}

void PhysicEngine::_applyFriction(int accPos, int velPos, const glm::vec3 &friction)
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

void PhysicEngine::_prepareFriction()
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

void PhysicEngine::_adjustVelocityByLimit(int pos)
{
	if (vel[pos] > SPEED_LIMIT)
	{
		vel[pos] = SPEED_LIMIT;
	}
	else if (vel[pos] < -SPEED_LIMIT)
	{
		vel[pos] = -SPEED_LIMIT;
	}
}

void PhysicEngine::_checkVelocityLimit()
{
	vel += acc;

	_adjustVelocityByLimit(0);
	_adjustVelocityByLimit(1);
	_adjustVelocityByLimit(2);
}

void PhysicEngine::_checkAndStopPartialVelocity(int pos)
{
	if ((vel[pos] < STOP_SPEED && vel[pos] >= 0) || (vel[pos] > -STOP_SPEED && vel[pos] <= 0))
	{
		vel[pos] = 0.0;
		acc[pos] = 0.0;
	}
}

void PhysicEngine::_adjustVelocity()
{
	_checkVelocityLimit();

	_checkAndStopPartialVelocity(0);
	_checkAndStopPartialVelocity(1);
	_checkAndStopPartialVelocity(2);
}

#endif // !PHYSICS_H
