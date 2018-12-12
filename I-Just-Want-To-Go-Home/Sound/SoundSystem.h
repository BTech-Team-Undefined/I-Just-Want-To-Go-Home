#pragma once

#include <typeinfo>
#include <typeindex>
#include "../EntitySystems/Entity.h"
#include "../EntitySystems/System.h"
#include "../EntitySystems/Component.h"
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
	bool playable=false;
	// pass the components you would like to listen for
	SoundSystem() : System({
		std::type_index(typeid(SoundComponent))
		}) {};
	~SoundSystem() {};

	virtual void update(float dt) override
	{
		// remember to cache the results!
		auto components = GetComponents<SoundComponent>();
		auto test1 = new Entity();

		// operate on the components
		for (SoundComponent* component : components)
		{
			auto action = component->GetAction();
			
			switch (action)
			{
			
			case AudioAction::REQUEST_PLAYMUSIC:
			{
				// load:   
				if (component->isMusic == true) {
					cout << "MUS" << endl;
					auto Bgm = GetBgm(component->audioPath);
					Mix_FadeInMusic(Bgm, -1, 2000);
					Mix_VolumeMusic(MIX_MAX_VOLUME * component->cVolume);
					component->SetAction(AudioAction::NOTHING);
				}
				
			}
			break;
			case AudioAction::REQUEST_PLAYFX: {
				cout << "sound" << endl;
				auto sfx = GetAudioChunk(component->audioPath);
				Mix_PlayChannel(component->cChannel, sfx, -1);
				Mix_Volume(component->cChannel, MIX_MAX_VOLUME * component->cVolume);
				// increment 
				//_currentChannel++;
				// clear component
				component->SetAction(AudioAction::NOTHING);
				break;
			}
			
			case AudioAction::REQUEST_STOP:
			{
				auto sfx = GetAudioChunk(component->audioPath);
				Mix_Pause(component->cChannel);
				//cout<<"test"<<endl;
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
			printf("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
		}

		// load new bgm 
		//Mix_FadeInMusic(gMusic, -1, 2000);
		//Mix_VolumeMusic(32);
		// if (Mix_PlayingMusic() == 0) {}
	}

private:
	Mix_Music *gMusic = NULL;
	Mix_Chunk *gSound = NULL;
	std::unordered_map<std::string, Mix_Chunk*> _sfx;
	std::unordered_map<std::string, Mix_Music*> _bgm;
	int _currentChannel = 1;

	Mix_Chunk* GetAudioChunk(std::string path)
	{
		// if chunk is unloaded - load 
		if (_sfx.find(path) == _sfx.end())
		{
			Mix_Chunk* chk = Mix_LoadWAV(path.c_str());
			_sfx[path] = chk;
			return  chk;
		}
		// else return loaded chunk
		else
		{
			return _sfx[path];
		}
	}
	Mix_Music* GetBgm(std::string pathbgm)
	{
		// if chunk is unloaded - load 
		if (_bgm.find(pathbgm) == _bgm.end())
		{
			Mix_Music* msc = Mix_LoadMUS(pathbgm.c_str());
			_bgm[pathbgm] = msc;
			return  msc;
		}
		// else return loaded chunk
		else
		{
			return _bgm[pathbgm];
		}
	}
};

