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

			//forard thrust
			switch (e.key.keysym.sym) {
			case SDLK_w:
				//e4->position = glm::vec3(e4->position.x, e4->position.y, e4->position.z - 0.1f);
				thrust = drift ? 17.0 : 20.0;
				break;
			case SDLK_s:
				//e4->position = glm::vec3(e4->position.x, e4->position.y, e4->position.z + 0.1f);
				thrust = -7.0;
				break;
			}

			//moving left or right; only when moving forward
			if (getEntity()->getComponent<PhysicsComponent>()->velocity.length() > 1) {
				switch (e.key.keysym.sym) {
				case SDLK_a:
					//e4->position = glm::vec3(e4->position.x - 0.1f, e4->position.y, e4->position.z);
					//e6->getComponent<PhysicsComponent>()->force.x = -10;
					getEntity()->getComponent<PhysicsComponent>()->angularForce = (drift ? 5 : 3) * sqrt(getEntity()->getComponent<PhysicsComponent>()->velocity.length());

					break;
				case SDLK_d:
					//e4->position = glm::vec3(e4->position.x + 0.1f, e4->position.y, e4->position.z);
					//e6->getComponent<PhysicsComponent>()->force.x = 10;
					getEntity()->getComponent<PhysicsComponent>()->angularForce = (drift ? -5 : -3) * sqrt(getEntity()->getComponent<PhysicsComponent>()->velocity.length());
					break;
				}
				//}
				//drifting
				//if (getEntity()->getComponent<PhysicsComponent>()->angularForce != 0) {
				if (e.key.keysym.sym == SDLK_LSHIFT) {
					drift = true;
					//if (getEntity()->getComponent<PhysicsComponent>()->angularForce >= 0) { //going left
					//	//getEntity()->getComponent<PhysicsComponent>()->angularForce += (0.25 * driftTime);
					//	getEntity()->getComponent<PhysicsComponent>()->angularForce = 7;
					//}
					//else { //going right
					//	//getEntity()->getComponent<PhysicsComponent>()->angularForce += (-0.25 * driftTime);
					//	getEntity()->getComponent<PhysicsComponent>()->angularForce = -7;
					//}
					//boost = 1 * sqrt(driftTime);
					//std::cout << "Boost: " << boost << std::endl;
				//}
				//else {
				//	drift = false;
				//	thrust += boost;
				//	boost -= 0.05;
				//	if (boost < 0) {
				//		boost = 0;
				//	}
				//	std::cout << "Boost: " << boost << "   |    Thrust: " << thrust << std::endl;
				//}
				}

				switch (e.key.keysym.sym) {
				case SDLK_SPACE:
					getEntity()->position.x = 0;
					getEntity()->position.z = 0;
					getEntity()->rotation.y = 0;
					getEntity()->getComponent<PhysicsComponent>()->force.x = 0;
					getEntity()->getComponent<PhysicsComponent>()->force.y = 0;
					thrust = 0;
					boost = 0;
					break;
				}
			};

			_actions[SDL_EventType::SDL_KEYUP] = [this](SDL_Event e)
			{
				switch (e.key.keysym.sym) {
				case SDLK_w:
					//e4->position = glm::vec3(e4->position.x, e4->position.y, e4->position.z - 0.1f);
					thrust = 0;
					break;
				case SDLK_s:
					//e4->position = glm::vec3(e4->position.x, e4->position.y, e4->position.z + 0.1f);
					thrust = -0;
					break;
				}

				switch (e.key.keysym.sym) {
				case SDLK_a:
					//e4->position = glm::vec3(e4->position.x - 0.1f, e4->position.y, e4->position.z);
					//e6->getComponent<PhysicsComponent>()->force.x = -10;
					getEntity()->getComponent<PhysicsComponent>()->angularForce = 0;

					break;
				case SDLK_d:
					//e4->position = glm::vec3(e4->position.x + 0.1f, e4->position.y, e4->position.z);
					//e6->getComponent<PhysicsComponent>()->force.x = 10;
					getEntity()->getComponent<PhysicsComponent>()->angularForce = -0;
					break;
				}



				if (e.key.keysym.sym == SDLK_LSHIFT) { // when drifting is done to get boost
					if (enableBoost)
					{
						enableBoost = false;
						thrust = 40;

					}
					drift = false;
				}
			};
		};
	}
	void update(float dt) {
	}

	void notify(vector<SDL_Event>* events, float dt)
	{
    if (drift)
		{
			driftTime += dt;
		}
		else
		{
			driftTime = 0;
		}
		if (driftTime >= maxDriftTime)
		{
			driftTime = 0;
			enableBoost = true;
		}
		//User requests quit
		for (auto ep = events->begin(); ep < events->end(); ep++) {
			auto e = *ep;
			cout << "Respond Event " << e.type << endl;
			auto it = _actions.find(e.type);
			if (it != _actions.end())
			{
				it->second(e);
			}
		}
		Entity* ent = getEntity();
		float dir = ent->rotation.y;
		ent->getComponent<PhysicsComponent>()->force.x = std::sin(dir) * thrust + std::sin(dir - 1.57) ;//* sideThrust;
		ent->getComponent<PhysicsComponent>()->force.y = std::cos(dir) * thrust + std::cos(dir - 1.57) ;//* sideThrust;
	}

private:
	std::unordered_map<Uint32, std::function<void(SDL_Event e)>> _actions;
	float thrust;
	float driftTime = 0.0f;
	const float maxDriftTime = 2.0f;
	float boost;
	bool enableBoost = false;
	bool drift;
};