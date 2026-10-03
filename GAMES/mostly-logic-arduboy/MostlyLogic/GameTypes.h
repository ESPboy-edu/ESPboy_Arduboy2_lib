// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 MostlyBuilds
#pragma once

/*
 * Platform-independent constants, state types, and geometry helpers.
 *
 * This header deliberately contains no mutable state and has no Arduino
 * dependency. Keeping the core data model portable allows its invariants to be
 * checked by the host-side tests without an AVR toolchain.
 */

#include <stddef.h>
#include <stdint.h>

namespace MostlyLogic {

// Board geometry. A 15-by-7 grid of 8-pixel cells occupies 120 by 56 pixels,
// leaving a small border on the Arduboy's 128-by-64 display.
constexpr uint8_t CELL_SIZE = 8;
constexpr uint8_t GRID_COLS = 15;
constexpr uint8_t GRID_ROWS = 7;
constexpr int8_t BOARD_X = 4;
constexpr int8_t BOARD_Y = 4;
constexpr uint8_t LEVEL_NAME_LENGTH = 16;
constexpr uint8_t LEVEL_INTRO_FRAMES = 78;
constexpr uint8_t NO_CELL = 255;

// Simulation timing and fixed-capacity storage. These limits are intentionally
// small and static because the ATmega32u4 has only 2.5 KiB of SRAM.
constexpr uint16_t FIRE_INTERVAL_MS = 1000;
constexpr uint16_t BALL_STEP_MS = 22;
constexpr uint8_t MAX_BALL_STEPS_PER_FRAME = 4;
constexpr uint8_t MAX_BALLS = 12;
constexpr uint8_t MAX_VISUAL_PATH = 48;
constexpr uint8_t TARGET_CHARGE_FRAMES = 150;
constexpr uint8_t PULSE_FRAMES_PER_CELL = 3;
constexpr uint8_t CANNON_CELEBRATE_FRAMES = 24;
constexpr uint8_t CANNON_EXPLOSION_FRAMES = 54;
constexpr uint8_t MAX_CANNON_FRAGMENTS = 18;
constexpr uint8_t SHATTER_EFFECT_FRAMES = 12;
constexpr uint8_t CONTROL_PULSE_FRAMES = 28;
constexpr uint8_t ACTIVATION_FRAMES_PER_CELL = 3;
constexpr uint8_t ACTIVATION_HOLD_FRAMES = 12;
constexpr uint8_t RESET_FLASH_FRAMES = 24;
constexpr uint8_t GATE_BURST_FRAMES = 14;
constexpr uint8_t RESET_CIRCUIT_FRAMES = 42;
constexpr uint8_t CANNON_KICK_FRAMES = 6;
constexpr uint8_t SHORT_RESET_FLASH_FRAMES = 8;
constexpr uint8_t DEFAULT_BACKBLAST_FRAMES = 18;
constexpr uint8_t MIN_BACKBLAST_FRAMES = 14;
constexpr uint8_t INVALID_PULSE_STEP = 0xFF;
constexpr uint8_t LEVEL_SELECTOR_VISIBLE_ROWS = 5;
constexpr uint8_t SHORT_GATE_BURST_FRAMES = 8;

/* Top-level screens handled by the game loop. */
enum GameScreen : uint8_t {
  SCREEN_TITLE,
  SCREEN_LEVEL_SELECT,
  SCREEN_GAME,
  SCREEN_GAME_OVER
};

/* Every value that can occupy one board cell. */
enum Tile : uint8_t {
  TILE_EMPTY,
  TILE_WALL,
  TILE_DEFLECT_X,
  TILE_DEFLECT_DOT,
  TILE_CANNON,
  TILE_TARGET,
  TILE_SWITCH_A,
  TILE_SWITCH_B,
  TILE_TRIPWIRE_SWITCH,
  TILE_SPLITTER,
  TILE_GATE
};

/*
 * Selects the circuit rules used by a level.
 *
 * Circuit modes intentionally encode campaign mechanics rather than general
 * electronic components. See the comments in Levels.cpp for each level's use.
 */
enum CircuitMode : uint8_t {
  CIRCUIT_NONE,
  CIRCUIT_SHARED_RESET,
  CIRCUIT_SEPARATE_RELAY,
  CIRCUIT_AND_DYNAMIC_TRIPWIRE,
  CIRCUIT_SPLITTER_OVERLOAD
};

/*
 * Eight compass directions in clockwise 45-degree steps.
 *
 * The numeric ordering is an invariant used by the 90-degree turn helpers:
 * adding two turns clockwise and subtracting two turns counterclockwise.
 */
enum Direction : uint8_t {
  DIR_E,
  DIR_SE,
  DIR_S,
  DIR_SW,
  DIR_W,
  DIR_NW,
  DIR_N,
  DIR_NE
};

inline int8_t directionDeltaX(Direction direction) {
  switch (direction) {
    case DIR_E:
    case DIR_SE:
    case DIR_NE:
      return 1;
    case DIR_SW:
    case DIR_W:
    case DIR_NW:
      return -1;
    case DIR_S:
    case DIR_N:
      return 0;
  }
  return 0;
}

inline int8_t directionDeltaY(Direction direction) {
  switch (direction) {
    case DIR_SE:
    case DIR_S:
    case DIR_SW:
      return 1;
    case DIR_NW:
    case DIR_N:
    case DIR_NE:
      return -1;
    case DIR_E:
    case DIR_W:
      return 0;
  }
  return 0;
}

constexpr Direction turnCounterclockwise(Direction direction) {
  return static_cast<Direction>((static_cast<uint8_t>(direction) + 6) % 8);
}

constexpr Direction turnClockwise(Direction direction) {
  return static_cast<Direction>((static_cast<uint8_t>(direction) + 2) % 8);
}

enum ActivationSwitch : uint8_t {
  ACTIVATION_PRIMARY_SWITCH,
  ACTIVATION_SECONDARY_SWITCH,
  ACTIVATION_SNEAKY_TRIPWIRE_SWITCH,
  ACTIVATION_FINALE_TRIPWIRE_SWITCH
};

/* States in the victory/failure presentation sequence. */
enum CelebrationState : uint8_t {
  CELEBRATION_NONE,
  CELEBRATION_TARGET,
  CELEBRATION_PULSE,
  CELEBRATION_CANNON,
  CELEBRATION_EXPLOSION,
  CELEBRATION_DONE
};

/* States in the switch-activation pulse sequence. */
enum ActivationState : uint8_t {
  ACTIVATION_NONE,
  ACTIVATION_CLEANUP,
  ACTIVATION_CONTROL_LINE,
  ACTIVATION_HOLD
};

/* Zero-based grid coordinate. NO_POSITION represents an absent object. */
struct CellPosition {
  uint8_t col;
  uint8_t row;
};

/* Inclusive horizontal or vertical segment used for tripwire collision. */
struct GridSegment {
  CellPosition start;
  CellPosition end;
};

/*
 * Complete immutable definition of one campaign level.
 *
 * LevelDef objects and their wall arrays live in flash (PROGMEM). Game::buildLevel
 * copies the selected definition into SRAM, then expands its packed walls into
 * the live board. See Levels.h for authoring rules and Levels.cpp for examples.
 */
struct LevelDef {
  /* Null-terminated display name, including the terminator. */
  char name[LEVEL_NAME_LENGTH];

