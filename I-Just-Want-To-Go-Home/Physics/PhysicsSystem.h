#pragma once

#include <iostream>
#include <map>
#include <memory>
#include "Collider2D.h"

using namespace std;

class PhysicsSystem
{
public:
	static PhysicsSystem &instance() { static PhysicsSystem ps; return ps; };

	void Update();
	int RegisterCollider(shared_ptr<Collider2D> collider);

	map<int, shared_ptr<Collider2D>> _colliders = {};
private:
	PhysicsSystem();

	void CheckCollisions();
	void RemoveCollision(shared_ptr<Collider2D> colliderA, shared_ptr<Collider2D> colliderB);
};

