#include "Game.h"
#include "../Physics/PhysicsSystem.h"
#include <SDL2\SDL.h>
#include <SDL2\SDL_mixer.h>
#include <chrono>

Game::~Game()
{
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

void Game::initialize()
{
	// measure performance 
	//	0: general use 
	//	1: entity resolve 
	//	2: fixed update cycle
	//	3: frame update cycle
	//	4: SDL initialization 
	_profiler.InitializeTimers(5);
	_profiler.LogOutput("Engine.log");	// optional
	_profiler.PrintOutput(true);		// optional
	_profiler.FormatMilliseconds(true);	// optional

	_profiler.StartTimer(4);

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		std::cerr << "ERROR: SDL could not initialize. SDL_Error:  " << SDL_GetError() << std::endl;
		return;
	}

	// prepare opengl version (4.5) for SDL 
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);	// using core as opposed to compatibility or ES 

	// create window
	_window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (_window == NULL)
	{
		std::cerr << "ERROR: SDL window could not be created. SDL_Error:  " << SDL_GetError() << std::endl;
		return;
	}

	// get window surface (not necessary)
	_screenSurface = SDL_GetWindowSurface(_window);

	// initialize sdl opengl context 
	_context = SDL_GL_CreateContext(_window);
	if (_context == NULL)
	{
		std::cerr << "ERROR: SDL failed to create openGL context. SDL_Error: " << SDL_GetError() << std::endl;
		return;
	}

	// initialize opengl 
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		std::cerr << "ERROR: GLAD failed to initialize opengl function pointers." << std::endl;
		return;
	}
	std::cout << "Vendor:\t" << glGetString(GL_VENDOR) << std::endl
		<< "Renderer:\t" << glGetString(GL_RENDERER) << std::endl
		<< "Version:\t" << glGetString(GL_VERSION) << std::endl;
	//cout << "SOUND:"<<SDL_GetCurrentAudioDriver()<<endl;

	// configure opengl 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	 //initialize SDL sound mixer context
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		return;
	}

	_profiler.StopTimer(4);
	std::cout << "Engine initialization finished: " << _profiler.GetDuration(4) << "ns" << std::endl;
}

void Game::setActiveScene(Scene* scene)
{
	std::cout << "WARNING: Game::setActiveScene() may produce undefined behaviour" << std::endl;
	// TODO: need to wait for update cycle to finish before changing scene.

	// disable previous sce7ne 
	if (activeScene)
		activeScene->rootEntity->setEnabled(false);

	// enable new scene 
	activeScene = scene;
	scene->rootEntity->setEnabled(true);
}

void Game::addSystem(std::unique_ptr<System> system)
{
	if (system->onlyReceiveFrameUpdates)
		_frameSystems.push_back(std::move(system));
	else
		_systems.push_back(std::move(system));
}

void Game::addEntity(Entity* entity)
{
	// verify the developer isn't doing funny business 
	const bool is_in = _additionVerification.find(entity->getID()) != _additionVerification.end();
	if (is_in)
	{
		std::cerr << "WARNING: GAME::addEntity - You tried to add an entity to the scene multiple times, please check your code" << std::endl;
		return;
	}

	// prepare to add this entity in the next frame
	_additionList.emplace_back(0, entity);
	_additionVerification.insert(entity->getID());
}

void Game::loop()
{
	_running = true;

	// ===== PERFORMANCE MEASUREMENTS =====
		// This is only to measure CPU performance. For GPU use OpenGLProfiler.
	std::chrono::nanoseconds timeSinceLastUpdate = std::chrono::nanoseconds(0);
	std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point previous = std::chrono::high_resolution_clock::now();

	int frame = 0;

	while (_running)
	{
		frame++;
		previous = current;
		current = std::chrono::high_resolution_clock::now();

		// 1. entity addition & deletion, and system component notification 
		_profiler.StartTimer(1);
		resolveEntities(activeScene->rootEntity.get(), true);
		resolveCleanup();
		_profiler.StopTimer(1);


		_profiler.StartTimer(2);
		auto frameDelta = std::chrono::duration_cast<std::chrono::nanoseconds>(current - previous);
		if (_isPause)
			frameDelta = std::chrono::nanoseconds(0);
		timeSinceLastUpdate += frameDelta;
		while (timeSinceLastUpdate > _frameTime)
		{
			// std::cout << "updated on frame: " << frame << std::endl;
			timeSinceLastUpdate -= _frameTime;
			// 2. entity update 
			std::chrono::duration<double> dt = (_frameTime);
			updateEntity(activeScene->rootEntity.get(), dt.count());

			// 3. fixed system update 
			for (int i = 0; i < _systems.size(); i++)
			{
				_systems[i]->update(dt.count());
			}
		}
		// 3. fixed system update 
		for (int i = 0; i < _systems.size(); i++)
		{
			_systems[i]->clearComponents();	// cleanup for next iteration
		}
		_profiler.StopTimer(2);

		// 3. frame system update 
		_profiler.StartTimer(3);
		for (int i = 0; i < _frameSystems.size(); i++)
		{
			_frameSystems[i]->update(frameDelta.count() / 1000000);
			_frameSystems[i]->clearComponents();	// cleanup for next iteration
		}
		_profiler.StopTimer(3);

		_profiler.FrameFinish();

		SDL_GL_SwapWindow(_window);
	}
}

