
// System 
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <vector>
#include <thread>
#include <fstream>
// Using SDL 
#include <SDL2\SDL.h>
#include <stb\stb_image.h>	// this is part of stb 
#include <SDL2\SDL_mixer.h> // this is sound mixer
// OpenGL support 
#include <gl\glad.h>
#include <SDL2\SDL_opengl.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
// jsoncpp
#include "libs/json/json.h"
#include "libs/json/value.h"
// Engine objects 
#include "Camera.h"
#include "EntitySystems\Entity.h"
#include "EntitySystems\Component.h"
#include "EntitySystems\InputSystem.h"
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
#include "Physics\Trigger.h"

#include "Rendering\UI\ImageComponent.h"
#include "Rendering\UI\TextComponent.h"
#include "Core\Game.h"
// Gameplay specific (engine agnostic)
#include "Game\SpeedDisplayComponent.h"
#include "Game\TimeDisplayComponent.h"
#include "Game\StickyTransformComponent.h"


extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

Mix_Music *gMusic = NULL;

int main(int argc, char* args[])
{
	// ===== INITIALIZE CORE GAME ENGINE =====
	Game::instance().initialize();

	// ===== INIT SYSTEMS =====
	auto rs = std::make_unique<RenderingSystem>();
	rs->SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	Game::instance().addSystem(std::move(rs), ThreadType::graphics);
	auto is = std::make_unique<InputSystem>();
	Game::instance().addSystem(std::move(is), ThreadType::graphics);

	//auto es = std::make_unique<ExampleSystem>();
	//Game::instance().addSystem(std::move(es));

	//auto ss = std::make_unique<SimpleSystem>();
	//Game::instance().addSystem(std::move(ss));

	auto ps = std::make_unique<PhysicsSystem>();
	Game::instance().addSystem(std::move(ps));

	// ===== INIT SCENE =====
	Scene* scene = new Scene();
	Game::instance().setActiveScene(scene);

	// ===== Test Sound =====
	gMusic = Mix_LoadMUS("Sound/BGM.wav");
	if (gMusic == NULL)
	{
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
	}
	if (Mix_PlayingMusic() == 0) {
		Mix_PlayMusic(gMusic, -1);
	}

	// ===== PLAYER ENTITY ===== 
	auto playerEntity = new Entity();
	playerEntity->position = glm::vec3(-5, -2, -5);

	const bool FLY_MODE = false;
	
	// physics 
	auto e6Collider = std::make_shared<Collider2D>("Player");
	vector<Point> e6ColliderBox;
	e6ColliderBox.push_back(Point(-0.25, -0.25)); // top left
	e6ColliderBox.push_back(Point(0.25, -0.25)); // top right
	e6ColliderBox.push_back(Point(0.25, 0.25)); // bottom right
	e6ColliderBox.push_back(Point(-0.25, 0.25)); // bottom left
	e6Collider->SetCollider(e6ColliderBox, Point(0, 0), 1.5f); // collider points and center point are relative to the origin
	playerEntity->addComponent<PhysicsComponent>();
	auto pc6 = playerEntity->getComponent<PhysicsComponent>();
	
	pc6->isStatic = false;
	pc6->directionalDrag = true;
	if (!FLY_MODE)
		pc6->AddCollider(e6Collider);
	// input 
	playerEntity->addComponent<DebugInputComponent>();
	// visuals 
	auto tankEntity = Game::instance().loader.LoadModel("Models/racingkit2/raceCarRed.obj");
	tankEntity->rotation = glm::vec3(0, glm::radians(180.0f), 0);
	// camera 
	auto eCam = new Entity();
	eCam->position = glm::vec3(0, 10, -8); //replace the camera position back if finished.
	eCam->rotation = glm::vec3(-0.7, 3.141, 0);
	eCam->addComponent<Camera>();
	auto cam = eCam->getComponent<Camera>();
	cam->aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
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

	// create generic colliders 
	std::vector<Point> colBox;
	colBox.push_back(Point(-1, -1));
	colBox.push_back(Point(1, -1));
	colBox.push_back(Point(1, 1));
	colBox.push_back(Point(-1, 1));

	// ===== LEVEL ENTITIES =====

	const float ENTITY_SCALE = 10;
	const float DECORATION_SCALE = 5;

	auto grass = Game::instance().loader.LoadModel("Models/racingkit2/grass.obj");
	grass->position = glm::vec3(200, -2, -100);
	grass->scale = glm::vec3(400, 1, 400);
	grass->setStatic(true);
	Game::instance().addEntity(grass.get());

	Json::Value stageData;
	std::ifstream stage_file("Maps/stage1.json", std::ifstream::binary);
	stage_file >> stageData;

	Json::Value player = stageData["player"];
	Json::Value playerPosition = player["position"];
	if (playerPosition != NULL)
	{
		playerEntity->position = glm::vec3(playerPosition[0].asDouble(), playerPosition[1].asDouble(), playerPosition[2].asDouble());
	}

	vector<unique_ptr<Entity>> trackEntities;
	Json::Value tracks = stageData["tracks"];
	for (int i = 0; i < tracks.size(); ++i)
	{
		string modelName = tracks[i]["model"].asString();
		trackEntities.push_back(Game::instance().loader.LoadModel("Models/racingkit2/" + modelName + ".obj"));
		int currentIndex = trackEntities.size() - 1;

		trackEntities[currentIndex]->setStatic(true);

		Json::Value position = tracks[i]["position"];
		double posX = position[0].asDouble();
		double posY = position[1].asDouble();
		double posZ = position[2].asDouble();
		trackEntities[currentIndex]->position = glm::vec3(posX * ENTITY_SCALE, -2 + posY * ENTITY_SCALE, posZ * ENTITY_SCALE);

		Json::Value rotation = tracks[i]["rotation"];
		if (rotation != NULL)
		{
			double rotX = rotation[0].asDouble();
			double rotY = rotation[1].asDouble();
			double rotZ = rotation[2].asDouble();
			trackEntities[currentIndex]->rotation = glm::vec3(glm::radians(rotX), glm::radians(rotY), glm::radians(rotZ));
		}

		bool isFinishLine = false;
		if (tracks[i]["finish"] != NULL)
			isFinishLine = tracks[i]["finish"].asBool();

		Json::Value colliders = tracks[i]["colliders"];
		const bool DEBUG_COLLIDER_VISUAL = true;

		if (colliders != NULL)
		{
			trackEntities[currentIndex]->addComponent<PhysicsComponent>();
			auto trackPhysics = trackEntities[currentIndex]->getComponent<PhysicsComponent>();
			trackPhysics->isStatic = true;
			trackPhysics->directionalDrag = false;

			if (isFinishLine)
				trackPhysics->hasPhysicsCollision = false;
			
			for (int i = 0; i < colliders.size(); ++i)
			{
				Json::Value collider = colliders[i];

				auto colliderReaction = [] { std::cout << "collision!"; };
				auto finishLineReaction = [] { std::cout << "finish!"; };

				auto colliderObj = isFinishLine ? std::make_shared<Trigger>(finishLineReaction) : std::make_shared<Trigger>(colliderReaction);
				colliderObj->hasPhysics = !isFinishLine;
				vector<Point> colliderBox;
				vector<Point> visualBox;

				for (int j = 0; j < collider.size(); ++j)
				{
					colliderBox.push_back(Point(collider[j][0].asDouble() * ENTITY_SCALE, collider[j][1].asDouble() * ENTITY_SCALE));

					if (DEBUG_COLLIDER_VISUAL)
						visualBox.push_back(Point(collider[j][0].asDouble() * ENTITY_SCALE, collider[j][1].asDouble() * ENTITY_SCALE));
				}

				colliderObj->SetCollider(colliderBox, Point(0, 0), 4.0f * ENTITY_SCALE);
				trackPhysics->AddCollider(colliderObj);

				if (DEBUG_COLLIDER_VISUAL)
				{
					auto visualEntity = new Entity();
					visualEntity->position = glm::vec3(posX * ENTITY_SCALE, -1 + posY * ENTITY_SCALE, posZ * ENTITY_SCALE);
					visualEntity->rotation = trackEntities[currentIndex]->rotation;
					visualEntity->scale = glm::vec3(1, 1, 1);
					visualEntity->addComponent<RenderComponent>();
					auto trackRdr = visualEntity->getComponent<RenderComponent>();
					auto r = make_shared<Renderable>();
					r->material = material1;
					r->mesh = make_shared<Mesh>(visualBox);
					trackRdr->addRenderable(r);
					Game::instance().addEntity(visualEntity);
				}
			}
		}
		
		trackEntities[currentIndex]->scale = glm::vec3(ENTITY_SCALE, ENTITY_SCALE, ENTITY_SCALE);
		trackEntities[currentIndex]->setStatic(true);
		Game::instance().addEntity(trackEntities[currentIndex].get());
	}

	vector<unique_ptr<Entity>> decorationEntities;
	Json::Value decorations = stageData["decorations"];
	for (int i = 0; i < decorations.size(); ++i)
	{
		string modelName = decorations[i]["model"].asString();
		decorationEntities.push_back(Game::instance().loader.LoadModel("Models/racingkit2/" + modelName + ".obj"));
		int currentIndex = decorationEntities.size() - 1;

		decorationEntities[currentIndex]->setStatic(true);

		Json::Value position = decorations[i]["position"];
		double posX = position[0].asDouble();
		double posY = position[1].asDouble();
		double posZ = position[2].asDouble();
		decorationEntities[currentIndex]->position = glm::vec3(posX * ENTITY_SCALE, -2 + posY * ENTITY_SCALE, posZ * ENTITY_SCALE);

		Json::Value rotation = decorations[i]["rotation"];
		if (rotation != NULL)
		{
			double rotX = rotation[0].asDouble();
			double rotY = rotation[1].asDouble();
			double rotZ = rotation[2].asDouble();
			decorationEntities[currentIndex]->rotation = glm::vec3(glm::radians(rotX), glm::radians(rotY), glm::radians(rotZ));
		}

		decorationEntities[currentIndex]->scale = glm::vec3(DECORATION_SCALE, DECORATION_SCALE, DECORATION_SCALE);
		decorationEntities[currentIndex]->setStatic(true);
		Game::instance().addEntity(decorationEntities[currentIndex].get());
	}
	/*
	auto e1 = new Entity();
	e1->addComponent<RenderComponent>();
	auto rc1 = e1->getComponent<RenderComponent>();
	rc1->renderables.push_back(cubeRenderable);	// use std::move(r1) if you don't want to reference it here 
	e1->position = glm::vec3(-2, 0, -2);

	auto e1Collider = std::make_shared<Trigger>([] {std::cout << "this is a test" << endl; });
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
	e3->scale = glm::vec3(10, 10, 10);

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
	*/

	// ===== LIGHT ENTITIES ====
	auto eLight = new Entity();
	eLight->addComponent<DirectionalLight>();
	eLight->position = glm::vec3(3, 10, -7);
	eLight->rotation = glm::vec3(glm::radians(-45.0f), glm::radians(200.0f), 0);

	auto eLight2 = new Entity();
	eLight2->addComponent<DirectionalLight>();
	eLight2->position = glm::vec3(-3, 10, -7);
	eLight2->rotation = glm::vec3(glm::radians(-45.0f), glm::radians(160.0f), 0);
	
	// keep lights centered on player (so there's always a shadow)
	auto eLightHolder = new Entity();
	eLightHolder->addComponent<StickyTransformComponent>();
	eLightHolder->getComponent<StickyTransformComponent>()->setTarget(playerEntity);
	eLightHolder->addChild(eLight);
	//eLightHolder->addChild(eLight2);

	auto ePLight1 = new Entity();
	ePLight1->addComponent<PointLight>();
	ePLight1->getComponent<PointLight>()->range = 1;
	ePLight1->getComponent<PointLight>()->color = glm::vec3(1,0,0);
	ePLight1->position = glm::vec3(-1, -0.5, 10);

	auto ePLightGateL = new Entity();
	ePLightGateL->addComponent<PointLight>();
	ePLightGateL->getComponent<PointLight>()->range = 0.8;
	ePLightGateL->getComponent<PointLight>()->color = glm::vec3(0, 1, 0);
	ePLightGateL->position = glm::vec3(-1.2, 0.9, 4);

	auto ePLightGateR = new Entity();
	ePLightGateR->addComponent<PointLight>();
	ePLightGateR->getComponent<PointLight>()->range = 0.8;
	ePLightGateR->getComponent<PointLight>()->color = glm::vec3(0, 1, 0);
	ePLightGateR->position = glm::vec3(-3.7, 0.9, 4);

	auto ePLightStartHolder = new Entity();
	ePLightStartHolder->addChild(ePLightGateL);
	ePLightStartHolder->addChild(ePLightGateR);

	for (int i = 0; i < 4; i++)
	{
		auto epLeft = new Entity();
		auto epRight = new Entity();
		epLeft->position = glm::vec3(-4.3, -0.5, 8 + i * 3);
		epRight->position = glm::vec3(-0.2, -0.5, 8 + i * 3);

		epLeft->addComponent<PointLight>();
		epLeft->getComponent<PointLight>()->color = (i % 2 == 0) ? glm::vec3(1, 0, 0) : glm::vec3(1, 1, 1);
		epLeft->getComponent<PointLight>()->range = 1.0;
		epRight->addComponent<PointLight>();
		epRight->getComponent<PointLight>()->color = (i % 2 != 0) ? glm::vec3(1, 0, 0) : glm::vec3(1, 1, 1);
		epRight->getComponent<PointLight>()->range = 1.0;

		ePLightStartHolder->addChild(epLeft);
		ePLightStartHolder->addChild(epRight);
	}


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

	// ===== UI ===== 
	auto eImage1 = new Entity();
	eImage1->scale = glm::vec3(0.1f, 0.1f, 0.1f);
	eImage1->position = glm::vec3(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, 0);
	eImage1->addComponent<ImageComponent>();
	auto image1 = eImage1->getComponent<ImageComponent>();
	image1->loadImage("textures/racecar.png");
	image1->layer = 0;
	
	auto eImage2 = new Entity();
	eImage2->scale = glm::vec3(2.0f, 2.0f, 2.0f);	// or set the image width / height
	eImage2->position = glm::vec3(SCREEN_WIDTH / 5, SCREEN_HEIGHT / 5, 0);
	eImage2->addComponent<ImageComponent>();
	auto image2 = eImage2->getComponent<ImageComponent>();
	image2->loadImage("textures/pinacle.png");
	image2->layer = 1;

	// speedometer img 
	auto eSpeedometerNeedle = new Entity();
	eSpeedometerNeedle->addComponent<ImageComponent>();
	eSpeedometerNeedle->getComponent<ImageComponent>()->loadImage("textures/needle.png");
	eSpeedometerNeedle->getComponent<ImageComponent>()->tint = glm::vec3(1, 0, 0);
	auto eSpeedometerBg = new Entity();
	eSpeedometerBg->addChild(eSpeedometerNeedle);
	eSpeedometerBg->scale = glm::vec3(0.25f, 0.25f, 0.25f);
	eSpeedometerBg->addComponent<ImageComponent>();
	eSpeedometerBg->getComponent<ImageComponent>()->loadImage("textures/speedometer.png");
	// speedometer text 
	auto eSpeedText = new Entity();
	eSpeedText->position = glm::vec3(0, -10, 0);
	eSpeedText->addComponent<TextComponent>();
	auto speedTextComponent = eSpeedText->getComponent<TextComponent>();
	speedTextComponent->color = glm::vec3(1, 1, 1);
	speedTextComponent->font = "fonts/futur.ttf";
	speedTextComponent->scale = 0.5;
	speedTextComponent->alignment = TextAlignment::Center;
	auto eSpeedBg = new Entity();
	eSpeedBg->scale = glm::vec3(0.8, 0.8, 1.0);
	eSpeedBg->position = glm::vec3(0, -40, 0);
	eSpeedBg->addChild(eSpeedText);
	eSpeedBg->addComponent<ImageComponent>();
	auto speedBgImg = eSpeedBg->getComponent<ImageComponent>();
	speedBgImg->loadImage("textures/UI/grey_button01.png");
	speedBgImg->tint = glm::vec3(0.1, 0.1, 0.1);
	speedBgImg->opacity = 0.7f;
	// speedometer parent entity 
	auto eSpeed = new Entity();
	eSpeed->position = glm::vec3(125, 100, 0);
	eSpeed->addChild(eSpeedometerBg);
	eSpeed->addChild(eSpeedBg);
	eSpeed->addComponent<SpeedDisplayComponent>();
	auto speedComponent = eSpeed->getComponent<SpeedDisplayComponent>();
	speedComponent->initialize(pc6, speedTextComponent, eSpeedometerNeedle);

	auto eTime = new Entity();
	eTime->addComponent<TextComponent>();
	eTime->addComponent<TimeDisplayComponent>();
	eTime->position = glm::vec3(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 100, 0);
	auto timeTextComponent = eTime->getComponent<TextComponent>();
	timeTextComponent->color = glm::vec3(1, 1, 1);
	timeTextComponent->font = "fonts/futur.ttf";
	timeTextComponent->alignment = TextAlignment::Center;

	// ===== FREEZE OBJECTS ===== 
	eTime->setStatic(true);

	// win text 
	auto eWinDisplay = new Entity();
	eWinDisplay->setEnabled(false);
	eWinDisplay->position = glm::vec3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0);
	eWinDisplay->addComponent<ImageComponent>();
	eWinDisplay->addComponent<TextComponent>();
	auto cWinBg = eWinDisplay->getComponent<ImageComponent>();
	cWinBg->loadImage("textures/UI/grey_panel.png");
	cWinBg->width = 400;
	cWinBg->height = 300;
	auto cWinText = eWinDisplay->getComponent<TextComponent>();
	cWinText->setText("FINISHED");
	cWinText->font = "fonts/futur.ttf";
	cWinText->alignment = TextAlignment::Center;

	// goal collider
	auto eGoal = new Entity();
	eGoal->position = glm::vec3(-3, -2, 25);
	eGoal->addComponent<PhysicsComponent>();
	eGoal->addComponent<RenderComponent>();
	auto cGoalPhys = eGoal->getComponent<PhysicsComponent>();
	cGoalPhys->hasPhysicsCollision = false;
	auto tGoalTrigger = std::make_shared<Trigger>(
	[&eWinDisplay]
	{		
		eWinDisplay->setEnabled(true);
		Game::instance().pause(true);
	});
	//std::bind(&Entity::setEnabled, eWinDisplay, true)
	
	tGoalTrigger->SetCollider(colBox, Point(0, 0), 1.5f);
	cGoalPhys->AddCollider(tGoalTrigger);
	auto cGoalRdr = eGoal->getComponent<RenderComponent>();
	cGoalRdr->addRenderable(cubeRenderable);

	// ===== START GAME ======
	// Game::instance().addEntity(eLight);
	// Game::instance().addEntity(eLight2);
	// Game::instance().addEntity(e1);
	// Game::instance().addEntity(e2);
	// Game::instance().addEntity(e3);
	// Game::instance().addEntity(e4);
	Game::instance().addEntity(eLightHolder);
	Game::instance().addEntity(ePLightStartHolder);
	Game::instance().addEntity(ePLight1);
	Game::instance().addEntity(playerEntity);
	// Game::instance().addEntity(eText1);
	// Game::instance().addEntity(eText3);
	// Game::instance().addEntity(eImage1);
	// Game::instance().addEntity(eImage2);
	Game::instance().addEntity(eSpeed);
	Game::instance().addEntity(eTime);
	Game::instance().addEntity(eWinDisplay);
	Game::instance().addEntity(eGoal);

	Game::instance().loop();

	return 0;
}