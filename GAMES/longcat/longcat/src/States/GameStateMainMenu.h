#pragma once

#include <stdint.h>
#include "../States/GameState.h"
#include "../GameMode/GameMode.h"
#include "../GameContext/GameContext.h"
#include "../Utils/CatChars.h"

class Game;
class GameStateMainMenu
{
  static const uint8_t maxSelection = 3;
  uint8_t selection = 0;

public:
  void update(Game &game);
  void render(Game &game);
};
