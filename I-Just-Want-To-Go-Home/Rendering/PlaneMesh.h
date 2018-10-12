#pragma once

#include "Mesh.h"

class PlaneMesh : public Mesh
{
public:
	PlaneMesh() : Mesh(vertices, indices) {};
	~PlaneMesh() {};
private:
	static const std::vector<Vertex> vertices;
	static const std::vector<unsigned int> indices;
};

