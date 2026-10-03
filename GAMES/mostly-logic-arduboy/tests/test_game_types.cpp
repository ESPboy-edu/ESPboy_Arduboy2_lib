// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 MostlyBuilds

#include "GameTypes.h"

#include <assert.h>

using namespace MostlyLogic;

int main() {
  assert(isInsideGrid(0, 0));
  assert(isInsideGrid(GRID_COLS - 1, GRID_ROWS - 1));
  assert(!isInsideGrid(-1, 0));
  assert(!isInsideGrid(0, -1));
  assert(!isInsideGrid(GRID_COLS, 0));
  assert(!isInsideGrid(0, GRID_ROWS));
  assert(packedCell(14, 6) == 0x6E);

  const int8_t expectedX[] = {1, 1, 0, -1, -1, -1, 0, 1};
  const int8_t expectedY[] = {0, 1, 1, 1, 0, -1, -1, -1};
  for (uint8_t i = 0; i < 8; ++i) {
    const Direction direction = static_cast<Direction>(i);
    assert(directionDeltaX(direction) == expectedX[i]);
    assert(directionDeltaY(direction) == expectedY[i]);
    assert(turnClockwise(turnCounterclockwise(direction)) == direction);
    assert(turnCounterclockwise(turnClockwise(direction)) == direction);
  }

  const GridSegment vertical = {{4, 1}, {4, 5}};
  assert(positionTouchesSegment(4, 1, vertical));
  assert(positionTouchesSegment(4, 3, vertical));
  assert(positionTouchesSegment(4, 5, vertical));
  assert(!positionTouchesSegment(3, 3, vertical));
  assert(!positionTouchesSegment(4, 6, vertical));

  const GridSegment horizontal = {{8, 4}, {2, 4}};
  assert(positionTouchesSegment(2, 4, horizontal));
  assert(positionTouchesSegment(6, 4, horizontal));
  assert(positionTouchesSegment(8, 4, horizontal));
  assert(!positionTouchesSegment(6, 3, horizontal));

  const GridSegment diagonal = {{1, 1}, {2, 2}};
  assert(!positionTouchesSegment(1, 1, diagonal));
  assert(!positionTouchesSegment(0, 0, NO_SEGMENT));

  Ball balls[4] = {};
  balls[0].active = true;
  balls[2].active = true;
  const uint16_t startOfTick = activeBallMask(balls);
  balls[1].active = true;
  assert((startOfTick & (1U << 0)) != 0);
  assert((startOfTick & (1U << 1)) == 0);
  assert((startOfTick & (1U << 2)) != 0);

  Ball fullSet[16] = {};
  fullSet[0].active = true;
  fullSet[15].active = true;
  assert(activeBallMask(fullSet) == 0x8001U);
}
