#pragma once
#include "EntitySystems/Component.h"
#include <string>
#include <iostream>
class SoundComponent : public Component
{
public:
	SoundComponent() : Component(std::type_index(typeid(SoundComponent)))
	{
	}
	
	
	virtual void update(float dt)override {
		std::cout << "Sound Component Update Test"<< std::endl;
	};
	std::string audioPath;
	bool playing = false;
};

