#pragma once

#include "GameObject.h"

#include <GLFW/glfw3.h>

class Player
	: GameObject
{
public:
	Player();
	Player(const std::string & filePath);

	void MoveCollider(bool *keys, GLfloat deltaTime);

	void IncreasePosition(glm::vec3 position);
	void IncreaseRotation(glm::vec3 rotation);

	void SetTransform(Transform trans) { transform = trans; }

	GLfloat GetTurnSpeed() { return turnSpeed; }

	glm::vec3 GetPosition() { return transform.position; }

private:
	const GLfloat moveSpeed = 30.0f;
	const GLfloat horizontalMoveSpeed = 20.0f;
	const GLfloat turnSpeed = 0.1f;
	const GLfloat jumpPower = 0.3f;
	const GLfloat gravity = -1.0f;

	glm::vec3 before;

	GLfloat upwardsSpeed;
	GLfloat currentMoveSpeed;
	GLfloat currentHorizontalSpeed;
	GLfloat terrainHeight = 1.0f;
	
	void _GetInputs(bool *keys, GLfloat deltaTime);

	void MoveRigidBody(glm::vec3 deltaMove);
};

void Player::MoveCollider(bool * keys, GLfloat deltaTime)
{
	before = transform.position;


	_GetInputs(keys, deltaTime);
	float verticalDistance = currentMoveSpeed * deltaTime;
	float theta = transform.rotation.y; // Its already in radians


	float horDistance = currentHorizontalSpeed * deltaTime;
	float hordX = horDistance * cos(-theta);
	float hordZ = horDistance * sin(-theta);

	transform.position += glm::vec3(verticalDistance * sin(theta), 0.0f, verticalDistance * cos(theta)) + glm::vec3(hordX, 0.0f, hordZ);

	upwardsSpeed += gravity * deltaTime;
	transform.position.y += upwardsSpeed;
}

void Player::IncreasePosition(glm::vec3 addition)
{
	transform.position += addition;
}

void Player::IncreaseRotation(glm::vec3 addition)
{
	transform.rotation += addition;
}

void Player::_GetInputs(bool * keys, GLfloat deltaTime)
{
	if (keys[GLFW_KEY_W])
	{
		this->AccelerateTowards(Directions::FORWARD);
	}
	if (keys[GLFW_KEY_S])
	{
		this->AccelerateTowards(Directions::BACKWARD);
	}
	if (keys[GLFW_KEY_D])
	{
		this->AccelerateTowards(Directions::RIGHT);
	}
	if (keys[GLFW_KEY_A])
	{
		this->AccelerateTowards(Directions::LEFT);
	}
	if (keys[GLFW_KEY_SPACE])
	{
		this->AccelerateTowards(Directions::UP);
	}
	if (keys[GLFW_KEY_LEFT_CONTROL])
	{
		this->AccelerateTowards(Directions::DOWN);
	}
}

