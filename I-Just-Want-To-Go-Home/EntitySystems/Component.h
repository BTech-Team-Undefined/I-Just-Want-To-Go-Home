#pragma once
#include <memory>
#include <typeindex>
#include <typeinfo>

class Entity;

class Component
{
public:
	Component(std::type_index t);
	virtual ~Component();
	virtual Component& operator=(const Component&) = delete;  // Disallow copying
	Component(const Component&) = delete;
	virtual void update(float dt) = 0;
	void Kill() { _isAlive = false; }
	Entity* getEntity() { return _entity; }
	void setEntity(Entity* e) { _entity = e; }	// todo: unbind from entity

private:
	bool _isAlive = true;
	Entity* _entity;
	std::type_index _type;
};