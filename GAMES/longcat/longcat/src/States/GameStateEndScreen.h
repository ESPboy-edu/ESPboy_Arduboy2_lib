#pragma once

#include <stdint.h>
#include "GameState.h"

class Game;
class GameStateEndScreen
{
public:
  void update(Game &game);
  void render(Game &game);
};
