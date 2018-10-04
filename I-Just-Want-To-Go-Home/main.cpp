// System 
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <vector>
// Using SDL 
#include <SDL2\SDL.h>
#include <stb\stb_image.h>	// this is part of stb 
// OpenGL support 
#include <gl\glad.h>
#include <SDL2\SDL_opengl.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
// Custom objects 
#include "Camera.h"
#include "Rendering\Shader.h"
#include "Rendering\RenderingSystem.h"
#include "Rendering\Lighting\Light.h"
#include "Rendering\Lighting\DirectionalLight.h"
#include "AssetLoader.h"


//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int MAX_LIGHTS = 32; 
struct LightS
{
	glm::vec3 Position; 
	glm::vec3 Color; 
};
std::vector<LightS> lights;


// Creates a texture and returns the ID 
unsigned int LoadTexture(const char* texturePath)
{
	// https://learnopengl.com/Getting-started/Textures

	// read image file 
	int width, height, nrChannels;	// nrchannels 3 rgb, 4 rgba
	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	stbi_set_flip_vertically_on_load(true);

	if (data == NULL)
	{
		std::cout << "Failed to load texture: " << texturePath << std::endl;
		return -1;
	}

	// generate opengl texture 
	unsigned int texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	// set texture wrap/filter options (these are all default values)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load source image to opengl 
	glTexImage2D(
		GL_TEXTURE_2D,		// for 2D texture (not 1D or 3D)
		0,					// manual mipmap level. 0 for default.
		GL_RGB,				// data format. our albedo does not support alpha.
		width,				// width of texture 
		height,				// height of texture 
		0,					// legacy. always set to 0.
		GL_RGB,				// data format of source. jpg has no alpha.
		GL_UNSIGNED_BYTE,	// data format of source. stbi_load reads as bytes. 
		data				// data source. 
	);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// free source 
	stbi_image_free(data);

	return texId;
}

