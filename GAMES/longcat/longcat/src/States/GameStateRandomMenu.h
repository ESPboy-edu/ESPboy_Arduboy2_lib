#pragma once

#include <stdint.h>
#include "../States/GameState.h"
#include "../GameMode/GameMode.h"
#include "../GameContext/GameContext.h"
#include "../Utils/CatChars.h"
#include "../Utils/SaveUtil.h"

class Game;
class GameStateRandomMenu
{
  static const uint8_t maxSelection = 1;
  uint8_t selection = 0;

public:
  void update(Game &game);
  void render(Game &game);
};