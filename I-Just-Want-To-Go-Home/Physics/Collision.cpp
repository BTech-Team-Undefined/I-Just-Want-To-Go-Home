#include "Collision.h"
vector<Point> Collision::getCollisionPoints() {
	auto colliderA = colliders[0].first;
	auto colliderB = colliders[0].second;
	Point aOrigin = colliderA->GetOrigin();
	Point bOrigin = colliderB->GetOrigin();
	float rotationA = colliderA->GetRotationY();
	float rotationB = colliderB->GetRotationY();

	vector<Point> v = vector<Point>();
	for (auto si = colliderA->collider.begin(); si < colliderA->collider.end(); si++) {
		auto fi = (si + 1);
		if (fi >= colliderA->collider.end()) {
			fi = colliderA->collider.begin();
		}
		Point a = aOrigin + si->Rotate(rotationA);
		Point b = aOrigin + fi->Rotate(rotationA) - a;

		for (auto sj = colliderB->collider.begin(); sj < colliderB->collider.end(); sj++) {
			auto fj = (sj + 1);
			if (fj >= colliderB->collider.end()) {
				fj = colliderB->collider.begin();
			}
			// Determine whether two line segments intersect
			// Adapted from https://gamedev.stackexchange.com/questions/44720/line-intersection-from-parametric-equation
			// si: start point of line A
			// fi: end point of line A
			// sj: start point of line B
			// fj: end point of line B

			Point c = bOrigin + sj->Rotate(rotationB);
			Point d = bOrigin + fj->Rotate(rotationB) - c;

			float denom = d.x * b.y - d.y * b.x;
			// If the lines are not parallel, they intersect somewhere
			if (denom != 0) {
				float u = (b.x * (c.y - a.y) + b.y * (a.x - c.x)) / denom;
				float t = (d.x * (a.y - c.y) + d.y * (c.x - a.x)) / -denom;
				if (u >= 0 && u <= 1 && t >= 0 && t <= 1) {
					Point p = a + Point(b.x * t, b.y * t);
					Point p2 = c + Point(d.x * u, d.y * u);
					v.push_back(p);
				}
			}
		}
	}
	return v;
};