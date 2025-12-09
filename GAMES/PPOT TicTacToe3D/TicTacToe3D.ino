#include <Arduboy2.h>
#include <FixedPoints.h>
#include <FixedPointsCommon.h>
#include "TicTacToe3D_Constants.h"
#include "TicTacToe3D_Images.h"
#include "TicTacToe3D_Enums.h"

#define DEBUG_BREAK    asm volatile("break\n");

using Fixed = SQ15x16;

Arduboy2 arduboy;

GameState gameState = GameState::Splash;

uint8_t cursor[3] = {2, 0, 0};				// z, y, x
uint8_t board[3][3][3] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};		// z, y, x

uint16_t bCount = 0;
uint8_t playerOrComp = 0;
uint8_t playAgainCursor = 0;
uint8_t player1Win = 0;
uint8_t player2Win = 0;


int8_t Background_M_Cursor_X[3] = 				{ 0, 8, 16 };
int8_t Background_M_Cursor_Y[3] = 				{ 16, 12, 8 };
int8_t Background_M_Cursor_Y_XOffset[3] = 		{ 0, -4, -8 };
int8_t Background_M_Cursor_X_YOffset[3] = 		{ 0, -8, -16 };

uint8_t numberOfPlayers = 1;

bool xRotate = false;
bool yRotate = false;
int8_t xRotateDirection = 0;
int8_t yRotateDirection = 0;
uint16_t delayCount = 0;

Fixed rotationAngle = 0;
Fixed gridSize = 26;
Fixed layerSpacing = 21;
uint8_t gameOverCounter = 0;
uint8_t middlePositionCount = 0;

