#pragma once

#include <vector>
#include "OpenGLProfiler.h"
#include "Renderable.h"
#include "RenderComponent.h"
#include "../Camera.h"

class RenderingSystem
{
public:
	std::vector<std::shared_ptr<RenderComponent>> components;
	Camera* activeCamera; 
	Shader* geometryShader;		// default geometry shader 
	Shader* compositionShader;	// default composition shader 

private:
	OpenGLProfiler profiler;
	unsigned int quadVAO;
	unsigned int FBO, posTex, nrmTex, colTex, dphTex;
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

public:
	RenderingSystem();
	~RenderingSystem();
	void Update();
	void SetCamera(Camera* camera);
	// Tentative
	void AddRenderable(std::shared_ptr<RenderComponent> rc);
	void RemoveRenderable(std::shared_ptr<RenderComponent> rc);

private: 
	void RenderGeometryPass();
	void RenderEntityGeometry(Entity* e, glm::mat4 transform);
	void RenderCompositionPass();
	void InitializeFrameBuffers();
	void InitializeScreenQuad();
	void DrawComponent(RenderComponent* component);
};

