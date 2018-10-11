#include "Game.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <time.h>

#pragma warning(disable : 4996) //disables warning for time
using namespace std;
Game::Game()
{
}


Game::~Game()
{

}

void Game::Loop() {
	int i = 1;

	while (true)
	{
		auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now());

		cout << i << " : " << ctime(&timenow) << endl;
		i++;
	}
}