bool flashCrosses = false;
bool flashNoughts = false;


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Setup
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void setup() {

	//arduboy.boot();
	//arduboy.flashlight(); 
	
	arduboy.begin();
	arduboy.audio.begin();

	arduboy.initRandomSeed();
	arduboy.setFrameRate(30);

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Main loop
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void loop() {

	if (!(arduboy.nextFrame())) return;
	arduboy.clear();
	arduboy.pollButtons();

	switch (gameState) {

		case GameState::Splash:
			showSplashScreen();
			break;

		case GameState::Title:
			showTitle();
			break;

		case GameState::NumberoFPlayers:
			selectNumberOfPlayers();
			break;

		case GameState::Instructions:
			showInstructions();
			break;

		case GameState::Game:
		case GameState::GameOver:
			playGame();
			break;

		case GameState::Game_MiddleWarning:
			playGame_MiddleWarning();
			break;

		case GameState::GameOver_View_1:
		case GameState::GameOver_View_2:
		case GameState::GameOver_View_3:
			showResults();
			break;

		case GameState::PlayAgain:
			playAgain();
			break;

	}

    arduboy.display(true);

}

void showTitle() {

	player1Win = 0;
	player2Win = 0;

	Sprites::drawOverwrite(0, 0, Images::Title, 0);

	if (arduboy.justPressed(A_BUTTON)) {

		startGame();

	}

}

void selectNumberOfPlayers() {

	Sprites::drawOverwrite(0, 0, Images::NumberOfPlayers, 0);
	Sprites::drawOverwrite(18, 17 + (numberOfPlayers * 13), Images::NumberOfPlayers_Cursor, 0);

	if (arduboy.justPressed(UP_BUTTON)) {
		numberOfPlayers = 1;
	}

	if (arduboy.justPressed(DOWN_BUTTON)) {
		numberOfPlayers = 2;
	}

	if (arduboy.justPressed(A_BUTTON)) {

		gameState = GameState::Instructions;

	}

}


void playAgain() {

	Sprites::drawOverwrite(0, 0, Images::PlayAgain, 0);
	Sprites::drawOverwrite(18, 30 + (playAgainCursor * 13), Images::NumberOfPlayers_Cursor, 0);

	if (arduboy.justPressed(UP_BUTTON)) {
		playAgainCursor = 0;
	}

	if (arduboy.justPressed(DOWN_BUTTON)) {
		playAgainCursor = 1;
	}

	if (arduboy.justPressed(A_BUTTON)) {

		if (playAgainCursor == 1) {
			gameState = GameState::Title;
		}
		else {
			startGame();
			gameState = GameState::Game;
		}

	}

}


void showInstructions() {

	Sprites::drawOverwrite(6, 8, Images::Instructions, 0);

	if (arduboy.justPressed(A_BUTTON)) {

		startGame();
		gameState = GameState::Game;

	}

}

void playGame_MiddleWarning() {

	renderBoard();
	Sprites::drawOverwrite(14, 20, Images::MiddlePosition, 0);

	if (arduboy.justPressed(A_BUTTON)) {

		gameState = GameState::Game;

	}

}

void playGame() {

	if (delayCount > 0) delay(delayCount);
	delayCount = 0;

	Fixed centerY = layerSpacing - Fixed(15);

	if (xRotate) {

	  	rotateGridX(xRotateDirection); 
		drawGridLayer_X(Fixed(-15), gridSize, centerY);                          		// Bottom layer
		drawGridLayer_X(centerY, gridSize, centerY);            						// Middle layer - fill bottom
		drawGridLayer_X((layerSpacing * Fixed(2)) - Fixed(15), gridSize, centerY);      // Top layer 


	}
	else if (yRotate) {

		rotateGridY(yRotateDirection); 
		drawGridLayer_Y(Fixed(-15), gridSize);
		drawGridLayer_Y(centerY, gridSize);
		drawGridLayer_Y((layerSpacing * Fixed(2)) - Fixed(15), gridSize);

	}
	else {
	
		renderBoard();

	}

	renderGameTally();

	switch (bCount) {

		case 0 ... 8:
			
			if (arduboy.justPressed(LEFT_BUTTON) && cursor[Constants::X_Axis] > 0) 		{ cursor[Constants::X_Axis]--; }
			if (arduboy.justPressed(RIGHT_BUTTON) && cursor[Constants::X_Axis] < 2) 	{ cursor[Constants::X_Axis]++; }
			if (arduboy.justPressed(UP_BUTTON) && cursor[Constants::Y_Axis] < 2) 		{ cursor[Constants::Y_Axis]++; }
			if (arduboy.justPressed(DOWN_BUTTON) && cursor[Constants::Y_Axis] > 0) 		{ cursor[Constants::Y_Axis]--; }

			break;

		default:

			if (arduboy.justPressed(UP_BUTTON)) {

				xRotate = true;
				xRotateDirection = 1;
				rotationAngle = 0;

			}
			
			if (arduboy.justPressed(DOWN_BUTTON)) {

				xRotate = true;
				xRotateDirection = -1;
				rotationAngle = 0;

			}

			if (arduboy.justPressed(LEFT_BUTTON)) {

				yRotate = true;
				yRotateDirection = 1;
				rotationAngle = 0;

			}

			if (arduboy.justPressed(RIGHT_BUTTON)) {

				yRotate = true;
				yRotateDirection = -1;
				rotationAngle = 0;
			
			}

			break;

	}

	if (arduboy.pressed(B_BUTTON)) {

		bCount++;

	}

	if (arduboy.justReleased(B_BUTTON)) {

		if (bCount < 8) {

			cursor[Constants::Z_Axis]++; 
			if (cursor[Constants::Z_Axis] > 2) cursor[Constants::Z_Axis] = 0; 

		}

		bCount = 0; 

	}


	// Have a turn ..

	switch (gameState) {

		case GameState::Game:

			if (playerOrComp == 0) {

				if (arduboy.justPressed(A_BUTTON) && board[cursor[Constants::Z_Axis]][cursor[Constants::Y_Axis]][cursor[Constants::X_Axis]] == Players::None) {

					if (middlePositionCount >= 4 || cursor[Constants::Z_Axis] != 1 || cursor[Constants::Y_Axis] != 1 || cursor[Constants::X_Axis] != 1) {
						
						board[cursor[Constants::Z_Axis]][cursor[Constants::Y_Axis]][cursor[Constants::X_Axis]] = Players::Noughts;

						switch (checkForWin()) {

							case Players::None:
								middlePositionCount++;
								playerOrComp = 1; 		// swap players
								break;

							case Players::Noughts:
								gameState = GameState::GameOver;	
								gameOverCounter = 0;								
								flashNoughts = true;
								player1Win++;
								break;

						}

					}
					else {

						gameState = GameState::Game_MiddleWarning;

					}

				}

			}

			else if (numberOfPlayers == 2 && playerOrComp == 1) {

				if (arduboy.justPressed(A_BUTTON) && board[cursor[Constants::Z_Axis]][cursor[Constants::Y_Axis]][cursor[Constants::X_Axis]] == Players::None) {

					if (middlePositionCount >= 4 || cursor[Constants::Z_Axis] != 1 || cursor[Constants::Y_Axis] != 1 || cursor[Constants::X_Axis] != 1) {
						
						board[cursor[Constants::Z_Axis]][cursor[Constants::Y_Axis]][cursor[Constants::X_Axis]] = Players::Crosses;

						switch (checkForWin()) {

							case Players::None:
								middlePositionCount++;
								playerOrComp = 0; 		// swap players
								break;

							case Players::Crosses:
								gameState = GameState::GameOver;
								gameOverCounter = 0;								
								flashCrosses = true;
								player2Win++;
								break;

						}

					}
					else {

						gameState = GameState::Game_MiddleWarning;

					}

				}

			}

			else {
			
				computerMove();
				middlePositionCount++;
				delayCount = 512;


				// Check for win ..

				if (checkForWin() != Players::None) {
				
					gameState = GameState::GameOver;
					gameOverCounter = 0;
					player2Win++;

				}
			
			}

			break;

		case GameState::GameOver:

			switch (checkForWin()) {

				case Players::Noughts:

					if (gameOverCounter != Constants::GameOverCounter_NoAction) {
						
						for (uint8_t i = 0; i < 15; i++) {

							uint8_t c = pgm_read_byte(&GameOver::YouWon_Letters[i]);
							uint8_t x = pgm_read_byte(&GameOver::YouWon_X[i]);
							int8_t y = pgm_read_byte(&GameOver::YouWon_Y[(gameOverCounter * 15) + i]);

							const uint8_t* letterPtr = (const uint8_t*)pgm_read_ptr(&Images::Letters[c]);
							const uint8_t* maskPtr = (const uint8_t*)pgm_read_ptr(&Images::Letters_Mask[c]);


							Sprites::drawExternalMask(x, y, letterPtr, maskPtr, 0, 0);
						}

						if (arduboy.frameCount % 2 == 0 && gameOverCounter < 54) gameOverCounter++;

					}			

					break;

				case Players::Crosses:

					if (gameOverCounter != Constants::GameOverCounter_NoAction) {

						if (numberOfPlayers == 2) {

							for (uint8_t i = 0; i < 15; i++) {

								uint8_t c = pgm_read_byte(&GameOver::YouWon_Letters[i]);
								uint8_t x = pgm_read_byte(&GameOver::YouWon_X[i]);
								int8_t y = pgm_read_byte(&GameOver::YouWon_Y[(gameOverCounter * 15) + i]);

								const uint8_t* letterPtr = (const uint8_t*)pgm_read_ptr(&Images::Letters[c]);
								const uint8_t* maskPtr = (const uint8_t*)pgm_read_ptr(&Images::Letters_Mask[c]);


								Sprites::drawExternalMask(x, y, letterPtr, maskPtr, 0, 0);
							}

							if (arduboy.frameCount % 2 == 0 && gameOverCounter < 54) gameOverCounter++;
							
						}
						else {

							for (uint8_t i = 0; i < 16; i++) {

								uint8_t c = pgm_read_byte(&GameOver::YouLost_Letters[i]);
								uint8_t x = pgm_read_byte(&GameOver::YouLost_X[i]);
								int8_t y = pgm_read_byte(&GameOver::YouLost_Y[(gameOverCounter * 16) + i]);

								const uint8_t* letterPtr = (const uint8_t*)pgm_read_ptr(&Images::Letters[c]);
								const uint8_t* maskPtr = (const uint8_t*)pgm_read_ptr(&Images::Letters_Mask[c]);

								Sprites::drawExternalMask(x, y, letterPtr, maskPtr, 0, 0);

							}

							if (arduboy.frameCount % 2 == 0 && gameOverCounter < 54) gameOverCounter++;

						}

					}				
	
					break;

				case Players::Draw:

					if (gameOverCounter != Constants::GameOverCounter_NoAction) {
						
						for (uint8_t i = 0; i < 13; i++) {

							uint8_t c = pgm_read_byte(&GameOver::Draw_Letters[i]);
							uint8_t x = pgm_read_byte(&GameOver::Draw_X[i]);
							int8_t y = pgm_read_byte(&GameOver::Draw_Y[(gameOverCounter * 13) + i]);

							const uint8_t* letterPtr = (const uint8_t*)pgm_read_ptr(&Images::Letters[c]);
							const uint8_t* maskPtr = (const uint8_t*)pgm_read_ptr(&Images::Letters_Mask[c]);

							Sprites::drawExternalMask(x, y, letterPtr, maskPtr, 0, 0);

						}

						if (arduboy.frameCount % 2 == 0 && gameOverCounter < 44) gameOverCounter++;

					}				

					break;

			}

			if (arduboy.justPressed(A_BUTTON)) {

				gameState = GameState::GameOver_View_1;
				bCount = 0;

			}

			break;

	}
    

	// if (arduboy.notPressed(A_BUTTON)) aCount = 0;
	if (arduboy.notPressed(B_BUTTON)) bCount = 0;


}

void showResults() {

	uint8_t xOffset = 0;

	renderGameTally();

	switch (gameState) {
		
		case GameState::GameOver_View_1:

			cursor[Constants::Z_Axis] = 2;
			Sprites::drawOverwrite(38, 40, Images::Background, 0);
			renderPieces(xOffset, 2, true);
			break;
	
		case GameState::GameOver_View_2:

			cursor[Constants::Z_Axis] = 2;
			Sprites::drawOverwrite(38, 40, Images::Background, 0);
			renderPieces(xOffset, 2, true);
			cursor[Constants::Z_Axis] = 1;
			Sprites::drawExternalMask(38, 18, Images::Background, Images::Background_Mask, 0, 0);
			renderPieces(xOffset, 1, true);
			break;
	
		case GameState::GameOver_View_3:

			cursor[Constants::Z_Axis] = 2;
			Sprites::drawOverwrite(38, 40,  Images::Background, 0);
			renderPieces(xOffset, 2, true);
			cursor[Constants::Z_Axis] = 1;
			Sprites::drawExternalMask(38, 18, Images::Background, Images::Background_Mask, 0, 0);
			renderPieces(xOffset, 1, true);
			cursor[Constants::Z_Axis] = 0;
			Sprites::drawExternalMask(38, -4, Images::Background, Images::Background_Mask, 0, 0);
			renderPieces(xOffset, 0, true);
			break;
	
	}


	// Cycle graphics ..

	bCount++;

	if (bCount % 48 == 0) {

		bCount = 0;
		gameState = static_cast<GameState>(static_cast<uint8_t>(gameState) + 1);

		if (gameState == GameState::EndState) { gameState = GameState::GameOver_View_1; }

	}

	if (arduboy.justPressed(A_BUTTON)) {

		gameState = GameState::PlayAgain;

	}

}

void showSplashScreen() {

	Sprites::drawOverwrite(32, 17, Images::PPOT, 0);

	uint8_t y = 17; // Default pixel position 1 is hidden in the top line of the image

	switch (arduboy.frameCount % 32) {

		case 8 ... 15:
			y = 30; // Move pixel down to position 2
			[[fallthrough]]

		case 0 ... 7:
			Sprites::drawOverwrite(91, 25, Images::PPOT_Arrow, 0); // Flash 'Play' arrow
			break;

		case 16 ... 24:
			y = 31; // Move pixel down to position 3
			break;

		default: // 36 ... 47:
			y = 32; // Move pixel down to position 4
			break;

	}

	arduboy.drawPixel(52, y, WHITE); // Falling pixel represents the tape spooling

	if (y % 2 == 0) { 

		Sprites::drawOverwrite(45, 28, Images::PPOT_Spindle, 0);

	}

	if (arduboy.justPressed(A_BUTTON)) {

		gameState = GameState::Title;
		arduboy.pollButtons();

	}

}
