#pragma once

#include <stdint.h>
#include "GameState.h"
#include "../Utils/SaveUtil.h"

class Game;
class GameStateOptionsMenu
{
  static const uint8_t maxSelection = 0;
  uint8_t selection = 0;
public:
  void update(Game &game);
  void render(Game &game);
};
