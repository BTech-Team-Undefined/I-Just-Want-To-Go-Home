#include <algorithm>
#include <string>
#include "PhysicsSystem.h"
#include <math.h> //added By Johnny Kang
using namespace std;

PhysicsSystem::PhysicsSystem() : System()
{
}

// registers the collider to physics system
// and returns its collider id
int PhysicsSystem::RegisterCollider(shared_ptr<Collider2D> collider)
{
	int index = static_cast<int>(this->_colliders.size());
	this->_colliders.insert(pair<int, shared_ptr<Collider2D>>(index, collider));
	collider->colliderId = index;
	return index;
}

int PhysicsSystem::RegisterEntity(Entity* entity)
{
	int index = entity->getID();
	this->_entities.insert(pair<int, Entity*>(index, entity));
	return index;
}

// should be called every update frame
void PhysicsSystem::update(float dt)
{
	for (auto i = _entities.begin(); i != _entities.end(); ++i) {
		this->physicsUpdate(i->second, dt);
	}
	this->CheckCollisions();
}

void PhysicsSystem::CheckCollisions()
{
	map<string, pair<Entity*, Entity*>> collisions = {};
	const float NEAR_THRESHOLD = 0.02f;

	this->_justChecked.clear();
	for (int from = 0; from < this->_colliders.size(); ++from)
	{
		for (int to = 0; to < this->_colliders.size(); ++to)
		{
			// if self
			if (from == to)
				continue;

			// if just checked
			string hash = from < to ? to_string(from) + "_" + to_string(to) : to_string(to) + "_" + to_string(from);
			if (find(this->_justChecked.begin(), this->_justChecked.end(), hash) != this->_justChecked.end())
				continue;
			this->_justChecked.push_back(hash);

			shared_ptr<Collider2D> fromCollider = this->_colliders[from];
			shared_ptr<Collider2D> toCollider = this->_colliders[to];

			Point fromOrigin = fromCollider->GetOrigin();
			Point toOrigin = toCollider->GetOrigin();

			// if too far
			float innerDistance = (fromCollider->roughCenter + fromOrigin).Distance(toCollider->roughCenter + toOrigin);
			float outerDistance = innerDistance - fromCollider->roughRadius - toCollider->roughRadius;
			if (outerDistance > 0)
			{
				this->RemoveCollision(fromCollider, toCollider);
				continue;
			}

			// calculate collisions
			vector<Point> fromPoints = fromCollider->collider;
			vector<Point> toPoints = toCollider->collider;

			// radians
			float fromRotation = fromCollider->GetRotationY();
			float toRotation = toCollider->GetRotationY();

			bool collision = false;
			for (int fep = 0; fep < fromPoints.size(); ++fep)
			{
				// line points for collider A
				Point pointA = fromPoints[fep].Rotate(fromRotation) + fromOrigin;
				Point pointB = fromPoints[0];
				if (fep < fromPoints.size() - 1)
					pointB = fromPoints[fep + 1];
				pointB = pointB.Rotate(fromRotation) + fromOrigin;

				//cout << fep << "f. " << fromRotation << " (" << pointA.x << ", " << pointA.y << ")\n" << endl;

				for (int tep = 0; tep < toPoints.size(); ++tep)
				{
					// line points for collider B
					Point pointC = toPoints[tep].Rotate(toRotation) + toOrigin;
					Point pointD = toPoints[0];
					if (tep < toPoints.size() - 1)
						pointD = toPoints[tep + 1];
					pointD = pointD.Rotate(toRotation) + toOrigin;
					
					//cout << tep << "t. " << toRotation << " (" << pointC.x << ", " << pointC.y << ")\n" << endl;

					if (pointA.Near(pointC) || pointA.Near(pointD) || pointB.Near(pointC) || pointB.Near(pointD))
					{
						collision = true;
						break;
					}

					bool lineAVertical = fabs(pointA.x - pointB.x) < NEAR_THRESHOLD;
					bool lineBVertical = fabs(pointC.x - pointD.x) < NEAR_THRESHOLD;
					bool lineAHorizontal = fabs(pointA.y - pointB.y) < NEAR_THRESHOLD;
					bool lineBHorizontal = fabs(pointC.y - pointD.y) < NEAR_THRESHOLD;

					// if both are vertical
					if (lineAVertical && lineBVertical)
					{
						// parallel
						if (fabs(pointA.x - pointC.x) >= NEAR_THRESHOLD)
							continue;

						if (fmin(pointC.y, pointD.y) <= pointA.y && pointA.y <= fmax(pointC.y, pointD.y) ||
							fmin(pointC.y, pointD.y) <= pointB.y && pointB.y <= fmax(pointC.y, pointD.y) ||
							fmin(pointA.y, pointB.y) <= pointC.y && pointC.y <= fmax(pointA.y, pointB.y) ||
							fmin(pointA.y, pointB.y) <= pointD.y && pointD.y <= fmax(pointA.y, pointB.y))
						{
							collision = true;
							break;
						}
						else
							continue;
					}

					// if both are horizontal
					if (lineAHorizontal && lineBHorizontal)
					{
						// parallel
						if (fabs(pointA.y - pointC.y) >= NEAR_THRESHOLD)
							continue;

						if (fmin(pointC.x, pointD.x) <= pointA.x && pointA.x <= fmax(pointC.x, pointD.x) ||
							fmin(pointC.x, pointD.x) <= pointB.x && pointB.x <= fmax(pointC.x, pointD.x) ||
							fmin(pointA.x, pointB.x) <= pointC.x && pointC.x <= fmax(pointA.x, pointB.x) ||
							fmin(pointA.x, pointB.x) <= pointD.x && pointD.x <= fmax(pointA.x, pointB.x))
						{
							collision = true;
							break;
						}
						else
							continue;
					}

					// if lineA is vertical
					if (lineAVertical)
					{
						if (lineBHorizontal)
						{
							if (fmin(pointC.x, pointD.x) < pointA.x && fmax(pointC.x, pointD.x) > pointA.x &&
								fmin(pointC.y, pointD.y) < fmax(pointA.y, pointB.y) && fmax(pointC.y, pointD.y) > fmin(pointA.y, pointB.y))
							{
								collision = true;
								break;
							}
							else
								continue;
						}
						else
						{
							float x = pointA.x;
							if (fmin(pointC.x, pointD.x) <= x && x <= fmax(pointC.x, pointD.x))
							{
								float m = (pointD.y - pointC.y) / (pointD.x - pointC.x);
								float b = pointC.y - pointC.x * m;
								float iY = m * x + b;
								if (fmin(pointA.y, pointB.y) <= iY && iY <= fmax(pointA.y, pointB.y))
								{
									collision = true;
									break;
								}
								else
									continue;
							}
							else
								continue;
						}
					}

					// if lineB is vertical
					if (lineBVertical)
					{
						if (lineAHorizontal)
						{
							if (fmin(pointA.x, pointB.x) < pointC.x && fmax(pointA.x, pointB.x) > pointC.x &&
								fmin(pointA.y, pointB.y) < fmax(pointC.y, pointD.y) && fmax(pointA.y, pointB.y) > fmin(pointC.y, pointD.y))
							{
								collision = true;
								break;
							}
							else
								continue;
						}
						else
						{
							float x = pointC.x;
							if (fmin(pointA.x, pointB.x) <= x && x <= fmax(pointA.x, pointB.x))
							{
								float m = (pointB.y - pointA.y) / (pointB.x - pointA.x);
								float b = pointA.y - pointA.x * m;
								float iY = m * x + b;
								if (fmin(pointC.y, pointD.y) <= iY && iY <= fmax(pointC.y, pointD.y))
								{
									collision = true;
									break;
								}
								else
									continue;
							}
							else
								continue;
						}
					}

					float a1 = (pointB.y - pointA.y) / (pointB.x - pointA.x);
					float b1 = pointA.y - a1 * pointA.x;
					float a2 = (pointD.y - pointC.y) / (pointD.x - pointC.x);
					float b2 = pointC.y - a2 * pointC.x;

					if (fabs(a1 - a2) < NEAR_THRESHOLD && fabs(b1 - b2) < NEAR_THRESHOLD)
					{
						if (fabs(pointA.x - pointC.x) < NEAR_THRESHOLD && fabs(pointB.x - pointD.x) < NEAR_THRESHOLD ||
							fabs(pointA.x - pointD.x) < NEAR_THRESHOLD && fabs(pointB.x - pointC.x) < NEAR_THRESHOLD)
						{
							collision = true;
							break;
						}
					}

					float x0 = -(b1 - b2) / (a1 - a2);
					if (fmin(pointA.x, pointB.x) <= x0 && x0 <= fmax(pointA.x, pointB.x) &&
						fmin(pointC.x, pointD.x) <= x0 && x0 <= fmax(pointC.x, pointD.x))
					{
						collision = true;
						break;
					}
				}
				if (collision)
					break;
			}

			if (collision)
			{
				// if not already colliding
				int fromID = fromCollider->GetEntity()->getID();
				int toID = toCollider->GetEntity()->getID();
				Entity* higher = fromID > toID ? fromCollider->GetEntity() : toCollider->GetEntity();
				Entity* lower = fromID > toID ? toCollider->GetEntity() : fromCollider->GetEntity();
				if (collisions.count(lower->getID() + "x" + higher->getID()) == 0 && lower != higher) {
					collisions.insert(
						pair<string, pair<Entity*, Entity*>>(
							lower->getID() + "x" + higher->getID(),
							pair<Entity*, Entity*>(lower, higher)
						)
					);
				}
				if (find(fromCollider->collidingIds.begin(), fromCollider->collidingIds.end(), toCollider->colliderId) == fromCollider->collidingIds.end())
				{
					fromCollider->OnCollision(toCollider->colliderId, toCollider->colliderName);
					fromCollider->collidingIds.push_back(toCollider->colliderId);
				}
				if (find(toCollider->collidingIds.begin(), toCollider->collidingIds.end(), fromCollider->colliderId) == toCollider->collidingIds.end())
				{
					toCollider->OnCollision(fromCollider->colliderId, fromCollider->colliderName);
					toCollider->collidingIds.push_back(fromCollider->colliderId);
				}
			}
			else
			{
				// no collision
				this->RemoveCollision(fromCollider, toCollider);
			}
		}
	}
	for (auto i = collisions.begin(); i != collisions.end(); i++) {
		ResolveCollision(i->second.first, i->second.second);
	}
}

