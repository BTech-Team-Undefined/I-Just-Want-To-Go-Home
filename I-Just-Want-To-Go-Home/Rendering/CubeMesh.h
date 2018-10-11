#pragma once

#include "Mesh.h"

class CubeMesh : public Mesh
{
public:
	CubeMesh() : Mesh(vertices, indices) {}
	~CubeMesh() {};
private:
	static const std::vector<Vertex> vertices; 
	static const std::vector<unsigned int> indices; 
};

