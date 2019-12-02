#pragma once
#include "glm/glm.hpp"

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

	static Point getRandomPoint()
	{
		double rand_x = rand() % 5;
		double rand_y = rand() % 5;
		double rand_z = rand() % 5;

		if (rand() % 2 == 0) rand_x *= -1.0;
		if (rand() % 2 == 0) rand_y *= -1.0;
		if (rand() % 2 == 0) rand_z *= -1.0;

		return Point(rand_x, rand_y, rand_z);
	}

	static glm::vec3 getRandomPointVector()
	{
		auto rand_pos = Point::getRandomPoint();
		return glm::vec3(rand_pos.x, rand_pos.y, rand_pos.z);
	}
};