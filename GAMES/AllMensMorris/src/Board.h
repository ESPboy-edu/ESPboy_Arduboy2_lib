#pragma once

#include <Arduino.h>

constexpr uint8_t MORRIS_MAX_POINT_COUNT = 36;
constexpr uint8_t MORRIS_MAX_MILL_COUNT = 24;
constexpr uint8_t MORRIS_NO_POINT = 255;

struct BoardPoint {
  uint8_t x;
  uint8_t y;
};

struct MillLine {
  uint8_t a;
  uint8_t b;
  uint8_t c;
};

struct BoardDefinition {
  const char *label;
  uint8_t pointCount;
  uint8_t millCount;
  const BoardPoint *points;
  const uint8_t *mills;
  const uint8_t *adjacencyOffsets;
  const uint8_t *adjacency;
};

extern const BoardPoint ClassicBoardPoints[] PROGMEM;
extern const uint8_t ClassicMills[] PROGMEM;
extern const uint8_t ClassicAdjacencyOffsets[] PROGMEM;
extern const uint8_t ClassicAdjacency[] PROGMEM;
extern const BoardDefinition ClassicBoardDefinition;

BoardPoint boardPoint(const BoardDefinition &board, uint8_t index);
MillLine millLine(const BoardDefinition &board, uint8_t index);
uint8_t adjacencyCount(const BoardDefinition &board, uint8_t point);
uint8_t adjacentPoint(const BoardDefinition &board, uint8_t point, uint8_t slot);
