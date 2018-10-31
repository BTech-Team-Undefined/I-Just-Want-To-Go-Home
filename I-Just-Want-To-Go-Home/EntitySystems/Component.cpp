#include "Component.h"
#include "../Core/Game.h"
#include <iostream>


Component::Component(std::type_index t) : _type(t)
{
	std::cout << "Component created" << std::endl;
	//Game::instance().componentCreated(t, this);
}

Component::~Component()
{
	std::cout << "Component destroyed" << std::endl;
	//Game::instance().componentDestroyed(_type, this);
}
