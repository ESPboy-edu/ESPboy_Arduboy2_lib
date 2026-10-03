// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 MostlyBuilds

#include "Levels.h"

#include <assert.h>
#include <string.h>

using namespace MostlyLogic;

namespace {

bool samePosition(CellPosition left, CellPosition right) {
  return left.col == right.col && left.row == right.row;
}

bool isAbsent(CellPosition position) {
  return samePosition(position, NO_POSITION);
}

bool isAbsent(GridSegment segment) {
  return samePosition(segment.start, NO_POSITION) && samePosition(segment.end, NO_POSITION);
}

bool wallAt(const LevelDef& level, CellPosition position) {
  if (!isValidPosition(position)) return false;
  for (uint8_t i = 0; i < level.wallCount; ++i) {
    const uint8_t packed = level.walls[i];
    if ((packed & 0x0F) == position.col && (packed >> 4) == position.row) {
      return true;
    }
  }
  return false;
}

void validateOptionalPosition(CellPosition position) {
  assert(isAbsent(position) || isValidPosition(position));
}

void validateSegment(GridSegment segment) {
  if (isAbsent(segment)) return;
  assert(isValidSegment(segment));
  assert(segment.start.col == segment.end.col || segment.start.row == segment.end.row);
}

void validateDistinctObjects(const LevelDef& level) {
  const CellPosition objects[] = {
      level.cannon, level.target, level.switchA, level.switchB, level.gate, level.splitter};
  for (size_t i = 0; i < arrayCount(objects); ++i) {
    if (isAbsent(objects[i])) continue;
    for (size_t j = i + 1; j < arrayCount(objects); ++j) {
      if (isAbsent(objects[j])) continue;
      assert(!samePosition(objects[i], objects[j]));
    }
  }
}

void validateCircuitObjects(const LevelDef& level) {
  switch (level.circuitMode) {
    case CIRCUIT_NONE:
      assert(isAbsent(level.switchA));
      assert(isAbsent(level.switchB));
      assert(isAbsent(level.gate));
      assert(isAbsent(level.tripwire));
      assert(isAbsent(level.tripwire2));
      assert(isAbsent(level.splitter));
      break;

    case CIRCUIT_SHARED_RESET:
      assert(isValidPosition(level.switchA));
      assert(isAbsent(level.switchB));
      assert(isValidPosition(level.gate));
      assert(isValidSegment(level.tripwire));
      assert(isAbsent(level.splitter));
      break;

    case CIRCUIT_SEPARATE_RELAY:
    case CIRCUIT_AND_DYNAMIC_TRIPWIRE:
      assert(isValidPosition(level.switchA));
      assert(isValidPosition(level.switchB));
      assert(isValidPosition(level.gate));
      assert(isValidSegment(level.tripwire));
      assert(isAbsent(level.splitter));
      break;

    case CIRCUIT_SPLITTER_OVERLOAD:
      assert(isValidPosition(level.switchA));
      assert(isValidPosition(level.switchB));
      assert(isValidPosition(level.gate));
      assert(isValidSegment(level.tripwire));
      assert(isValidSegment(level.tripwire2));
      assert(isValidPosition(level.splitter));
      break;

    default:
      assert(false);
  }
}

void validateTemporarySwitch(const LevelDef& level, CellPosition position) {
  assert(isValidPosition(position));
  assert(!wallAt(level, position));
  const CellPosition objects[] = {
      level.cannon, level.target, level.switchA, level.switchB, level.gate, level.splitter};
  for (const CellPosition object : objects) {
    if (!isAbsent(object)) assert(!samePosition(position, object));
  }
}

}  // namespace

int main() {
  for (uint8_t levelIndex = 0; levelIndex < LEVEL_COUNT; ++levelIndex) {
    const LevelDef& level = levels[levelIndex];
    assert(level.name[0] != '\0');
    assert(memchr(level.name, '\0', LEVEL_NAME_LENGTH) != nullptr);
    assert(isValidPosition(level.cannon));
    assert(level.cannon.col == 0);
    assert(isValidPosition(level.target));
    assert(!samePosition(level.cannon, level.target));
    assert(level.cannonDirection <= DIR_NE);
    assert(level.wallCount == 0 || level.walls != nullptr);

    bool seen[GRID_ROWS][GRID_COLS] = {};
    for (uint8_t i = 0; i < level.wallCount; ++i) {
      const uint8_t packed = level.walls[i];
      const uint8_t col = packed & 0x0F;
      const uint8_t row = packed >> 4;
      assert(isInsideGrid(col, row));
      assert(!seen[row][col]);
      seen[row][col] = true;
    }

    assert(!wallAt(level, level.cannon));
    assert(!wallAt(level, level.target));
    validateOptionalPosition(level.switchA);
    validateOptionalPosition(level.switchB);
    validateOptionalPosition(level.gate);
    validateOptionalPosition(level.splitter);
    assert(!wallAt(level, level.switchA));
    assert(!wallAt(level, level.switchB));
    assert(!wallAt(level, level.gate));
    assert(!wallAt(level, level.splitter));
    validateSegment(level.tripwire);
    validateSegment(level.tripwire2);
    validateDistinctObjects(level);
    validateCircuitObjects(level);
    if (level.circuitMode == CIRCUIT_AND_DYNAMIC_TRIPWIRE) {
      validateTemporarySwitch(level, SNEAKY_TRIPWIRE_SWITCH_POSITION);
    }
    if (level.circuitMode == CIRCUIT_SPLITTER_OVERLOAD) {
      validateTemporarySwitch(level, FINALE_TRIPWIRE_SWITCH_POSITION);
    }
  }
}
