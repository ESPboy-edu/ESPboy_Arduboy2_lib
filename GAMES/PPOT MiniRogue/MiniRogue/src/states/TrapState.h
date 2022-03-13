#pragma once

#include "BaseState.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"

class TrapState : public BaseState {
  
  enum class ViewState : uint8_t {
    SkillCheck,
    SkillCheckResult,
    RollDice,
    UpdateStats,
    PlayerDead
  };

  private:

    void printTrapName();

    ViewState viewState = ViewState::SkillCheck;
    
    uint8_t counter = 0;
    uint8_t dice = 0;

  public:

    void activate(StateMachine & machine) override;
    void update(StateMachine & machine) override;
    void render(StateMachine & machine) override;

};