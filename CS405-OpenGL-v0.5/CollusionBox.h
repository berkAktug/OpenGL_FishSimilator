#ifndef COLLUSIONBOX_H
#define COLLUSIONBOX_H

#include <glm/glm.hpp>
#include <iostream>
#include <cmath>

struct Point {
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;

	Point() : x(), y(), z() 
	{}

	Point(double x,double y,double z) : x(x), y(y), z(z)
	{}

	const double operator[](const int idx) const
	{
		if (idx == 0) return x;
		if (idx == 1) return y;
		if (idx == 2) return z;
		//if (idx == 3) return w;
		assert(0);
	}
};

struct sphereLimits {
	
	sphereLimits() : c(), r() {}

	sphereLimits(const Point & center, const Point & halfwidths)
		: c(center), r (halfwidths)
	{}

	Point c;	// Center Point
	Point r;	// halfwidths

	double Abs(double a)
	{
		return std::fabs(a);
	}

	bool checkCollusion(sphereLimits other)
	{
		bool xOverlap = true;

		bool yOverlap = true;

		bool zOverlap = true;

		bool anyOverlap = false;

		if (Abs(c[0] - other.c[0]) > (r[0] + other.r[0])) xOverlap = false;

		if (Abs(c[1] - other.c[1]) > (r[1] + other.r[1])) yOverlap = false;

		if (Abs(c[2] - other.c[2]) > (r[2] + other.r[2])) zOverlap = false;

		anyOverlap = xOverlap && yOverlap && zOverlap;

		return anyOverlap;
	}
};

struct boxLimits {

	Point max;
	Point min;

	boxLimits() : max(), min()
	{}

	boxLimits(Point max, Point min) 
		: max(max), min(min)
	{}
};

class CollusionBox
{
public:
	CollusionBox() : canMove(true) 
	{}
	
	boxLimits getCollusionBox()
	{
		return this->box;
	}

	void setCollusionBox(boxLimits box)
	{
		this->box = box;
	}

	void scale(glm::vec3 scale)
	{
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
	}

	void move()
	{
		box.max.x += velocity.x;
		box.min.x += velocity.x;

		box.max.y += velocity.y;
		box.min.y += velocity.y;

		box.max.z += velocity.z;
		box.min.z += velocity.z;
	}

	void setupCollusionSphere()
	{
		auto diffx = box.max.x - box.min.x;
		auto diffy = box.max.y - box.min.y;
		auto diffz = box.max.z - box.min.z;

		auto maxdiff = std::fmax(std::fmax(diffx, diffy), diffz);
		maxdiff = maxdiff / 2;

		auto center = Point(diffx / 2, diffy / 2, diffz / 2);
		auto radius = Point(maxdiff - diffx, maxdiff - diffy, maxdiff - diffz);
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
		std::cout << " max_X: " << box.max.x
				  << " min_X: " << box.min.y
				  << " max_Y: " << box.max.y
				  << " min_Y: " << box.min.y
				  << " max_Z: " << box.max.z
				  << " min_Z: " << box.min.z << std::endl;
	}

	void doCollusionAABB(CollusionBox &other)
	{
		if (_checkCollusionAABBbox(other))
		{
			// Do something.
			_solveCollusionAABBbox(other);
		}
		if (_sheckCollusionAABBsphere(other))
		{
			_solveCollusionAABBsphere(other);
		}
	}

	bool isMoveable()
	{
		return this->canMove;
	}

	void setImmoveable() 
	{
		this->canMove = false;
	}

