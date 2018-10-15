#pragma once

#include "Component.h"
#include <SDL2/SDL.h>

class Transform : public Component
{
public:
	Transform() : Component(std::type_index(typeid(Transform))), test(0) { }
	void update(float dt) {}
	int getTest() { return test; }

private:
	int test;
};