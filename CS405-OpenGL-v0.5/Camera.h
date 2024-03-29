#ifndef CAMERA_H
#define CAMERA_H

#include "Include/glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//#include "Player.h"

#include <vector>

#include "Enums.h"
#include "values.h"

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		_updateCameraVectors();
	}
	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		_updateCameraVectors();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix();

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Directions direction, float deltaTime);

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset);
	
	glm::vec3 getPosition();

	void setPosition(glm::vec3 pos);

	void setZoom(float zoom);

	float getZoom();

private:	
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	//Player *player;
	float distanceFromPlayer = 13.0f;
	float angleAroundPlayer = 0.0f;

	// Euler Angles
	float Yaw;
	float Pitch;

	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	float _CalculateVerticalDistance();

	float _CalculateHorizontalDistance();


	// Calculates the front vector from the Camera's (updated) Euler Angles
	void _updateCameraVectors();
};

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(Directions direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
	if (direction == Directions::FORWARD)
		Position += Front * velocity;
	if (direction == Directions::BACKWARD)
		Position -= Front * velocity;
	if (direction == Directions::RIGHT)
		Position += Right * velocity;
	if (direction == Directions::LEFT)
		Position -= Right * velocity;
	if (direction == Directions::UP)
		Position += Up * velocity;
	if (direction == Directions::DOWN)
		Position -= Up * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	// _Render Front, Right and Up Vectors using the updated Euler angles
	_updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	if (Zoom >= 1.0f && Zoom <= 45.0f)
		Zoom -= yoffset;
	if (Zoom <= 1.0f)
		Zoom = 1.0f;
	if (Zoom >= 45.0f)
		Zoom = 45.0f;
}

glm::vec3 Camera::getPosition()
{
	return Position;
}

void Camera::setPosition(glm::vec3 pos)
{
	Position = pos;
}

void Camera::setZoom(float zoom)
{
	Zoom = zoom;
}

float Camera::getZoom()
{
	return this->Zoom;
}

//float Camera::_CalculateVerticalDistance()
//{
//	return (float)(distanceFromPlayer * sin(glm::radians(pitch)));
//}
//
//float Camera::_CalculateHorizontalDistance()
//{
//	return (float)(distanceFromPlayer * cos(glm::radians(pitch)));
//}

void Camera::_updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front)); 
}
#endif