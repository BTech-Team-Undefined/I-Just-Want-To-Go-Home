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
	bool hasPhysics = true;
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

	void SetEntity(Entity *entity);

	Entity* GetEntity();

	virtual void OnCollision(int colliderId, string colliderName);

	void SetCollider(vector<Point> vertices, Point roughCenter, float roughRadius);

	Point GetOrigin();

	float GetRotationY();

private:
	Entity * _entity;
};

