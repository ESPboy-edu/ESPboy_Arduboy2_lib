#pragma once

#include "BaseState.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"


char const eventDice_Caption_01[] PROGMEM = "Found~Loot    +2~Gold";
char const eventDice_Caption_02[] PROGMEM = "Health~Potion   +2~HP";
char const eventDice_Caption_03[] PROGMEM = "Found~Meat    +1~Food";
char const eventDice_Caption_04[] PROGMEM = "Whetstone      ~+2~XP";
char const eventDice_Caption_05[] PROGMEM = "Found~Shield    +1~AR";
char const eventDice_Caption_06[] PROGMEM = "Monster        ~5~DMG";

char const * const eventDice_Captions[] = {
	eventDice_Caption_01,
	eventDice_Caption_02,
	eventDice_Caption_03,
	eventDice_Caption_04,
	eventDice_Caption_05,
	eventDice_Caption_06,
};

class EventState : public BaseState {
  
  enum class ViewState : uint8_t {
    RollDice,
    SkillCheck,
    SkillCheckResult,
    SelectCard,
    UpdateStats,
  };

  private:

    void printEventName(uint8_t dice);
    void renderLargeSpinningCard(StateMachine & machine, uint8_t x, uint8_t y, uint8_t dice);

    ViewState viewState = ViewState::RollDice;
    ViewState nextState = ViewState::RollDice;
    
    uint8_t counter = 0;
    uint8_t skillCheck = 0;
    uint8_t dice[3] = { 0, 0, 0 };
    uint8_t selection = 0;


  public:

    void activate(StateMachine & machine) override;
    void update(StateMachine & machine) override;
    void render(StateMachine & machine) override;
    
};