#pragma once
#include "EntitySystems/Component.h"
#include <string>
#include <iostream>

enum AudioAction
{
	NOTHING,
	REQUEST_PLAY,
	REQUEST_STOP
};

class SoundComponent : public Component
{
public:
	std::string audioPath;
	bool playing = false;

	SoundComponent() : Component(std::type_index(typeid(SoundComponent)))
	{
	}
	
	virtual void update(float dt)override {};


	void Play()
	{
		_request = AudioAction::REQUEST_PLAY;
	}

	void Stop()
	{
		_request = AudioAction::REQUEST_STOP;
	}

	// Convenience function for systems. Only call if you know what you're doing.
	AudioAction GetAction()
	{
		return _request;
	}

	void SetAction(AudioAction action)
	{
		_request = action;
	}

private: 
	AudioAction _request = AudioAction::NOTHING;
};

