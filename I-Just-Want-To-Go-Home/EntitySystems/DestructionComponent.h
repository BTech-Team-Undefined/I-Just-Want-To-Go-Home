#pragma once

#include <typeindex>
#include <typeinfo>
#include <iostream>
#include "Component.h"
#include "Entity.h"

// A component that self-destructs an entity after a specified amount of time. 
class DestructionComponent : public Component
{
public:
	float delay = 5.0f;
	float elapsed = 0.0f;

	DestructionComponent() : Component(std::type_index(typeid(DestructionComponent)))
	{
	}

	~DestructionComponent()
	{
		std::cout << "DestructionComponent destroyed" << std::endl;
	}

	void update(float dt) override {
		if ((elapsed += dt) > delay)
		{
			std::cout << "DestructionComponent exploded on " << getEntity()->getID() << std::endl;
			getEntity()->destroy();
		}
	};
};

