#include "src/Arduboy2Ext.h"
#include <Sprites.h>
#include "src/Enums.h"
#include "src/Images.h"
#include "src/Constants.h"
#include "src/Menu.h"
#include "src/Maze.h"
#include "src/Player.h"
#include "src/EEPROM_Utils.h"

//#define DEBUG_BREAK    asm volatile("break\n");

Arduboy2Ext arduboy;


// Camera offset
Point camera = { 0, 0 };
Point camera_Small = { 0, 0 };


Maze maze;
Menu menu;
Player player;
Item puff;
Item death;
Item bullet;

GameState gameState = GameState::PPOT;

uint8_t clearedLevel = 0;
uint8_t level = 0;
uint8_t mapLevel = 0;
uint8_t showEnemyCursors = 0;
uint8_t show = 0;
uint8_t displayChestsCounter = 0;
uint8_t enableMaps = 0;
uint8_t menuCursor = 0;

bool darkMode = false;
int16_t darkModeCounter = -256;


void setup() {

	arduboy.begin();
	arduboy.setFrameRate(25);
	randomSeed(arduboy.generateRandomSeed());
	
  EEPROM_Utils::initEEPROM(false);
}

void loop() {

	if (!(arduboy.nextFrame())) return;

	arduboy.pollButtons();
	arduboy.clear();

	uint8_t justPressed = arduboy.justPressedButtons();
	uint8_t pressed = arduboy.pressedButtons();

	switch (gameState) {

		case GameState::PPOT:
			splashScreen();
			break;

		case GameState::Menu_Init:

			initCoins();
			gameState = GameState::Menu;
			[[fallthrough]]
	
		case GameState::Menu:

			handleTitle();
			break;

		case GameState::Menu_Select:

			Sprites::drawExternalMask(0, 22, Images::Title, Images::Title_Mask, 0, 0);
			Sprites::drawOverwrite(19, 44, Images::Cursors, menuCursor == 1 || arduboy.frameCount % 40 < 20 ? showEnemyCursors : 2);
			Sprites::drawOverwrite(19, 52, Images::EnableMaps, menuCursor == 0 || arduboy.frameCount % 40 < 20 ? enableMaps : 2);

			if (justPressed & UP_BUTTON) {
				menuCursor = 0;
			}

			if (justPressed & DOWN_BUTTON) {
				menuCursor = 1;
			}

			if (justPressed & LEFT_BUTTON) {

				if (menuCursor == 0) {
					showEnemyCursors = 0;
				}
				else {
					enableMaps = 0;
				}

			}

			if (justPressed & RIGHT_BUTTON) {

				if (menuCursor == 0) {
					showEnemyCursors = 1;
				}
				else {
					enableMaps = 1;
				}

			}

			if (justPressed & A_BUTTON) {

				randomSeed(arduboy.generateRandomSeed());
				gameState = GameState::GamePlay;
				arduboy.frameCount = 0;
				maze.setEnemyCount(2);

				clearedLevel = 0;
				startGame(true);

			}	
			break;

		case GameState::GamePlay:

			handleDarkMode();
			updatePlayer();
			updateCamera();
			updateEnemys(level);
			checkCollisions(level);
			drawStuff();

			if (displayChestsCounter > 0) {

				Sprites::drawOverwrite(0, 26, Images::ChestsCollected, 0);
				Sprites::drawOverwrite(98, 26, Images::ChestsCollected_Numbers, 9 - maze.getActiveChests());
				displayChestsCounter--;
			
			}

			break;

		case GameState::ShowMenu:

			handleMenu();
			break;

		case GameState::Map:

			handleMenu_ShowMap();
			break;

		case GameState::GameOver:
			{
				darkMode = false;
				darkModeCounter = -256;
				uint16_t score = clearedLevel * 10 + (10 - maze.getActiveChests());

				drawStuff(); 

				Sprites::drawOverwrite(0, 24, Images::GameOver, 0);

				if (justPressed & A_BUTTON) {

					if (score >= EEPROM_Utils::getScore()) {
						EEPROM_Utils::saveScore(score);
					}

					gameState = GameState::HighScore;
				}

			}

			break;

		case GameState::HighScore:
			{
				uint16_t score = clearedLevel * 10 + (10 - maze.getActiveChests());
				Sprites::drawOverwrite(0, 24, Images::HighScore_Banner, 0);

				if ((score >= EEPROM_Utils::getScore() && arduboy.frameCount % 36 < 18) || score < EEPROM_Utils::getScore()) {

					renderHighScore(score);

				}

				if (justPressed & A_BUTTON) {

					gameState = GameState::Menu_Init;

				}

			}

			break;

		case GameState::LevelUp:
			// drawMaze(level);
			// drawChests(level);
			// drawEnemies(level);
			// drawPlayer();
			// drawDeath();
			// drawPuff();
			// drawFlashlight();    
			drawStuff();

			Sprites::drawOverwrite(0, 24, Images::LevelUp, 0);

			if (justPressed & A_BUTTON) {

				if (maze.getEnemyCount() < Constants::MaxEnemys / 2) maze.setEnemyCount(maze.getEnemyCount() + 1);

				gameState = GameState::GamePlay;
				clearedLevel++;
				startGame(false);

			}
			break;

	}

	arduboy.display();

}

void drawStuff() {

	drawMaze(level);
	drawChests(level);
	drawItems(level);
	drawEnemies(level);
	drawPlayer();
	drawDeath();
	drawPuff();
	drawBullet();
	drawFlashlight();   

}

void startGame(bool clearInventory) {

	uint8_t stairsPlaced = 0;

	level = 0;

	maze.generateMaze(player, arduboy.sBuffer);
	player.reset(clearInventory, enableMaps == 0);

	camera.x = 0;
	camera.y = 0;

	menu.y = 0;
	menu.top = 0;

	displayChestsCounter = 0;
	arduboy.frameCount == 0;
	darkMode = false;
	darkModeCounter = -256;

}

void handleDarkMode() {

	if (darkModeCounter == -256  && (level == 1 || clearedLevel > 1 || (level == 0 && clearedLevel == 0 && arduboy.frameCount > 4096)) && random(512) == 0) {
		darkModeCounter = 312;
	}

	switch (darkModeCounter) {

		case 312:
		case 310:
		case 308:
		case 305:

		case 10:
		case 7:
		case 5:
			arduboy.invert(true);
			darkModeCounter--;
			break;

		case 302:
			arduboy.invert(true);
			darkModeCounter--;
			darkMode = true;
			break;

		case 2:
			arduboy.invert(true);
			darkModeCounter--;
			darkMode = false;
			break;

		case -256:
			break;

		default:
			arduboy.invert(false);
			darkModeCounter--;
			break;

	}

}
