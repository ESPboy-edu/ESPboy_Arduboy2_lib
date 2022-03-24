#pragma once

#include <Arduboy2.h>
#include <stdint.h>
#include "../GameContext/GameContext.h"
#include "../States/GameState.h"
#include "../GameMode/GameMode.h"
#include "../Direction/Direction.h"
#include "../Map/TileTypes.h"
#include "../Map/Map.h"
#include "../Map/maps.h"
#include "../Utils/PackedConverter.h"
#include "../Utils/LevelRenderer.h"
#include "../Utils/LevelUtils.h"

// DO THIS WHEN HAVE TIME

class Game;
class GameStateLoadLevel
{
public:
  void update(Game &game);
  void render(Game &game);

private:
  void loadStaticLevel(Game &game);

  bool canMoveRandom(int8_t x, int8_t y, Direction currentDirection, uint8_t currentDepth, Game &game);

  uint16_t getTilePacked(int8_t x, int8_t y, Game &game);

  uint8_t getNeighbourUsed(int8_t x, int8_t y, Game &game);

  uint8_t checkSteps(int8_t x, int8_t y, Direction direction, uint8_t depth, Game &game);

  void generateRandomLevel(Game &game);

  void loadRandomLevel(Game &game);
};
