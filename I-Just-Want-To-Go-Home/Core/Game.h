#pragma once

#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <memory>
#include "../EntitySystems/System.h"

class Game
{

// singleton pattern 
public:
	static Game* instance()
	{
		if (!s_instance)
			s_instance = new Game();
		return s_instance;
	}
private:
	static Game *s_instance;
	Game() { };


};

