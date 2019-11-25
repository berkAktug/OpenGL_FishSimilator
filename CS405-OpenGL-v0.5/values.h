#ifndef VALUES_H
#define VALUES_H

#include "glm/glm.hpp"

// Default Object movement values
const glm::vec3 VECTOR_ZERO = glm::vec3(0.0, 0.0, 0.0);
const double SPEED_LIMIT = 0.1;
const double ACCELERATE_RATE = 0.03;
const double STOP_SPEED = 0.02;
const double FRICTION_COEFFICIENT = 1.0;
const double FRICTION_LIMIT = 0.08;


//const double GAMEZONE_MAX_X =  20.0;
//const double GAMEZONE_MIN_X =	0.0;
//const double GAMEZONE_MAX_Y =  50.0;
//const double GAMEZONE_MIN_Y = -50.0;
//const double GAMEZONE_MAX_Z =  50.0;
//const double GAMEZONE_MIN_Z = -50.0;

const double FLOOR_DIMENSION_X = 20.0;
const double FLOOR_DIMENSION_Z = 20.0;
const double FLOOR_DIMENSION_Y = 0.1;

// temporary Vectors
const glm::vec3 VECTOR_Y_POSITIVE = glm::vec3(0.0f, 0.1f, 0.0f);

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

#endif // !VALUES_H