  /* Cannon location and the direction in which projectiles are launched. */
  CellPosition cannon;
  Direction cannonDirection;

  /* Destination that completes the level. */
  CellPosition target;

  /* PROGMEM array of packed wall coordinates and its element count. */
  const uint8_t* walls;
  uint8_t wallCount;

  /* Optional circuit objects; use NO_POSITION when an object is absent. */
  CellPosition switchA;
  CellPosition switchB;
  CellPosition gate;

  /* Rules governing switches, gates, tripwires, and the splitter. */
  CircuitMode circuitMode;

  /* Optional inclusive tripwires; use NO_SEGMENT when absent. */
  GridSegment tripwire;
  GridSegment tripwire2;

  /* Optional splitter location. */
  CellPosition splitter;
};

/* One projectile moving between grid cells. */
struct Ball {
  bool active;
  int8_t col;
  int8_t row;
  Direction direction;
  uint8_t progress;
  CellPosition lastDeflector;
  bool canSplit;
};

template <size_t N>
uint16_t activeBallMask(const Ball (&balls)[N]) {
  static_assert(N <= 16, "Active-ball snapshots use a 16-bit mask");
  uint16_t mask = 0;
  for (size_t i = 0; i < N; ++i) {
    if (balls[i].active) mask |= static_cast<uint16_t>(1U << i);
  }
  return mask;
}

/* Bounded cell route used only for activation and celebration effects. */
struct CellPath {
  CellPosition cells[MAX_VISUAL_PATH];
  uint8_t length;
};

/* Fixed-point particle used by the cannon explosion. Positions use eighth-pixels. */
struct CannonFragment {
  int16_t x8;
  int16_t y8;
  int8_t vx8;
  int8_t vy8;
  uint8_t life;
  uint8_t shape;
};

// Sentinel values and campaign-specific dynamically revealed switch positions.
constexpr CellPosition NO_POSITION = {NO_CELL, NO_CELL};
constexpr GridSegment NO_SEGMENT = {NO_POSITION, NO_POSITION};
constexpr CellPosition GAMEPLAY_CURSOR_START = {1, 1};
constexpr CellPosition SNEAKY_TRIPWIRE_SWITCH_POSITION = {2, 0};
constexpr CellPosition FINALE_TRIPWIRE_SWITCH_POSITION = {2, 6};

/* Packs a wall coordinate as row in the high nibble and column in the low nibble. */
constexpr uint8_t packedCell(uint8_t col, uint8_t row) {
  return static_cast<uint8_t>((row << 4) | col);
}

template <typename T, size_t N>
constexpr size_t arrayCount(const T (&)[N]) {
  return N;
}

constexpr bool isInsideGrid(int16_t col, int16_t row) {
  return col >= 0 && col < GRID_COLS && row >= 0 && row < GRID_ROWS;
}

constexpr bool isValidPosition(CellPosition position) {
  return position.col < GRID_COLS && position.row < GRID_ROWS;
}

constexpr bool isValidSegment(GridSegment segment) {
  return isValidPosition(segment.start) && isValidPosition(segment.end);
}

inline bool positionTouchesSegment(int8_t col, int8_t row, GridSegment segment) {
  if (!isValidSegment(segment)) return false;

  if (segment.start.col == segment.end.col) {
    const uint8_t minRow =
        segment.start.row < segment.end.row ? segment.start.row : segment.end.row;
    const uint8_t maxRow =
        segment.start.row > segment.end.row ? segment.start.row : segment.end.row;
    return col == segment.start.col && row >= minRow && row <= maxRow;
  }

  if (segment.start.row == segment.end.row) {
    const uint8_t minCol =
        segment.start.col < segment.end.col ? segment.start.col : segment.end.col;
    const uint8_t maxCol =
        segment.start.col > segment.end.col ? segment.start.col : segment.end.col;
    return row == segment.start.row && col >= minCol && col <= maxCol;
  }

  return false;
}

constexpr bool isDeflectorTile(Tile tile) {
  return tile == TILE_DEFLECT_X || tile == TILE_DEFLECT_DOT;
}

constexpr bool isAndCircuit(CircuitMode mode) {
  return mode == CIRCUIT_AND_DYNAMIC_TRIPWIRE || mode == CIRCUIT_SPLITTER_OVERLOAD;
}

constexpr bool isSeparateRelayCircuit(CircuitMode mode) {
  return mode == CIRCUIT_SEPARATE_RELAY;
}

constexpr int16_t cellCenterX(int16_t col) {
  return static_cast<int16_t>(BOARD_X + col * CELL_SIZE + CELL_SIZE / 2);
}

constexpr int16_t cellCenterY(int16_t row) {
  return static_cast<int16_t>(BOARD_Y + row * CELL_SIZE + CELL_SIZE / 2);
}

static_assert(GRID_COLS <= 16, "Packed wall cells support at most 16 columns");
static_assert(GRID_ROWS <= 16, "Packed wall cells support at most 16 rows");
static_assert(GRID_COLS <= INT8_MAX && GRID_ROWS <= INT8_MAX,
              "Ball coordinates must fit in int8_t");
static_assert(MAX_VISUAL_PATH <= UINT8_MAX, "CellPath::length must fit in uint8_t");
static_assert(MAX_BALLS <= 16, "The per-tick active-ball snapshot uses a 16-bit mask");
static_assert(DIR_E == 0 && DIR_S == 2 && DIR_W == 4 && DIR_N == 6,
              "Direction rotation helpers require 45-degree enum spacing");

}  // namespace MostlyLogic
