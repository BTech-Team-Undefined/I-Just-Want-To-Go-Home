#include "Renderable.h"

#include <glm\gtc\matrix_transform.hpp>

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
	// TODO: no entity / position implemented yet ... dang
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	return model;
}
