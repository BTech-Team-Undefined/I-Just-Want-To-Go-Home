#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <ctime>
#include "Collider2D.h"
#include "PhysicsVector.h"
#include "PhysicsComponent.h"

class Collider2D;

using namespace std;

class PhysicsSystem
{
public:
	static PhysicsSystem &instance() { static PhysicsSystem ps; return ps; };

	void Update();
	int RegisterCollider(shared_ptr<Collider2D> collider);
	int RegisterEntity(Entity* entity);
	map<int, shared_ptr<Collider2D>> _colliders = {};
	map<int, Entity*> _entities = {};
	float timeelapsed;
	int frame = 0;
	float fixedDeltatime = 20.0/1000.0;
    float constantAcceleration;
	void physicsUpdate(Entity* e);

private:
	vector<string> _justChecked;
	clock_t _lastUpdate;

	PhysicsSystem();

	void CheckCollisions();
	void RemoveCollision(shared_ptr<Collider2D> colliderA, shared_ptr<Collider2D> colliderB);

	
};

