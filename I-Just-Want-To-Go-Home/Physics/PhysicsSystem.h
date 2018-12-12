#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <ctime>
#include "Collider2D.h"
#include "PhysicsVector.h"
#include "PhysicsComponent.h"
#include "Collision.h"
#include "../EntitySystems/System.h"

class Collider2D;

using namespace std;

class PhysicsSystem : public System
{
public:
	PhysicsSystem();
	int RegisterCollider(shared_ptr<Collider2D> collider);
	int RegisterEntity(Entity* entity);
	map<int, shared_ptr<Collider2D>> _colliders = {};
	map<int, Entity*> _entities = {};
	float timeelapsed;
	int frame = 0;
    float constantAcceleration;
	void physicsUpdate(Entity* e, float delta);
	virtual void update(float dt) override;
	virtual void addComponent(std::type_index t, Component* component) override;
	virtual void clearComponents() override;

private:
	vector<string> _justChecked;
	void CheckCollisions();
	void ResolveCollision(Collision* c);
	void RemoveCollision(shared_ptr<Collider2D> colliderA, shared_ptr<Collider2D> colliderB);

	
};

