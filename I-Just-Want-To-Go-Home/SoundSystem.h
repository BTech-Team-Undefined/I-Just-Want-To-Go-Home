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

	Mix_Music *gMusic = NULL;
	Mix_Chunk *gSound = NULL;
	void playSound(const char* fSource) {
		gSound = Mix_LoadWAV(fSource);
		cout << "soundtest:" << fSource << endl;
	}

	virtual void update(float dt) override
	{
		// remember to cache the results!
		auto components = GetComponents<SoundComponent>();

		// operate on the components
		// ===== Test Sound =====
		playSound("file.test");
		gMusic = Mix_LoadMUS("Sound/BGM.wav");
		if (gMusic == NULL)
		{
			printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
		}
		if (Mix_PlayingMusic() == 0) {
			Mix_PlayMusic(gMusic, -1);
		}
		//playSound("");

		for (SoundComponent* component : components)
		{
			if (!component->playing)
			{
				playSound(component->audioPath.c_str());
				// load audio ( component -> audioPath );
				// play audio 
			}
		}
	}

};