#include "Rendering\Mesh.h"
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

	std::vector<Vertex> cubeVertex = {
		// Front
		Vertex(1, -1,  1, 1, 0,  0, 0, 1), // 0
		Vertex(1,  1,  1, 1, 1,  0, 0, 1), // 1
		Vertex(-1,  1,  1, 0, 1,  0, 0, 1), // 2
		Vertex(-1, -1,  1, 0, 0,  0, 0, 1), // 3

		// Back
		Vertex(-1, -1, -1, 1, 0,  0, 0,-1), // 4
		Vertex(-1,  1, -1, 1, 1,  0, 0,-1), // 5
		Vertex(1,  1, -1, 0, 1,  0, 0,-1), // 6
		Vertex(1, -1, -1, 0, 0,  0, 0,-1), // 7

		// Left
		Vertex(-1, -1,  1, 1, 0, -1, 0, 0), // 8
		Vertex(-1,  1,  1, 1, 1, -1, 0, 0), // 9
		Vertex(-1,  1, -1, 0, 1, -1, 0, 0), // 10
		Vertex(-1, -1, -1, 0, 0, -1, 0, 0), // 11

		// Right 				 
		Vertex(1, -1, -1, 1, 0,  1, 0, 0), // 12
		Vertex(1,  1, -1, 1, 1,  1, 0, 0), // 13
		Vertex(1,  1,  1, 0, 1,  1, 0, 0), // 14
		Vertex(1, -1,  1, 0, 0,  1, 0, 0), // 15

		// Top
		Vertex(1,  1,  1, 1, 0,  0, 1, 0), // 16
		Vertex(1,  1, -1, 1, 1,  0, 1, 0), // 17
		Vertex(-1,  1, -1, 0, 1,  0, 1, 0), // 18
		Vertex(-1,  1,  1, 0, 0,  0, 1, 0), // 19

		// Bottom
		Vertex(1, -1, -1, 1, 0,  0,-1, 0), // 20
		Vertex(1, -1,  1, 1, 1,  0,-1, 0), // 21
		Vertex(-1, -1,  1, 0, 1,  0,-1, 0), // 22
		Vertex(-1, -1, -1, 0, 0,  0,-1, 0), // 23
	};

	std::vector<unsigned int> cubeIndex = {
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
		glm::vec3(2,0,-5),
		glm::vec3(-2,0,-5),
		glm::vec3(1,2,-7),
		glm::vec3(-1,-2,-8),
		glm::vec3(-2,-2,-16),
		glm::vec3(2, 2,-20)
	};

	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	std::vector<Vertex> planeVertex = {
		Vertex(-10, 0,  10, 0, 1,  0, 1, 0), // 0
		Vertex(-10, 0, -10, 0, 0,  0, 1, 0), // 1
		Vertex( 10, 0,  10, 1, 1,  0, 1, 0), // 2
		Vertex( 10, 0, -10, 1, 0,  0, 1, 0), // 3
	};

	std::vector<unsigned int> planeIndex = {
		2, 1, 0,
		1, 2, 3,
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

	// create vertex data buffer
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

	// create index buffer 
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	// create albedo texture 
	unsigned int cubeColTex = LoadTexture("textures/brickwall.jpg");
	unsigned int cubeNrmTex = LoadTexture("textures/brickwall_normal.jpg");

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);	// reference?
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
	unsigned int posTex, nrmTex, colTex, dphTex;
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
	// depth texture (create as texture so we can read from it) 
	glGenTextures(1, &dphTex);
	glBindTexture(GL_TEXTURE_2D, dphTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dphTex, 0);

	// Create our render targets. Order in array determines order for shader layout = #. 
	unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// create depth buffer (you could also make this a texture)
	//unsigned int RBO;
	//glGenRenderbuffers(1, &RBO);
	//glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	// check if FBO is ok 
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "OPENGL: Failed to create FBO. GL ERROR: " << glGetError() << std::endl;
		return 4;
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// ===== CAMERA ======
	Camera cam(SCREEN_WIDTH / SCREEN_HEIGHT);
	cam.position = glm::vec3(0, 0, 5);
	
	// ===== LIGHT CONFIG ====
	float ambient = 1.0f;
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		float x = (float)std::rand() / (float)RAND_MAX;
		float y = (float)std::rand() / (float)RAND_MAX;
		float z = (float)std::rand() / (float)RAND_MAX;
		LightS light;
		light.Position = glm::vec3(x * 8 - 4, y * 4 - 2, z * 10);
		light.Color = glm::vec3(x, y, z);
		lights.push_back(light);
	}

	// ===== PERFORMANCE MEASUREMENTS =====
	// guess what? this doesn't really work. You want this:
	// http://www.lighthouse3d.com/tutorials/opengl-timer-query/
	// but that takes time to implement so until then...
	std::chrono::high_resolution_clock::time_point startRenderingTime; 
	std::chrono::high_resolution_clock::time_point endRenderingTime;


	RenderingSystem renderingSystem = RenderingSystem();
	renderingSystem.SetCamera(&cam);
	
	auto loader = AssetLoader();
	TextureInfo texture;
	texture.id = loader.TextureFromFile("brickwall.jpg", "textures");
	texture.uniform = "u_ColTex";
	texture.path = "textures/brickwall.jpg";

	TextureInfo texture1;
	texture1.id = loader.TextureFromFile("brickwall_normal.jpg", "textures");
	texture1.uniform = "u_NrmTex";
	texture1.path = "textures/brickwall_normal.jpg";

	std::cout << "Loaded texture with id: " << texture.id << std::endl;

	auto r1 = Renderable();
	r1.mesh = new Mesh(cubeVertex, cubeIndex);
	r1.material = new Material();
	r1.material->AddTexture(texture);
	r1.position = glm::vec3(-1.6, 0.5, -8);

	auto r2 = Renderable();
	r2.mesh = new Mesh(cubeVertex, cubeIndex);
	r2.material = new Material();
	r2.material->AddTexture(texture);
	r2.position = glm::vec3(2, 1.2, -6);
	
	//auto r3 = loader.LoadModel("Models/nanosuit/nanosuit.obj");
	//r3->position = glm::vec3(0, -1, -10);
	//r3->modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, -10, -20));
	//renderingSystem.AddEntity(*r3);
	//for (int i = 0; i < r3->children.size(); i++)
	//{
	//	std::cout << i << ":" << r3->children[i]->renderables.size() << std::endl;
	//}

	auto r4 = Renderable();
	r4.mesh = new Mesh(planeVertex, planeIndex);
	r4.material = new Material();
	r4.material->AddTexture(texture);
	r4.position = glm::vec3(0, -2, -10);

	renderingSystem.AddRenderable(r1);
	renderingSystem.AddRenderable(r2);
	renderingSystem.AddRenderable(r4);
	
	auto mesh1 = Mesh(cubeVertex, cubeIndex);
	auto material1 = Material();
	material1.AddTexture(texture);
	material1.AddTexture(texture1);

	// LIGHTING 
	auto dl = new DirectionalLight();
	dl->position = glm::vec3(0, 7, -5);
	renderingSystem.AddLight(dl);


	while (1)
	{
		// TODO: listen for events 
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				SDL_Quit();
				return 0;
			}
			//User presses a key
			else if (e.type == SDL_KEYDOWN)
			{
				compositionShader->use();
				compositionShader->setBool("u_DisplayPos", false);
				compositionShader->setBool("u_DisplayNrm", false);
				compositionShader->setBool("u_DisplayCol", false);
				compositionShader->setBool("u_DisplayDph", false);

				//Select surfaces based on key press
				switch (e.key.keysym.sym)
				{
				case SDLK_UP:
					ambient = glm::clamp(ambient + 0.2f, 0.0f, 2.0f);
					compositionShader->setFloat("u_AmbientIntensity", ambient);
					break;
				case SDLK_DOWN:
					ambient = glm::clamp(ambient - 0.2f, 0.0f, 2.0f);
					compositionShader->setFloat("u_AmbientIntensity", ambient);
					break;
				case SDLK_q:
					compositionShader->setBool("u_DisplayPos", true);
					break;
				case SDLK_w:
					compositionShader->setBool("u_DisplayNrm", true);
					break;
				case SDLK_e:
					compositionShader->setBool("u_DisplayCol", true);
					break;
				case SDLK_r:
					compositionShader->setBool("u_DisplayDph", true);
					break;
				default:
					break;
				}
			}
		}

		renderingSystem.Update();
		SDL_GL_SwapWindow(window);

		/*
		startRenderingTime = std::chrono::high_resolution_clock::now();
		GLint64 timer; 
		glGetInteger64v(GL_TIMESTAMP, &timer);

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
		geometryShader->setInt("u_ColTex", 0);
		geometryShader->setInt("u_NrmTex", 1);
		geometryShader->setMat4("u_Projection", projection);
		geometryShader->setMat4("u_View", view);

		for (int i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			// todo: rotation 
			// todo: scale 
			geometryShader->setMat4("u_Model", model);

			// draw 
			glBindVertexArray(mesh1.VAO);

			material1.LoadMaterial(geometryShader, 0);

			glDrawElements(GL_TRIANGLES, mesh1.indices.size(), GL_UNSIGNED_INT, 0);	// VAO is equipped with EBO 
			
			glBindVertexArray(0);
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// ? 
		/// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render to back buffer (compose textures) 
		compositionShader->use();
		compositionShader->setInt("u_PosTex", 0);	// set order 
		compositionShader->setInt("u_NrmTex", 1);
		compositionShader->setInt("u_ColTex", 2);
		compositionShader->setInt("u_DphTex", 3);

		// enable the sampler2D shader variables 
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, posTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, nrmTex);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, colTex);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, dphTex);

		// material has the uniform name + texture address 
		// but render system handles setting up the texture num 
		
		// attach lights 
		compositionShader->setVec3("u_ViewPosiion", cam.position);
		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			compositionShader->setVec3("u_Lights[" + std::to_string(i) + "].Position", lights[i].Position);
			compositionShader->setVec3("u_Lights[" + std::to_string(i) + "].Color", lights[i].Color);
		}

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		endRenderingTime = std::chrono::high_resolution_clock::now();

		SDL_GL_SwapWindow(window);

		// show info 
		auto renderingDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endRenderingTime - startRenderingTime).count();
		std::cout << "Rendering duration:\t" << renderingDuration << "ms\r" << std::flush;
		*/
	}

	//Destroy window
	SDL_DestroyWindow( window );

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}