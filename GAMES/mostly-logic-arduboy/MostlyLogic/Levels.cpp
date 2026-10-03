// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 MostlyBuilds

/*
 * Flash-resident wall layouts and campaign definitions.
 *
 * Read Levels.h for the complete authoring contract. Keep this file declarative:
 * level-specific behavior belongs to CircuitMode handling in Game.cpp.
 */

#include "Levels.h"

namespace MostlyLogic {

// Wall coordinates are grouped spatially where doing so makes the layout easier
// to recognize. clang-format is disabled for this data table because deliberate
// grouping communicates the level geometry better than mechanical wrapping.
// clang-format off

const uint8_t wallsFirstBounce[] PROGMEM = {
  packedCell(7, 0), packedCell(7, 1), packedCell(7, 2),
  packedCell(7, 3), packedCell(7, 4)
};

const uint8_t wallsAroundIt[] PROGMEM = {
  packedCell(4, 1), packedCell(5, 1), packedCell(6, 1),
  packedCell(7, 1), packedCell(8, 1), packedCell(9, 1),
  packedCell(9, 2), packedCell(9, 3), packedCell(9, 4),
  packedCell(5, 5), packedCell(6, 5), packedCell(7, 5),
  packedCell(8, 5), packedCell(9, 5)
};

const uint8_t wallsTripCircuit[] PROGMEM = {
  packedCell(12, 1), packedCell(13, 1), packedCell(14, 1),
  packedCell(12, 5), packedCell(13, 5), packedCell(14, 5),
  packedCell(14, 2), packedCell(14, 3), packedCell(14, 4),
  packedCell(12, 2), packedCell(12, 4)
};

const uint8_t wallsFailsafe[] PROGMEM = {
  packedCell(12, 1), packedCell(13, 1), packedCell(14, 1),
  packedCell(12, 3), packedCell(13, 3), packedCell(14, 3),
  packedCell(14, 2),
  packedCell(8, 6), packedCell(9, 6), packedCell(10, 6),
  packedCell(11, 6), packedCell(12, 6), packedCell(13, 6), packedCell(14, 6),
  packedCell(2, 2), packedCell(4, 3), packedCell(10, 4)
};

const uint8_t wallsRelay[] PROGMEM = {
  packedCell(8, 0), packedCell(9, 0), packedCell(10, 0), packedCell(11, 0),
  packedCell(12, 0), packedCell(13, 0), packedCell(14, 0),
  packedCell(8, 6), packedCell(9, 6), packedCell(10, 6), packedCell(11, 6),
  packedCell(12, 6), packedCell(13, 6), packedCell(14, 6),
  packedCell(8, 1), packedCell(8, 2), packedCell(8, 4), packedCell(8, 5),
  packedCell(14, 1), packedCell(14, 2), packedCell(14, 4), packedCell(14, 5),
  packedCell(10, 4)
};

const uint8_t wallsSneaky[] PROGMEM = {
  packedCell(12, 2), packedCell(13, 2), packedCell(14, 2),
  packedCell(12, 4), packedCell(13, 4), packedCell(14, 4),
  packedCell(14, 3),
  packedCell(2, 2), packedCell(3, 2),
  packedCell(6, 0), packedCell(6, 1), packedCell(6, 2), packedCell(6, 3),
  packedCell(8, 3), packedCell(8, 4), packedCell(8, 5), packedCell(8, 6),
  packedCell(10, 1), packedCell(11, 1),
  packedCell(3, 5), packedCell(4, 5), packedCell(5, 5)
};

const uint8_t wallsChainReaction[] PROGMEM = {
  packedCell(12, 2), packedCell(13, 2), packedCell(14, 2),
  packedCell(12, 4), packedCell(13, 4), packedCell(14, 4),
  packedCell(14, 3),
  packedCell(4, 1), packedCell(5, 1),
  packedCell(4, 5), packedCell(5, 5),
  packedCell(9, 2), packedCell(9, 4),
  packedCell(11, 2), packedCell(11, 4),
  packedCell(7, 0), packedCell(7, 6)
};

// Positional aggregate initialization keeps level data compact on the AVR. The
// field comments stay beside every value so each entry can be understood without
// repeatedly cross-referencing LevelDef's declaration.
const LevelDef levels[] PROGMEM = {
  {
    "FIRST BOUNCE",                                         // name
    {0, 3}, DIR_E,                                          // cannon, launch direction
    {5, 1},                                                 // target
    wallsFirstBounce, arrayCount(wallsFirstBounce),         // walls
    NO_POSITION, NO_POSITION, NO_POSITION,                  // switch A, switch B, gate
    CIRCUIT_NONE,                                           // circuit mode
    NO_SEGMENT, NO_SEGMENT,                                 // tripwires
    NO_POSITION                                             // splitter
  },
  {
    "AROUND IT",                                            // name
    {0, 3}, DIR_E,                                          // cannon, launch direction
    {12, 3},                                                // target
    wallsAroundIt, arrayCount(wallsAroundIt),               // walls
    NO_POSITION, NO_POSITION, NO_POSITION,                  // switch A, switch B, gate
    CIRCUIT_NONE,                                           // circuit mode
    NO_SEGMENT, NO_SEGMENT,                                 // tripwires
    NO_POSITION                                             // splitter
  },
  {
    "TRIP CIRCUIT",                                         // name
    {0, 6}, DIR_E,                                          // cannon, launch direction
    {13, 3},                                                // target
    wallsTripCircuit, arrayCount(wallsTripCircuit),         // walls
    {5, 1}, NO_POSITION, {12, 3},                           // switch A, switch B, gate
    CIRCUIT_SHARED_RESET,                                   // circuit mode
    {{8, 2}, {8, 5}}, NO_SEGMENT,                           // tripwires
    NO_POSITION                                             // splitter
  },
  {
    "FAILSAFE",                                             // name
    {0, 6}, DIR_E,                                          // cannon, launch direction
    {13, 2},                                                // target
    wallsFailsafe, arrayCount(wallsFailsafe),               // walls
    {3, 5}, NO_POSITION, {12, 2},                           // switch A, switch B, gate
    CIRCUIT_SHARED_RESET,                                   // circuit mode
    {{7, 1}, {7, 5}}, {{7, 5}, {11, 5}},                    // tripwires
    NO_POSITION                                             // splitter
  },
  {
    "RELAY",                                                // name
    {0, 6}, DIR_E,                                          // cannon, launch direction
    {14, 3},                                                // target
    wallsRelay, arrayCount(wallsRelay),                     // walls
    {5, 3}, {10, 2}, {8, 3},                                // switch A, switch B, gate
    CIRCUIT_SEPARATE_RELAY,                                 // circuit mode
    {{12, 1}, {12, 5}}, NO_SEGMENT,                         // tripwires
    NO_POSITION                                             // splitter
  },
  {
    "SNEAKY",                                               // name
    {0, 6}, DIR_E,                                          // cannon, launch direction
    {13, 3},                                                // target
    wallsSneaky, arrayCount(wallsSneaky),                   // walls
    {4, 1}, {10, 5}, {12, 3},                               // switch A, switch B, gate
    CIRCUIT_AND_DYNAMIC_TRIPWIRE,                           // circuit mode
    {{7, 0}, {7, 6}}, NO_SEGMENT,                           // tripwires
    NO_POSITION                                             // splitter
  },
  {
    "CHAIN REACTION",                                       // name
    {0, 3}, DIR_E,                                          // cannon, launch direction
    {13, 3},                                                // target
    wallsChainReaction, arrayCount(wallsChainReaction),     // walls
    {10, 1}, {10, 5}, {12, 3},                              // switch A, switch B, gate
    CIRCUIT_SPLITTER_OVERLOAD,                              // circuit mode
    {{8, 4}, {8, 6}}, {{3, 0}, {3, 6}},                     // tripwires
    {6, 3}                                                  // splitter
  }
};

// clang-format on

static_assert(arrayCount(levels) <= UINT8_MAX, "Level count must fit in uint8_t");
static_assert(arrayCount(levels) == LEVEL_COUNT, "LEVEL_COUNT must match campaign data");

}  // namespace MostlyLogic
