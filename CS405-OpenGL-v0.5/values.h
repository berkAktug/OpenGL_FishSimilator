#ifndef VALUES_H
#define VALUES_H

#include "glm/glm.hpp"

// Default Object movement values
const glm::vec3 VECTOR_ZERO = glm::vec3(0.0, 0.0, 0.0);

const double divisionCoefficient = 40;
const double SPEED_LIMIT = 1.0 / divisionCoefficient;
const double ACCELERATE_RATE = 0.6 / divisionCoefficient;
const double STOP_SPEED = 0.4 / divisionCoefficient;
const double FRICTION_COEFFICIENT = 20.0 / divisionCoefficient;
const double FRICTION_LIMIT = 1.6 / divisionCoefficient;

const double TempSpeed = SPEED_LIMIT;

const double GAMEBOUNDRY_X =  30.0;
const double GAMEBOUNDRY_Y =  20.0;
const double GAMEBOUNDRY_Z =  50.0;

// temporary Vectors
const glm::vec3 VECTOR_Y_POSITIVE = glm::vec3(0.0f, 0.1f, 0.0f);

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
int VAR_TOTAL_LIVES = 3;
int VAR_TOTAL_SCORE = 17;
float VAR_HUNGER = 0;

#endif // !VALUES_H


