#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Point.h"
#include "PhysicsSystem.h"
#include "..\EntitySystems\Entity.h"

using namespace std;

class Collider2D
{
public:
	int colliderId;
	string colliderName = "";
	// relative to the origin of the renderable object
	Point roughCenter = Point(0, 0);
	float roughRadius = 0;
	// relative to the origin of the renderable object
	vector<Point> collider;
	vector<int> collidingIds;

	Collider2D(string name)
	{
		this->colliderName = name;
	}

	Collider2D()
	{
	}

	virtual ~Collider2D()
	{
	}

	void SetEntity(Entity *entity)
	{
		this->_entity = entity;
	}

	virtual void OnCollision(int colliderId, string colliderName)
	{
		cout << "Collision: " + this->colliderName + "(id: " + to_string(this->colliderId) + ") & " + colliderName + "(id: " + to_string(colliderId) + ")" << endl;
	}

	void SetCollider(vector<Point> vertices, Point roughCenter, float roughRadius)
	{
		this->collider.clear();
		this->collider.insert(this->collider.end(), vertices.begin(), vertices.end());
		this->roughCenter = roughCenter;
		this->roughRadius = roughRadius;
	}

	Point GetOrigin()
	{
		glm::vec3 pos = this->_entity->position;
		return Point(pos.x, pos.z);
	}

private:
	Entity * _entity;
};

