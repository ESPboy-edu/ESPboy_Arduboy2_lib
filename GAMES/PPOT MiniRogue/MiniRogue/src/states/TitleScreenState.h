#pragma once

#include "../utils/GameContext.h"
#include "../utils/GameState.h"
#include "../utils/Enums.h"
#include "../images/Images.h"
#include "BaseState.h"

class TitleScreenState : public BaseState {

  private:

    uint8_t restart = 0;
    #ifdef SOUND_ON_OFF
    uint8_t sound_counter = 0;
    uint8_t sound_state = 0;
    #endif

  public:	
  
    void activate(StateMachine & machine) override;
    void update(StateMachine & machine) override;
    void render(StateMachine & machine) override;

    void toggleSoundSettings(StateMachine & machine);

};

