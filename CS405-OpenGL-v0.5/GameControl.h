#ifndef GAMECONTROL_H
#define GAMECONTROL_H

#include "Game.h"
/*
	There is a bug where object appears upside down when
	mouse starts to move very fast. invastivage this bug.
*/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class GameControl
{
public:
	
	static GameControl &getIntance();
	void init();

	~GameControl();

	void computeMatricesFromInputs();

	glm::mat4 getProjectionMatrix();

	glm::mat4 getViewMatrix();

private:
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	glm::vec3 position;
	float horizintalAngle;
	float verticalAngle;
	float initialFoV;

	float speed;
	float mouseSpeed;

	GameControl();
};

#endif