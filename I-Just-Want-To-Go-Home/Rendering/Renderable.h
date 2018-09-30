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
	
	// (this should be in transform)
	glm::vec3 position;	// hardcoded for now 

public:
	Renderable();
	Renderable(Mesh* mesh, Material* material, Shader* shader);
	~Renderable();
	glm::mat4 GetModelMatrix();
};

