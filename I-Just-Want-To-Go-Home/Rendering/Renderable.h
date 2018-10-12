#pragma once

#include <vector>
#include <memory>
#include <glm\glm.hpp>
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"

class Renderable
{
public:
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
	std::shared_ptr<Shader> shader;

public:
	Renderable() {};
	Renderable(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, std::shared_ptr<Shader> shader)
	{
		this->mesh = mesh;
		this->material = material;
		this->shader = shader;
	}
	~Renderable() {};
};

