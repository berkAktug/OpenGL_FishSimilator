#ifndef VALUES_H
#define VALUES_H

#include "glm/glm.hpp"

// Default Object movement values

const double DIVISION_COEFFICIENT = 40;
const double SPEED_LIMIT = 1.0 / DIVISION_COEFFICIENT;
const double VECTOR_COEFFICIENT = 1.0 / DIVISION_COEFFICIENT;

const double LOWEST_ACCELERATION  = 0.3 / DIVISION_COEFFICIENT;
const double HIGHEST_ACCELERATION = 4.0 / DIVISION_COEFFICIENT;

const double GAMEBOUNDRY_X =  30.0;
const double GAMEBOUNDRY_Y =  20.0;
const double GAMEBOUNDRY_Z =  50.0;

// Constant Vectors
const glm::vec3 VECTOR_ZERO = glm::vec3(0.0, 0.0, 0.0);
const glm::vec3 VECTOR_UP = glm::vec3(0.0f, VECTOR_COEFFICIENT, 0.0f);
const glm::vec3 VECTOR_RIGHT = glm::vec3(VECTOR_COEFFICIENT, 0.0f, 0.0f);
const glm::vec3 VECTOR_FORWARD = glm::vec3(0.0f, 0.0f, VECTOR_COEFFICIENT);

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// Window settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;


//light settings -- currently doesn't work.
glm::vec3 lightPos(0.0f, 60.0f, 64.9f);

// Global Game values
int TOTAL_LIVES = 3;
int TOTAL_SCORE = 17;
float VAR_HUNGER = 0;


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
		double rand_x = rand() % 10;
		double rand_y = rand() % 10;
		double rand_z = rand() % 10;

		int rand_sign_x = rand() % 2;
		int rand_sign_y = rand() % 2;
		int rand_sign_z = rand() % 2;

		if (rand_sign_x == 0) rand_x *= -1.0;
		if (rand_sign_y == 0) rand_y *= -1.0;
		if (rand_sign_z == 0) rand_z *= -1.0;

		return Point(rand_x, rand_y, rand_z);
	}

	static glm::vec3 getRandomPointVector()
	{
		auto rand_pos = Point::getRandomPoint();
		return glm::vec3(rand_pos.x, rand_pos.y, rand_pos.z);
	}
};


#endif // !VALUES_H