void PhysicsSystem::RemoveCollision(shared_ptr<Collider2D> colliderA, shared_ptr<Collider2D> colliderB)
{
	// if already colliding
	// todo: add OnExitCollision?
	vector<int>::iterator it;

	it = find(colliderA->collidingIds.begin(), colliderA->collidingIds.end(), colliderB->colliderId);
	if (it != colliderA->collidingIds.end())
	{
		int index = static_cast<int>(distance(colliderA->collidingIds.begin(), it));
		colliderA->collidingIds.erase(colliderA->collidingIds.begin() + index);
	}

	it = find(colliderB->collidingIds.begin(), colliderB->collidingIds.end(), colliderA->colliderId);
	if (it != colliderB->collidingIds.end())
	{
		int index = static_cast<int>(distance(colliderB->collidingIds.begin(), it));
		colliderB->collidingIds.erase(colliderB->collidingIds.begin() + index);
	}
}

void PhysicsSystem::physicsUpdate(Entity* e, float delta) {
	const float gravity = 9.8; // Acceleration from gravity; for purpose of calculating friction
	auto pc = e->getComponent<PhysicsComponent>();
	float im = 1 / pc->mass;
	float length = 4; // length of the chasis. Added by Johnny Kang
	PhysicsVector f = pc->force;
	float af = pc->angularForce;
	PhysicsVector v = pc->velocity;
	float av = pc->angularVelocity;

	if (v.length() > 0) {
		float drag = -v.dot(v) * pc->mass * pc->getDrag();
		float fric = -pc->mass * gravity * pc->getFriction();
		f += (drag + fric) * v.unit(); // fric is already negative so we don't need to worry about sign
	}

	if (av != 0) {
		float adrag = -av * av * pc->mass * pc->getRotationDrag();
		float fric = -pc->mass * gravity * pc->getRotationFriction();
		af += (adrag + fric) * (av > 0 ? 1 : -1);
	}

	PhysicsVector a = f * im;
	float aa = af * im;

	PhysicsVector pos = PhysicsVector(e->position.x, e->position.z);
	pos += v * delta + 0.5 * a * delta * delta;// use the curPos variable to move the entity
	v += a * delta;

	float rot = e->rotation.y;
	rot += av * delta + 0.5 * aa * delta * delta;
	av += aa * delta;

	if (av < 0.05 && av > -0.05) {
		av = 0.0f;
	}

	if (pc->velocity.x < 0.05 && pc->velocity.x > -0.05) {
		pc->velocity.x = 0.0f;
	}

	if (pc->velocity.y < 0.05 && pc->velocity.y > -0.05) {
		pc->velocity.y = 0.0f;
	}
	
	pc->velocity.x = v.x;
	pc->velocity.y = v.y;
	pc->angularVelocity = av;

	e->position = glm::vec3(pos.x, e->position.y, pos.y);
	e->rotation = glm::vec3(e->rotation.x, rot, e->rotation.z);
}

