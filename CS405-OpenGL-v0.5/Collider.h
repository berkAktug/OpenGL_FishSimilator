#pragma once

#include <glm/glm.hpp>
#include <iostream>

#include "values.h"

class Collider
{
public:

	Collider() : max(), min()
	{}

	Collider(glm::vec3 max, glm::vec3 min)
		: max(max), min(min), center()
	{
		center.x = (max.x - min.x) / 2;
		center.y = (max.y - min.y) / 2;
		center.z = (max.z - min.z) / 2;
	}

	static bool CheckIntersect(Collider a, Collider b) {
		return  (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
			(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
			(a.min.z <= b.max.z && a.max.z >= b.min.z);
	}

	void ScaleCollider(glm::vec3 scale);
	void MoveCollider(const glm::vec3 &vec);
	void MoveColliderTo(glm::vec3 point);
	bool IsInsideBoxBoxAABB(const Collider & biggerBox);
	bool IsPointInsideAABB(glm::vec3 point);
	void PrintCollider();

	bool CheckCollusion(Collider *other);
	void DoCollusion(Collider *other);
	void DoBoundryCollusion();


	glm::vec3 GetMax() { return max; }
	glm::vec3 GetMin() { return min; }
	glm::vec3 GetCenter() { return center; }

	glm::vec3 GetDistance(const Collider *other) 
	{
		return center - other->center;
	}

private:
	glm::vec3 max;
	glm::vec3 min;
	glm::vec3 center;

	void _AdjustCenter();
	void _CheckBoxBoundry();
	void _SolveSurfaceCollusion();
	bool _CheckIsInGameField();
	void _SolveCollusionBox(Collider &other);
};

static Collider gameBoundry(glm::vec3(GAMEBOUNDRY_X, GAMEBOUNDRY_Y, GAMEBOUNDRY_Z),
	glm::vec3(-GAMEBOUNDRY_X, -GAMEBOUNDRY_Y, -GAMEBOUNDRY_Z));


void Collider::ScaleCollider(glm::vec3 scale)
{
	double depth = max.x - min.x;
	double height= max.y - min.y;
	double width = max.z - min.z;

	double diff_depth = (depth  * scale.x) - depth;
	double diff_height = (height * scale.y) - height;
	double diff_width = (width  * scale.z) - width;

	max.x += diff_depth / 2;
	min.x -= diff_depth / 2;

	max.z += diff_width / 2;
	min.z -= diff_width / 2;

	max.y += diff_height / 2;
	min.y -= diff_height / 2;

	_CheckBoxBoundry();
}

void Collider::MoveCollider(const glm::vec3 & vec)
{
	max.x += vec.x;
	min.x += vec.x;

	max.y += vec.y;
	min.y += vec.y;

	max.z += vec.z;
	min.z += vec.z;

	center.x += vec.x;
	center.y += vec.y;
	center.z += vec.z;
}

void Collider::MoveColliderTo(glm::vec3 point)
{
	_CheckBoxBoundry();

	double dist_x = max.x - min.x;
	double dist_y = max.y - min.y;
	double dist_z = max.z - min.z;

	max.x = point.x + dist_x / 2;
	min.x = point.x - dist_x / 2;

	max.y = point.y + dist_y / 2;
	min.y = point.y - dist_y / 2;

	max.z = point.z + dist_z / 2;
	min.z = point.z - dist_z / 2;

	_AdjustCenter();
}

bool Collider::IsInsideBoxBoxAABB(const Collider & biggerBox)
{
	auto is_inside_x = false;
	auto is_inside_y = false;
	auto is_inside_z = false;

	auto is_inside_all = true;

	if (biggerBox.max.x > this->max.x && biggerBox.min.x < this->min.x)
	{
		is_inside_x = true;
	}

	if (biggerBox.max.y > this->max.y && biggerBox.min.y < this->min.y)
	{
		is_inside_y = true;
	}

	if (biggerBox.max.z > this->max.z && biggerBox.min.z < this->min.z)
	{
		is_inside_z = true;
	}

	is_inside_all = is_inside_x && is_inside_y && is_inside_z;

	return is_inside_all;
}

bool Collider::IsPointInsideAABB(glm::vec3 point) {
	return (point.x >= min.x && point.x <= max.x) &&
		(point.y >= min.y && point.y <= max.y) &&
		(point.z >= min.z && point.z <= max.z);
}

void Collider::PrintCollider()
{
	std::cout
		<< "max_X: " << max.x
		<< " max Y: " << max.y
		<< " max Z: " << max.z << std::endl
		<< "min X: " << min.x
		<< " min Y: " << min.y
		<< " min Z: " << min.z << std::endl
		<< "Center X: " << center.x
		<< " Center Y: " << center.y
		<< " Center Z: " << center.z << std::endl;
}

bool Collider::CheckCollusion(Collider *other)
{
	return CheckIntersect(*this, *other);
}

void Collider::DoCollusion(Collider *other)
{
	if (CheckCollusion(other))
	{
		std::cout << "Collusion Detected AABB Box" << std::endl;
		PrintCollider();
		other->PrintCollider();

		_SolveCollusionBox(*other);
	}
}

void Collider::DoBoundryCollusion()
{
	if (!this->_CheckIsInGameField())
	{
		// lhs object is not within game field.
		std::cout << "Object is not in gamefield. Returning them to the mother base. Over." << std::endl;
		std::cout << "Before function coordinates are: ";
		PrintCollider();

		_SolveSurfaceCollusion();
	}
}

void Collider::_AdjustCenter()
{
	auto center_x = min.x + (max.x - min.x) / 2;
	auto center_y = min.y + (max.y - min.y) / 2;
	auto center_z = min.z + (max.z - min.z) / 2;

	center = glm::vec3(center_x, center_y, center_z);
}

void Collider::_CheckBoxBoundry()
{
	if (max.x < min.x)
	{
		std::swap(max.x, min.x);
	}
	if (max.y < min.y)
	{
		std::swap(max.y, min.y);
	}
	if (max.z < min.z)
	{
		std::swap(max.z, min.z);
	}
}

void Collider::_SolveSurfaceCollusion()
{
	auto dist_x = center.x;
	auto dist_y = center.y;
	auto dist_z = center.z;

	if (max.x > gameBoundry.max.x)
	{
		dist_x = gameBoundry.max.x - max.x;
	}
	else if (min.x < gameBoundry.min.x)
	{
		dist_x = min.x - gameBoundry.min.x;
	}

	if (max.y > gameBoundry.max.y)
	{
		dist_y = gameBoundry.max.y - max.y;
	}
	else if (min.y < gameBoundry.min.y)
	{
		dist_y = min.y - gameBoundry.min.y;
	}

	if (max.z > gameBoundry.max.z)
	{
		dist_z = gameBoundry.max.z - max.z;
	}
	else if (min.z < gameBoundry.min.z)
	{
		dist_z = min.z - gameBoundry.min.z;
	}

	MoveColliderTo(glm::vec3(dist_x, dist_y, dist_z));
}

bool Collider::_CheckIsInGameField()
{
	if (IsInsideBoxBoxAABB(gameBoundry))
	{
		return true;
	}
	return false;
}

void Collider::_SolveCollusionBox(Collider & other)
{
	double dist_x = 0;
	double dist_y = 0;
	double dist_z = 0;

	/*
	Case 1: This object has engulfed Other object.
	Case 2: This object is on top the of Other object.
	Case 3: This object is under the Other object.
	Case 4: This object has been engulfed by Other object.
	*/
	if (other.max.x > min.x && other.max.x < max.x)
	{
		// Covers case 1 && 2
		dist_x = other.max.x - min.x;
	}
	else if (max.x > other.min.x && max.x < other.max.x)
	{
		// Covers case 3 && 4
		dist_x = max.x - other.min.x;
	}
	dist_x = dist_x / 2;

	if (other.max.y > min.y && other.max.y < max.y)
	{
		// Covers case 1 && 2
		dist_y = other.max.y - min.y;
	}
	else if (max.y > other.min.y && max.y < other.max.y)
	{
		// Covers case 3 && 4
		dist_y = max.y - other.min.y;
	}
	dist_y = dist_y / 2;

	if (other.max.z > min.z && other.max.z < max.z)
	{
		// Covers case 1 && 2
		dist_z = other.max.z - min.z;
	}
	else if (max.z > other.min.z && max.z < other.max.z)
	{
		// Covers case 3 && 4
		dist_z = max.z - other.min.z;
	}
	dist_z = dist_z / 2;

	auto push_dist = glm::vec3(dist_x, dist_y, dist_z);

	MoveCollider(push_dist);
	other.MoveCollider(-push_dist);
}
