#pragma once

#include <vector>
#include <glm\glm.hpp>
#include "Rendering\Renderable.h"

// Debug Entity for Rendering 
class Entity
{
public:
	glm::vec3 position; 
	glm::mat4 modelMatrix;	// need to decompose this into pos,rot,scale
	std::vector<Entity*> children; 
	std::vector<Renderable*> renderables;	// this is supposed to be components 
	Entity();
	~Entity();
};

