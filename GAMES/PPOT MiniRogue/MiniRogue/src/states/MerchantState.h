#pragma once

#include "BaseState.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"

char const error_01[] PROGMEM = "   You need\n  more gold!";
char const error_02[] PROGMEM = "~You dont have\n one to sell!";
char const error_03[] PROGMEM = " ~You already\n have enough!";

char const * const error_Captions[] = {
	error_01,
	error_02,
	error_03,
};

class MerchantState : public BaseState {
  
  enum class ViewState : uint8_t {
    Buying,
    Selling,
  };

  private:

    ViewState viewState = ViewState::Buying;
    
    uint8_t selectedItem = 0;
    uint8_t errorNumber = 0;

    FlashSettings settings;

  public:

    void activate(StateMachine & machine) override;
    void update(StateMachine & machine) override;
    void render(StateMachine & machine) override;

};