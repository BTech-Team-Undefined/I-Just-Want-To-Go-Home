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

Point Collider2D::GetCenter()
{
	return GetOrigin() + GetLocalCenter();
}

Point Collider2D::GetLocalCenter()
{
	Point c = Point(0, 0);
	float weight = 1.0 / collider.size();
	for (auto a = collider.begin(); a < collider.end(); a++) {
		c.x += a->x * weight;
		c.y += a->y * weight;
	}
	return c;
}

float Collider2D::GetRotationY()
{
	return this->_entity->rotation.y;
}