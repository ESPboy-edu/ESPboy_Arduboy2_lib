// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 MostlyBuilds
#pragma once

/*
 * Immutable campaign data and the authoritative level-authoring guide.
 *
 * ## Coordinates
 *
 * The board is GRID_COLS by GRID_ROWS. Coordinates are zero-based `{col, row}`.
 * The cannon belongs in column zero so its artwork connects to the left edge.
 * Use NO_POSITION for an absent object and NO_SEGMENT for an absent tripwire.
 *
 * ## Walls
 *
 * Each level owns a `const uint8_t[] PROGMEM` wall array. Construct entries with
 * `packedCell(col, row)` rather than writing hexadecimal values by hand. The row
 * occupies the high nibble and the column occupies the low nibble.
 *
 * ## Tripwires
 *
 * GridSegment endpoints are inclusive. Segments must be horizontal or vertical;
 * diagonal segments are rejected by the host-side level validator.
 *
 * ## Adding a level
 *
 * 1. Add its packed wall array to Levels.cpp.
 * 2. Add one fully annotated LevelDef entry to `levels`.
 * 3. Increment LEVEL_COUNT below.
 * 4. Run `sh tests/run-host-tests.sh` to validate names, coordinates, walls,
 *    object/wall overlap, and tripwire orientation.
 * 5. Run `pio run`, then play the level on hardware or in an emulator.
 *
 * The host validator cannot prove that a puzzle is solvable or that its fixed
 * elements produce the intended behavior; those remain playtest checks.
 */

#include <Arduino.h>

#include "GameTypes.h"

namespace MostlyLogic {

/* Campaign definitions stored in AVR program memory. */
extern const LevelDef levels[] PROGMEM;

/* Number of entries in levels; a static assertion in Levels.cpp keeps it in sync. */
constexpr uint8_t LEVEL_COUNT = 7;

}  // namespace MostlyLogic
