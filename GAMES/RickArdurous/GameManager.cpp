/**
* The Game Manager handle the whole game state (menu, game playing, game over, etc...)
*/

#include "RickArdurous.h"
#include "GameManager.h"
#include "MapManager.h"
#include "HUD.h"
#include "MainMenu.h"
#include "Rick.h"
#include "MapData.h"
#include "MapDataConstVariables.h"
#include "Graal.h"

GameManager::GameState GameManager::CurrentGameState;

void GameManager::Update()
{
	switch (CurrentGameState)
	{
		case GameState::MAIN_MENU:
			MapManager::Update();
			MainMenu::UpdateMainMenu();
			break;
			
		case GameState::PLAYING:
			// update the managers
			MapManager::Update();
			// update the HUD
			HUD::Update();
			break;
			
		case GameState::GAME_OVER:
			MainMenu::UpdateGameOver(false);
			break;

		case GameState::VICTORY:
			MainMenu::UpdateGameOver(true);
			break;

		case GameState::HELP_SCREEN:
			MainMenu::UpdateHelpScreen();
			break;
	}
}

void GameManager::StartNewGame()
{
	// reset the life count and inventory of Rick
	Rick::Reset();
	
	// reset the alive status of all the items
	MapManager::SaveAndLoadAliveStatusForAllItems(0, 0);
	
	// reset also the graal live state
	graal1.SetProperty(Item::PropertyFlags::ALIVE);
	
	// call reset of the MapManger with the first screen id of the game
	MapManager::Reset(1, MapManager::FIRST_PUZZLE_SCREEN_CAMERA_X, MapManager::FIRST_PUZZLE_SCREEN_CAMERA_Y);
	
	// switch the current game state
	CurrentGameState = GameState::PLAYING;
}

void GameManager::GoToMainMenu()
{
	// init the Map Manager with the first screen
	MapManager::Reset(0, MapManager::MAIN_MENU_CAMERA_X, MapManager::MAIN_MENU_CAMERA_Y);
	
	// move Rick out the screen
	Rick::MoveOutsideMainMenu();
	
	// switch the current game state
	CurrentGameState = GameState::MAIN_MENU;
}