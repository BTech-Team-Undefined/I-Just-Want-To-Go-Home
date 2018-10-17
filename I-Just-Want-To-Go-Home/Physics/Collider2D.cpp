#include "Collider2D.h"

void Collider2D::SetEntity(Entity *entity)
{
	this->_entity = entity;
}

Entity* Collider2D::GetEntity() {
	return this->_entity;
}

void Collider2D::OnCollision(int colliderId, string colliderName)
{
	cout << "Collision: " + this->colliderName + "(id: " + to_string(this->colliderId) + ") & " + colliderName + "(id: " + to_string(colliderId) + ")" << endl;
}

void Collider2D::SetCollider(vector<Point> vertices, Point roughCenter, float roughRadius)
{
	this->collider.clear();
	this->collider.insert(this->collider.end(), vertices.begin(), vertices.end());
	this->roughCenter = roughCenter;
	this->roughRadius = roughRadius;
}

Point Collider2D::GetOrigin()
{
	glm::vec3 pos = this->_entity->position;
	return Point(pos.x, pos.z);
}

float Collider2D::GetRotationY()
{
	return this->_entity->rotation.y;
}