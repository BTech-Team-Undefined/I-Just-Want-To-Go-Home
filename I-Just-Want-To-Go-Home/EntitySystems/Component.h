#pragma once
#include <memory>

class Entity;

class Component
{
public:
	Component(Entity* e) :_entity(e) {};
	virtual ~Component() {};
	virtual Component& operator=(const Component&) = delete;  // Disallow copying
	Component(const Component&) = delete;
	virtual void update(float dt) = 0;
	void Kill() { _isAlive = false; }
	Entity* getEntity() { return _entity; }

private:
	bool _isAlive = true;
	Entity* _entity;
};