#pragma once

#include <iostream>
#include <map>
#include "IPhysicsEntity.h"

using namespace std;

class PhysicsSystem
{
public:
	static PhysicsSystem &instance() { static PhysicsSystem ps; return ps; };

	void Update();
	int RegisterObject(IPhysicsEntity entity, string entityName);

	map<int, IPhysicsEntity> _entities = {};
private:
	PhysicsSystem();

	void CheckCollisions();
	void RemoveCollision(IPhysicsEntity entityA, IPhysicsEntity entityB);
};

