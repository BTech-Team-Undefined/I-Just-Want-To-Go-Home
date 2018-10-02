#pragma once

#include <vector>
#include "OpenGLProfiler.h"
#include "Renderable.h"
#include "../Camera.h"
#include "../Entity.h"
#include "Lighting\Light.h"

class RenderingSystem
{
public:
	std::vector<Renderable> renderables;
	std::vector<Entity> entities; 
	std::vector<Light*> lights;
	Camera* activeCamera; 
	Shader* geometryShader;		// default geometry shader 
	Shader* compositionShader;	// default composition shader 
	Shader* shadowmapShader;	// default shadowmap shader 

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
	void AddLight(Light* l)
	{
		lights.push_back(l);
	}
	// Tentative
	void AddRenderable(Renderable r);
	void RemoveRenderable(Renderable r);
	// Tentative (alternate)
	void AddEntity(Entity e);
	void RemoveEntity(Entity e);

private: 
	void RenderGeometryPass();
	void RenderEntityGeometry(Entity* e, glm::mat4 transform);
	void RenderCompositionPass();
	void InitializeFrameBuffers();
	void InitializeScreenQuad();
};

