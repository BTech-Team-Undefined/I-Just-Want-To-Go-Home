#pragma once
#include "Component.h"
#include <SDL2/SDL.h>
#include <unordered_map>
#include <functional>
#include <stdio.h>


class MenuInputComponent : public Component
{
public:

	MenuInputComponent() : Component(std::type_index(typeid(MenuInputComponent)))
	{
		_actions[SDL_EventType::SDL_KEYDOWN] = [this](SDL_Event e)
		{

		};

		_actions[SDL_EventType::SDL_KEYUP] = [this](SDL_Event e)
		{

		};

		_actions[SDL_EventType::SDL_MOUSEMOTION] = [this](SDL_Event e)
		{
			rot = static_cast<float>(e.motion.xrel);
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
	float xvel;
	float yvel;
	float rot;

};