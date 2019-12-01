#ifndef VALUES_H
#define VALUES_H

#include "glm/glm.hpp"

// Default Object movement values

const double DIVISION_COEFFICIENT = 10;
const double VECTOR_COEFFICIENT = 1.0 / DIVISION_COEFFICIENT;

const double LOWEST_ACCELERATION  = 0.3 / DIVISION_COEFFICIENT;
const double HIGHEST_ACCELERATION = 1.0 / DIVISION_COEFFICIENT;

const double GAMEBOUNDRY_X =  50.0;
const double GAMEBOUNDRY_Y =  50.0;
const double GAMEBOUNDRY_Z =  50.0;

// Constant Vectors
const glm::vec3 VECTOR_ZERO = glm::vec3(0.0, 0.0, 0.0);
const glm::vec3 VECTOR_UP = glm::vec3(0.0f, VECTOR_COEFFICIENT, 0.0f);
const glm::vec3 VECTOR_LEFT = glm::vec3(VECTOR_COEFFICIENT, 0.0f, 0.0f);
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

#endif // !VALUES_H


