#ifndef _GAME_MANAGER_H_
#define _GAME_MANAGER_H_

namespace GameManager
{
	enum GameState : unsigned char
	{
		MAIN_MENU = 0,
		PLAYING,
		GAME_OVER,
		VICTORY,
		HELP_SCREEN,
	};
	
	extern GameState CurrentGameState;
	
	void Update();
	void StartNewGame();
	void GoToMainMenu();
}

#endif