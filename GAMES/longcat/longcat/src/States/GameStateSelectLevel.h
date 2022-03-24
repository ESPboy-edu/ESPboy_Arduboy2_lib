#pragma once

#include <stdint.h>
#include "GameState.h"
#include "../Utils/LevelUtils.h"
#include "../Utils/LevelRenderer.h"
#include "../Map/maps.h"

// DO THIS WHEN HAVE TIME

class Game;
class GameStateSelectLevel
{
private:
  uint8_t selection = 0;

public:
  void update(Game &game);
  void render(Game &game);

private:
};
