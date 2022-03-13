#pragma once

#include "BaseState.h"
#include "../utils/Enums.h"

class RestingState : public BaseState {
  
  enum class ViewState : uint8_t {
    SelectReward,
    UpdateStats,
  };
  
  enum class SelectedItem : uint8_t {
    Food,
    Heal,
    Weapon,
  };

  private:

    ViewState viewState = ViewState::SelectReward;
    SelectedItem selectedItem = SelectedItem::Food;
    SelectedItem prevSelection = SelectedItem::Food;
    
    uint8_t counter = 0;


  public:

    void activate(StateMachine & machine) override;
    void update(StateMachine & machine) override;
    void render(StateMachine & machine) override;

};