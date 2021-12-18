#pragma once

#include "Utility.hpp"

/* Class that implements the worm. */
class Worm {
 public:
  Worm(Arduboy2& arduboy) : arduboy(arduboy) {}
  bool moveTo(Cell newHead, bool enlarge, bool shorten);
  void render();
  void addPiece(Cell c);
  void reset(Cell c);
  Cell getHead() { return cells[0]; }
  bool fall(uint16_t solids[8]);
  bool intersects(uint16_t solids[8]);

 private:
  Arduboy2& arduboy;

  /* The body can be at most 16 cells long. First cell is the head. */
  Cell cells[16] = {};
  uint8_t count = 0;

  static const uint8_t R = 0;
  static const uint8_t U = 8;
  static const uint8_t L = 16;
  static const uint8_t D = 24;

  uint8_t extrRotation(uint8_t f, uint8_t l);
  uint8_t joinRotation(uint8_t j);
};
