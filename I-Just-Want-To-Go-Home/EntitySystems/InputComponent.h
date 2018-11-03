#pragma once
#include "Component.h"
#include <SDL2/SDL.h>
#include <unordered_map>
#include <functional>
#include <stdio.h>


class InputComponent : public Component
{
public:

	InputComponent() : Component(std::type_index(typeid(InputComponent)))
	{
		_actions[SDL_EventType::SDL_KEYDOWN] = [this](SDL_Event e)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_LEFT:
				xvel = -1.0f;
				break;
			case SDLK_RIGHT:
				xvel = 1.0f;
				break;
			case SDLK_UP:
				yvel = -1;
				break;
			case SDLK_DOWN:
				yvel = 1;
				break;
			default:
				break;
			}
		};

		_actions[SDL_EventType::SDL_KEYUP] = [this](SDL_Event e)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_LEFT:
				xvel = 0;
				break;
			case SDLK_RIGHT:
				xvel = 0;
				break;
			case SDLK_UP:
				yvel = 0;
				break;
			case SDLK_DOWN:
				yvel = 0;
				break;
			default:
				break;
			}
		};

		_actions[SDL_EventType::SDL_MOUSEMOTION] = [](SDL_Event e)
		{
			
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
		getEntity()->position.x += xvel * dt * 2.0f;
		getEntity()->position.y += yvel * dt * 2.0f;
	}

private:
	std::unordered_map<Uint32, std::function<void(SDL_Event e)>> _actions;
	float xvel;
	float yvel;

};