#include "PlaneMesh.h"

const std::vector<Vertex> PlaneMesh::vertices = {
	Vertex(-10, 0,  10, 0, 1,  0, 1, 0), // 0
	Vertex(-10, 0, -10, 0, 0,  0, 1, 0), // 1
	Vertex(10, 0,  10, 1, 1,  0, 1, 0), // 2
	Vertex(10, 0, -10, 1, 0,  0, 1, 0), // 3
};

const std::vector<unsigned int> PlaneMesh::indices = {
	2, 1, 0,
	1, 2, 3,
};