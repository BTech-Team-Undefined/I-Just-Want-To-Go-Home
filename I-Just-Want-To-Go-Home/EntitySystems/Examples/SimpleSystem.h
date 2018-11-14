#pragma once

#include <typeinfo>
#include <typeindex>
#include "../System.h"
#include "../Component.h"
#include "../../Camera.h"
#include "../../Rendering/RenderComponent.h"
#include "../../Rendering/Lighting/DirectionalLight.h"

/**
Example of a simple system. Components are automatically registered and cleared.
This method is less efficient than ExampleSystem.
*/
class SimpleSystem : public System
{
public:
	// pass the components you would like to listen for
	SimpleSystem() : System({
		std::type_index(typeid(RenderComponent)),
		std::type_index(typeid(Camera)),
		std::type_index(typeid(DirectionalLight))
	}) {};
	~SimpleSystem() {};

	virtual void update(float dt) override
	{
		// remember to cache the results!
		auto renderComponents = GetComponents<RenderComponent>();
		auto cameraComponents = GetComponents<Camera>();
		auto dlightComponents = GetComponents<DirectionalLight>();

		// operate on the components
		std::cout << "SimpleSystem active RenderComponents: " << renderComponents.size()
			<< " Camera: " << cameraComponents.size()
			<< " D.Lights: " << dlightComponents.size()
			<< "\t" << std::flush;
	}

};

