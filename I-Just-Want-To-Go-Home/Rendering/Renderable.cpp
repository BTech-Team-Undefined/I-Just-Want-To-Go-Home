#include "Renderable.h"


Renderable::Renderable()
{

}

Renderable::Renderable(Mesh * mesh, Material * material, Shader * shader)
{
	this->mesh		= mesh; 
	this->material	= material; 
	this->shader	= shader; 
}


Renderable::~Renderable()
{
}

glm::mat4 Renderable::GetModelMatrix()
{
	float x = (float)std::rand() / (float)RAND_MAX;
	float y = (float)std::rand() / (float)RAND_MAX;
	float z = (float)std::rand() / (float)RAND_MAX;

	// TODO: no entity / position implemented yet ... dang

	return glm::mat4(1.0f);
}
