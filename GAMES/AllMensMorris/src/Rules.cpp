#include "Rules.h"

void resetMorrisGame(MorrisGameState &game, const BoardDefinition &board, const RuleSet &rules) {
  game.board = &board;
  game.rules = &rules;
  for (uint8_t i = 0; i < MORRIS_PACKED_POINT_BYTES; i++) {
    game.points[i] = 0;
  }
  game.currentPlayer = PLAYER_ONE;
  game.winner = PLAYER_NONE;
  game.phase = PHASE_PLACING;
  game.winReason = WIN_NONE;
  game.cursor = 0;
  game.selected = 255;
  game.piecesToPlace[0] = rules.piecesPerPlayer;
  game.piecesToPlace[1] = rules.piecesPerPlayer;
  game.piecesOnBoard[0] = 0;
  game.piecesOnBoard[1] = 0;
  game.turnsSinceCapture = 0;
  game.phaseAfterCapture = PHASE_PLACING;
  game.actionMode = TURN_ACTION_PLACE;
  game.stateFlags = 0;
  if (ruleFlag(rules, RULE_START_IN_MOVING)) {
    game.phase = PHASE_MOVING;
  }
}

void resetMorrisGame(MorrisGameState &game) {
  resetMorrisGame(game, ClassicBoardDefinition, ClassicRuleSet);
}

uint8_t playerIndex(Player player) {
  return player == PLAYER_TWO ? 1 : 0;
}

Player opponentOf(Player player) {
  return player == PLAYER_ONE ? PLAYER_TWO : PLAYER_ONE;
}

Player pointAt(const MorrisGameState &game, uint8_t point) {
  uint8_t shift = (point & 3) * 2;
  return static_cast<Player>((game.points[point >> 2] >> shift) & 3);
}

void setPointAt(MorrisGameState &game, uint8_t point, Player player) {
  uint8_t shift = (point & 3) * 2;
  uint8_t mask = 3 << shift;
  uint8_t &byte = game.points[point >> 2];
  byte = (byte & ~mask) | (static_cast<uint8_t>(player) << shift);
}

bool isMillAt(const MorrisGameState &game, uint8_t point, Player player) {
  if (player == PLAYER_NONE) {
    return false;
  }

  for (uint8_t i = 0; i < game.board->millCount; i++) {
    MillLine line = millLine(*game.board, i);
    bool containsPoint = line.a == point || line.b == point || line.c == point;
    if (!containsPoint) {
      continue;
    }
    if (pointAt(game, line.a) == player
        && pointAt(game, line.b) == player
        && pointAt(game, line.c) == player) {
      return true;
    }
  }
  return false;
}

static uint8_t emptyPointCount(const MorrisGameState &game) {
  uint8_t count = 0;
  for (uint8_t point = 0; point < game.board->pointCount; point++) {
    if (pointAt(game, point) == PLAYER_NONE) {
      count++;
    }
  }
  return count;
}

static bool phaseAllowsPlacement(const MorrisGameState &game) {
  return game.phase == PHASE_PLACING
      || (game.phase == PHASE_MOVING && ruleFlag(*game.rules, RULE_MIXED_PLACEMENT_MOVEMENT));
}

static bool playerCanPlace(const MorrisGameState &game, Player player) {
  return phaseAllowsPlacement(game)
      && game.piecesToPlace[playerIndex(player)] > 0
      && emptyPointCount(game) > game.rules->placementStopEmptyPoints;
}

bool canPlaceAt(const MorrisGameState &game, uint8_t point) {
  return playerCanPlace(game, game.currentPlayer) && pointAt(game, point) == PLAYER_NONE;
}

bool playerCanFly(const MorrisGameState &game, Player player) {
  uint8_t index = playerIndex(player);
  return ruleFlag(*game.rules, RULE_FLYING_ENABLED)
      && game.phase == PHASE_MOVING
      && game.piecesToPlace[index] == 0
      && game.piecesOnBoard[index] == game.rules->flyPieceCount;
}

