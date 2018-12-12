#pragma once

#include <sstream>
#include <iomanip>
#include "../EntitySystems/Component.h"
#include "../Physics/PhysicsComponent.h"
#include "../Rendering/UI/TextComponent.h"

class TimeDisplayComponent : public Component
{
public:
	TimeDisplayComponent() : Component(std::type_index(typeid(TimeDisplayComponent)))
	{
		_formatter << std::setprecision(1) << std::fixed;
	}
	// ~TimeDisplayComponent();

	virtual void update(float dt) override
	{
		auto e = getEntity();
		auto text = e->getComponent<TextComponent>();

		_elapsedTime += dt;

		_formatter.str(std::string());	// clear the stream 
		_formatter << "TIME: " << _elapsedTime;
		text->setText(_formatter.str());
	}

	void reset()
	{
		_elapsedTime = 0.0f;
	}

private:
	std::stringstream _formatter;
	bool _initialized = false;
	float _elapsedTime = 0.0f;
};

