#pragma once

#include <typeinfo>
#include <typeindex>
#include "../System.h"
#include "../Component.h"
#include "../../Camera.h"
#include "../../Rendering/RenderComponent.h"
#include "../../Rendering/Lighting/DirectionalLight.h"

/**
Example of a customized system. Components are manually registered and cleared.
This method is more efficient. See RenderingSystem for another example
*/
class ExampleSystem : public System
{
public:
	ExampleSystem() : System() {};
	~ExampleSystem() {};

	virtual void update(float dt) override
	{
		std::cout << "ExampleSystem active RenderComponents: " << _renders.size()
			<< " Camera: " << _cameras.size() 
			<< "\t" << std::flush;
	}

	virtual void addComponent(std::type_index t, Component* component) override
	{
		if (t == std::type_index(typeid(RenderComponent)))
			_renders.push_back(static_cast<RenderComponent*>(component));
		else if (t == std::type_index(typeid(Camera)))
			_cameras.push_back(static_cast<Camera*>(component));
	}

	virtual void clearComponents() override
	{
		_renders.clear();	// should clear contents but keep reserved memory
		_cameras.clear();
	}

private: 
	std::vector<RenderComponent*> _renders;
	std::vector<Camera*> _cameras;
};

