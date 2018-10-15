#pragma once

#include <typeindex>
#include <typeinfo>
#include <unordered_map>
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
	std::vector<std::unique_ptr<System>> _systems;
	bool _running = false;

// functions 
public:
	// Changes the active scene. Does not dispose of the previous scene, but does disable it.
	void setActiveScene(Scene* scene);
	
	// Add a system to receive updates. Note components update function will run regardless 
	// if the system is in the update list or not.
	void addSystem(std::unique_ptr<System> system);
	
	// Convenience function to add an entity to active scene. 
	void addEntity(std::unique_ptr<Entity> entity);
	
	// Starts the game loop. Blocking.
	void loop();
	
	// Stops the game loops.
	void stop();
};