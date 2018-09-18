/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/


// Using SDL and standard IO
#include <SDL2\SDL.h>
#include <stdio.h>
#include <iostream>
// OpenGL support 
#include "glad.h"
#include <SDL2\SDL_opengl.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
// Custom objects 
#include "Camera.h"
#include "Shader.h"


//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;



int main(int argc, char* args[])
{
	// Debug data 
	// 192
	float cubeVertices[] = {
		// x y z, u v, nx ny nz

		// Front
		 1, -1,  1, 1, 0,  0, 0, 1, // 0
		 1,  1,  1, 1, 1,  0, 0, 1, // 1
		-1,  1,  1, 0, 1,  0, 0, 1, // 2
		-1, -1,  1, 0, 0,  0, 0, 1, // 3

		// Back
		-1, -1, -1, 1, 0,  0, 0,-1, // 4
		-1,  1, -1, 1, 1,  0, 0,-1, // 5
		 1,  1, -1, 0, 1,  0, 0,-1, // 6
		 1, -1, -1, 0, 0,  0, 0,-1, // 7

		// Left
		-1, -1,  1, 1, 0, -1, 0, 0, // 8
		-1,  1,  1, 1, 1, -1, 0, 0, // 9
		-1,  1, -1, 0, 1, -1, 0, 0, // 10
		-1, -1, -1, 0, 0, -1, 0, 0, // 11

		// Right 
		 1, -1, -1, 1, 0,  1, 0, 0, // 12
		 1,  1, -1, 1, 1,  1, 0, 0, // 13
		 1,  1,  1, 0, 1,  1, 0, 0, // 14
		 1, -1,  1, 0, 0,  1, 0, 0, // 15

		// Top 
		 1,  1,  1, 1, 0,  0, 1, 0, // 16
		 1,  1, -1, 1, 1,  0, 1, 0, // 17
		-1,  1, -1, 0, 1,  0, 1, 0, // 18
		-1,  1,  1, 0, 0,  0, 1, 0, // 19

		// Bottom
		 1, -1, -1, 1, 0,  0,-1, 0, // 20
		 1, -1,  1, 1, 1,  0,-1, 0, // 21
		-1, -1,  1, 0, 1,  0,-1, 0, // 22
		-1, -1, -1, 0, 0,  0,-1, 0, // 23
	};

	//36
	unsigned int cubeIndices[] = {
		// Front
		0, 1, 2,
		2, 3, 0,

		// Back
		4, 5, 6,
		6, 7, 4,

		// Left
		8, 9, 10,
		10, 11, 8,

		// Right
		12, 13, 14,
		14, 15, 12,

		// Top
		16, 17, 18,
		18, 19, 16,

		// Bottom
		20, 21, 22,
		22, 23, 20
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(2,0,2),
		glm::vec3(-2,0,-2),
		glm::vec3(1,2,-4),
		glm::vec3(-1,-1,3)
	};

	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	// ===== INITIAILIZE SDL & OPENGL =====
	
	//The window we'll be rendering to
	SDL_Window* window = NULL;
	//The surface contained by the window
	SDL_Surface* screenSurface = NULL;
	//The openGL context 
	SDL_GLContext context = NULL;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "ERROR: SDL could not initialize. SDL_Error:  " << SDL_GetError() << std::endl;
		return 1;
	}

	// prepare opengl version (4.5) for SDL 
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);	// using core as opposed to compatibility or ES 

	// create window
	window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		std::cerr << "ERROR: SDL window could not be created. SDL_Error:  " << SDL_GetError() << std::endl;
		return 2;
	}

	// get window surface (not necessary)
	screenSurface = SDL_GetWindowSurface(window);

	// initialize sdl opengl context 
	context = SDL_GL_CreateContext(window);
	if (context == NULL)
	{
		std::cerr << "ERROR: SDL failed to create openGL context. SDL_Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	// initialize opengl 
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		std::cerr << "ERROR: GLAD failed to initialize opengl function pointers." << std::endl;
		return 3;
	}
	std::cout << "Vendor:\t" << glGetString(GL_VENDOR) << std::endl
		<< "Renderer:\t" << glGetString(GL_RENDERER) << std::endl
		<< "Version:\t" << glGetString(GL_VERSION) << std::endl;

	// configure opengl 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// ===== SHADER CREATION ======
	Shader* geometryShader = new Shader("shaders/geometry_vertex.glsl", "shaders/geometry_fragment.glsl");
	Shader* compositionShader = new Shader("shaders/comp_vertex.glsl", "shaders/comp_fragment.glsl");



	// ===== VAO FOR CUBE =====
	// create VAO for a simple cube 
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	auto f = sizeof(cubeVertices);
	auto b = sizeof(cubeIndices);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	// enable position data 
	glEnableVertexAttribArray(0);	// enable the variable in the shader 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	// enable uv data 
	glEnableVertexAttribArray(1);	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	// enable normal data 
	glEnableVertexAttribArray(2);	
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_INDEX);

	// unbind - finished 
	glBindVertexArray(0);


	// ===== VAO FOR QUAD ======
	// create VAO for a simple cube 
	unsigned int quadVAO;
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	unsigned int quadVBO;
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);	// reference?
	// enable position data 
	glEnableVertexAttribArray(0);	// enable the variable in the shader 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	// enable uv data 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	// unbind - finished 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// ===== FBO FOR DEFERRED RENDERING =====
	// initialize frame buffer object 
	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	// create textures
	// position texture 
	unsigned int posTex, nrmTex, colTex;
	glGenTextures(1, &posTex);
	glBindTexture(GL_TEXTURE_2D, posTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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

	// Create our render targets. Order in array determines order for shader layout = #. 
	unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// create depth buffer (you could also make this a texture)
	unsigned int RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	// check if FBO is ok 
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "OPENGL: Failed to create FBO. GL ERROR: " << glGetError() << std::endl;
		return 4;
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// ===== CAMERA ======
	Camera cam(SCREEN_WIDTH / SCREEN_HEIGHT);


	while (1)
	{
		// TODO: listen for events 
		SDL_Event e;
		SDL_PollEvent(&e);

		// Render cleanup 
		glClearDepth(1.0f);					// this sets the depth to be cleared to 
		glClearColor(0.0, 0.0, 0.0, 1.0);	// this sets the color to be cleared to 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// this actually clears (sets) the buffer 

		// Render to FBO (draw geomtry to textures)
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glDrawBuffers(3, attachments);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = cam.GetViewMatrix();
		glm::mat4 projection = cam.GetProjectionMatrix();
		geometryShader->use();
		geometryShader->setInt("u_PosTex", 0);
		geometryShader->setInt("u_NrmTex", 1);
		geometryShader->setInt("u_ColTex", 2);
		geometryShader->setMat4("i_Projection", projection);
		geometryShader->setMat4("i_View", view);

		for (int i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			// todo: rotation 
			// todo: scale 
			geometryShader->setMat4("i_Model", model);

			// draw 
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);	// VAO is equipped with EBO 
			glBindVertexArray(0);
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// ? 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render to back buffer (compose textures) 
		compositionShader->use();
		
		// enable the sampler2D shader variables 
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, posTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, nrmTex);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, colTex);
		
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		SDL_GL_SwapWindow(window);
	}

	//Destroy window
	SDL_DestroyWindow( window );

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}