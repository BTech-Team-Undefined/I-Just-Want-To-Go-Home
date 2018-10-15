#include "Game.h"
#include <SDL2\SDL.h>

void Game::setActiveScene(Scene* scene)
{
	std::cerr << "WARNING: Game::setActiveScene hasn't been implemented properly" << std::endl;
	
	if (activeScene)
	{
		// todo: implement
		// activeScene->rootEntity.setEnabled(false);
	}

	activeScene = scene;
	// scene->rootEntity.setEnabled(true);
}

void Game::addSystem(std::unique_ptr<System> system)
{
	_systems.push_back(std::move( system ));
}

void Game::addEntity(std::unique_ptr<Entity> entity)
{
	std::cerr << "WARNING: Game::addEntity hasn't been implemented properly" << std::endl;
	activeScene->rootEntity->addChild(entity.get());
}

void Game::loop()
{
	_running = true;

	while (_running)
	{
		
		// todo: update entities (components)
		activeScene->rootEntity->update(0.16f);
		// update systems 
		for (int i = 0; i < _systems.size(); i++)
		{
			_systems[i]->update(0.16f);
		}

		SDL_GL_SwapWindow(window);
	}
}

void Game::stop()
{
	_running = false;
}