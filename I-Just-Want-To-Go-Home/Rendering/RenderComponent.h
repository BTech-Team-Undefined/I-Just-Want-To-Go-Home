#pragma once

#include <vector>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "../EntitySystems/Entity.h";
#include "../EntitySystems/Component.h";
#include "Renderable.h"

class RenderComponent : public Component
{
// variables 
public:
	std::vector<std::shared_ptr<Renderable>> renderables; 

// functions 
public:
	RenderComponent(Entity* e) : Component(e) {};
	~RenderComponent() {};

	virtual void Update(float dt) {};
	virtual void Draw() {};
};

