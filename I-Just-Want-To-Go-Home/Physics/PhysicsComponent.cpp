#include "PhysicsComponent.h";

class PhysicsSystem; // forward declaration to resolve circular dependency

void PhysicsComponent::AddCollider(shared_ptr<Collider2D> c)
{
	c->SetEntity(this->GetEntity());
	int id = PhysicsSystem::instance().RegisterCollider(c);
	c->colliderId = id;
	colliders.push_back(c);
}

void PhysicsComponent::Register()
{
	PhysicsSystem::instance().RegisterEntity(this->GetEntity());
}