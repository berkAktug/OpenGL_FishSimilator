#pragma once

#include <string>
#include <glm/glm.hpp>


class Transform
{
public:

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	Transform();
	Transform(glm::vec3 pos);
	Transform(glm::vec3 pos, glm::vec3 rot);
	Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 sca);

	std::string toString();

	~Transform();

};

Transform::Transform()
{
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

Transform::Transform(glm::vec3 pos)
{
	position = pos;
	rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

Transform::Transform(glm::vec3 pos, glm::vec3 rot)
{
	position = pos;
	rotation = rot;

}

Transform::Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 sca)
{
	position = pos;
	rotation = rot;
	scale = sca;
}

std::string Transform::toString()
{

	return std::string("\nPosition: " + std::to_string(position.x) + " ," + std::to_string(position.y) + ", " + std::to_string(position.z) +
		" - Rotation: " + std::to_string(rotation.x) + " ," + std::to_string(rotation.y) + ", " + std::to_string(rotation.z) +
		" - Scale: " + std::to_string(scale.x) + " ," + std::to_string(scale.y) + ", " + std::to_string(scale.z) + "\n");
}

Transform::~Transform()
{
}
