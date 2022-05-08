/**
* The Main menu display the menu to start the game or change options
*/

#include "RickArdurous.h"
#include "MainMenu.h"
#include "GameManager.h"
#include "Input.h"
#include "MapData.h"
#include "SpriteData.h"
#include "Graal.h"
#include "Rick.h"
#include "FXManager.h"

namespace MainMenu
{
	static constexpr int LAST_MENU_OPTION = 2;
	static constexpr int MENU_X = 40;
	static constexpr int GAME_OVER_ANIM_SPEED = 4;
	
	unsigned char SelectedOption = 0;
	
	// variable for game over animation
	signed char SpriteY;
	unsigned char GameOverAnimId = SpriteData::RickAnimFrameId::DEATH_START;
	signed char GameOverAnimDirection = -1;
	
	void DrawMainMenu();
	void PrintNumber(unsigned char number);
	void DrawStatuetteCount();
	void DrawVictory();
	void DrawGameOver();
	void DrawHelpScreen();
	void InitGameOver();
}

void MainMenu::UpdateMainMenu()
{
	bool playFX = false;
	
	// check the input
	if (Input::IsJustPressed(DOWN_BUTTON) && (SelectedOption < LAST_MENU_OPTION))
	{
		SelectedOption++;
		playFX = true;
	}
	else if (Input::IsJustPressed(UP_BUTTON) && (SelectedOption > 0))
	{
		SelectedOption--;
		playFX = true;
	}
	else if (Input::IsJustPressed(A_BUTTON) || Input::IsJustPressed(B_BUTTON))
	{
		switch (SelectedOption)
		{
			case 0:
				InitGameOver();
				GameManager::StartNewGame();
				break;
			case 1:
				FXManager::IsSoundOn = !FXManager::IsSoundOn;
				break;
			case 2:
				// switch the current game state
				GameManager::CurrentGameState = GameManager::GameState::HELP_SCREEN;
				break;
		}
	}
	
	// play the sound fx when moving the menu option
	if (playFX)
		FXManager::StartFXCommand(FXManager::BufferId::GAME, {10, 0, 0, 1, 0, 60, 0, 1});
	
	// draw the menu
	DrawMainMenu();
}

void MainMenu::UpdateGameOver(bool isVictory)
{
	// wait for user to press a button
	if (Input::IsJustPressed(A_BUTTON) || Input::IsJustPressed(B_BUTTON))
		GameManager::GoToMainMenu();
	
	if (isVictory)
		DrawVictory();
	else
		DrawGameOver();
}

void MainMenu::DrawMainMenu()
{
	// draw an extra line of stones on top of the screen (to fill the gap of the HUD)
	for (unsigned char i = 0; i < 16; ++i)
		arduboy.drawBitmapExtended(i * SpriteData::LEVEL_SPRITE_WIDTH, 0,
				SpriteData::Walls[1 + (i % 2)], SpriteData::LEVEL_SPRITE_WIDTH, SpriteData::LEVEL_SPRITE_HEIGHT, WHITE, (i % 3));

	// draw the title in black over white
	arduboy.fillRect(24, 1, 84, 11, WHITE);
	unsigned char title[] = "Rick Ardurous";
	for (unsigned char i = 0; i < sizeof(title) - 1; ++i)
		arduboy.drawChar(28 + (i*6), 3, title[i], BLACK, WHITE);
	
	// draw a black rectangle to erase the center of the level
	arduboy.fillRect(56, 46, 16, 16, BLACK);
	arduboy.fillRect(19, 62, 109, 2, BLACK);

	// draw the menu
	unsigned char menuY[] = { 30, 41, 52 };
	arduboy.setCursor(MENU_X, menuY[0]);
	arduboy.print("Play");
	arduboy.setCursor(MENU_X, menuY[1]);
	arduboy.print("Sound O");
	if (FXManager::IsSoundOn)
		arduboy.print("n");
	else
		arduboy.print("ff");
	arduboy.setCursor(MENU_X, menuY[2]);
	arduboy.print("Controls");
	
	// draw the selected indicator
	arduboy.drawChar(MENU_X - 8, menuY[SelectedOption], '>', WHITE, BLACK);
}

