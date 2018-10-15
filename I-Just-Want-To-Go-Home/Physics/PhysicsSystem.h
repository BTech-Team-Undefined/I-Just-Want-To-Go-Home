#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <ctime>
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
	vector<string> _justChecked;
	clock_t _lastUpdate;

	PhysicsSystem();

	void CheckCollisions();
	void RemoveCollision(shared_ptr<Collider2D> colliderA, shared_ptr<Collider2D> colliderB);
};

