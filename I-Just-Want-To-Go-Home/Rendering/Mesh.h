#pragma once

#include <vector>
#include <glm/glm.hpp>

/* Testing out javadoc style documentation */

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 Uv;
	glm::vec3 Normal; 

	Vertex() {};
	Vertex(float x, float y, float z, 
		float u, float v, 
		float nx, float ny, float nz) 
	{
		// TODO: use proper construtors
		Position = glm::vec3(x, y, z);	
		Uv = glm::vec2(u, v);
		Normal = glm::vec3(nx, ny, nz);
	};
};


/**
Geometry data for a model. Includes position, normals, and texture coordinates. 
Vertex data is sequenced: { x y z u v nx ny nz }
*/
class Mesh
{
public: 
	// Might want to revise this into array of Vertex
	std::vector<Vertex> vertices; 
	std::vector<unsigned int> indices; 
	unsigned int VAO; 

public:
	Mesh(std::vector<Vertex> vertices, 
		std::vector<unsigned int> indices);
	~Mesh();
};

