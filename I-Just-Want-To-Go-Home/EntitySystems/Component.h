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
	//virtual std::shared_ptr<Component> clone() = 0;
	virtual void Update(float dt) = 0;
	virtual void Draw() = 0;
	void Kill() { _isAlive = false; }
	Entity* GetEntity() { return _entity; }

private:
	bool _isAlive = true;
	Entity* _entity;
};