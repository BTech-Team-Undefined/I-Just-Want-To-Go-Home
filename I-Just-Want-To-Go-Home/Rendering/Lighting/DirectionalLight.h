#pragma once

#include <iostream>
#include <gl\glad.h>
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Light.h"
#include "../../EntitySystems/Entity.h"

class DirectionalLight : public Light
{
public:
	GLuint FBO, TexId; 

	DirectionalLight(Entity* e) : Light(e)
	{
		// Generate shadow map 
		glGenFramebuffers(1, &FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		glGenTextures(1, &TexId);
		glBindTexture(GL_TEXTURE_2D, TexId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);	// make out of border unshadowed 

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, TexId, 0);

		glDrawBuffer(GL_NONE);

		// Always check that our framebuffer is ok
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "ERROR: DirectionalLight failed to create shadow map" << std::endl;
		}
	}
	~DirectionalLight() {};

	virtual void LoadUniforms(Shader* shader) 
	{
	}

	// TODO: TONS OF MAGIC NUMBERS
	virtual void PrepareShadowmap(Shader* shader)
	{
		//glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 11.0f);
		//// glm::mat4 view = glm::lookAt(position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		//glm::mat4 view = glm::inverse(GetEntity()->getWorldTransformation());
		//glm::mat4 lightspace = projection * view; 
		//
		//shader->use();
		//shader->setMat4("u_LightSpaceMatrix", lightspace);

		glViewport(0, 0, 1024, 1024);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	// don't call this 
	virtual void CleanupShadowmap(Shader* shader) 
	{
		std::cerr << "I SAID DONT CALL CLEANUPSHADOWMAP" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 640, 480);
	}

	glm::mat4 getLightSpaceMatrix() override
	{
		glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
		//glm::mat4 view = glm::lookAt(GetEntity()->position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 view = GetEntity()->getWorldTransformation();
		return projection * view;
	}
};

