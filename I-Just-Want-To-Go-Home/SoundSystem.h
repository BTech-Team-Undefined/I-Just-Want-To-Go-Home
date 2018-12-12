#pragma once

#include <typeinfo>
#include <typeindex>
#include "EntitySystems/System.h"
#include "EntitySystems/Component.h"
#include <SDL2\SDL_mixer.h> // this is sound mixer
#include "SoundComponent.h"
#include <iostream>
using namespace std;

/**
Example of a simple system. Components are automatically registered and cleared.
This method is less efficient than ExampleSystem.
*/
class SoundSystem : public System
{
public:
	// pass the components you would like to listen for
	SoundSystem() : System({
		std::type_index(typeid(SoundComponent))
	}) {};
	~SoundSystem() {};


	virtual void update(float dt) override
	{
		// remember to cache the results!
		auto components = GetComponents<SoundComponent>();

		// operate on the components
		for (SoundComponent* component : components)
		{
			auto action = component->GetAction();

			switch (action)
			{
			case AudioAction::REQUEST_PLAY:
			{
				// load:   
				auto sfx = GetAudioChunk(component->audioPath);
				// play:    
				Mix_PlayChannel(_currentChannel, sfx, 0);
				// increment 
				_currentChannel++;
				// clear component
				component->SetAction(AudioAction::NOTHING);
			}
			case AudioAction::REQUEST_STOP:
			{
				// STOP AUDIO HERE 
			}
			default:
			{

			}
			}
		}
	}

	void loadBgm(std::string path)
	{
		std::cout << "Loading BGM at " << path << std::endl;
		gMusic = Mix_LoadMUS(path.c_str());

		// check if failed to load 
		if (gMusic == NULL)
		{
			printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
		}

		// load new bgm 
		Mix_PlayMusic(gMusic, -1);

		// if (Mix_PlayingMusic() == 0) {}
	}

private:
	Mix_Music *gMusic = NULL;
	Mix_Chunk *gSound = NULL;
	std::unordered_map<std::string, Mix_Chunk*> _sfx;
	int _currentChannel = 1;

	Mix_Chunk* GetAudioChunk(std::string path)
	{
		// if chunk is unloaded - load 
		if (_sfx.find(path) == _sfx.end())
		{
			Mix_Chunk* chk = Mix_LoadWAV(path.c_str());
			_sfx[path] = chk;
			return chk;
		}
		// else return loaded chunk
		else
		{
			return _sfx[path];
		}
	}
};