void Game::stop()
{
	_running = false;
}

void Game::pause(bool p)
{
	_isPause = p;
}

// will update all components in an entity, children in entity, and notify systems
void Game::updateEntity(Entity * entity, float dt)
{
	// stop if disabled 
	if (!entity->getEnabled()) return;

	// go thru all children first (remember structure incase of move)
	auto children = entity->getChildren();
	for (int i = 0; i < children.size(); i++)
	{
		updateEntity(children[i], dt);
	}

	// for all components ...
	auto components = entity->getComponents();
	for (int i = 0; i < components.size(); i++)
	{
		// ... ensure it is enabled ...
		if (!components[i]->getEnabled()) continue;
		// ... update it ...
		auto type = std::type_index(typeid(*components[i]));
		components[i]->update(dt);
	}
}

void Game::resolveEntities(Entity * entity, bool parentEnabled)
{
	int id = entity->getID();

	// check if branch should be deleted
	if (_deletionList.size() > 0)
	{
		auto toDelete = _deletionList.find(id);
		if (toDelete != _deletionList.end())
		{
			entity->release();
			_deletionList.erase(toDelete);
			return;
		}
	}

	// check if something should be added 
	if (_additionList.size() > 0)
	{
		bool updateList = false;

		for (auto& entry : _additionList)
		{
			if (entry.target == id)
			{
				std::cout << "Adding entity " << entry.entity->getID() << " to parent entity " << id << std::endl;
				entity->addChild(entry.entity);
				updateList = true;
			}
		}

		if (updateList)
		{
			_additionList.erase(
				std::remove_if(_additionList.begin(), _additionList.end(), [id](const EntityAction& e) { return e.target == id; }),
				_additionList.end());
		}
	}

	if (parentEnabled)
	{
		// precalculate world transformation matrix 
		if (entity->getEnabled() && !entity->getStatic())
			entity->configureTransform();
		
		if (entity->getEnabled())
		{
			// for all components notify systems 
			auto components = entity->getComponents();
			for (int i = 0; i < components.size(); i++)
			{
				// ... ensure it is enabled ...
				if (!components[i]->getEnabled()) continue;
				// ... and notify systems
				auto type = std::type_index(typeid(*components[i]));
				for (int j = 0; j < _systems.size(); j++)
					_systems[j]->addComponent(type, components[i]);
				for (int j = 0; j < _frameSystems.size(); j++)
					_frameSystems[j]->addComponent(type, components[i]);
			}
		}
	}

	// go thru remaining children 
	auto children = entity->getChildren();
	for (int i = 0; i < children.size(); i++)
	{
		resolveEntities(children[i], parentEnabled && entity->getEnabled());
	}
}

void Game::resolveCleanup()
{
	if (_deletionList.size() > 0)
	{
		std::cout << "Game::resolveCleanup() found " << _deletionList.size() << " entities not explicitly deleted. Entity may have been deleted via parent deletion or leaked" << std::endl;
	}
	_deletionList.clear();

	if (_additionList.size() > 0)
	{
		for (auto& entry : _additionList)
		{
			std::cout << "Game::resolveCleanup() could not add entity: " << entry.entity->getID() << " as parent: " << entry.target << " was not found" << std::endl;
			entry.entity->release();
		}
	}
	_additionList.clear();
	_additionVerification.clear();
}
