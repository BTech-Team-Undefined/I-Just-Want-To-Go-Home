#pragma once

#include <vector>
#include "OpenGLProfiler.h"
#include "Renderable.h"
#include "RenderComponent.h"
#include "../Camera.h"
#include "Lighting\Light.h"
#include "Lighting\DirectionalLight.h"
#include "../EntitySystems/System.h"

class RenderingSystem : public System
{
public:
	Camera* activeCamera; 
	Shader* geometryShader;		// default geometry shader 
	Shader* compositionShader;	// default composition shader 
	Shader* shadowmapShader;	// default shadowmap shader 

private:
	OpenGLProfiler profiler;
	unsigned int quadVAO;
	unsigned int FBO, posTex, nrmTex, colTex, dphTex;
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	unsigned int screenWidth, screenHeight; 
	
	std::vector<RenderComponent*> _components;
	std::vector<DirectionalLight *> _dlights;
	std::vector<Camera*> _cameras;
	

public:
	RenderingSystem();
	~RenderingSystem();
	void SetSize(unsigned int width, unsigned int height);
	void SetCamera(Camera* camera);
	virtual void update(float dt) override;
	virtual void addComponent(std::type_index t, Component* component) override;
	virtual void clearComponents() override;

private: 
	void RenderGeometryPass();
	void RenderEntityGeometry(Entity* e, glm::mat4 transform);
	void RenderCompositionPass();
	void InitializeFrameBuffers();
	void InitializeScreenQuad();
	void DrawComponent(RenderComponent* component);
};

