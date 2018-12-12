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
	float dragCoefficient = 0.05f;
	float frictionCoefficient = 0.03f;
	float latDragCoefficient = 0.33;
	float latFricCoefficient = 0.85;
	float rotDragCoefficient = 0.35;
	float rotFricCoefficient = 0.6;
	bool isStatic = false;
	bool hasPhysicsCollision = true;
	PhysicsVector velocity;
	PhysicsVector force;
	float angularForce;
	float angularVelocity;

	bool directionalDrag;

public:
	PhysicsComponent() : Component(std::type_index(typeid(PhysicsComponent))) {};
	~PhysicsComponent() {};

	virtual void update(float dt) override {};
	virtual void draw() {};

	virtual float getFriction();
	virtual float getDrag();

	virtual float getRotationDrag();
	virtual float getRotationFriction();

	void AddCollider(shared_ptr<Collider2D> c);
};