#pragma once

#include <vector>
#include "../EntitySystems/Entity.h"
#include "../EntitySystems/Component.h"
#include "Collider2D.h"
#include "PhysicsVector.h"
using namespace std;

class Collider2D;

class PhysicsComponent : public Component
{
public:
	vector<shared_ptr<Collider2D>> colliders;
	float mass = 1.0f;
	float elasticity = 0.35f;
	float dragCoefficient = 0.1f;
	float frictionCoefficient = 0.1f;
	bool isStatic = false;
	PhysicsVector velocity;
	PhysicsVector force;
	float angularForce;
	float angularVelocity;

public:
	PhysicsComponent(Entity* e) : Component(e) {};
	~PhysicsComponent() {};

	virtual void Update(float dt) {};
	virtual void Draw() {};

	void AddCollider(shared_ptr<Collider2D> c);

	void Register();
};