bool canMovePiece(const MorrisGameState &game, uint8_t from, uint8_t to) {
  if (game.phase != PHASE_MOVING
      || pointAt(game, from) != game.currentPlayer
      || pointAt(game, to) != PLAYER_NONE) {
    return false;
  }

  if (playerCanFly(game, game.currentPlayer)) {
    return true;
  }

  for (uint8_t slot = 0; slot < adjacencyCount(*game.board, from); slot++) {
    if (adjacentPoint(*game.board, from, slot) == to) {
      return true;
    }
  }
  return false;
}

static bool playerHasPieceOutsideMill(const MorrisGameState &game, Player player) {
  for (uint8_t point = 0; point < game.board->pointCount; point++) {
    if (pointAt(game, point) == player && !isMillAt(game, point, player)) {
      return true;
    }
  }
  return false;
}

bool canCaptureAt(const MorrisGameState &game, uint8_t point) {
  if (game.phase != PHASE_CAPTURING || !gameStateFlag(game, MORRIS_STATE_MILL_PENDING)) {
    return false;
  }

  Player opponent = opponentOf(game.currentPlayer);
  if (pointAt(game, point) != opponent) {
    return false;
  }

  return !ruleFlag(*game.rules, RULE_PROTECT_PIECES_IN_MILLS)
      || !isMillAt(game, point, opponent)
      || !playerHasPieceOutsideMill(game, opponent);
}

static void endTurn(MorrisGameState &game) {
  game.currentPlayer = opponentOf(game.currentPlayer);
}

static bool playerHasOpenPoint(const MorrisGameState &game) {
  for (uint8_t point = 0; point < game.board->pointCount; point++) {
    if (pointAt(game, point) == PLAYER_NONE) {
      return true;
    }
  }
  return false;
}

static bool playerHasLegalMove(const MorrisGameState &game, Player player) {
  if (game.phase != PHASE_MOVING) {
    return true;
  }

  if (playerCanFly(game, player)) {
    return playerHasOpenPoint(game);
  }

  for (uint8_t point = 0; point < game.board->pointCount; point++) {
    if (pointAt(game, point) != player) {
      continue;
    }

    for (uint8_t slot = 0; slot < adjacencyCount(*game.board, point); slot++) {
      uint8_t adjacent = adjacentPoint(*game.board, point, slot);
      if (adjacent != 255 && pointAt(game, adjacent) == PLAYER_NONE) {
        return true;
      }
    }
  }
  return false;
}

static bool playerHasLegalAction(const MorrisGameState &game, Player player) {
  return playerCanPlace(game, player) || playerHasLegalMove(game, player);
}

static bool playerHasTooFewPieces(const MorrisGameState &game, Player player) {
  uint8_t index = playerIndex(player);
  return ruleFlag(*game.rules, RULE_MATERIAL_WIN_ENABLED)
      && (!ruleFlag(*game.rules, RULE_MATERIAL_WIN_REQUIRES_RESERVE_EMPTY) || game.piecesToPlace[index] == 0)
      && game.piecesOnBoard[index] < game.rules->minPiecesToContinue;
}

static bool blockWinAllowed(const MorrisGameState &game, Player player) {
  return !ruleFlag(*game.rules, RULE_BLOCK_WIN_REQUIRES_RESERVE_EMPTY)
      || game.piecesToPlace[playerIndex(player)] == 0;
}

static void setGameOver(MorrisGameState &game, Player winner, WinReason reason) {
  game.phase = PHASE_GAME_OVER;
  game.winner = winner;
  game.winReason = reason;
  game.selected = 255;
  setGameStateFlag(game, MORRIS_STATE_MILL_PENDING, false);
}

static void setDraw(MorrisGameState &game, WinReason reason) {
  setGameOver(game, PLAYER_NONE, reason);
}

void normalizeTurnActionMode(MorrisGameState &game) {
  if (game.phase == PHASE_PLACING) {
    game.actionMode = TURN_ACTION_PLACE;
    return;
  }

  if (game.phase != PHASE_MOVING) {
    return;
  }

  bool canPlace = playerCanPlace(game, game.currentPlayer);
  bool canMove = playerHasLegalMove(game, game.currentPlayer);
  if (game.actionMode == TURN_ACTION_PLACE && !canPlace && canMove) {
    game.actionMode = TURN_ACTION_MOVE;
  } else if (game.actionMode == TURN_ACTION_MOVE && !canMove && canPlace) {
    game.actionMode = TURN_ACTION_PLACE;
  }
}

