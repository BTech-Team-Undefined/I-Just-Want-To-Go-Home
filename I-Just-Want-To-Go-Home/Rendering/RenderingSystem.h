#pragma once

#include <vector>
#include "Renderable.h"
#include "../Camera.h"

class RenderingSystem
{
public:
	std::vector<Renderable> renderables;
	Camera* activeCamera; 
	Shader* compositionShader;

private:
	unsigned int quadVAO;
	unsigned int FBO, posTex, nrmTex, colTex, dphTex;
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

public:
	RenderingSystem();
	~RenderingSystem();
	void Update();
	void AddRenderable(Renderable r);
	void RemoveRenderable(Renderable r);
	void SetCamera(Camera* camera);

private: 
	void RenderGeometryPass();
	void RenderCompositionPass();
	void InitializeFrameBuffers();
	void InitializeScreenQuad();
};

