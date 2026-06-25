#pragma once

#include <Arduino.h>

#include "Board.h"

enum Player : uint8_t {
  PLAYER_NONE = 0,
  PLAYER_ONE = 1,
  PLAYER_TWO = 2,
};

enum GamePhase : uint8_t {
  PHASE_PLACING,
  PHASE_CAPTURING,
  PHASE_MOVING,
  PHASE_GAME_OVER,
};

enum WinReason : uint8_t {
  WIN_NONE,
  WIN_BY_MATERIAL,
  WIN_BY_BLOCK,
  WIN_BY_MILL,
  WIN_DRAW_NO_CAPTURE,
};

constexpr uint8_t DRAW_NO_CAPTURE_TURN_LIMIT = 50;

enum MillAction : uint8_t {
  MILL_ACTION_CAPTURE,
  MILL_ACTION_WIN,
};

enum TurnActionMode : uint8_t {
  TURN_ACTION_PLACE,
  TURN_ACTION_MOVE,
};

constexpr uint8_t MORRIS_PACKED_POINT_BYTES = (MORRIS_MAX_POINT_COUNT + 3) / 4;

enum MorrisStateFlag : uint8_t {
  MORRIS_STATE_MILL_PENDING = 1 << 0,
  MORRIS_STATE_LAST_MOVE_MADE_MILL = 1 << 1,
};

enum RuleFlag : uint16_t {
  RULE_MILL_ACTION_WIN = 1 << 0,
  RULE_FLYING_ENABLED = 1 << 1,
  RULE_MIXED_PLACEMENT_MOVEMENT = 1 << 2,
  RULE_PROTECT_PIECES_IN_MILLS = 1 << 3,
  RULE_BLOCK_WIN_ENABLED = 1 << 4,
  RULE_MATERIAL_WIN_ENABLED = 1 << 5,
  RULE_BLOCK_WIN_REQUIRES_RESERVE_EMPTY = 1 << 6,
  RULE_MATERIAL_WIN_REQUIRES_RESERVE_EMPTY = 1 << 7,
  RULE_SKIP_BLOCKED_WITH_RESERVE = 1 << 8,
  RULE_START_IN_MOVING = 1 << 9,
};

struct RuleSet {
  uint8_t piecesPerPlayer;
  uint8_t minPiecesToContinue;
  uint8_t flyPieceCount;
  uint8_t noCaptureDrawTurnLimit;
  uint8_t placementStopEmptyPoints;
  uint16_t flags;
};

extern const RuleSet ClassicRuleSet;

inline bool ruleFlag(const RuleSet &rules, RuleFlag flag) {
  return (rules.flags & flag) != 0;
}

inline MillAction millAction(const RuleSet &rules) {
  return ruleFlag(rules, RULE_MILL_ACTION_WIN) ? MILL_ACTION_WIN : MILL_ACTION_CAPTURE;
}

struct MorrisGameState {
  const BoardDefinition *board = &ClassicBoardDefinition;
  const RuleSet *rules = &ClassicRuleSet;
  uint8_t points[MORRIS_PACKED_POINT_BYTES] = {};
  Player currentPlayer = PLAYER_ONE;
  Player winner = PLAYER_NONE;
  GamePhase phase = PHASE_PLACING;
  WinReason winReason = WIN_NONE;
  uint8_t cursor = 0;
  uint8_t selected = 255;
  uint8_t piecesToPlace[2] = {9, 9};
  uint8_t piecesOnBoard[2] = {0, 0};
  uint8_t turnsSinceCapture = 0;
  GamePhase phaseAfterCapture = PHASE_PLACING;
  TurnActionMode actionMode = TURN_ACTION_PLACE;
  uint8_t stateFlags = 0;
};

inline bool gameStateFlag(const MorrisGameState &game, MorrisStateFlag flag) {
  return (game.stateFlags & flag) != 0;
}

inline void setGameStateFlag(MorrisGameState &game, MorrisStateFlag flag, bool enabled) {
  if (enabled) {
    game.stateFlags |= flag;
  } else {
    game.stateFlags &= ~flag;
  }
}

void resetMorrisGame(MorrisGameState &game);
void resetMorrisGame(MorrisGameState &game, const BoardDefinition &board, const RuleSet &rules);
uint8_t playerIndex(Player player);
Player opponentOf(Player player);
Player pointAt(const MorrisGameState &game, uint8_t point);
void setPointAt(MorrisGameState &game, uint8_t point, Player player);
bool isMillAt(const MorrisGameState &game, uint8_t point, Player player);
bool canPlaceAt(const MorrisGameState &game, uint8_t point);
bool playerCanFly(const MorrisGameState &game, Player player);
bool canMovePiece(const MorrisGameState &game, uint8_t from, uint8_t to);
bool canCaptureAt(const MorrisGameState &game, uint8_t point);
void normalizeTurnActionMode(MorrisGameState &game);
bool applyPrimaryAction(MorrisGameState &game);
void advanceCursor(MorrisGameState &game, int8_t delta);
