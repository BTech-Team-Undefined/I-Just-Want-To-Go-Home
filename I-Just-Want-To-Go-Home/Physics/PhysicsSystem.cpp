#include <algorithm>
#include <string>
#include "PhysicsSystem.h"

using namespace std;

PhysicsSystem::PhysicsSystem()
{
	this->_lastUpdate = clock();
}

// registers the collider to physics system
// and returns its collider id
int PhysicsSystem::RegisterCollider(shared_ptr<Collider2D> collider)
{
	int index = static_cast<int>(this->_colliders.size());
	this->_colliders.insert(pair<int, shared_ptr<Collider2D>>(index, collider));
	return index;
}

int PhysicsSystem::RegisterEntity(Entity* entity)
{
	int index = entity->getID();
	this->_entities.insert(pair<int, Entity*>(index, entity));
	return index;
}

// should be called every update frame
void PhysicsSystem::Update()
{
	// update once every 20ms (50 times in 1 second)
	// same as unity's default setting for FixedUpdate
	const double UPDATE_THRESHOLD = 20;

	clock_t currentTime = clock();
	double diff = currentTime - this->_lastUpdate;
	double diffMilliseconds = (diff) / (CLOCKS_PER_SEC / 1000);
	if (diffMilliseconds < UPDATE_THRESHOLD)
		return;
	this->CheckCollisions();
	this->_lastUpdate = currentTime;
}

void PhysicsSystem::CheckCollisions()
{
	map<string, pair<Entity*, Entity*>> collisions = {};
	const float NEAR_THRESHOLD = 0.02f;

	for (int i = 0; i < this->_entities.size(); ++i) {
		this->physicsUpdate(this->_entities[i]);
	}

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

void PhysicsSystem::physicsUpdate(Entity* e) {
	auto pc = e->getComponent<PhysicsComponent>();
	
	PhysicsVector f = pc->force;
	PhysicsVector a = f * (1 / pc->mass);
	PhysicsVector v = pc->velocity;
	PhysicsVector pos = PhysicsVector(e->position.x, e->position.z);
	pos += v * fixedDeltatime + 0.5 * v * fixedDeltatime * fixedDeltatime;// use the curPos variable to move the entity
	v += a * fixedDeltatime;

	pc->velocity.x = v.x;
	pc->velocity.y = v.y;

	e->position = glm::vec3(pos.x, e->position.y, pos.y);
}

void PhysicsSystem::ResolveCollision(Entity* e1, Entity* e2) {
	auto pc1 = e1->getComponent<PhysicsComponent>();
	auto pc2 = e2->getComponent<PhysicsComponent>();
	float im1 = 1 / pc1->mass;
	float im2 = 1 / pc2->mass;
	PhysicsVector n = PhysicsVector(e2->position.x - e1->position.x, e2->position.z - e1->position.z).unit();
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



