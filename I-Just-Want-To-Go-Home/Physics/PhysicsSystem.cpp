#include <algorithm>
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
	return index;
}

// should be called every update frame
void PhysicsSystem::Update()
{
	this->CheckCollisions();
}

void PhysicsSystem::CheckCollisions()
{
	for (int from = 0; from < this->_entities.size(); ++from)
	{
		for (int to = 0; to < this->_entities.size(); ++to)
		{
			// if self
			if (from == to)
				continue;

			IPhysicsEntity fromEntity = this->_entities[from];
			IPhysicsEntity toEntity = this->_entities[to];

			// if too far
			float innerDistance = fromEntity._roughCenter.Distance(toEntity._roughCenter);
			float outerDistance = innerDistance - fromEntity._roughRadius - toEntity._roughRadius;
			if (outerDistance > 0)
			{
				this->RemoveCollision(fromEntity, toEntity);
				continue;
			}

			// calculate collisions
			vector<Point> fromPoints = fromEntity._collider;
			vector<Point> toPoints = toEntity._collider;

			bool collision = false;
			for (int fep = 0; fep < fromPoints.size(); ++fep)
			{
				// line points for collider A
				Point pointA = fromPoints[fep];
				Point pointB = fromPoints[0];
				if (fep < fromPoints.size() - 1)
					pointB = fromPoints[fep + 1];

				for (int tep = 0; fep < toPoints.size(); ++tep)
				{
					// line points for collider B
					Point pointC = toPoints[tep];
					Point pointD = toPoints[0];
					if (tep < toPoints.size() - 1)
						pointD = toPoints[tep + 1];

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
				if (find(fromEntity._collidingIds.begin(), fromEntity._collidingIds.end(), toEntity._physicsId) == fromEntity._collidingIds.end())
				{
					fromEntity.OnCollision(toEntity._physicsId, toEntity._physicsName);
					fromEntity._collidingIds.push_back(toEntity._physicsId);
				}
				if (find(toEntity._collidingIds.begin(), toEntity._collidingIds.end(), fromEntity._physicsId) == toEntity._collidingIds.end())
				{
					toEntity.OnCollision(fromEntity._physicsId, fromEntity._physicsName);
					toEntity._collidingIds.push_back(fromEntity._physicsId);
				}
			}
			else
			{
				// no collision
				this->RemoveCollision(fromEntity, toEntity);
			}
		}
	}
}

void PhysicsSystem::RemoveCollision(IPhysicsEntity entityA, IPhysicsEntity entityB)
{
	// if already colliding
	// todo: add OnExitCollision?
	vector<int>::iterator it;

	it = find(entityA._collidingIds.begin(), entityA._collidingIds.end(), entityB._physicsId);
	if (it != entityA._collidingIds.end())
	{
		int index = static_cast<int>(distance(entityA._collidingIds.begin(), it));
		entityA._collidingIds.erase(entityA._collidingIds.begin() + index);
	}

	it = find(entityB._collidingIds.begin(), entityB._collidingIds.end(), entityA._physicsId);
	if (it != entityB._collidingIds.end())
	{
		int index = static_cast<int>(distance(entityB._collidingIds.begin(), it));
		entityB._collidingIds.erase(entityB._collidingIds.begin() + index);
	}
}