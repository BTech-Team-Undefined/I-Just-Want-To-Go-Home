#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "..\Rendering\Shader.h"

class Light
{
public:
	Light();
	~Light();
	
	virtual void LoadUniforms(Shader* shader) {};
	virtual void PrepareShadowmap(Shader* shader) {};
	virtual void CleanupShadowmap(Shader* shader) {};

	// debug 
	glm::vec3 position; 
	glm::vec3 rotation; 
	
	glm::mat4 GetTransformMatrix()
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::rotate(model, rotation.x, glm::vec3(1, 0, 0));
		model = glm::rotate(model, rotation.y, glm::vec3(0, 1, 0));
		model = glm::rotate(model, rotation.z, glm::vec3(0, 0, 1));
		return model;
	};
};

