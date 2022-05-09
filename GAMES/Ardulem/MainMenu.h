#ifndef MAIN_MENU_H
#define MAIN_MENU_H
/*
 * This file contains the code for the main menu
 */

namespace MainMenu
{
	// an address in the EEPROM to save the progression (the last level played and successful)
	const int16_t PROGRESSION_SAVE_ADDRESS = 16;
	
	enum GameState
	{
		MENU_PLAY = 0,
		MENU_LEVEL,
		MENU_MUSIC,
		MENU_RESET_SAVE,
		RESULT_PAGE,
		PLAYING,
		PLAYING_ABANDON,  // the lem are playing normally, but the player has abandon, and all lem will explode
		PAUSED,
		QUIT_WARNING,
	};
	
	// get the current game state
	extern GameState	GetCurrentGameState();
	extern void 		SetCurrentGameState(GameState state);
	extern bool			IsCurrentGameStatePlaying();
	extern void 		ResetSavedProgression();
	
	extern void Update();
}

#endif
