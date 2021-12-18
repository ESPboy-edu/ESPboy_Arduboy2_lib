#pragma once
#include "Utility.hpp"

class Levels {
 public:
  /* Convenience functions for reading from PROGMEM */
  const char* tutorial() const;
  const void goal(Cell* res) const;
  const void head(Cell* res) const;
  const void body(Cell* res) const;
  const void tail(Cell* res) const;
  const void rock(uint16_t* res) const;
  const void soil(uint16_t* res) const;
  const void food(uint16_t* res) const;
  const void poop(uint16_t* res) const;
  const void nail(uint16_t* res) const;

  static const char* tutorial(const uint8_t id);
  static const void setupCells(Cell* rGoal,
                               Cell* rHead,
                               Cell* rBody,
                               Cell* rTail,
                               const uint8_t id);
  static const void setupTiles(uint16_t* rRock,
                               uint16_t* rSoil,
                               uint16_t* rFood,
                               uint16_t* rPoop,
                               uint16_t* rNail,
                               const uint8_t id);

  static const uint8_t count() { return levelsCount; }

  const char* _tutorial;
  const uint8_t _goalX;
  const uint8_t _goalY;
  const uint8_t _headX;
  const uint8_t _headY;
  const uint8_t _bodyX;
  const uint8_t _bodyY;
  const uint8_t _tailX;
  const uint8_t _tailY;
  const uint16_t _rock[8];
  const uint16_t _soil[8];
  const uint16_t _food[8];
  const uint16_t _poop[8];
  const uint16_t _nail[8];

  static const uint8_t levelsCount = 16;
};
