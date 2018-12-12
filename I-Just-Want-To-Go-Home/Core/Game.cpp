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

void Game::addSystem(std::unique_ptr<System> system, ThreadType type)
{
	if (system->onlyReceiveFrameUpdates)
		_frameSystems[type].push_back(std::move(system));
	else
		_systems[type].push_back(std::move(system));
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

void Game::loop(ThreadType type)
{
	_running[type] = true;

	// background thread 
	std::thread primaryThread(&Game::primary_loop, this);
	// current thread (for SDL context)
	ui_loop();	// blocking

	// ensure both threads are dead 
	primaryThread.join();
}

void Game::stop(ThreadType type)
{
	_running[type] = false;
}

void Game::pause(bool p)
{
	_isPause = p;
}

void Game::ui_loop()
{
	_running[ThreadType::graphics] = true;

	// ===== PERFORMANCE MEASUREMENTS =====
	// This is only to measure CPU performance. For GPU use OpenGLProfiler.
	std::chrono::nanoseconds timeSinceLastUpdate = std::chrono::nanoseconds(0);
	std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point previous = std::chrono::high_resolution_clock::now();

	int frame = 0;

	while (_running[ThreadType::graphics])
	{
		frame++;
		previous = current;
		current = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> dt = current - previous;

		//Event handler
		SDL_Event e;
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				SDL_Quit();
				return;
			}
		}

		_entitiesMtx.lock();

		// freeze transforms for entities 
		_precomputeMtx.lock();
		resolvePrecomputeFreeze(activeScene->rootEntity.get());
		_precomputeMtx.unlock();

		// notify UI systems 
		resolveSystemNotification(activeScene->rootEntity.get(), ThreadType::graphics);

		// system update 
		for (int i = 0; i < _systems[ThreadType::graphics].size(); i++)
		{
			_systems[ThreadType::graphics][i]->update(dt.count());
			_systems[ThreadType::graphics][i]->clearComponents();
		}
		for (int i = 0; i < _frameSystems[ThreadType::graphics].size(); i++)
		{
			_frameSystems[ThreadType::graphics][i]->update(dt.count());
			_frameSystems[ThreadType::graphics][i]->clearComponents();	// cleanup for next iteration
		}

		// TODO
		// _isPause

		_entitiesMtx.unlock();

		// SDL actions should only happen in the graphics thread
		SDL_GL_SwapWindow(_window);
	}
}

void Game::primary_loop()
{
	_running[ThreadType::primary] = true;

	// ===== PERFORMANCE MEASUREMENTS =====
		// This is only to measure CPU performance. For GPU use OpenGLProfiler.
	std::chrono::nanoseconds timeSinceLastUpdate = std::chrono::nanoseconds(0);
	std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point previous = std::chrono::high_resolution_clock::now();

	int frame = 0;

	while (_running[ThreadType::primary])
	{
		frame++;
		previous = current;
		current = std::chrono::high_resolution_clock::now();

		auto frameDelta = std::chrono::duration_cast<std::chrono::nanoseconds>(current - previous);
		timeSinceLastUpdate += frameDelta;

		while (timeSinceLastUpdate > _frameTime)
		{
			timeSinceLastUpdate -= _frameTime;
			std::chrono::duration<double> dt = (_frameTime);
			
			// check for entity addition/deletion 
			if (_additionList.size() > 0 || _deletionList.size() > 0)
			{
				_entitiesMtx.lock();
				resolveAdditionDeletion(activeScene->rootEntity.get());
				resolveCleanup();
				_entitiesMtx.unlock();
			}

			// precompute transforms 
			_precomputeMtx.lock();
			resolvePrecompute(activeScene->rootEntity.get());
			_precomputeMtx.unlock();
			
			// system notification 
			resolveSystemNotification(activeScene->rootEntity.get(), ThreadType::primary);

			// entity update 
			updateEntity(activeScene->rootEntity.get(), dt.count());

			// system update 
			for (int i = 0; i < _systems[ThreadType::primary].size(); i++)
			{
				_systems[ThreadType::primary][i]->update(dt.count());
				_systems[ThreadType::primary][i]->clearComponents();
			}
			for (int i = 0; i < _frameSystems[ThreadType::primary].size(); i++)
			{
				_frameSystems[ThreadType::primary][i]->update(dt.count());
				_frameSystems[ThreadType::primary][i]->clearComponents();	// cleanup for next iteration
			}
		}
	}
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

void Game::resolveAdditionDeletion(Entity *entity) {
	
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

	// go thru remaining children 
	auto children = entity->getChildren();
	for (int i = 0; i < children.size(); i++)
	{
		resolveAdditionDeletion(children[i]);
	}

}

void Game::resolveEntities(Entity * entity, bool collectComponents, ThreadType type)
{
	std::cerr << "ERROR: YOU ARE CALLING A DEPRECATED FUNCTION" << std::endl;
	/*
	int id = entity->getID();

	// for all components notify systems 
	if (collectComponents)
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
		resolveEntities(children[i], collectComponents && entity->getEnabled(), type);
	}
	*/
}

void Game::resolvePrecompute(Entity* entity)
{
	if (!entity->getEnabled())
		return;

	// precalculate world transformation matrix 
	if (!entity->getStatic())
		entity->configureTransform();

	// go thru remaining children 
	auto children = entity->getChildren();
	for (int i = 0; i < children.size(); i++)
		resolvePrecompute(children[i]);
}

void Game::resolvePrecomputeFreeze(Entity* entity)
{
	if (!entity->getEnabled())
		return;

	// freeze the world transformation for the renderer 
	if (!entity->getStatic())
		entity->updateTransform();

	// go thru remaining children 
	auto children = entity->getChildren();
	for (int i = 0; i < children.size(); i++)
		resolvePrecomputeFreeze(children[i]);
}

void Game::resolveSystemNotification(Entity* entity, ThreadType threadType)
{
	if (!entity->getEnabled())
		return;

	// for all components notify systems 
	auto components = entity->getComponents();
	for (int i = 0; i < components.size(); i++)
	{
		// ... ensure it is enabled ...
		if (!components[i]->getEnabled()) continue;
		// ... and notify systems
		auto type = std::type_index(typeid(*components[i]));
		for (int j = 0; j < _systems[threadType].size(); j++)
			_systems[threadType][j]->addComponent(type, components[i]);
		for (int j = 0; j < _frameSystems[threadType].size(); j++)
			_frameSystems[threadType][j]->addComponent(type, components[i]);
	}

	// go thru remaining children 
	auto children = entity->getChildren();
	for (int i = 0; i < children.size(); i++)
		resolveSystemNotification(children[i], threadType);
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
