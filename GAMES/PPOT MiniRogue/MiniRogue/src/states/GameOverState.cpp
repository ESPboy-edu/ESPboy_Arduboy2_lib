#include "GameOverState.h"
#include "../utils/Arduboy2Ext.h"
#include "../images/Images.h"
//#include <avr/eeprom.h> 


#define eeprom_read_byte EEPROM.read
#define eeprom_update_byte EEPROM.write

#define EEPROM_START                  (/*(uint8_t *)*/140)
#define EEPROM_START_C1               (/*(uint8_t *)*/140)
#define EEPROM_START_C2               (/*(uint8_t *)*/141)
#define EEPROM_SCORE                  142

#define FlashString FPSTR

// ----------------------------------------------------------------------------
//  Initialise state ..
//
void GameOverState::activate(StateMachine & machine) {
	
	auto & gameStats = machine.getContext().gameStats;
	auto & playerStats = machine.getContext().playerStats;

	initEEPROM(false);
	
	viewState = ViewState::Winner;
	score = 0;
	highScore = 0;
	
	switch (machine.getContext().gameState) {

		case GameStateType::Winner:
			viewState = ViewState::Winner; 
			break;

		default:
			viewState = ViewState::HighScore; 
			break;

	}

	this->score = 0;
	this->score += (gameStats.skillLevel * 3);
	this->score += (playerStats.xpTrack * 2);
	this->score += ((gameStats.level + 1) * 3);
	this->score += (playerStats.hp * 2);
	this->score += (playerStats.food);
	this->score += (playerStats.armour);
	this->score += (playerStats.gold * 2);
	this->score += (playerStats.bossesKilled * 2);
	this->score += (playerStats.itemCount());

	this->highScore = eeprom_read_byte((/*(uint8_t *)*/EEPROM_SCORE + gameStats.skillLevel));

	if (this->score > this->highScore) {

		this->highScore = this->score;
		eeprom_update_byte(/*(uint8_t *)*/(EEPROM_SCORE + gameStats.skillLevel), this->score);

	}
	EEPROM.commit();

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void GameOverState::update(StateMachine & machine) { 

	auto & arduboy = machine.getContext().arduboy;
  auto justPressed = arduboy.justPressedButtons();
  auto pressed = arduboy.pressedButtons();

  if (justPressed & A_BUTTON) { 
		
		switch (this->viewState) {

			case ViewState::Winner: 
				viewState = ViewState::HighScore; 
				break;

			case ViewState::HighScore:
				machine.changeState(GameStateType::TitleScreen); 
				break;

		}

	}


	// Clear scores ..

	if ((pressed & UP_BUTTON) && (pressed & DOWN_BUTTON)) {

		clearScores++;

		switch (clearScores) {

			case 21 ... 60:
				#ifdef USE_LEDS             
				arduboy.setRGBled(RED_LED, 128 - (clearScores * 2));
				#endif
				break;

			case 61:
				clearScores = 0;
				#ifdef USE_LEDS             
				arduboy.setRGBled(RED_LED, 0);
				#endif
				initEEPROM(true);
				this->highScore = 0;
				this->score = 0;
				return;

		}

	}
	else {

		if (clearScores > 0) {
		
			#ifdef USE_LEDS             
			arduboy.setRGBled(RED_LED, 0);
			#endif
			clearScores = 0;

		}
		
	}

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void GameOverState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & ardBitmap = machine.getContext().ardBitmap;
	auto & gameStats = machine.getContext().gameStats;
	auto & playerStats = machine.getContext().playerStats;

	BaseState::renderTitleBackground(machine, (viewState != ViewState::HighScore));

	switch (this->viewState) {

		case ViewState::Winner:
			ardBitmap.drawCompressed(24, 15, Images::Winner, WHITE, ALIGN_NONE, MIRROR_NONE);

			if (arduboy.getFrameCount(70) < 7) {
				ardBitmap.drawCompressed(51, 4, Images::Blink_Eyes_2, WHITE, ALIGN_NONE, MIRROR_NONE);
			}
			else {
				ardBitmap.drawCompressed(51, 4, Images::Blink_Eyes_1, WHITE, ALIGN_NONE, MIRROR_NONE);
			}

			break;

		case ViewState::HighScore:
			{
				#ifdef ORIG_HIGH_SCORE
				ardBitmap.drawCompressed(20, 21, Images::High_Score, WHITE, ALIGN_NONE, MIRROR_NONE);
				SpritesB::drawOverwrite(24, 4, Images::Game_Over_Banner, 0);
				font3x5.setCursor(23, 20);
				font3x5.print(FlashString(level_Captions[ static_cast<uint8_t>(gameStats.skillLevel) ]));
				font3x5.setCursor(56, 20);
				renderTwoDigitNumeric(gameStats.skillLevel * 3);

				font3x5.setCursor(33, 29);
				renderTwoDigitNumeric(playerStats.hp * 2);
				renderTwoDigitNumeric((gameStats.level + 1) * 3);
				renderTwoDigitNumeric(playerStats.food);
				renderTwoDigitNumeric(playerStats.armour);

				font3x5.setCursor(56, 29);
				renderTwoDigitNumeric(playerStats.xpTrack * 2);
				renderTwoDigitNumeric(playerStats.gold * 2);
				renderTwoDigitNumeric(playerStats.bossesKilled * 2);
				renderTwoDigitNumeric(playerStats.itemCount());

				font3x5.setCursor(95, 20);
				renderThreeDigitNumeric(this->score);
				font3x5.setCursor(95, 29);
				renderThreeDigitNumeric(this->highScore);
				#else
				ardBitmap.drawCompressed(19, 17, Images::High_Score2, WHITE, ALIGN_NONE, MIRROR_NONE);
				SpritesB::drawOverwrite(24, 4, Images::Game_Over_Banner, 0);

				font3x5.setHeight(7);
				font3x5.setCursor(25, 16);
				font3x5.print(F("Skill~Level\nArea~Reached\nBosses~Slayed"));
		
				font3x5.setCursor(95, 16);
				renderTwoDigitNumeric(gameStats.skillLevel * 2);
				renderTwoDigitNumeric((gameStats.level + 1) * 3);
				renderTwoDigitNumeric(playerStats.bossesKilled * 2);

				font3x5.setCursor(37, 39);
				renderTwoDigitNumeric(playerStats.xpTrack * 2);
				renderTwoDigitNumeric(playerStats.hp * 2);

				font3x5.setCursor(70, 39);
				renderTwoDigitNumeric(playerStats.gold * 2);
				renderTwoDigitNumeric(playerStats.food);

				font3x5.setCursor(102, 39);
				renderTwoDigitNumeric(playerStats.armour);
				renderTwoDigitNumeric(playerStats.itemCount());

				font3x5.setCursor(19, 57);
				font3x5.print(F("Score:   ~High~Score:"));
				font3x5.setCursor(42, 57);
				renderThreeDigitNumeric(this->score);
				font3x5.setCursor(98, 57);
				renderThreeDigitNumeric(this->highScore);

				arduboy.drawFastHLine(19, 55, 90);
				#endif

			}
			break;


	}


}

void GameOverState::renderTwoDigitNumeric(uint8_t val) { 

	if (val < 10) font3x5.print(F("0"));
	font3x5.print(val);
	font3x5.print(F("\n"));

}

void GameOverState::renderThreeDigitNumeric(uint8_t val) { 

	if (val < 100) font3x5.print(F("0"));
	if (val < 10) font3x5.print(F("0"));

	font3x5.print(val);
//	font3x5.print(F("\n"));

}


/* ----------------------------------------------------------------------------
 *   Is the EEPROM initialised?
 *
 *   Looks for the characters 'A' and 'R' in the first two bytes of the EEPROM
 *   memory range starting from byte EEPROM_STORAGE_SPACE_START.  If not found,
 *   it resets the settings ..
 */

const uint8_t letter1 = 'M'; 
const uint8_t letter2 = 'R'; 

void GameOverState::initEEPROM(bool forceClear) {

  byte c1 = eeprom_read_byte(EEPROM_START_C1);
  byte c2 = eeprom_read_byte(EEPROM_START_C2);

  if (forceClear || c1 != letter1 || c2 != letter2) { 

    uint8_t score = 0;

    eeprom_update_byte(/*(uint8_t *)*/(EEPROM_START_C1), letter1);
    eeprom_update_byte(/*(uint8_t *)*/(EEPROM_START_C2), letter2);
		for (uint8_t i = 0; i < 4; i++) eeprom_update_byte(/*(uint8_t *)*/(EEPROM_SCORE + i), score);

  }
  EEPROM.commit();
}