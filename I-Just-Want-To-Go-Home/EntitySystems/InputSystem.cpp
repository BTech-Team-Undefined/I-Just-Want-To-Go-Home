#include "InputSystem.h"
InputSystem::InputSystem() : System() {
	onlyReceiveFrameUpdates = true;
}
void InputSystem::update(float dt) {
	SDL_Event e;
	vector<SDL_Event>* events = new vector<SDL_Event>();
	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
		{
			SDL_Quit();
			return;
		}
		else {
			events->push_back(e);
		}
	}
	if (events->begin() != events->end()) {
		for (auto i = inputComponents.begin(); i < inputComponents.end(); i++) {
			//(*i)->notify(events, dt);
		}
		for (auto i = debugInputComponents.begin(); i < debugInputComponents.end(); i++) {
			(*i)->notify(events, dt);
		}
	}
	delete events;
}

void InputSystem::addComponent(std::type_index t, Component* component) {
	if (t == std::type_index(typeid(InputComponent))) {
		inputComponents.push_back(static_cast<InputComponent*>(component));
	}
	else if (t == std::type_index(typeid(DebugInputComponent))) {
		debugInputComponents.push_back(static_cast<DebugInputComponent*>(component));
	}
}

void InputSystem::clearComponents() {
	inputComponents.clear();
	debugInputComponents.clear();
}