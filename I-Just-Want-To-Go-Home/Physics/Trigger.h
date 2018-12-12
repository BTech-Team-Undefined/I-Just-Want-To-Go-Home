#pragma once
#include <functional>

#include "PhysicsComponent.h"

//#triggered
class Trigger: public Collider2D
{
public:
	Trigger(std::function<void(void)> trigger): _trigger(trigger) { }
	//triggered trigger (me rn)
	virtual void OnCollision(int colliderId, string colliderName) { if(colliderName == "Player") _trigger(); }

private:
	std::function<void(void)> _trigger;
};