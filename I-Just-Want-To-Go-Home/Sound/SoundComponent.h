#pragma once
#include "../EntitySystems/Component.h"
#include <string>
#include <iostream>

enum AudioAction {
	NOTHING,
	REQUEST_PLAYMUSIC,
	REQUEST_PLAYFX,
	REQUEST_STOP
};
class SoundComponent : public Component
{
public:
	std::string audioPath;
	int cChannel;
	float cVolume;
	bool isMusic = true;
	int FX_Type=-1;
	bool playing = false;
	bool repeat = true;

	SoundComponent() : Component(std::type_index(typeid(SoundComponent)))
	{
	}

	virtual void update(float dt) {	
		/*if (FX_Type == 0) {
			if (Mix_Playing(3) == 0) {
				//audioPath = "Sound/car_idle.wav";
				//FX_Type = 1;
				//PlayFx();
				
			//}
		}*/
	};
	
	void PlayBgm()
	{
		_request = AudioAction::REQUEST_PLAYMUSIC;
	}
	void PlayFx()
	{
		_request = AudioAction::REQUEST_PLAYFX;
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

