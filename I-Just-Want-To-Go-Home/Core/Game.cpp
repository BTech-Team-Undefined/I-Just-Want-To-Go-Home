#include "Game.h"
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

	while (_running)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				SDL_Quit();
				return;
			}
		}

		// 1. additions & deletions
		resolveEntities(activeScene->rootEntity.get());
		resolveCleanup();

		// 2. entity update 
		updateEntity(activeScene->rootEntity.get(), 0.016f);

		// 3. system update 
		for (int i = 0; i < _systems.size(); i++)
		{
			_systems[i]->update(0.16f);
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
	int id = entity->getID();

	// check if branch should be deleted
	auto toDelete = _deletionList.find(id);
	if (toDelete != _deletionList.end())
	{
		entity->release();
		_deletionList.erase(toDelete);
		return;
	}

	// check if something should be added 
	for (auto& entry : _additionList)
	{
		if (entry.target == id)
		{
			std::cout << "Adding entity " << entry.entity->getID() << " to parent entity " << id << std::endl;
			entity->addChild(entry.entity);
		}
	}
	// this is pretty efficient apparently. 
	_additionList.erase(
		std::remove_if(_additionList.begin(), _additionList.end(), [id](const EntityAction& e) { return e.target == id; }),
		_additionList.end());	

	// go thru remaining children 
	auto children = entity->getChildren();
	for (int i = 0; i < children.size(); i++)
	{
		resolveEntities(children[i]);
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
