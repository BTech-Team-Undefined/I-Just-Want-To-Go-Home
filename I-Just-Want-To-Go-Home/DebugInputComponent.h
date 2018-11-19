#pragma once
#include "./EntitySystems/Component.h"
#include "./Physics/PhysicsComponent.h"
#include <SDL2/SDL.h>
#include <unordered_map>
#include <functional>
#include <stdio.h>


class DebugInputComponent : public Component
{
public:

	DebugInputComponent() : Component(std::type_index(typeid(DebugInputComponent)))
	{
		_actions[SDL_EventType::SDL_KEYDOWN] = [this](SDL_Event e)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_w: {
				//e4->position = glm::vec3(e4->position.x, e4->position.y, e4->position.z - 0.1f);
				thrust = 50.0;
				break;
			}
			case SDLK_s: {
				//e4->position = glm::vec3(e4->position.x, e4->position.y, e4->position.z + 0.1f);
				thrust = -7.0;
				break;
			}
			case SDLK_a:
				//e4->position = glm::vec3(e4->position.x - 0.1f, e4->position.y, e4->position.z);
				//e6->getComponent<PhysicsComponent>()->force.x = -10;
				getEntity()->getComponent<PhysicsComponent>()->angularForce += 10; // changed to increment
				break;
			case SDLK_d:
				//e4->position = glm::vec3(e4->position.x + 0.1f, e4->position.y, e4->position.z);
				//e6->getComponent<PhysicsComponent>()->force.x = 10;
				getEntity()->getComponent<PhysicsComponent>()->angularForce -= 10;//changed to decrement
				break;
			case SDLK_q:
				//e6->rotation = glm::vec3(e4->rotation.x, e4->rotation.y - glm::radians(1.0f), e4->rotation.z);
				sideThrust = -30.0;
				break;
			case SDLK_e:
				//e6->rotation = glm::vec3(e4->rotation.x, e4->rotation.y + glm::radians(1.0f), e4->rotation.z);
			sideThrust = 30.0;
				break;
			}
		};

		_actions[SDL_EventType::SDL_KEYUP] = [this](SDL_Event e)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_w:
				//e4->position = glm::vec3(e4->position.x, e4->position.y, e4->position.z - 0.1f);
				thrust = 0;
				break;
			case SDLK_s:
				//e4->position = glm::vec3(e4->position.x, e4->position.y, e4->position.z + 0.1f);
				thrust = 0;
				break;
			case SDLK_a:
				//e4->position = glm::vec3(e4->position.x - 0.1f, e4->position.y, e4->position.z);
				getEntity()->getComponent<PhysicsComponent>()->angularForce = 0;
				break;
			case SDLK_d:
				//e4->position = glm::vec3(e4->position.x + 0.1f, e4->position.y, e4->position.z);
				getEntity()->getComponent<PhysicsComponent>()->angularForce = 0;
				break;
			case SDLK_q:
				//e6->rotation = glm::vec3(e4->rotation.x, e4->rotation.y - glm::radians(1.0f), e4->rotation.z);
				sideThrust = 0;
				break;
			case SDLK_e:
				//e6->rotation = glm::vec3(e4->rotation.x, e4->rotation.y + glm::radians(1.0f), e4->rotation.z);
				sideThrust = 0;
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
			if (e.type == SDL_QUIT)
			{
				SDL_Quit();
				return;
			}
		}
		Entity* ent = getEntity();
		float dir = ent->rotation.y;
		ent->getComponent<PhysicsComponent>()->force.x = std::sin(dir) * thrust + std::sin(dir - 1.57) * sideThrust;
		ent->getComponent<PhysicsComponent>()->force.y = std::cos(dir) * thrust + std::cos(dir - 1.57) * sideThrust;
	}

private:
	std::unordered_map<Uint32, std::function<void(SDL_Event e)>> _actions;
	float thrust;
	float sideThrust;
};