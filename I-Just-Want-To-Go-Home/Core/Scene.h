#pragma once

#include <memory>
#include "../EntitySystems/Entity.h"
#include "../EntitySystems/RootEntity.h"

class Scene
{
public:
	std::unique_ptr<RootEntity> rootEntity;

public:
	Scene()
	{
		rootEntity = std::make_unique<RootEntity>();
	};
	~Scene() {};
};

