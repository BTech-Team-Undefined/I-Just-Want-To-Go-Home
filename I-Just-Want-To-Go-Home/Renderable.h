#pragma once

#include "Rendering\Mesh.h"
#include "Rendering\Material.h"
#include "Rendering\Shader.h"

class Renderable
{
public:
	Mesh* Mesh; 
	Material* Material;
	Shader* Shader;

public:
	Renderable();
	// Renderable(Mesh* mesh, Material* material, Shader* shader);
	~Renderable();
};

