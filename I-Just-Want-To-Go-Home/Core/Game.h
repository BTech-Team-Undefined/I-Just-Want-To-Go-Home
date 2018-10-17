#pragma once

#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <set>
#include <memory>
#include <SDL2\SDL.h>
#include <boost\signals2\signal.hpp>
#include "../AssetLoader.h"
#include "..\EntitySystems\System.h"
#include "Scene.h"


class Game
{

// singleton pattern (threadsafe: https://stackoverflow.com/questions/1008019/c-singleton-design-pattern)
public:
	static Game& instance()
	{
		static Game _instance;
		return _instance;
	}
	Game(Game const&) = delete;
	void operator=(Game const&) = delete;
private:
	Game() { };

// variables 
public:
	Scene* activeScene = nullptr;
	AssetLoader loader;

	// events 
	boost::signals2::signal<void(std::type_index, Component*)> componentCreated;
	boost::signals2::signal<void(std::type_index, Component*)> componentDestroyed;
	// DEBUG
	SDL_Window* window = NULL;

private:
	std::set<int> _deletionList;
	std::unordered_map<int, Entity*> _additionList;
	std::vector<std::unique_ptr<System>> _systems;
	bool _initialized = false;
	bool _running = false;

// functions 
public:
	// Initializes the core engine.
	void initialize()
	{
		std::cout << "WARNING: Game::initialize() hasn't been implemented yet!" << std::endl;
		// TODO: initialize SDL & OpenGL
	}

	// Changes the active scene. Does not dispose of the previous scene, but does disable it.
	void setActiveScene(Scene* scene);
	
	// Add a system to receive updates and enabled components. 
	// Note: components update function will run regardless if the system is in the update list or not.
	void addSystem(std::unique_ptr<System> system);

	// DEPRECATED
	void addEntity(std::unique_ptr<Entity> entity);
	
	// Starts the game loop. Blocking.
	void loop();
	
	// Stops the game loop.
	void stop();

	// Properly dispose an entity in the next frame. 
	// Note: Better to call Entity.delete() as there may be some custom funtionality.
	void deleteEntity(int id)
	{
		_deletionList.insert(id);
	}

	// Properly dispose an entity in the next frame.
	// Note: Better to call Entity.delete() as there may be some custom functionality.
	void deleteEntity(Entity* e)
	{
		deleteEntity(e->getID());
	}

	// Properly add an entity in the next frame. 
	void addEntity(Entity* entity)
	{
		std::cout << "WARNING: Game::addEntity() hasn't been implemented yet!" << std::endl;
		_additionList[0] = entity;	// needs refactoring
	}

	// Properly add an entity to a specified parent in the next frame.
	void addEntity(Entity* entity, int parent)
	{
		std::cout << "WARNING: Game::addEntity() hasn't been implemented yet!" << std::endl;
	}

private: 

	// will update all components in an entity, children in entity, and notify systems
	void updateEntity(Entity* entity, float dt);

	// crawls thru scene and resolves entity addition and deletion before updates
	void resolveEntities(Entity* entity);
};