#pragma once

#include "BaseState.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"

class TreasureState : public BaseState {
  
  enum class ViewState : uint8_t {
    InitialRoll,
    RollDice,
    UpdateStats
  };

  private:

    void printCaption(uint8_t index);
    void renderSelectTreasure(StateMachine & machine);
    void renderChestResults(StateMachine & machine);

    ViewState viewState = ViewState::InitialRoll;
    
    uint8_t counter = 0;
    uint8_t dice = 0;
    bool foundTreasure = false;

  public:

    void activate(StateMachine & machine) override;
    void update(StateMachine & machine) override;
    void render(StateMachine & machine) override;

};