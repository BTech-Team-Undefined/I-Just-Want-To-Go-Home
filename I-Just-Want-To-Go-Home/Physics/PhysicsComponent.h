#pragma once

#include <vector>
#include "../EntitySystems/Entity.h"
#include "../EntitySystems/Component.h"
#include "Collider2D.h"

using namespace std;

class PhysicsComponent : public Component
{
public:
	vector<shared_ptr<Collider2D>> colliders;

public:
	PhysicsComponent(Entity* e) : Component(e) {};
	~PhysicsComponent() {};

	virtual void Update(float dt) {};
	virtual void Draw() {};

	void AddCollider(shared_ptr<Collider2D> c)
	{
		c->SetEntity(this->GetEntity());
		int id = PhysicsSystem::instance().RegisterCollider(c);
		c->colliderId = id;
		colliders.push_back(c);
	}
};