static void finishPlacementIfReady(MorrisGameState &game) {
  if (game.phase == PHASE_PLACING
      && ((game.rules->placementStopEmptyPoints > 0
           && emptyPointCount(game) <= game.rules->placementStopEmptyPoints)
          || (game.piecesToPlace[0] == 0 && game.piecesToPlace[1] == 0))) {
    game.phase = PHASE_MOVING;
    game.actionMode = TURN_ACTION_MOVE;
    normalizeTurnActionMode(game);
  }
}

static GamePhase phaseAfterAction(MorrisGameState &game, GamePhase currentPhase) {
  if (currentPhase == PHASE_PLACING
      && ((game.rules->placementStopEmptyPoints > 0
           && emptyPointCount(game) <= game.rules->placementStopEmptyPoints)
          || (game.piecesToPlace[0] == 0 && game.piecesToPlace[1] == 0))) {
    return PHASE_MOVING;
  }
  return currentPhase;
}

static void checkCurrentPlayerLoss(MorrisGameState &game) {
  if (game.phase != PHASE_MOVING) {
    return;
  }

  if (playerHasTooFewPieces(game, game.currentPlayer)) {
    setGameOver(game, opponentOf(game.currentPlayer), WIN_BY_MATERIAL);
    return;
  }

  if (ruleFlag(*game.rules, RULE_BLOCK_WIN_ENABLED)
      && !playerHasLegalAction(game, game.currentPlayer)
      && blockWinAllowed(game, game.currentPlayer)) {
    setGameOver(game, opponentOf(game.currentPlayer), WIN_BY_BLOCK);
  }
}

static void resolveCurrentPlayerTurn(MorrisGameState &game) {
  for (uint8_t attempt = 0; attempt < 2 && game.phase == PHASE_MOVING; attempt++) {
    if (playerHasTooFewPieces(game, game.currentPlayer)) {
      setGameOver(game, opponentOf(game.currentPlayer), WIN_BY_MATERIAL);
      return;
    }

    if (playerHasLegalAction(game, game.currentPlayer)) {
      normalizeTurnActionMode(game);
      return;
    }

    if (ruleFlag(*game.rules, RULE_BLOCK_WIN_ENABLED) && blockWinAllowed(game, game.currentPlayer)) {
      setGameOver(game, opponentOf(game.currentPlayer), WIN_BY_BLOCK);
      return;
    }

    if (ruleFlag(*game.rules, RULE_SKIP_BLOCKED_WITH_RESERVE)
        && game.piecesToPlace[playerIndex(game.currentPlayer)] > 0) {
      endTurn(game);
      continue;
    }

    if (ruleFlag(*game.rules, RULE_BLOCK_WIN_ENABLED)) {
      setGameOver(game, opponentOf(game.currentPlayer), WIN_BY_BLOCK);
      return;
    }
  }
}

static bool checkNoCaptureDraw(MorrisGameState &game) {
  if (game.phase != PHASE_MOVING) {
    return false;
  }

  if (game.rules->noCaptureDrawTurnLimit > 0
      && game.turnsSinceCapture >= game.rules->noCaptureDrawTurnLimit) {
    setDraw(game, WIN_DRAW_NO_CAPTURE);
    return true;
  }
  return false;
}

static void enterCapture(MorrisGameState &game, GamePhase phaseAfterCapture) {
  game.phase = PHASE_CAPTURING;
  game.phaseAfterCapture = phaseAfterCapture;
  setGameStateFlag(game, MORRIS_STATE_MILL_PENDING, true);
}

