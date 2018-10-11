#include "PhysicsSystem.h"

using namespace std;

PhysicsSystem::PhysicsSystem()
{
}

// registers the physics entity to physics system
// and returns its physics id
int PhysicsSystem::RegisterObject(IPhysicsEntity entity, string entityName)
{
	int index = static_cast<int>(this->_entities.size());
	this->_entities.insert(pair<int, IPhysicsEntity>(index, entity));
	this->_entitieNames.insert(pair<int, string>(index, entityName));
	return index;
}

// should be called every update frame
void PhysicsSystem::Update()
{

}

void PhysicsSystem::CheckCollisions()
{

}
