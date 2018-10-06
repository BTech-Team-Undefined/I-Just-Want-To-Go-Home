#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "..\Rendering\Shader.h"
#include "..\..\EntitySystems\Component.h"

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

