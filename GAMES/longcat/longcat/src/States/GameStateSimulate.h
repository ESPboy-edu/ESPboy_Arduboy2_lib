#pragma once

#include <stdint.h>
#include <Arduboy2.h>
#include "../GameContext/GameContext.h"
#include "../States/GameState.h"
#include "../Images/Bitmaps.h"
#include "../Map/Map.h"
#include "../Map/TileTypes.h"
#include "../Direction/Direction.h"
#include "../Utils/LevelRenderer.h"
#include "../Utils/LevelUtils.h"
#include "../Utils/SaveUtil.h"

// DO THIS WHEN HAVE TIME

class Game;
class GameStateSimulate
{
public:
  void update(Game &game);
  void render(Game &game);

private:
  
  bool movePlayer(Game &game);
  void placeCatPiece(int8_t x, int8_t y, Game &game);
  void gameEnd(Game &game);
  void nextStage(Game &game);
};