static void finishAction(MorrisGameState &game, GamePhase currentPhase) {
  if (gameStateFlag(game, MORRIS_STATE_LAST_MOVE_MADE_MILL)) {
    if (millAction(*game.rules) == MILL_ACTION_WIN) {
      setGameOver(game, game.currentPlayer, WIN_BY_MILL);
    } else {
      enterCapture(game, phaseAfterAction(game, currentPhase));
    }
    return;
  }

  finishPlacementIfReady(game);
  bool shouldCheckDraw = false;
  if (currentPhase == PHASE_MOVING && game.phase == PHASE_MOVING) {
    game.turnsSinceCapture++;
    shouldCheckDraw = true;
  }
  endTurn(game);
  resolveCurrentPlayerTurn(game);
  if (shouldCheckDraw && game.phase != PHASE_GAME_OVER) {
    checkNoCaptureDraw(game);
  }
}

static bool placeAtCursor(MorrisGameState &game, GamePhase currentPhase) {
  if (!canPlaceAt(game, game.cursor)) {
    return false;
  }

  uint8_t index = playerIndex(game.currentPlayer);
  setPointAt(game, game.cursor, game.currentPlayer);
  game.piecesToPlace[index]--;
  game.piecesOnBoard[index]++;
  setGameStateFlag(game, MORRIS_STATE_LAST_MOVE_MADE_MILL, isMillAt(game, game.cursor, game.currentPlayer));
  finishAction(game, currentPhase);
  return true;
}

static bool moveFromSelection(MorrisGameState &game) {
  if (game.selected == 255) {
    if (pointAt(game, game.cursor) == game.currentPlayer) {
      game.selected = game.cursor;
      return true;
    }
    return false;
  }

  if (game.cursor == game.selected) {
    game.selected = 255;
    return true;
  }

  if (!canMovePiece(game, game.selected, game.cursor)) {
    return false;
  }

  setPointAt(game, game.cursor, game.currentPlayer);
  setPointAt(game, game.selected, PLAYER_NONE);
  setGameStateFlag(game, MORRIS_STATE_LAST_MOVE_MADE_MILL, isMillAt(game, game.cursor, game.currentPlayer));
  game.selected = 255;
  finishAction(game, PHASE_MOVING);
  return true;
}

static bool captureAtCursor(MorrisGameState &game) {
  if (!canCaptureAt(game, game.cursor)) {
    return false;
  }

  uint8_t opponentIndex = playerIndex(opponentOf(game.currentPlayer));
  setPointAt(game, game.cursor, PLAYER_NONE);
  if (game.piecesOnBoard[opponentIndex] > 0) {
    game.piecesOnBoard[opponentIndex]--;
  }

  game.phase = game.phaseAfterCapture;
  game.phaseAfterCapture = PHASE_PLACING;
  setGameStateFlag(game, MORRIS_STATE_MILL_PENDING, false);
  setGameStateFlag(game, MORRIS_STATE_LAST_MOVE_MADE_MILL, false);
  game.selected = 255;
  game.turnsSinceCapture = 0;
  finishPlacementIfReady(game);
  if (game.phase == PHASE_MOVING && playerHasTooFewPieces(game, opponentOf(game.currentPlayer))) {
    setGameOver(game, game.currentPlayer, WIN_BY_MATERIAL);
    return true;
  }
  endTurn(game);
  resolveCurrentPlayerTurn(game);
  return true;
}

bool applyPrimaryAction(MorrisGameState &game) {
  setGameStateFlag(game, MORRIS_STATE_LAST_MOVE_MADE_MILL, false);
  if (game.phase == PHASE_PLACING) {
    return placeAtCursor(game, PHASE_PLACING);
  }
  if (game.phase == PHASE_CAPTURING) {
    return captureAtCursor(game);
  }
  if (game.phase == PHASE_MOVING) {
    normalizeTurnActionMode(game);
    if (game.selected == 255 && canPlaceAt(game, game.cursor)) {
      game.actionMode = TURN_ACTION_PLACE;
      return placeAtCursor(game, PHASE_MOVING);
    }
    game.actionMode = TURN_ACTION_MOVE;
    return moveFromSelection(game);
  }
  return false;
}

void advanceCursor(MorrisGameState &game, int8_t delta) {
  int8_t next = static_cast<int8_t>(game.cursor) + delta;
  while (next < 0) {
    next += game.board->pointCount;
  }
  while (next >= game.board->pointCount) {
    next -= game.board->pointCount;
  }
  game.cursor = static_cast<uint8_t>(next);
}
