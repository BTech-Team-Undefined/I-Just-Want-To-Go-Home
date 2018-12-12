#pragma once

#include <typeinfo>
#include <typeindex>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "..\Rendering\Shader.h"
#include "..\..\EntitySystems\Component.h"

// Use this struct for simple light handling 
const int MAX_LIGHTS = 32;
struct LightSimple
{
	glm::vec3 Position;
	glm::vec3 Color;
};

// Generic Light class for Point and Directional lights. 
class Light : public Component
{
public:
	glm::vec3 color = glm::vec3(1.0, 1.0, 1.0);
	float intensity = 1.0;
	float range = 5.0;

public:
	Light(std::type_index t) : Component(t) {};
	
	virtual void LoadUniforms(Shader* shader)	  = 0;
	virtual void PrepareShadowmap(Shader* shader) = 0;
	virtual void CleanupShadowmap(Shader* shader) = 0;
	virtual glm::mat4 getLightSpaceMatrix() { return glm::mat4(1.0f); };

	virtual void update(float dt) override {};
};

