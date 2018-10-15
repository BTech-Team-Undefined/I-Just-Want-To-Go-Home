#pragma once
#include "Component.h"
#include <SDL2/SDL.h>
#include <unordered_map>
#include <functional>
#include <stdio.h>


class InputComponent : public Component
{
public:

	InputComponent(Entity* e) : Component(e)
	{
		_actions[SDL_EventType::SDL_KEYDOWN] = [](SDL_Event e)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_LEFT:
				// = -1;
				break;
			case SDLK_RIGHT:
				//alien_xvel = 1;
				break;
			case SDLK_UP:
				//alien_yvel = -1;
				break;
			case SDLK_DOWN:
				//alien_yvel = 1;
				break;
			default:
				break;
			}
		};

		_actions[SDL_EventType::SDL_KEYUP] = [](SDL_Event e)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_LEFT:
				// = -1;
				break;
			case SDLK_RIGHT:
				//alien_xvel = 1;
				break;
			case SDLK_UP:
				//alien_yvel = -1;
				break;
			case SDLK_DOWN:
				//alien_yvel = 1;
				break;
			default:
				break;
			}
		};

	}
	void update(float dt)
	{
		//Event handler
		SDL_Event e;
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			auto it = _actions.find(e.type);
			if (it != _actions.end())
			{
				it->second(e);
				return;
			}
		}
	}

private:
	std::unordered_map<Uint32, std::function<void(SDL_Event e)>> _actions;
};