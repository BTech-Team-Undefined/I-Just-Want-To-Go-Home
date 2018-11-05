
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
#include "Physics\PhysicsSystem.h"
#include "Physics\PhysicsComponent.h"
#include "Physics\Collider2D.h"
#include "Physics\Point.h"
#include "Physics\PhysicsVector.h"
#include "DebugInputComponent.h"
#include "EntitySystems/Transform.h"
#include "EntitySystems/DestructionComponent.h"
#include "EntitySystems\Examples\ExampleSystem.h"
#include "EntitySystems\Examples\SimpleSystem.h"
#include "TextComponent.h"

#include "Core\Game.h"


extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

int main(int argc, char* args[])
{
	// ===== INITIALIZE CORE GAME ENGINE =====
	Game::instance().initialize();

	// ===== INIT SYSTEMS =====
	auto rs = std::make_unique<RenderingSystem>();
	rs->SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	Game::instance().addSystem(std::move(rs));

	//auto es = std::make_unique<ExampleSystem>();
	//Game::instance().addSystem(std::move(es));

	//auto ss = std::make_unique<SimpleSystem>();
	//Game::instance().addSystem(std::move(ss));

	auto ps = std::make_unique<PhysicsSystem>();
	Game::instance().addSystem(std::move(ps));

	// ===== INIT SCENE =====
	Scene* scene = new Scene();
	Game::instance().setActiveScene(scene);

	// ===== PLAYER ENTITY ===== 
	auto playerEntity = new Entity();
	playerEntity->position = glm::vec3(-2.5, -2, -5);
	// physics 
	auto e6Collider = std::make_shared<Collider2D>("e1Box");
	vector<Point> e6ColliderBox;
	e6ColliderBox.push_back(Point(-1, -1)); // top left
	e6ColliderBox.push_back(Point(1, -1)); // top right
	e6ColliderBox.push_back(Point(1, 1)); // bottom right
	e6ColliderBox.push_back(Point(-1, 1)); // bottom left
	e6Collider->SetCollider(e6ColliderBox, Point(0, 0), 1.5f); // collider points and center point are relative to the origin
	playerEntity->addComponent<PhysicsComponent>();
	auto pc6 = playerEntity->getComponent<PhysicsComponent>();
	pc6->isStatic = false;
	pc6->AddCollider(e6Collider);
	// input 
	playerEntity->addComponent<DebugInputComponent>();
	// visuals 
	auto tankEntity = Game::instance().loader.LoadModel("Models/tank/M11_39.obj");
	tankEntity->rotation = glm::vec3(glm::radians(-90.0f), 0, 0);
	// camera 
	auto eCam = new Entity();
	eCam->position = glm::vec3(0, 10, 8); //replace the camera position back if finished.
	eCam->rotation = glm::vec3(-0.7, 0, 0);
	eCam->addComponent<Camera>();
	auto cam = eCam->getComponent<Camera>();
	cam->aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
	cam->fov = 60.0f;
	// compose player 
	playerEntity->addChild(eCam);
	playerEntity->addChild(tankEntity.get());

	// ===== INIT DATA (FOR ENTITIES) ===== 
	TextureInfo texture;
	texture.id = Game::instance().loader.TextureFromFile("brickwall.jpg", "textures");
	texture.uniform = "u_ColTex";
	texture.path = "textures/brickwall.jpg";

	TextureInfo texture1;
	texture1.id = Game::instance().loader.TextureFromFile("brickwall_normal.jpg", "textures");
	texture1.uniform = "u_NrmTex";
	texture1.path = "textures/brickwall_normal.jpg";

	// create mesh 
	auto cubeMesh = std::make_shared<CubeMesh>();
	auto planeMesh = std::make_shared<PlaneMesh>();

	// create materials 
	auto material1 = std::make_shared<Material>();
	material1->AddTexture(texture);

	// create shaders
	// (not created, use default shaders) 

	// create renderables packages 
	auto cubeRenderable = std::make_shared<Renderable>();	// cube 
	cubeRenderable->mesh = cubeMesh;
	cubeRenderable->material = material1;
	auto planeRenderable = std::make_shared<Renderable>();	// plane 
	planeRenderable->mesh = planeMesh;
	planeRenderable->material = material1;

	// ===== LEVEL ENTITIES =====
	auto e1 = new Entity();
	e1->addComponent<RenderComponent>();
	auto rc1 = e1->getComponent<RenderComponent>();
	rc1->renderables.push_back(cubeRenderable);	// use std::move(r1) if you don't want to reference it here 
	e1->position = glm::vec3(-2, 0, -2);
	e1->rotation = glm::vec3(glm::radians(30.0f), 0, 0);

	auto e1Collider = std::make_shared<Collider2D>("e1Box");
	vector<Point> e1ColliderBox;
	e1ColliderBox.push_back(Point(-1, -1)); // top left
	e1ColliderBox.push_back(Point(1, -1)); // top right
	e1ColliderBox.push_back(Point(1, 1)); // bottom right
	e1ColliderBox.push_back(Point(-1, 1)); // bottom left
	e1Collider->SetCollider(e1ColliderBox, Point(0, 0), 1.5f); // collider points and center point are relative to the origin
	e1->addComponent<PhysicsComponent>();
	auto pc1 = e1->getComponent<PhysicsComponent>();
	pc1->isStatic = true;
	pc1->AddCollider(e1Collider);

	auto e2 = new Entity();
	e2->addComponent<RenderComponent>();
	e2->getComponent<RenderComponent>()->renderables.push_back(cubeRenderable);
	e2->position = glm::vec3(2, 0, -5);
	e2->rotation = glm::vec3(0, glm::radians(45.0f), 0);

	auto e2Collider = std::make_shared<Collider2D>("e1Box");
	vector<Point> e2ColliderBox;
	e2ColliderBox.push_back(Point(-1, -1)); // top left
	e2ColliderBox.push_back(Point(1, -1)); // top right
	e2ColliderBox.push_back(Point(1, 1)); // bottom right
	e2ColliderBox.push_back(Point(-1, 1)); // bottom left
	e2Collider->SetCollider(e2ColliderBox, Point(0, 0), 1.5f); // collider points and center point are relative to the origin
	e2->addComponent<PhysicsComponent>();
	auto pc2 = e2->getComponent<PhysicsComponent>();
	pc2->isStatic = true;
	pc2->AddCollider(e2Collider);

	auto e3 = new Entity();
	e3->addComponent<RenderComponent>();
	e3->getComponent<RenderComponent>()->renderables.push_back(planeRenderable);
	e3->position = glm::vec3(0, -2, -5);

	auto e3Collider = std::make_shared<Collider2D>("e1Box");
	vector<Point> e3ColliderBox;
	e3ColliderBox.push_back(Point(-1, -1)); // top left
	e3ColliderBox.push_back(Point(1, -1)); // top right
	e3ColliderBox.push_back(Point(1, 1)); // bottom right
	e3ColliderBox.push_back(Point(-1, 1)); // bottom left
	e3Collider->SetCollider(e3ColliderBox, Point(0, 0), 1.5f); // collider points and center point are relative to the origin
	e3->addComponent<PhysicsComponent>();
	auto pc3 = e3->getComponent<PhysicsComponent>();
	pc3->isStatic = true;
	pc3->AddCollider(e3Collider);

	auto e4 = new Entity();
	e4->addComponent<RenderComponent>();
	e4->getComponent<RenderComponent>()->renderables.push_back(cubeRenderable);
	e4->position = glm::vec3(2, 0, -2);

	auto e4Collider = std::make_shared<Collider2D>("e4Box");
	vector<Point> e4ColliderBox;
	e4ColliderBox.push_back(Point(-1, -1));
	e4ColliderBox.push_back(Point(1, -1));
	e4ColliderBox.push_back(Point(1, 1));
	e4ColliderBox.push_back(Point(-1, 1));
	e4Collider->SetCollider(e4ColliderBox, Point(0, 0), 1.5f);
	e4->addComponent<PhysicsComponent>();
	auto pc4 = e4->getComponent<PhysicsComponent>();
	pc4->AddCollider(e4Collider);

	// ===== LIGHT ENTITIES ====
	auto eLight = new Entity();
	eLight->addComponent<DirectionalLight>();
	eLight->position = glm::vec3(3, 3, -7);
	eLight->rotation = glm::vec3(glm::radians(-45.0f), glm::radians(200.0f), 0);

	auto eLight2 = new Entity();
	eLight2->addComponent<DirectionalLight>();
	eLight2->position = glm::vec3(-3, 3, -7);
	eLight2->rotation = glm::vec3(glm::radians(-45.0f), glm::radians(160.0f), 0);

	// ===== TEXT =====
	auto eText1 = new Entity();
	eText1->position = glm::vec3(SCREEN_WIDTH / 2, SCREEN_HEIGHT -50.0f, 0);
	eText1->addComponent<TextComponent>();
	auto text1 = eText1->getComponent<TextComponent>();
	text1->setText("I JUST WANT TO GO HOME");
	text1->color = glm::vec3(1.0f, 0.0f, 0.0f);
	text1->font = "fonts/futur.ttf";
	
	auto eText2 = new Entity();
	eText2->position = glm::vec3(0, -50.0f, 0);
	eText2->addComponent<TextComponent>();
	auto text2 = eText2->getComponent<TextComponent>();
	text2->setText("By Team Undefined");
	text2->color = glm::vec3(0.0f, 1.0f, 0.0f);
	text2->scale = 0.5f;
	text2->font = "fonts/Cool.ttf";
	eText1->addChild(eText2);

	auto eText3 = new Entity();
	eText3->position = glm::vec3(0, 0, 0);
	eText3->addComponent<TextComponent>();
	auto text3 = eText3->getComponent<TextComponent>();
	text3->setText("GAS, GAS, GAS! I'M GONNA STEP ON THE GAS, TONIGHT I'LL FLY! AND BE YOUR LOVER, YEAH YEAH YEAH! I'LL BE SO QUICK AS A FLASH, AND I'LL BE YOUR HERO! ");
	text3->color = glm::vec3(0.0f, 0.0f, 1.0f);
	text3->scale = 0.2f;

	// ===== START GAME ======
	Game::instance().addEntity(eLight);
	Game::instance().addEntity(eLight2);
	Game::instance().addEntity(e1);
	Game::instance().addEntity(e2);
	Game::instance().addEntity(e3);
	Game::instance().addEntity(e4);
	Game::instance().addEntity(playerEntity);
	Game::instance().addEntity(eText1);
	Game::instance().addEntity(eText3);

	Game::instance().loop();

	return 0;
}