void PhysicsSystem::ResolveCollision(Entity* e1, Entity* e2) {
	auto pc1 = e1->getComponent<PhysicsComponent>();
	auto pc2 = e2->getComponent<PhysicsComponent>();
	if (pc1->isStatic && pc2->isStatic) {
		return; // Both are static; no point in calculating anything
	}
	float im1 = 1 / pc1->mass;
	float im2 = 1 / pc2->mass;
	if (pc1->isStatic) {
		im1 = 0;
	}
	if (pc2->isStatic) {
		im2 = 0;
	}
	PhysicsVector dist = PhysicsVector(e2->position.x - e1->position.x, e2->position.z - e1->position.z);
	PhysicsVector n = dist.unit();
	// Bad positional correction
	const float ratio = 0.035;
	PhysicsVector corr1 = -n * ratio * im1;
	PhysicsVector corr2 = n * ratio * im2;
	e1->position = glm::vec3(e1->position.x + corr1.x, e1->position.y, e1->position.z + corr1.y);
	e2->position = glm::vec3(e2->position.x + corr2.x, e2->position.y, e2->position.z + corr2.y);
	float vNorm = (pc2->velocity - pc1->velocity).dot(n);
	if (vNorm > 0) {
		return;
	}

	float e = pc1->elasticity < pc2->elasticity ? pc1->elasticity : pc2->elasticity;
	float j = -(1 + e) * vNorm / (im1 + im2);
	PhysicsVector impulse = j * n;
	pc1->velocity -= impulse * im1;
	pc2->velocity += impulse * im2;
}

void PhysicsSystem::addComponent(std::type_index t, Component* component) {
	if (t == std::type_index(typeid(PhysicsComponent))) {
		PhysicsComponent* pc = static_cast<PhysicsComponent*>(component);
		RegisterEntity(pc->getEntity());
		for (auto c : pc->colliders) {
			RegisterCollider(c);
		}
	}
}

void PhysicsSystem::clearComponents() {
	_colliders.clear();
	_entities.clear();
}