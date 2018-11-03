#include "PhysicsComponent.h";

class PhysicsSystem; // forward declaration to resolve circular dependency

void PhysicsComponent::AddCollider(shared_ptr<Collider2D> c)
{
	c->SetEntity(this->getEntity());
	colliders.push_back(c);
}