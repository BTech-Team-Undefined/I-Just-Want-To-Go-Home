#include "RenderingSystem.h"

#include <iostream>
#include <iomanip>
#include "Shader.h"


RenderingSystem::RenderingSystem()
{
	// debug? initialize the composition shader 
	 geometryShader = new Shader("shaders/geometry_vertex.glsl", "shaders/geometry_fragment.glsl");
	 compositionShader = new Shader("shaders/comp_vertex.glsl", "shaders/comp_fragment.glsl");
	 shadowmapShader = new Shader("shaders/shadowmap_vertex.glsl", "shaders/shadowmap_fragment.glsl");

	// initialize frame buffers for geometry rendering pass 
	InitializeFrameBuffers();
	
	// create a quad that covers the screen for the composition pass 
	InitializeScreenQuad();

	// create profiler 
	profiler.InitializeTimers(3);	// 1 for each pass so far 
}

RenderingSystem::~RenderingSystem()
{
}

void RenderingSystem::Update()
{
	// Draw everything 
	glClearDepth(1.0f);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderGeometryPass();
}

void RenderingSystem::SetCamera(Camera * camera)
{
	this->activeCamera = camera;
}

void RenderingSystem::AddRenderable(std::shared_ptr<RenderComponent> rc)
{
	this->components.push_back(rc);
}

void RenderingSystem::RemoveRenderable(std::shared_ptr<RenderComponent> rc)
{
	std::cerr << "ERROR: RemoveRenderable() is not implemented yet" << std::endl;
}

void RenderingSystem::RenderGeometryPass()
{

	// 1. First pass - Geometry into gbuffers 
	profiler.StartTimer(0);

	geometryShader->use();
	// bind geometry frame buffers 
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glDrawBuffers(3, attachments);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// begin rendering each renderable 
	auto projection = activeCamera->GetProjectionMatrix();
	auto view = activeCamera->GetViewMatrix();
	geometryShader->setMat4("u_Projection", projection);
	geometryShader->setMat4("u_View", view);

	// go thru each component 
	for (int i = 0; i < components.size(); i++)
	{
		auto rc = components[i];
		auto model = rc->GetEntity()->getWorldTransformation();		// this can be optimized 
		geometryShader->setMat4("u_Model", model);

		// go thru each renderable package 
		for (int i = 0; i < rc->renderables.size(); i++)
		{
			auto r = rc->renderables[i];
			r->material->LoadMaterial(geometryShader, 0);
			glBindVertexArray(r->mesh->VAO);
			glDrawElements(GL_TRIANGLES, r->mesh->indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			/* Proper way
			if (custom shader has been set) then
				shader use
				shader set model 
				load material (shader) 
				load mesh () 
			*/
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	profiler.StopTimer(0);


	// 2nd Pass - lighting shadow map   
	profiler.StartTimer(1);

	for (int i = 0; i < lights.size(); i++)
	{
		shadowmapShader->use();
		shadowmapShader->setMat4("u_LightSpaceMatrix", lights[i]->getLightSpaceMatrix());
		lights[i]->PrepareShadowmap(shadowmapShader);

		// go thru each component 
		for (int i = 0; i < components.size(); i++)
		{
			auto rc = components[i];
			auto model = rc->GetEntity()->getWorldTransformation();		// this can be optimized 
			shadowmapShader->setMat4("u_Model", model);

			// go thru each renderable package 
			for (int i = 0; i < rc->renderables.size(); i++)
			{
				auto r = rc->renderables[i];
				glBindVertexArray(r->mesh->VAO);
				glDrawElements(GL_TRIANGLES, r->mesh->indices.size(), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 640, 480);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	profiler.StopTimer(1);


	// 3rd pass - composision
	profiler.StartTimer(2);
	
	compositionShader->use();
	compositionShader->setInt("u_PosTex", 0);	// set order 
	compositionShader->setInt("u_NrmTex", 1);
	compositionShader->setInt("u_ColTex", 2);
	compositionShader->setInt("u_DphTex", 3);
	compositionShader->setInt("u_ShadowMap", 4);

	// enable the sampler2D shader variables 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, posTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, nrmTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, colTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, dphTex);
	
	for (int i = 0; i < lights.size(); i++)
	{
		compositionShader->setVec3("u_LightPos", lights[i]->GetEntity()->position);
		compositionShader->setMat4("u_LightSpaceMatrix", lights[i]->getLightSpaceMatrix());
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, dynamic_cast<DirectionalLight&>(*lights[i]).TexId);
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	profiler.StopTimer(2);

	profiler.FrameFinish();
	
	std::cout << std::setprecision(2) 
		<< "Geometry Pass: " << profiler.GetDuration(0) / 1000000.0 << "ms\t" 
		<< "Shadowmap Pass: " << profiler.GetDuration(1) / 1000000.0 << "ms\t"
		<< "Composition Pass: " << profiler.GetDuration(2) / 1000000.0 << "ms\r" << std::flush;
}

void RenderingSystem::DrawComponent(RenderComponent* component)
{
	glm::mat4 model = component->GetEntity()->getWorldTransformation();

	for (int i = 0; i < component->renderables.size(); i++)
	{
		auto r = component->renderables[i];
		
	}
}

void RenderingSystem::RenderCompositionPass()
{
}

void RenderingSystem::InitializeFrameBuffers()
{
	// TODO: DEBUG
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;

	// ===== FBO FOR DEFERRED RENDERING =====
	// initialize frame buffer object 
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	// position texture 
	glGenTextures(1, &posTex);
	glBindTexture(GL_TEXTURE_2D, posTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posTex, 0);
	// normal texture 
	glGenTextures(1, &nrmTex);
	glBindTexture(GL_TEXTURE_2D, nrmTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, nrmTex, 0);
	// color (albedo) texture
	glGenTextures(1, &colTex);
	glBindTexture(GL_TEXTURE_2D, colTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, colTex, 0);
	// depth texture (create as texture so we can read from it) 
	glGenTextures(1, &dphTex);
	glBindTexture(GL_TEXTURE_2D, dphTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dphTex, 0);

	// Create our render targets. Order in array determines order for shader layout = #. 
	unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// check if FBO is ok 
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "OPENGL: Failed to create FBO. GL ERROR: " << glGetError() << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderingSystem::InitializeScreenQuad()
{
	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);	// remember this syntax only works here b/c we defined the array in the same scope 

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}



/* Notes:

	// create depth buffer (you could also make this a texture - if you need to read from it)
	//unsigned int RBO;
	//glGenRenderbuffers(1, &RBO);
	//glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	// more texture parameters you might need
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

*/

