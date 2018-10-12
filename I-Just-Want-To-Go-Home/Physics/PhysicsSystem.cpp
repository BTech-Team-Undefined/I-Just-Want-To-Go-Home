#include <algorithm>
#include "PhysicsSystem.h"

using namespace std;

PhysicsSystem::PhysicsSystem()
{
}

// registers the collider to physics system
// and returns its collider id
int PhysicsSystem::RegisterCollider(shared_ptr<Collider2D> collider)
{
	int index = static_cast<int>(this->_colliders.size());
	this->_colliders.insert(pair<int, shared_ptr<Collider2D>>(index, collider));
	return index;
}

// should be called every update frame
void PhysicsSystem::Update()
{
	this->CheckCollisions();
}

void PhysicsSystem::CheckCollisions()
{
	for (int from = 0; from < this->_colliders.size(); ++from)
	{
		for (int to = 0; to < this->_colliders.size(); ++to)
		{
			// if self
			if (from == to)
				continue;

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

			bool collision = false;
			for (int fep = 0; fep < fromPoints.size(); ++fep)
			{
				// line points for collider A
				Point pointA = fromPoints[fep] + fromOrigin;
				Point pointB = fromPoints[0] + fromOrigin;
				if (fep < fromPoints.size() - 1)
					pointB = fromPoints[fep + 1] + fromOrigin;

				for (int tep = 0; tep < toPoints.size(); ++tep)
				{
					// line points for collider B
					Point pointC = toPoints[tep] + toOrigin;
					Point pointD = toPoints[0] + toOrigin;
					if (tep < toPoints.size() - 1)
						pointD = toPoints[tep + 1] + toOrigin;

					if (pointA.Near(pointC) || pointA.Near(pointD) || pointB.Near(pointC) || pointB.Near(pointD))
					{
						collision = true;
						break;
					}

					if (fabs(pointA.x - pointB.x) < 0.2f || fabs(pointC.x - pointD.x) < 0.2f)
					{
						if (fabs(pointA.x - pointB.x) < 0.2f && fabs(pointC.x - pointD.x) < 0.2f && fabs(pointA.x - pointC.x) >= 0.2f)
						{
							// no collision
							continue;
						}

						// if there's horizontal line
						if (fabs(pointA.y - pointB.y) < 0.2f || fabs(pointC.y - pointD.y) < 0.2f)
						{
							// if lineA is vertical
							if (fabs(pointA.x - pointB.x) < 0.2f)
							{
								if (fmin(pointC.x, pointD.x) < pointA.x && fmax(pointC.x, pointD.x) > pointA.x &&
									fmin(pointC.y, pointD.y) < fmax(pointA.y, pointB.y) && fmax(pointC.y, pointD.y) > fmin(pointA.y, pointB.y))
								{
									collision = true;
									break;
								}
							}

							// if lineB is vertical
							if (fabs(pointC.x - pointD.x) < 0.2f)
							{
								if (fmin(pointA.x, pointB.x) < pointC.x && fmax(pointA.x, pointB.x) > pointC.x &&
									fmin(pointA.y, pointB.y) < fmax(pointC.y, pointD.y) && fmax(pointA.y, pointB.y) > fmin(pointC.y, pointD.y))
								{
									collision = true;
									break;
								}
							}
						}
					}

					float a1 = (pointB.y - pointA.y) / (pointB.x - pointA.x);
					float b1 = pointA.y - a1 * pointA.x;
					float a2 = (pointD.y - pointC.y) / (pointD.x - pointC.x);
					float b2 = pointC.y - a2 * pointC.x;

					if (fabs(a1 - a2) < 0.2f && fabs(b1 - b2) < 0.2f)
					{
						if (fabs(pointA.x - pointC.x) < 0.2f && fabs(pointB.x - pointD.x) < 0.2f ||
							fabs(pointA.x - pointD.x) < 0.2f && fabs(pointB.x - pointC.x) < 0.2f)
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