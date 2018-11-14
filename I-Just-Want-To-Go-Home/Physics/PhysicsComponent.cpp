#include "PhysicsComponent.h";

class PhysicsSystem; // forward declaration to resolve circular dependency

void PhysicsComponent::AddCollider(shared_ptr<Collider2D> c)
{
	c->SetEntity(this->getEntity());
	colliders.push_back(c);
}

float PhysicsComponent::getFriction() {
	if (directionalDrag) {
		if (velocity.x == 0 && velocity.y == 0) {
			// If we don't have velocity, default to latitudinal friction coefficient
			return latFricCoefficient;
		}
		float rot = getEntity()->rotation.y;
		PhysicsVector fricVec = PhysicsVector(frictionCoefficient, latFricCoefficient);
		PhysicsVector dir = PhysicsVector(velocity.x * sin(-rot), velocity.x * cos(-rot))
			+ PhysicsVector(velocity.y * sin(-rot - 1.57), velocity.y * cos(-rot - 1.57));
		PhysicsVector unitDir = dir.unit();

		// All vectors should be positive
		PhysicsVector fricVec2 = PhysicsVector(abs(fricVec.x), abs(fricVec.y));
		PhysicsVector dir2 = PhysicsVector(abs(unitDir.x), abs(unitDir.y));
		return fricVec2.dot(dir2);
	}
	else {
		return frictionCoefficient;
	}
}

float PhysicsComponent::getDrag() {
	if (directionalDrag) {
		if (velocity.x == 0 && velocity.y == 0) {
			// If we don't have velocity, default to latitudinal drag coefficient
			return latDragCoefficient;
		}
		float rot = getEntity()->rotation.y;
		// Treat the standard drag coefficient as the longitudinal drag
		PhysicsVector dragVec = PhysicsVector(dragCoefficient, latDragCoefficient);
		PhysicsVector dir = PhysicsVector(velocity.x * sin(-rot), velocity.x * cos(-rot))
			+ PhysicsVector(velocity.y * sin(-rot - 1.57), velocity.y * cos(-rot - 1.57));
		PhysicsVector unitDir = dir.unit();

		// All vectors should be positive
		PhysicsVector dragVec2 = PhysicsVector(abs(dragVec.x), abs(dragVec.y));
		PhysicsVector dir2 = PhysicsVector(abs(unitDir.x), abs(unitDir.y));

		return dragVec2.dot(dir2);
	}
	else {
		return dragCoefficient;
	}
}

float PhysicsComponent::getRotationDrag() {
	return rotDragCoefficient;
}

float PhysicsComponent::getRotationFriction() {
	return rotFricCoefficient;
}