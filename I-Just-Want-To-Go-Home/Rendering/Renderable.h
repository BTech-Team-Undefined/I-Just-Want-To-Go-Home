#pragma once

#include <vector>
#include <glm\glm.hpp>
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"

class Renderable
{
public:
	Mesh* mesh; 
	Material* material;
	Shader* shader;
	
	// Debug... (this should be in entity) 
	std::vector<Renderable*> children; 
	Renderable* parent; 

public:
	Renderable();
	Renderable(Mesh* mesh, Material* material, Shader* shader);
	~Renderable();
	glm::mat4 GetModelMatrix();
};

