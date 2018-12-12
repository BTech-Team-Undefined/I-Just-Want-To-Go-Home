#pragma once

#include "../Physics/Point.h"
#include <vector>
#include <glm/glm.hpp>

/* Testing out javadoc style documentation */

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 Uv;
	glm::vec3 Normal; 
	glm::vec3 Tangent;
	glm::vec3 Bitangent;

	/**
	Create an empty Vertex.
	*/
	Vertex() {};
	
	/**
	Create a basic Vertex. Includes position, normals, and texture coordinates. 
	*/
	Vertex(float x, float y, float z, 
		float u, float v, 
		float nx, float ny, float nz) 
		: Position(x,y,z), Uv(u,v), Normal(nx,ny,nz) 
	{ }

	/**
	Create a fully defined Vertex. Includes position, normals, texture coordinates, and bitangent for normal mapping. 
	*/
	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float btx, float bty, float btz)
		: Position(x, y, z), Uv(u, v), Normal(nx, ny, nz), Tangent(tx, ty, tz), Bitangent(btx, bty, btz)
	{ }
};


/**
Geometry data for a model. Includes position, normals, and texture coordinates. 
Vertex data is sequenced: { x y z u v nx ny nz tx ty tz btx bty btz }
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
	Mesh(std::vector<Point> colliderPoints);
	~Mesh();
};

