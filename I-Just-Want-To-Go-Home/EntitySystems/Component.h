#pragma once

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
	bool getEnabled() { return _enabled; }
	bool setEnabled(bool enabled) { _enabled = enabled; }
	Entity* getEntity() { return _entity; }
	void setEntity(Entity* e) { _entity = e; }	

private:
	bool _enabled = true;
	bool _isAlive = true;
	Entity* _entity;
	std::type_index _type;
};