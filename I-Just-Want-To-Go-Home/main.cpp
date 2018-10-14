
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
#include "EntitySystems\Entity.h"
#include "EntitySystems\Component.h"
#include "Rendering\Shader.h"
#include "Rendering\Renderable.h"
#include "Rendering\RenderingSystem.h"
#include "Rendering\Lighting\Light.h"
#include "Rendering\Lighting\DirectionalLight.h"
#include "Rendering\Mesh.h"
#include "Rendering\CubeMesh.h"
#include "Rendering\PlaneMesh.h"
#include "AssetLoader.h"
#include "EntitySystems/InputComponent.h"
#include "EntitySystems/Transform.h"


//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

int main(int argc, char* args[])
{
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

	// ===== PERFORMANCE MEASUREMENTS =====
	// This is only to measure CPU performance. For GPU use OpenGLProfiler. 
	std::chrono::high_resolution_clock::time_point startRenderingTime;
	std::chrono::high_resolution_clock::time_point endRenderingTime;

	/* ===== RENDERING DOCUMENTATION =====
	Check Rendering/DOCUMENTATION.txt
	*/

	// ===== CAMERA ======
	auto eCam = new Entity();
	eCam->position = glm::vec3(0, 0, 5);
	eCam->addComponent<Camera>();
	auto cam = eCam->getComponent<Camera>();
	cam->aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
	cam->fov = 60.0f;

	// ===== INIT RENDERING SYSTEM =====
	RenderingSystem renderingSystem = RenderingSystem();
	renderingSystem.SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	renderingSystem.SetCamera(&*cam);	// todo: gross - use smart pointers
	
	// ===== INIT DATA ===== 
	auto loader = AssetLoader();
	
	TextureInfo texture;
	texture.id = loader.TextureFromFile("brickwall.jpg", "textures");
	texture.uniform = "u_ColTex";
	texture.path = "textures/brickwall.jpg";

	TextureInfo texture1;
	texture1.id = loader.TextureFromFile("brickwall_normal.jpg", "textures");
	texture1.uniform = "u_NrmTex";
	texture1.path = "textures/brickwall_normal.jpg";

	// create mesh 
	auto mesh1 = std::make_shared<CubeMesh>();
	auto mesh2 = std::make_shared<PlaneMesh>();
	
	/* Example: Custom hardcoded data 
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

	auto mesh3 = std::make_shared<Mesh>(planeVertex, planeIndex);
	*/

	// create materials 
	auto material1 = std::make_shared<Material>();
	material1->AddTexture(texture);
	
	// create shaders
	// (not created, use default shaders) 

	// create renderables packages 
	auto r1 = std::make_shared<Renderable>();	// cube 
	r1->mesh = mesh1;
	r1->material = material1;
	auto r2 = std::make_shared<Renderable>();	// plane 
	r2->mesh = mesh2;
	r2->material = material1;

	// create entities 
	auto e1 = new Entity();
	e1->addComponent<RenderComponent>();
	auto rc1 = e1->getComponent<RenderComponent>();
	rc1->renderables.push_back(r1);	// use std::move(r1) if you don't want to reference it here 
	e1->position = glm::vec3(-2, 2, -3);
	e1->rotation = glm::vec3(glm::radians(30.0f), 0, 0);

	auto e2 = new Entity();
	e2->addComponent<RenderComponent>();
	e2->getComponent<RenderComponent>()->renderables.push_back(r1);
	e2->position = glm::vec3(2, 0, -5);
	e2->rotation = glm::vec3(0, glm::radians(45.0f), 0);

	auto e3 = new Entity();
	e3->addComponent<RenderComponent>();
	e3->getComponent<RenderComponent>()->renderables.push_back(r2);
	e3->position = glm::vec3(0, -2, -5);

	auto e4 = new Entity();
	e4->addComponent<RenderComponent>();
	e4->getComponent<RenderComponent>()->renderables.push_back(r1);
	e4->position = glm::vec3(2, 0, -2);

	// e1->addChild(e4);

	//auto e5 = loader.LoadModel("Models/nanosuit/nanosuit.obj");
	//e5->position = glm::vec3(0, -10, -20);
	//std::vector<std::shared_ptr<RenderComponent>> e5components; 
	//e5->getComponents<RenderComponent>(e5components);

	auto e6 = loader.LoadModel("Models/tank/M11_39.obj");
	e6->position = glm::vec3(-2.5, -2, -5);
	e6->rotation = glm::vec3(glm::radians(-90.0f), 0, 0);
	std::vector<std::shared_ptr<RenderComponent>> e6components;
	e6->getComponents<RenderComponent>(e6components);
	
	renderingSystem.AddRenderable(e1->getComponent<RenderComponent>());
	renderingSystem.AddRenderable(e2->getComponent<RenderComponent>());
	renderingSystem.AddRenderable(e3->getComponent<RenderComponent>());
	renderingSystem.AddRenderable(e4->getComponent<RenderComponent>());
	//for (int i = 0; i < e5components.size(); i++)
	//	renderingSystem.AddRenderable(e5components[i]);
	for (int i = 0; i < e6components.size(); i++)
		renderingSystem.AddRenderable(e6components[i]);

	// ===== LIGHTING ====
	auto eLight = new Entity();
	eLight->addComponent<DirectionalLight>();
	eLight->position = glm::vec3(3, 3, -7);
	eLight->rotation = glm::vec3(glm::radians(-45.0f), glm::radians(200.0f), 0);
	renderingSystem.AddLight(eLight->getComponent<DirectionalLight>());

	auto eLight2 = new Entity();
	eLight2->addComponent<DirectionalLight>();
	eLight2->position = glm::vec3(-3, 3, -7);
	eLight2->rotation = glm::vec3(glm::radians(-45.0f), glm::radians(160.0f), 0);
	renderingSystem.AddLight(eLight2->getComponent<DirectionalLight>());
	

	/* Debug struct - use this if not using shadow maps
	std::vector<LightSimple> lights;
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
	*/

	std::shared_ptr<Entity> t1 = std::shared_ptr<Entity>(new Entity());
	t1->addComponent<Transform>();
	printf("%d", t1->getComponent<Transform>()->getTest());
	t1->removeComponent<Transform>();
	t1->addComponent<InputComponent>();


	while (1)
	{
		// TODO: listen for events 
		/*SDL_Event e;
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
				/* TODO: Move debug handling code once input manager is implemented 
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
		}*/
		t1->update(.05f);
		renderingSystem.Update();
		SDL_GL_SwapWindow(window);
	}

	//Destroy window
	SDL_DestroyWindow( window );

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}