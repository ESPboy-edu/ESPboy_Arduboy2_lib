#pragma once
#include <stdint.h>
#include <Arduboy2.h>
#include "../GameContext/GameContext.h"
#include "../States/GameState.h"
#include "../Images/Bitmaps.h"
#include "../Map/Map.h"
#include "../Map/TileTypes.h"
#include "../Direction/Direction.h"
#include "../Utils/CatChars.h"
#include "../Utils/RandKey.h"
#include "../Utils/LevelRenderer.h"

// DO THIS WHEN HAVE TIME

class Game;
class GameStateGamePlay
{
public:
  void update(Game &game);
  void render(Game &game);

private:
  void readPlayerInput(Game &game);
};
