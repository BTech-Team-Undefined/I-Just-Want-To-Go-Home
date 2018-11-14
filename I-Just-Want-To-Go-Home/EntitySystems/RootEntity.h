#pragma once

#include "Entity.h"

class RootEntity : public Entity
{
public:
	RootEntity() : Entity(0u)
	{
		setEnabled(false);
	}

	template<class T>
	void addComponent() = delete;

	void setParent(Entity* parent) = delete;

	// TODO:
	// probably can't move transform (stuck at 0)
};