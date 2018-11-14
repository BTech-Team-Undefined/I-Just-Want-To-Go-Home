#pragma once

#include <sstream>
#include <iomanip>
#include "../EntitySystems/Component.h"
#include "../Physics/PhysicsComponent.h"
#include "../Rendering/UI/TextComponent.h"

class SpeedDisplayComponent : public Component
{
public:
	SpeedDisplayComponent() : Component(std::type_index(typeid(SpeedDisplayComponent)))
	{
		_formatter << std::setprecision(0);
	}
	// ~SpeedDisplayComponent() {};

	virtual void update(float dt) override
	{
		if (!_initialized) return;

		auto speed = _car->velocity.length() * 3.6;
		auto percent = speed / _maxSpeed;
		auto zRot = _minDeg + (_maxDeg - _minDeg) * percent;

		_formatter.str(std::string());	// clear the stream 
		_formatter << (int)speed << " KM/H";
		_text->setText(_formatter.str());
		_speedometerNeedle->rotation.z = glm::radians(-zRot);
	}

	// need to explicitly initialize this component
	void initialize(PhysicsComponent* carPhysics, TextComponent* text, Entity* needle)
	{
		_car = carPhysics;
		_text = text;
		_speedometerNeedle = needle;
		_initialized = true;
	}

	void setNeedleRange(float degreeMin, float degreeMax)
	{
		_minDeg = degreeMin;
		_maxDeg = degreeMax;
	}

	void setMaxSpeed(float kmh)
	{
		_maxSpeed = kmh;
	}

private:
	std::stringstream _formatter;
	bool _initialized = false;
	PhysicsComponent* _car;
	TextComponent* _text;
	Entity* _speedometerNeedle;
	float _minDeg = -135.0f;
	float _maxDeg = 0.0f;
	float _maxSpeed = 80.0f;
};

