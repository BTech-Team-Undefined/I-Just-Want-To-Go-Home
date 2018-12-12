#pragma once
#include "Collider2D.h"

class Collision {
public:
	Collision(
		pair<Entity*, Entity*> collidingEntities,
		vector<pair<shared_ptr<Collider2D>, shared_ptr<Collider2D>>> collidingColliders) :
		entities(collidingEntities),
		colliders(collidingColliders) {}

	Collision(
		pair<Entity*, Entity*> collidingEntities,
		pair<shared_ptr<Collider2D>, shared_ptr<Collider2D>> collidingColliderPair) :
		entities(collidingEntities)
		{
		colliders = vector<pair<shared_ptr<Collider2D>, shared_ptr<Collider2D>>>();
		colliders.push_back(collidingColliderPair);
	}

	pair<Entity*, Entity*> entities;
	vector<pair<shared_ptr<Collider2D>, shared_ptr<Collider2D>>> colliders;
};