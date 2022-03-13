#pragma once

#include "../utils/GameContext.h"
#include "../utils/GameState.h"
#include "BaseState.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"


char const area_Caption_01[] PROGMEM = ":~The~Black~Sewers";
char const area_Caption_02[] PROGMEM = ":~Poisonous~Dungeons";
char const area_Caption_03[] PROGMEM = ":~Undead~Catacombs";
char const area_Caption_04[] PROGMEM = ":~Flaming~Underworld";
char const area_Caption_05[] PROGMEM = ":~Sunken~Keep~of~Og!";

char const * const area_Captions[] = {
	area_Caption_01,
	area_Caption_02,
	area_Caption_03,
	area_Caption_04,
	area_Caption_05,
};


class ShowCardsState : public BaseState {
  
  private:

    enum class ViewState : uint8_t {
      DealCards,
      PlayCard,
      PlayerDead
    };

    ViewState viewState = ViewState::DealCards;
    
    uint8_t counter = 0;           
    int8_t displayCard = 0;           
    uint8_t numberOfCardsToDisplay = 6;

  public:	
  
  	void activate(StateMachine & machine) override;
    void update(StateMachine & machine) override;
    void render(StateMachine & machine) override;
    
};