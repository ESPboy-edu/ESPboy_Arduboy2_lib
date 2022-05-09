/**********************************************************************************
 * Ardulem is an attempt to bring the legendary Lemmings to the Arduboy platform.
 * Ardulem is my first game on Arduboy to learn that console, so please be indulgent with the result.
 * 
 * This program is free and open source.
 * Author: Alban Nanty, 2017
 * 
 * If you want to report bugs or feature request, please do it through the bitbucket project page were
 * this code is hosted. However, there's no guaranty that I will fix them.
 *
 * NOTE TO CASUAL MODDER:
 *    - If you're looking to some constant that you can tune, 
 *      please open the "Ardulem.h" file, this is were I put them.
 *    - If you want to create your own levels, please read the README file
 *      where I explain how to use the level editor.
 */

#include "Ardulem.h"
#include "LemManager.h"
#include "HUD.h"
#include "Input.h"
#include "MapManager.h"
#include "MainMenu.h"
#include "MapData.h"
#include "LEDManager.h"
#include "ESPboyPlaytune.h"

CustomArduboy arduboy;
ESPboyPlaytune tune(arduboy.audio.enabled);


void setup(){
  
 Serial.begin(115200);
  
  arduboy.begin();
  arduboy.clear();
  tune.setSynth(EPT_SYNTH_PIN, 10); //decrease 10 to even thinner sound
  tune.initChannel(0);
	
	// read the current progression from the save, and set the current map id with it
	unsigned char progressionValue = EEPROM.read(MainMenu::PROGRESSION_SAVE_ADDRESS);
	// check if the value as been saved before
	if (progressionValue == 255){
		MainMenu::ResetSavedProgression();}
	else{
		// modulo the value, in case a previous value from another game was saved at that place
		MapManager::CurrentMapId = progressionValue % MapData::GetMapCount();}
}



void loop(){
	// early exit if the frame is not ready
	if (!arduboy.nextFrame())
		return;

	//Serial.println(arduboy.cpuLoad());

	// clear the screen
	arduboy.clear();

	// update the input
	Input::Update();

	// and draw the correct scene according to the current game state
	MainMenu::GameState currentGameState = MainMenu::GetCurrentGameState();
	if (currentGameState < MainMenu::GameState::PLAYING)
	{
		MainMenu::Update();
	}
	else
	{
		// update the various managers
		// at first draw the Map (and nothing else before, because the lem will use the screen buffer information)
		MapManager::Update();
		// then update the lem and draw them in two pass, so that all the lems have updated their logic, before being drawn
		// and polluting the screen buffer.
		LemManager::Update();
		LemManager::Draw();
		// then draw the start and home
		MapManager::DrawStartAndHome();
		// finally draw the HUD that will override the map and lem which has been drawn behind
		HUD::Update();
	}

	// update also the LED in any case
	LEDManager::Update();

	// draw the frame buffer
	arduboy.display();
}
