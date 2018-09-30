#include "Entity.h"
#include <iostream>

Entity::Entity()
{
	std::cout << "Entity Created" << std::endl;
}

Entity::~Entity()
{
	std::cout << "Entity DESTROYED" << std::endl;
}
