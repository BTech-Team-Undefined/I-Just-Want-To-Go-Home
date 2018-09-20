#pragma once

#include <glm/glm.hpp>

/* Testing out javadoc style documentation */

/**
Geometry data for a model. Includes position, normals, and texture coordinates. 
Vertex data is sequenced: { x y z u v nx ny nz }
*/
class Mesh
{
public: 
	float* Vertices;
	unsigned int* Indices;
	unsigned int VertexCount; 
	unsigned int IndexCount; 
	unsigned int VAO; 

public:
	Mesh(float vertices[], unsigned int indices[]);
	~Mesh();
};

