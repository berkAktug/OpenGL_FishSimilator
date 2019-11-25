#ifndef COLLUSIONALGO_H
#define COLLUSIONALGO_H


#include <glm/glm.hpp>
#include <iostream>
#include <cmath>

#include "PhysicsEngine.h"

class CollusionAlgo {

public:
	static void doCollusionAABB(BoxAABB lhs, BoxAABB rhs);

	static void doCollusionAABB(SphereAABB lhs, SphereAABB rhs);

	static void doCollusionAABB(SphereAABB lhs, BoxAABB rhs);
	static void doCollusionAABB(BoxAABB lhs, SphereAABB rhs);

	static void doCollusionSimple(BoxAABB lhs, BoxAABB rhs);
	static void doCollusionSimple(SphereAABB lhs, SphereAABB rhs);
	static void doCollusionSimple(BoxAABB lhs, SphereAABB rhs);

	static bool checkCollusionAABB(BoxAABB lhs, BoxAABB rhs);
	static bool checkCollusionAABB(SphereAABB lhs, SphereAABB rhs);
	static bool checkCollusionAABB(BoxAABB box, SphereAABB sphere);

private:
	// singleton. We don't want any object from this class
	CollusionAlgo() {}

	static void solveCollusionAABB(BoxAABB lhs, BoxAABB rhs);
	static void solveCollusionAABB(SphereAABB lhs, SphereAABB rhs);
	static void solveCollusionAABB(BoxAABB lhs, SphereAABB rhs);

	//static bool checkCollusionAABB(BoxAABB lhs, BoxAABB rhs);
	//static bool checkCollusionAABB(SphereAABB lhs, SphereAABB rhs);
	//static bool checkCollusionAABB(BoxAABB box, SphereAABB sphere);
};


/*
	Start Collusion AABB
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

void CollusionAlgo::doCollusionAABB(BoxAABB lhs, BoxAABB rhs)
{
	if (checkCollusionAABB(lhs, rhs))
	{
		solveCollusionAABB(lhs, rhs);
	}
}

void CollusionAlgo::doCollusionAABB(SphereAABB lhs, SphereAABB rhs)
{
	if (checkCollusionAABB(lhs, rhs))
	{
		solveCollusionAABB(lhs, rhs);
	}
}

void CollusionAlgo::doCollusionAABB(SphereAABB sphere, BoxAABB box)
{
	if (checkCollusionAABB(box, sphere))
	{
		solveCollusionAABB(box, sphere);
	}
}

void CollusionAlgo::doCollusionAABB(BoxAABB box, SphereAABB sphere)
{
	if (checkCollusionAABB(box, sphere))
	{
		solveCollusionAABB(box, sphere);
	}
}


/*
	Start Collusion Simple
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

void CollusionAlgo::doCollusionSimple(BoxAABB lhs, BoxAABB rhs)
{
	throw;
}

void CollusionAlgo::doCollusionSimple(SphereAABB lhs, SphereAABB rhs)
{
	throw;
}

inline void CollusionAlgo::doCollusionSimple(BoxAABB lhs, SphereAABB rhs)
{
	throw;
}

/*
	Check Collusion
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
inline bool CollusionAlgo::checkCollusionAABB(BoxAABB lhs, BoxAABB rhs)
{
	return (lhs.intersect(rhs) || rhs.intersect(lhs));
}

inline bool CollusionAlgo::checkCollusionAABB(SphereAABB lhs, SphereAABB rhs)
{
	return (lhs.intersect(rhs) || rhs.intersect(lhs));
}

inline bool CollusionAlgo::checkCollusionAABB(BoxAABB box, SphereAABB sphere)
{
	// get box closest point to sphere center by clamping
	auto x = std::fmax(box.min.x, std::fmin(sphere.c.x, box.max.x));
	auto y = std::fmax(box.min.y, std::fmin(sphere.c.y, box.max.y));
	auto z = std::fmax(box.min.z, std::fmin(sphere.c.z, box.max.z));

	return sphere.isPointInside(Point(x, y, z));
}

/*
	Solve Collusion
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
void CollusionAlgo::solveCollusionAABB(BoxAABB lhs, BoxAABB rhs)
{

}

void CollusionAlgo::solveCollusionAABB(SphereAABB lhs, SphereAABB rhs)
{

}

void CollusionAlgo::solveCollusionAABB(BoxAABB lhs, SphereAABB rhs)
{
	
}


#endif // !COLLUSIONALGO_H
