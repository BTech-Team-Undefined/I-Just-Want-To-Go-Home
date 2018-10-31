#include "Game.h"
#include "../Physics/PhysicsSystem.h"
#include <SDL2\SDL.h>

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
	_systems.push_back(std::move(system));
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
		// 1. additions 
		// TODO: implement

		// 2. deletions 
		resolveEntities(activeScene->rootEntity.get());

		// 3. entity update 
		updateEntity(activeScene->rootEntity.get(), 0.016f);

		// 4. system update 
		for (int i = 0; i < _systems.size(); i++)
		{
			_systems[i]->update(0.016f);
			_systems[i]->clearComponents();	// cleanup for next iteration
		}
		SDL_GL_SwapWindow(window);
	}
}

void Game::stop()
{
	_running = false;
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
		// ... and notify systems
		for (int j = 0; j < _systems.size(); j++)
		{
			_systems[j]->addComponent(type, components[i]);
		}
	}
}

void Game::resolveEntities(Entity * entity)
{
	// check if branch should be deleted
	const bool is_in = _deletionList.find(entity->getID()) != _deletionList.end();
	if (is_in)
	{
		entity->release();
		return;
	}

	// go thru remaining children 
	auto children = entity->getChildren();
	for (int i = 0; i < children.size(); i++)
	{
		resolveEntities(children[i]);
	}
}
