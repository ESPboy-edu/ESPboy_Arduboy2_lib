#pragma once 
#include "Direction.h"


struct Hero{
  int8_t x;
  int8_t y;
  int8_t dx;
  int8_t dy;
  Direction direction;
  Direction lastDirection;
};