	void stopMovement()
	{
		this->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	void setMoveable()
	{
		this->canMove = true;
	}

	glm::vec3 getVelocity()
	{
		return this->velocity;
	}

	void setVelocity(glm::vec3 velocity)
	{
		this->velocity = velocity;
	}

	bool isMoving()
	{
		return velocity == glm::vec3(0.0f, 0.0f, 0.0f);
	}

	//void rotate(glm::vec3 rotate) 
	//{
	//}

private:
	boxLimits box;
	sphereLimits sphere;
	Point center;

	bool canMove;

	glm::vec3 velocity;
	
	bool _sheckCollusionAABBsphere(CollusionBox &other)
	{
		return sphere.checkCollusion(other.sphere);
	}

	bool _checkCollusionAABBbox(CollusionBox &other)
	{
		if (!this->canMove && !other.canMove)
		{
			// Error case! Problem occoured while loading.
			// TODO: IMPLEMENT A LOGIC FOR THIS CASE
			return false;
		}
		if ((box.min.x <= other.box.max.x && box.max.x >= other.box.max.x || other.box.min.x < box.max.x && other.box.max.x > box.max.x) &&
			(box.min.y <= other.box.max.y && box.max.y >= other.box.max.y || other.box.min.y < box.max.y && other.box.max.y > box.max.y) &&
			(box.min.z <= other.box.max.z && box.max.y >= other.box.max.z || other.box.min.z < box.max.z && other.box.max.z > box.max.z))
		{
			// Collusion
			std::cout << "Collusion Detected _CHECKAABB" << std::endl;
			printBox();
			other.printBox();
			return true;
		}
	}

	void _solveCollusionAABBbox(CollusionBox &other)
	{
		if (!this->canMove && !other.canMove)
		{
			// Error case! Problem occoured while loading.
			// TODO: IMPLEMENT A LOGIC FOR THIS CASE
			return;
		}
		if (!this->canMove)
		{
			if (! other.isMoving())
			{
				other.velocity = glm::vec3(0.0f, 1.0f, 0.0f);
			}
			else 
			{
				other.velocity = -other.velocity;
			}
		}
		else if (!other.canMove)
		{
			if (! this->isMoving())
			{
				this->velocity = glm::vec3(0.0f, 1.0f, 0.0f);
			}
			else 
			{
				this->velocity = -this->velocity;
			}
		}
		else
		{
			if (! this->isMoving() && ! other.isMoving())
			{
				auto Tx = box.max.x - box.min.x;
				auto Ty = box.max.y - box.min.y;
				auto Tz = box.max.z - box.min.z;

				auto Ox = other.box.max.x - other.box.min.x;
				auto Oy = other.box.max.y - other.box.min.y;
				auto Oz = other.box.max.z - other.box.min.z;

				this->velocity = glm::vec3((Tx - Ox) / 2, (Ty - Oy) / 2, (Tz - Oz) / 2);
				other.velocity = glm::vec3(-(Tx - Ox) / 2, -(Ty - Oy) / 2, -(Tz - Oz) / 2);
			}
			else
			{
				this->velocity = -this->velocity;
				other.velocity = -other.velocity;
			}
		}
		//move();
		//other.move();
	}

	void _solveCollusionAABBsphere(CollusionBox &other)
	{
		if (!this->canMove && !other.canMove)
		{
			// Error case! Problem occoured while loading.
			// Do nothing for now.
			// TODO: IMPLEMENT A LOGIC FOR THIS CASE
			return;
		}
		if (!this->canMove)
		{
			if (! other.isMoving())
			{
				other.velocity = glm::vec3(0.0f, 1.0f, 0.0f);
			}
			else
			{
				other.velocity = -other.velocity;
			}
		}
		else if (!other.canMove)
		{
			if (! this->isMoving())
			{
				this->velocity = glm::vec3(0.0f, 1.0f, 0.0f);
			}
			else
			{
				this->velocity = -this->velocity;
			}
		}
		else
		{
			if (! this->isMoving() && ! other.isMoving())
			{
				auto Tx = sphere.c[0];
				auto Ty = sphere.c[1];
				auto Tz = sphere.c[2];

				auto Ox = other.sphere.c[0];
				auto Oy = other.sphere.c[1];
				auto Oz = other.sphere.c[2];
				
				this->velocity = glm::vec3((Tx - Ox) / 2, (Ty - Oy) / 2, (Tz - Oz) / 2);
				other.velocity = glm::vec3(-(Tx - Ox) / 2, -(Ty - Oy) / 2, -(Tz - Oz) / 2);
			}
			else
			{
				this->velocity = -this->velocity;
				other.velocity = -other.velocity;
			}
		}
		//move();
		//other.move();
	}

};

#endif