#pragma once
#include "System.h"
#include "InputComponent.h"
#include "../DebugInputComponent.h"
#include <vector>
class InputSystem : public System {
private:
	std::vector<InputComponent*> inputComponents;
	std::vector<DebugInputComponent*> debugInputComponents;
public:
	InputSystem();
	virtual void update(float dt) override;
	virtual void addComponent(std::type_index t, Component* component) override;
	virtual void clearComponents() override;
};