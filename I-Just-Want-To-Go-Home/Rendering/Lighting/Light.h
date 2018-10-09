#pragma once

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
	Light(Entity* e) : Component(e) {};
	~Light() {};
	
	virtual void LoadUniforms(Shader* shader) {};
	virtual void PrepareShadowmap(Shader* shader) {};
	virtual void CleanupShadowmap(Shader* shader) {};
	virtual glm::mat4 getLightSpaceMatrix() { return glm::mat4(1.0f); };

	void Update(float dt) {};
	void Draw() {};
};

