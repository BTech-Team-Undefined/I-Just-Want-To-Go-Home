#pragma once
#include "EntitySystems/Component.h"
#include <string>
class SoundComponent : public Component
{
public:
	SoundComponent() : Component(std::type_index(typeid(SoundComponent)))
	{
	}
	
	
	virtual void update(float dt) {};

	std::string audioPath;
	bool playing = false;
};

