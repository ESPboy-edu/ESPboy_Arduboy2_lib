#include "TitleScreenState.h"
#include "../images/Images.h"

#define FlashString FPSTR

constexpr const static uint8_t MIN_LEVEL = 0;
constexpr const static uint8_t MAX_LEVEL = 3;
constexpr const static uint8_t UPLOAD_DELAY = 16;


// ----------------------------------------------------------------------------
//  Initialise state ..
//
void TitleScreenState::activate(StateMachine & machine) {

	#ifdef SOUND_ON_OFF
	auto & arduboy = machine.getContext().arduboy;

  this->restart = 0;
	this->sound_state = arduboy.audio.enabled();
	#else
	(void)machine;
	#endif

/*
	#ifdef SOUND

		// set up Timer 3
		TCCR3A = 0; // normal operation
		TCCR3B = _BV(WGM32) | _BV(CS30); // CTC, no pre-scaling
		OCR3A = 1999; // compare A register value (8000Hz)
		TIMSK3 = _BV(OCIE3A); // interrupt on Compare A Match		// // set up Timer 3
		// TCCR3A = 0; // normal operation
		// TCCR3B = _BV(WGM32) | _BV(CS30); // CTC, no pre-scaling
		// OCR3A = 1999; // compare A register value (8000Hz)
		// TIMSK3 = _BV(OCIE3A); // interrupt on Compare A Match

		//set up Timer 4
		TCCR4A = _BV(COM4A0) | _BV(PWM4A); // Fast-PWM 8-bit
		TCCR4B = _BV(CS40); // 62500Hz
		OCR4C = 0xFF; // Resolution to 8-bit (TOP=0xFF)

	#endif
	
*/
}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void TitleScreenState::update(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;
	auto & gameStats = machine.getContext().gameStats;
  auto justPressed = arduboy.justPressedButtons();
  auto pressed = arduboy.pressedButtons();

	// Restart ?

	if (pressed & DOWN_BUTTON) {

		if (restart < UPLOAD_DELAY) {
			restart++;
		}
		else {
			arduboy.exitToBootloader();
		}

	}
	else {
		restart = 0;
	}


	// Handle other input ..

	if ((justPressed & LEFT_BUTTON) && gameStats.skillLevel > MIN_LEVEL) 		--gameStats.skillLevel;
	if ((justPressed & RIGHT_BUTTON) && gameStats.skillLevel < MAX_LEVEL) 	++gameStats.skillLevel;

	if (justPressed & A_BUTTON) {

		constexpr const static uint8_t ELEMENTS_PER_ROW = 4;

		uint8_t idx = gameStats.skillLevel * ELEMENTS_PER_ROW;
		playerStats.xp = 0;
		playerStats.armour = pgm_read_byte(&InitSettings[idx++]);
		playerStats.hp = pgm_read_byte(&InitSettings[idx++]);
		playerStats.gold = pgm_read_byte(&InitSettings[idx++]);
		playerStats.food = pgm_read_byte(&InitSettings[idx]);
		playerStats.xpTrack = 1;

		machine.getContext().resetGame();
		machine.changeState(GameStateType::ShowCards);
		arduboy.resetFrameCount();
	
		//playerStats.xpTrack = 4;  //sjh
		//playerStats.hp = 1;  //sjh
		// playerStats.items[0] = 1; //sjh
		// playerStats.items[1] = 1; //sjh
		
	}
  #ifdef SOUND_ON_OFF
	if (justPressed & B_BUTTON) {

		this->sound_counter = 60;
		this->sound_state = !this->sound_state;
		toggleSoundSettings(machine);

	}

	if (this->sound_counter > 0) this->sound_counter--;
	#endif

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void TitleScreenState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & gameStats = machine.getContext().gameStats;
	auto & ardBitmap = machine.getContext().ardBitmap;

	BaseState::renderTitleBackground(machine, true);
	ardBitmap.drawCompressed(29, 16, Images::Title_Mini_Rogue, WHITE, ALIGN_NONE, MIRROR_NONE);
	ardBitmap.drawCompressed(19, 56, Images::Title_Levels, WHITE, ALIGN_NONE, MIRROR_NONE);

	if (arduboy.getFrameCount(70) < 7) {
		ardBitmap.drawCompressed(51, 4, Images::Blink_Eyes_2, WHITE, ALIGN_NONE, MIRROR_NONE);
	}
	else {
		ardBitmap.drawCompressed(51, 4, Images::Blink_Eyes_1, WHITE, ALIGN_NONE, MIRROR_NONE);
	}

	{

    static const uint8_t xPos[] PROGMEM = { 19, 42, 74, 96 };
    static const uint8_t width[] PROGMEM = { 15, 23, 14, 11 };

		arduboy.drawFastHLine(pgm_read_byte(&xPos[gameStats.skillLevel]), 63, pgm_read_byte(&width[gameStats.skillLevel]));

	}

  #ifdef SOUND_ON_OFF

	if (this->sound_counter > 0) {

		ardBitmap.drawCompressed(119, 56, Images::Sound_Mask, BLACK, ALIGN_NONE, MIRROR_NONE);

		if (this->sound_counter % 16 < 8) {

			ardBitmap.drawCompressed(119, 56, (this->sound_state == 1 ? Images::Sound_On : Images::Sound_Off), WHITE, ALIGN_NONE, MIRROR_NONE);

		}

	}

	#endif

}

/* ----------------------------------------------------------------------------
 *  Toggle the sound setting and commit to the EEPROM.
 */
#ifdef SOUND_ON_OFF
void TitleScreenState::toggleSoundSettings(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;

  if (arduboy.audio.enabled()) {
  
    arduboy.audio.off(); 
//    arduboy.audio.saveOnOff();
  
  }
  else {
  
    arduboy.audio.on(); 
 //   arduboy.audio.saveOnOff();
  
  }
    
}
#endif