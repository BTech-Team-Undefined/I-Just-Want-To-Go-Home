#pragma once

#include "../EntitySystems/Entity.h"
#include "../EntitySystems/Component.h"

class StickyTransformComponent : public Component
{
public:
	StickyTransformComponent() : Component(std::type_index(typeid(StickyTransformComponent)))
	{
	}
	
	virtual void update(float dt) override
	{
		if (_target == nullptr) return;

		// this isn't going to work if target is rotated
		getEntity()->position = _target->getWorldPosition();
	}

	void setTarget(Entity* entity)
	{
		_target = entity;
	}

private:
	Entity* _target = nullptr;

};

