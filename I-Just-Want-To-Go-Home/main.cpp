
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
#include "EntitySystems/DestructionComponent.h"
#include "EntitySystems\Examples\ExampleSystem.h"
#include "EntitySystems\Examples\SimpleSystem.h"

#include "Core\Game.h"


int main(int argc, char* args[])
{
	// ===== INITIALIZE CORE GAME ENGINE =====
	Game::instance().initialize();


	/* ===== RENDERING DOCUMENTATION =====
	Check Rendering/DOCUMENTATION.txt
	*/

	// ===== INIT SCENE =====
	Scene* scene = new Scene();
	Game::instance().setActiveScene(scene);
		
	// ===== INIT SYSTEMS =====
	auto rs = std::make_unique<RenderingSystem>();
	rs->SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	Game::instance().addSystem(std::move(rs));

	auto es = std::make_unique<ExampleSystem>();
	Game::instance().addSystem(std::move(es));

	auto ss = std::make_unique<SimpleSystem>();
	Game::instance().addSystem(std::move(ss));

	// ===== CAMERA =====
	auto eCam = new Entity();
	eCam->position = glm::vec3(0, 0, 5);
	eCam->addComponent<Camera>();
	auto cam = eCam->getComponent<Camera>();
	cam->aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
	cam->fov = 60.0f;
	Game::instance().activeScene->rootEntity->addChild(eCam);

	// ===== INIT DATA ===== 
	auto e = Game::instance().loader.LoadModel("Models/tank/M11_39.obj");
	e->position = glm::vec3(-2.5, -2, -5);
	e->rotation = glm::vec3(glm::radians(-90.0f), 0, 0);

	TextureInfo texture;
	texture.id = Game::instance().loader.TextureFromFile("brickwall.jpg", "textures");
	texture.uniform = "u_ColTex";
	texture.path = "textures/brickwall.jpg";

	TextureInfo texture1;
	texture1.id = Game::instance().loader.TextureFromFile("brickwall_normal.jpg", "textures");
	texture1.uniform = "u_NrmTex";
	texture1.path = "textures/brickwall_normal.jpg";

	// create mesh 
	auto mesh1 = std::make_shared<CubeMesh>();
	auto mesh2 = std::make_shared<PlaneMesh>();

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

	auto e3 = new Entity();
	e3->addComponent<RenderComponent>();
	e3->getComponent<RenderComponent>()->renderables.push_back(r2);
	e3->position = glm::vec3(0, -2, -5);
	e3->addComponent<DestructionComponent>();
	e3->addChild(e1);

	Game::instance().addEntity(e.get());
	// Game::instance().addEntity(e1); // e1 is a child of e3!
	Game::instance().addEntity(e3);

	// ===== LIGHTING ====
	auto eLight = new Entity();
	eLight->addComponent<DirectionalLight>();
	eLight->position = glm::vec3(3, 3, -7);
	eLight->rotation = glm::vec3(glm::radians(-45.0f), glm::radians(200.0f), 0);
	Game::instance().activeScene->rootEntity->addChild(eLight);

	auto eLight2 = new Entity();
	eLight2->addComponent<DirectionalLight>();
	eLight2->position = glm::vec3(-3, 3, -7);
	eLight2->rotation = glm::vec3(glm::radians(-45.0f), glm::radians(160.0f), 0);
	Game::instance().activeScene->rootEntity->addChild(eLight2);

	// ===== START GAME ======
	Game::instance().loop();



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

	/*
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
		}

		// renderingSystem.Update();
		SDL_GL_SwapWindow(window);
	}
	*/



	return 0;
}