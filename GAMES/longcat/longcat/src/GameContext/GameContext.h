#pragma once

#include "../Map/Map.h"
#include "../Hero/Hero.h"

class GameContext
{
public:
  unsigned long currentSeed = 0;
  uint8_t stage = 5;
  Map mapObject;
  Hero hero;
  //Settings
  bool audioEnabled;
  uint8_t randomDifficulty;
};
