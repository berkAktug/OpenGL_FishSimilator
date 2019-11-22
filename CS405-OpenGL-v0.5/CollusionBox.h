#ifndef COLLUSIONBOX_H
#define COLLUSIONBOX_H

#include <glm/glm.hpp>
#include <iostream>

struct Point {
	double x;
	double y;
	double z;

	Point() : x(), y(), z() 
	{}

	Point(double x,double y,double z) : x(x), y(y), z(z)
	{}
};

struct boxLimits {
	double max_x;
	double max_y;
	double max_z;

	double min_y;
	double min_z;
	double min_x;

	boxLimits() : max_x(), max_y(), max_z(), min_x(), min_y(), min_z()
	{}

	boxLimits(double max_x, double max_y, double max_z, double min_x, double min_y, double min_z) 
		: max_x(max_x), max_y(max_y), max_z(max_z), min_x(min_x), min_y(min_y), min_z(min_z)
	{}
};

class CollusionBox
{
public:
	CollusionBox() : canMove(true) 
	{}
	
	boxLimits getCollusionBox()
	{
		return this->cage;
	}

	void setCollusionBox(boxLimits box)
	{
		this->cage = box;
	}

	void scale(glm::vec3 scale)
	{
		double depth  = cage.max_x - cage.min_x;
		double height = cage.max_y - cage.min_y;
		double width  = cage.max_z - cage.min_z;
		
		double diff_depth  = (depth  * scale.x) - depth;
		double diff_height = (height * scale.y) - height;
		double diff_width  = (width  * scale.z) - width;

		cage.max_x += diff_depth / 2;
		cage.min_x -= diff_depth / 2;

		cage.max_z += diff_width / 2;
		cage.min_z -= diff_width / 2;

		cage.max_y += diff_height / 2;
		cage.min_y -= diff_height / 2;
	}

	void move()
	{
		cage.max_x += velocity.x;
		cage.min_x += velocity.x;

		cage.max_y += velocity.y;
		cage.min_y += velocity.y;

		cage.max_z += velocity.z;
		cage.min_z += velocity.z;
	}

	void setupCollusionBox(glm::vec3 vector)
	{
		if (cage.min_x > vector.x) { cage.min_x = vector.x; }
		if (cage.min_y > vector.y) { cage.min_y = vector.x; }
		if (cage.min_z > vector.z) { cage.min_z = vector.z; }

		if (cage.max_x < vector.x) { cage.max_x = vector.x; }
		if (cage.max_y < vector.y) { cage.max_y = vector.x; }
		if (cage.max_z < vector.z) { cage.max_z = vector.z; }
	}

	void printBox() 
	{
		std::cout << " max_X: " << cage.max_x
				  << " min_X: " << cage.min_y
				  << " max_Y: " << cage.max_y
				  << " min_Y: " << cage.min_y
				  << " max_Z: " << cage.max_z
				  << " min_Z: " << cage.min_z << std::endl;
	}

	void doCollusionAABB(CollusionBox &other)
	{
		if (_checkCollusionAABB(other))
		{
			// Do something.
			_solveCollusionAABB(other);
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
	boxLimits cage;
	Point center;

	bool canMove;

	glm::vec3 velocity;
	
	bool _checkCollusionAABB(CollusionBox &other)
	{
		if (!this->canMove && !other.canMove)
		{
			// Error case! Problem occoured while loading.
			// Do nothing for now.
			// TODO: IMPLEMENT A LOGIC FOR THIS CASE
			return false;
		}
		if ((cage.min_x <= other.cage.max_x && cage.max_x >= other.cage.max_x || other.cage.min_x < cage.max_x && other.cage.max_x > cage.max_x) &&
			(cage.min_y <= other.cage.max_y && cage.max_y >= other.cage.max_y || other.cage.min_y < cage.max_y && other.cage.max_y > cage.max_y) &&
			(cage.min_z <= other.cage.max_z && cage.max_y >= other.cage.max_z || other.cage.min_z < cage.max_z && other.cage.max_z > cage.max_z))
		{
			// Collusion
			std::cout << "Collusion Detected _CHECKAABB" << std::endl;
			printBox();
			other.printBox();
			return true;
		}
	}

	void _solveCollusionAABB(CollusionBox &other)
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
			if (other.isMoving())
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
			if (this->isMoving())
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
			if (this->isMoving() && other.isMoving())
			{
				auto Tx = cage.max_x - cage.min_x;
				auto Ty = cage.max_y - cage.min_y;
				auto Tz = cage.max_z - cage.min_z;

				auto Ox = other.cage.max_x - other.cage.min_x;
				auto Oy = other.cage.max_y - other.cage.min_y;
				auto Oz = other.cage.max_z - other.cage.min_z;

				this->velocity = glm::vec3((Tx - Ox) / 2, (Ty - Oy) / 2, (Tz - Oz) / 2);
				other.velocity = glm::vec3(-(Tx - Ox) / 2, -(Ty - Oy) / 2, -(Tz - Oz) / 2);
			}
			else
			{
				this->velocity = -this->velocity;
				other.velocity = -other.velocity;
			}
		}
		move();
		other.move();
	}
};

#endif