#include "SplashScreenState.h"
#include "../utils/Arduboy2Ext.h"
#include "../images/Images.h"
#include "../utils/Enums.h"
#include <ArduboyTones.h>

extern ArduboyTones sound;

// ----------------------------------------------------------------------------
//  Initialise state ..
//
void SplashScreenState::activate(StateMachine & machine) {

  //auto & sound = machine.getContext().sound;
	auto & arduboy = machine.getContext().arduboy;

  //sound.setOutputEnabled(arduboy.audio.enabled);
  //sound.volumeMode(VOLUME_ALWAYS_NORMAL);
  
}


// ----------------------------------------------------------------------------
//  Handle state updates ..
//
void SplashScreenState::update(StateMachine & machine) {

  //auto & sound = machine.getContext().sound;
	auto & arduboy = machine.getContext().arduboy;
	auto justPressed = arduboy.justPressedButtons();

  if (justPressed > 0 && this->counter > 0) {

    this->counter = 124;

  }

  if (justPressed > 0 && this->counter == 0) {

#ifdef PLAY_SOUNDS    
    sound.tone(NOTE_A2, 30);
    sound.tone(NOTE_F1, 30);
#endif

    this->counter = 1;

  }

  if (this->counter > 0) {

    this->counter++;

    if (this->counter == 125) machine.changeState(GameStateType::TitleScreen); 

  }

}


// ----------------------------------------------------------------------------
//  Render the state ..
//
void SplashScreenState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;

  Sprites::drawOverwrite(47, 17, Images::Ppot_Buttons, 0);
  Sprites::drawOverwrite(43, 26, Images::Ppot_ButtonUp, 0);
  Sprites::drawOverwrite(73, 26, Images::Ppot_ButtonUp, 0);

  if (this->counter == 0) {

    Sprites::drawOverwrite(58, 26, Images::Ppot_ButtonUp, 0);
    Sprites::drawOverwrite(26, 46, Images::Ppot_Caption, 0);

  }
  else {

    Sprites::drawOverwrite(58, 26, Images::Ppot_ButtonDown, 0);
    Sprites::drawOverwrite(44, 46, Images::Ppot_Loading, 0);

    uint8_t i = (this->counter / 15) % 4;

    for (uint8_t j = 0; j < i; j++) {

        arduboy.drawPixel(79 + (j * 2), 49, WHITE);

    }

  }

}
