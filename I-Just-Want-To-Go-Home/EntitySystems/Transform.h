#pragma once

#include "Component.h"
#include <SDL2/SDL.h>

class Transform : public Component
{
public:
	Transform(Entity* e) : Component(e), test(0) { }
	void Draw() {}
	void Update(float dt) {}

	int GetTest() { return test; }


private:
	int test;
};