void MainMenu::PrintNumber(unsigned char number)
{
	// count how many character we need to print the specified number
	unsigned char charCount = 0;
	unsigned char numberCopy = number;
	do
	{
		charCount++;
		numberCopy /= 10;
	} while (numberCopy > 0);
	
	// get the current cursor position, and reset it at the end of the number
	unsigned char x = arduboy.GetCursorX() + ((charCount - 1) * 6);
	unsigned char y = arduboy.GetCursorY();
	arduboy.setCursor(x + 6, y);
	
	// iterate on the number of charecter to draw
	for (unsigned char i = 0; i < charCount; ++i)
	{
		arduboy.drawChar(x, y, (unsigned char)(48 + (number % 10)), WHITE, BLACK);
		number /= 10;
		x -= 6;
	}
}

void MainMenu::DrawStatuetteCount()
{
	// print the statuette count and the max statuette count
	PrintNumber(Rick::StatuetteCount);
	arduboy.print("/");
	PrintNumber(MapManager::MAX_STATUETTE_COUNT);
	// draw the statuette icon
	arduboy.drawBitmapExtended(arduboy.GetCursorX() + 2, arduboy.GetCursorY() -1, SpriteData::Statuette, SpriteData::STATUETTE_SPRITE_WIDTH, SpriteData::STATUETTE_SPRITE_HEIGHT, WHITE, false);
	// add a space and the width of the statuette for the cursor
	arduboy.setCursor(arduboy.GetCursorX() + (8 + SpriteData::STATUETTE_SPRITE_WIDTH), arduboy.GetCursorY());
}

void MainMenu::DrawVictory()
{
	// draw the graal!
	arduboy.drawBitmapExtended(61, 8, SpriteData::Graal, SpriteData::GRAAL_SPRITE_WIDTH, SpriteData::GRAAL_SPRITE_HEIGHT, WHITE, false);
	graal1.UpdateShineStar(61, 8, -2, 3, -2, 4);

	// draw the message
	arduboy.setCursor(21, 26);
	arduboy.print("Congratulation!");
	arduboy.setCursor(7, 46);
	arduboy.print("You found the Grail");
	arduboy.setCursor(43, 56);
	arduboy.print("and ");
	DrawStatuetteCount();
}

void MainMenu::InitGameOver()
{
	SpriteY = 63;
	GameOverAnimId = SpriteData::RickAnimFrameId::DEATH_START;
	GameOverAnimDirection = -1;
}

void MainMenu::DrawGameOver()
{
	// updat the animation of Rick sprite
	if (arduboy.everyXFrames(GAME_OVER_ANIM_SPEED))
	{
		// reverse direction if we reach the end of the loop
		if ((GameOverAnimId == SpriteData::RickAnimFrameId::DEATH_START) ||
			(GameOverAnimId == SpriteData::RickAnimFrameId::DEATH_END))
			GameOverAnimDirection = -GameOverAnimDirection;
		// increase the anim frame
		GameOverAnimId += GameOverAnimDirection;
		// decrease altitude
		SpriteY--;
	}
	// draw rick sprite
	arduboy.drawBitmapExtended(GameOverAnimId - SpriteData::RickAnimFrameId::DEATH_START, SpriteY, SpriteData::Rick[GameOverAnimId], SpriteData::RICK_SPRITE_WIDTH, SpriteData::RICK_SPRITE_HEIGHT, WHITE, false);

	arduboy.setCursor(16, 8);
	arduboy.print("Game Over...");
	arduboy.setCursor(16, 26);
	arduboy.print("You died before");
	arduboy.setCursor(16, 36);
	arduboy.print("reaching the Grail");
	arduboy.setCursor(16, 54);
	DrawStatuetteCount();
	arduboy.print("collected");
}

void MainMenu::UpdateHelpScreen()
{
	// wait for user to press a button
	if (Input::IsJustPressed(A_BUTTON) || Input::IsJustPressed(B_BUTTON))
		GameManager::GoToMainMenu();
	
	DrawHelpScreen();
}

void MainMenu::DrawHelpScreen()
{
	arduboy.setCursor(35, 0);
	arduboy.print("Controls");
	arduboy.fillRect(33, 10, 51, 2, WHITE);
	arduboy.setCursor(9, 20);
	arduboy.print("+: Move");
	arduboy.setCursor(9, 28);
	arduboy.print("A: Jump");
	arduboy.setCursor(9, 36);
	arduboy.print("B: Fire");
	arduboy.setCursor(2, 44);
	arduboy.print("Down + B: Dynamite");
	arduboy.setCursor(0, 52);
	arduboy.print("Down + <->: Crawl");
}
