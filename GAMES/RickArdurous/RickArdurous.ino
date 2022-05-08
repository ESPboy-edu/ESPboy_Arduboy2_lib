/**
* Rick Ardurous is a freeware Game for Arduboy platform written by Alban Nanty
*/

#include "RickArdurous.h"
#include "Input.h"
#include "FXManager.h"
#include "GameManager.h"

// instance of the arduboy class
CustomArduboy arduboy;

void setup(){

	Serial.begin(115200);

	arduboy.begin();
	arduboy.clear();
	
	// init the random generator
	//arduboy.initRandomSeed();
	
	// go to main menu
	GameManager::GoToMainMenu();
}

void loop()
{
	// early exit if the frame is not ready
	if (!arduboy.nextFrame())
		return;

//#ifdef PROFILING
//	Serial.println(arduboy.cpuLoad());
//#endif

	// clear the screen
	arduboy.clear();

	// update the input
	Input::Update();

	// update the Special Effects
	FXManager::Update();
	
	// update the game state
	GameManager::Update();
	
	// draw the frame buffer
	arduboy.display();
}
