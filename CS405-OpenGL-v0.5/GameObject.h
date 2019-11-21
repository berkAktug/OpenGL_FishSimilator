#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

struct Point {
	double x;
	double y;
	double z;

	Point() : x(), y(), z() { }

	Point(double x, double y, double z) : x(x), y(y), z(z) { }

	void move(glm::vec3 velocity)
	{
		x += velocity.x;
		y += velocity.y;
		z += velocity.z;
	}
};

struct ObjectBound {
	double height;
	double width;
	double depth;
	ObjectBound() : height(), width(), depth() { }

	ObjectBound(double height, double width, double depth) : height(height), width(width), depth(depth) { }
};

class GameObject
{
public:
	GameObject() : center(Point()), boundry(ObjectBound()) { isFloor = false; }
	GameObject(Point center, ObjectBound boundry);
	~GameObject();

	void move();
	void move(glm::vec3 vel);

	glm::vec3 getSpeed()
	{
		return vel;
	}

	bool doCollision(GameObject &other);

	void setFloor()
	{
		center.x = 0;
		center.y = 0;
		center.z = 0;
		vel = glm::vec3(0.0f, 0.0f, 0.0f);
		isFloor = true;
	}

	void setSpeed(glm::vec3 new_vel)
	{
		if (!isFloor)
		 vel = new_vel;
	}

	void stopMovement()
	{
		vel = glm::vec3(0.0f);
		acc = glm::vec3(0.0f);
	}

	bool getIsFloor()
	{
		return isFloor;
	}

	Point getCenter();
	ObjectBound getBoundry();
	void scaleBoundry(glm::vec3 scale);

private:
	/*  Object Position data  */
	Point center;
	/*  Object Outer Boundry data  */
	ObjectBound boundry;
	/*  Object Speed  */
	glm::vec3 vel;
	/*  Object Accelaration  */
	glm::vec3 acc;
	/*  Object Mass  */
	double mass;

	bool isFloor;

	bool checkCollision(GameObject &other);
};

GameObject::GameObject(Point center, ObjectBound boundry) : center(center), boundry(boundry)
{
	isFloor = false;
}

GameObject::~GameObject()
{
}

void GameObject::move()
{
	this->center.move(this->vel);
}

void GameObject::move(glm::vec3 velocity)
{
	center.move(velocity);
}

bool GameObject::doCollision(GameObject &other)
{
	return checkCollision(other);
}

bool GameObject::checkCollision(GameObject &other)
{
	auto lmax_x = this->center.x + this->boundry.depth;
	auto lmax_y = this->center.y + this->boundry.height;
	auto lmax_z = this->center.z + this->boundry.width;
	auto lmin_x = this->center.x - this->boundry.depth;
	auto lmin_y = this->center.y - this->boundry.height;
	auto lmin_z = this->center.z - this->boundry.width;

	auto rmax_x = other.center.x + other.boundry.depth;
	auto rmax_y = other.center.y + other.boundry.height;
	auto rmax_z = other.center.z + other.boundry.width;
	auto rmin_x = other.center.x - other.boundry.depth;
	auto rmin_y = other.center.y - other.boundry.height;
	auto rmin_z = other.center.z - other.boundry.width;

	if ((lmin_x <= rmax_x && lmax_x >= rmax_x || rmin_x > lmax_x && rmax_x > lmax_x)
		&& (lmin_y <= rmax_y && lmax_y >= rmax_y || rmin_y < lmax_y && rmax_y > lmax_y)
		&& (lmin_z <= rmax_z && lmax_y >= rmax_z || rmin_z < lmax_z && rmax_z > lmax_z))
	{
		// Collision
		//printf("Collision Detected\n");
		return true;
	}
	return false;

	////if (this->center.x < other.center.x + other.boundry.width  &&
	////	this->center.x + this->boundry.width > other.center.x  &&
	////	this->center.y < other.center.y + other.boundry.height &&
	////	this->center.y + this->boundry.height > other.center.y &&
	////	this->center.z < other.center.z + other.boundry.depth  &&
	////	this->center.z + this->boundry.depth > other.center.z)
	////{
	////	printf("Collision Detected\n");
	////	return true;
	////}
	//return false;
}

Point GameObject::getCenter()
{
	return center;
}

ObjectBound GameObject::getBoundry()
{
	return boundry;
}

void GameObject::scaleBoundry(glm::vec3 scale)
{
	boundry.height	*= scale.x;
	boundry.width	*= scale.y;
	boundry.depth	*= scale.z;
}

#endif