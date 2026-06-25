#include "Ai.h"

namespace {
constexpr int16_t WIN_SCORE = 30000;
constexpr uint8_t NO_POINT = 255;
constexpr uint8_t TOP_ACTION_COUNT = 3;

struct RankedAction {
  AiAction action;
  int16_t score;
};

int16_t absoluteScore(int16_t value) {
  return value < 0 ? -value : value;
}

uint8_t countCompleteMills(const MorrisGameState &game, Player player) {
  uint8_t count = 0;
  for (uint8_t i = 0; i < game.board->millCount; i++) {
    MillLine line = millLine(*game.board, i);
    if (pointAt(game, line.a) == player
        && pointAt(game, line.b) == player
        && pointAt(game, line.c) == player) {
      count++;
    }
  }
  return count;
}

uint8_t countOpenTwoMills(const MorrisGameState &game, Player player) {
  uint8_t count = 0;
  for (uint8_t i = 0; i < game.board->millCount; i++) {
    MillLine line = millLine(*game.board, i);
    uint8_t owned = 0;
    uint8_t empty = 0;
    Player a = pointAt(game, line.a);
    Player b = pointAt(game, line.b);
    Player c = pointAt(game, line.c);
    owned += a == player ? 1 : 0;
    owned += b == player ? 1 : 0;
    owned += c == player ? 1 : 0;
    empty += a == PLAYER_NONE ? 1 : 0;
    empty += b == PLAYER_NONE ? 1 : 0;
    empty += c == PLAYER_NONE ? 1 : 0;
    if (owned == 2 && empty == 1) {
      count++;
    }
  }
  return count;
}

uint8_t pointDegree(const MorrisGameState &game, uint8_t point) {
  uint8_t count = 0;
  for (uint8_t slot = 0; slot < adjacencyCount(*game.board, point); slot++) {
    if (adjacentPoint(*game.board, point, slot) != 255) {
      count++;
    }
  }
  return count;
}

uint8_t mobilityFor(const MorrisGameState &game, Player player) {
  if (game.phase != PHASE_MOVING && game.phase != PHASE_PLACING) {
    return 0;
  }

  uint8_t moves = 0;
  for (uint8_t from = 0; from < game.board->pointCount; from++) {
    if (pointAt(game, from) != player) {
      continue;
    }

    if (playerCanFly(game, player)) {
      for (uint8_t to = 0; to < game.board->pointCount; to++) {
        if (pointAt(game, to) == PLAYER_NONE) {
          moves++;
        }
      }
      continue;
    }

    for (uint8_t slot = 0; slot < adjacencyCount(*game.board, from); slot++) {
      uint8_t to = adjacentPoint(*game.board, from, slot);
      if (to != 255 && pointAt(game, to) == PLAYER_NONE) {
        moves++;
      }
    }
  }
  return moves;
}

int16_t evaluateState(const MorrisGameState &game, Player aiPlayer) {
  Player opponent = opponentOf(aiPlayer);
  if (game.phase == PHASE_GAME_OVER) {
    if (game.winner == aiPlayer) {
      return WIN_SCORE;
    }
    if (game.winner == opponent) {
      return -WIN_SCORE;
    }
    return 0;
  }

  uint8_t aiIndex = playerIndex(aiPlayer);
  uint8_t opponentIndex = playerIndex(opponent);
  int16_t score = 0;
  score += static_cast<int16_t>(game.piecesOnBoard[aiIndex] - game.piecesOnBoard[opponentIndex]) * 120;
  score += static_cast<int16_t>(game.piecesToPlace[aiIndex] - game.piecesToPlace[opponentIndex]) * 12;
  score += static_cast<int16_t>(countCompleteMills(game, aiPlayer) - countCompleteMills(game, opponent)) * 90;
  score += static_cast<int16_t>(countOpenTwoMills(game, aiPlayer) - countOpenTwoMills(game, opponent)) * 35;
  score += static_cast<int16_t>(mobilityFor(game, aiPlayer) - mobilityFor(game, opponent)) * 4;

  for (uint8_t point = 0; point < game.board->pointCount; point++) {
    if (pointAt(game, point) == aiPlayer) {
      score += pointDegree(game, point) * 3;
    } else if (pointAt(game, point) == opponent) {
      score -= pointDegree(game, point) * 3;
    }
  }

  if (game.phase == PHASE_CAPTURING && game.currentPlayer == aiPlayer) {
    score += 500;
  }
  if (gameStateFlag(game, MORRIS_STATE_LAST_MOVE_MADE_MILL) && game.currentPlayer == aiPlayer) {
    score += millAction(*game.rules) == MILL_ACTION_WIN ? 1200 : 500;
  }
  return score;
}

bool applyAiAction(const MorrisGameState &game, const AiAction &action, MorrisGameState &result) {
  result = game;
  result.selected = action.from;
  result.cursor = action.to;
  result.actionMode = action.mode;
  return applyPrimaryAction(result);
}

int16_t scoreAction(const MorrisGameState &game, const AiAction &action, Player aiPlayer) {
  MorrisGameState after;
  if (!applyAiAction(game, action, after)) {
    return -WIN_SCORE;
  }

  int16_t score = evaluateState(after, aiPlayer);
  if (game.phase == PHASE_CAPTURING && pointAt(game, action.to) == opponentOf(aiPlayer)) {
    score += isMillAt(game, action.to, opponentOf(aiPlayer)) ? 10 : 80;
    score += pointDegree(game, action.to) * 8;
  }
  if (after.phase == PHASE_CAPTURING && after.currentPlayer == aiPlayer) {
    score += 600;
  }
  if (after.phase == PHASE_GAME_OVER && after.winner == aiPlayer) {
    score += 2000;
  }
  score -= absoluteScore(static_cast<int16_t>(action.to) - static_cast<int16_t>(game.cursor));
  return score;
}

int16_t minimaxScore(const MorrisGameState &game, Player aiPlayer, uint8_t depth);

int16_t scoreActionForDifficulty(const MorrisGameState &game, const AiAction &action,
                                 Player aiPlayer, AiDifficulty difficulty) {
  if (difficulty == AI_EASY) {
    return scoreAction(game, action, aiPlayer);
  }

  MorrisGameState after;
  if (!applyAiAction(game, action, after)) {
    return -WIN_SCORE;
  }
  int16_t score = minimaxScore(after, aiPlayer, 1);
  if (after.phase == PHASE_CAPTURING && after.currentPlayer == aiPlayer) {
    score += 120;
  }
  score -= absoluteScore(static_cast<int16_t>(action.to) - static_cast<int16_t>(game.cursor)) / 2;
  return score;
}

void addRankedAction(RankedAction ranked[], uint8_t &count, const AiAction &action, int16_t score) {
  uint8_t insertAt = count;
  while (insertAt > 0 && score > ranked[insertAt - 1].score) {
    if (insertAt < TOP_ACTION_COUNT) {
      ranked[insertAt] = ranked[insertAt - 1];
    }
    insertAt--;
  }

  if (insertAt >= TOP_ACTION_COUNT) {
    return;
  }
  if (count < TOP_ACTION_COUNT) {
    count++;
  }
  ranked[insertAt] = {action, score};
}

void rankAction(const MorrisGameState &game, const AiAction &action, Player aiPlayer,
                AiDifficulty difficulty, RankedAction ranked[], uint8_t &count) {
  int16_t score = scoreActionForDifficulty(game, action, aiPlayer, difficulty);
  addRankedAction(ranked, count, action, score);
}

void rankPlaceActions(const MorrisGameState &game, Player aiPlayer, TurnActionMode mode,
                      AiDifficulty difficulty, RankedAction ranked[], uint8_t &count) {
  for (uint8_t point = 0; point < game.board->pointCount; point++) {
    MorrisGameState candidate = game;
    candidate.actionMode = mode;
    candidate.cursor = point;
    if (canPlaceAt(candidate, point)) {
      rankAction(game, {NO_POINT, point, mode}, aiPlayer, difficulty, ranked, count);
    }
  }
}

void rankMoveActions(const MorrisGameState &game, Player aiPlayer, AiDifficulty difficulty,
                     RankedAction ranked[], uint8_t &count) {
  for (uint8_t from = 0; from < game.board->pointCount; from++) {
    if (pointAt(game, from) != aiPlayer) {
      continue;
    }
    for (uint8_t to = 0; to < game.board->pointCount; to++) {
      MorrisGameState candidate = game;
      candidate.selected = from;
      candidate.cursor = to;
      candidate.actionMode = TURN_ACTION_MOVE;
      if (canMovePiece(candidate, from, to)) {
        rankAction(game, {from, to, TURN_ACTION_MOVE}, aiPlayer, difficulty, ranked, count);
      }
    }
  }
}

void rankCaptureActions(const MorrisGameState &game, Player aiPlayer, AiDifficulty difficulty,
                        RankedAction ranked[], uint8_t &count) {
  for (uint8_t point = 0; point < game.board->pointCount; point++) {
    MorrisGameState candidate = game;
    candidate.cursor = point;
    if (canCaptureAt(candidate, point)) {
      rankAction(game, {NO_POINT, point, game.actionMode}, aiPlayer, difficulty, ranked, count);
    }
  }
}

uint16_t actionWeight(int16_t score, int16_t floorScore) {
  int16_t delta = score - floorScore;
  if (delta < 0) {
    delta = 0;
  } else if (delta > 180) {
    delta = 180;
  }
  return static_cast<uint16_t>(delta) + 1;
}

bool chooseRankedAction(const MorrisGameState &game, Player aiPlayer, AiDifficulty difficulty,
                        AiAction &action) {
  RankedAction ranked[TOP_ACTION_COUNT];
  uint8_t count = 0;

  if (game.phase == PHASE_CAPTURING) {
    rankCaptureActions(game, aiPlayer, difficulty, ranked, count);
  } else if (game.phase == PHASE_PLACING) {
    rankPlaceActions(game, aiPlayer, TURN_ACTION_PLACE, difficulty, ranked, count);
  } else if (game.phase == PHASE_MOVING) {
    if (ruleFlag(*game.rules, RULE_MIXED_PLACEMENT_MOVEMENT)) {
      rankPlaceActions(game, aiPlayer, TURN_ACTION_PLACE, difficulty, ranked, count);
    }
    rankMoveActions(game, aiPlayer, difficulty, ranked, count);
  }

  if (count == 0) {
    return false;
  }

  int16_t floorScore = ranked[count - 1].score;
  uint16_t totalWeight = 0;
  for (uint8_t i = 0; i < count; i++) {
    totalWeight += actionWeight(ranked[i].score, floorScore);
  }

  uint16_t pick = random(totalWeight);
  for (uint8_t i = 0; i < count; i++) {
    uint16_t weight = actionWeight(ranked[i].score, floorScore);
    if (pick < weight) {
      action = ranked[i].action;
      return true;
    }
    pick -= weight;
  }

  action = ranked[0].action;
  return true;
}

int16_t minimaxActionScore(const MorrisGameState &game, const AiAction &action,
                           Player aiPlayer, uint8_t depth) {
  MorrisGameState after;
  if (!applyAiAction(game, action, after)) {
    return game.currentPlayer == aiPlayer ? -WIN_SCORE : WIN_SCORE;
  }
  return minimaxScore(after, aiPlayer, depth - 1);
}

void considerMinimaxAction(const MorrisGameState &game, const AiAction &action, Player aiPlayer,
                           uint8_t depth, bool maximizing, bool &hasBest, int16_t &bestScore) {
  int16_t score = minimaxActionScore(game, action, aiPlayer, depth);
  if (!hasBest || (maximizing && score > bestScore) || (!maximizing && score < bestScore)) {
    hasBest = true;
    bestScore = score;
  }
}

int16_t minimaxScore(const MorrisGameState &game, Player aiPlayer, uint8_t depth) {
  if (depth == 0 || game.phase == PHASE_GAME_OVER) {
    return evaluateState(game, aiPlayer);
  }

  Player player = game.currentPlayer;
  bool maximizing = player == aiPlayer;
  bool hasBest = false;
  int16_t bestScore = maximizing ? -WIN_SCORE : WIN_SCORE;

  if (game.phase == PHASE_CAPTURING) {
    for (uint8_t point = 0; point < game.board->pointCount; point++) {
      MorrisGameState candidate = game;
      candidate.cursor = point;
      if (canCaptureAt(candidate, point)) {
        considerMinimaxAction(game, {NO_POINT, point, game.actionMode}, aiPlayer, depth,
                              maximizing, hasBest, bestScore);
      }
    }
  } else if (game.phase == PHASE_PLACING) {
    for (uint8_t point = 0; point < game.board->pointCount; point++) {
      MorrisGameState candidate = game;
      candidate.actionMode = TURN_ACTION_PLACE;
      candidate.cursor = point;
      if (canPlaceAt(candidate, point)) {
        considerMinimaxAction(game, {NO_POINT, point, TURN_ACTION_PLACE}, aiPlayer, depth,
                              maximizing, hasBest, bestScore);
      }
    }
  } else if (game.phase == PHASE_MOVING) {
    if (ruleFlag(*game.rules, RULE_MIXED_PLACEMENT_MOVEMENT)) {
      for (uint8_t point = 0; point < game.board->pointCount; point++) {
        MorrisGameState candidate = game;
        candidate.actionMode = TURN_ACTION_PLACE;
        candidate.cursor = point;
        if (canPlaceAt(candidate, point)) {
          considerMinimaxAction(game, {NO_POINT, point, TURN_ACTION_PLACE}, aiPlayer, depth,
                                maximizing, hasBest, bestScore);
        }
      }
    }
    for (uint8_t from = 0; from < game.board->pointCount; from++) {
      if (pointAt(game, from) != player) {
        continue;
      }
      for (uint8_t to = 0; to < game.board->pointCount; to++) {
        MorrisGameState candidate = game;
        candidate.selected = from;
        candidate.cursor = to;
        candidate.actionMode = TURN_ACTION_MOVE;
        if (canMovePiece(candidate, from, to)) {
          considerMinimaxAction(game, {from, to, TURN_ACTION_MOVE}, aiPlayer, depth,
                                maximizing, hasBest, bestScore);
        }
      }
    }
  }

  return hasBest ? bestScore : evaluateState(game, aiPlayer);
}
}

bool chooseAiAction(const MorrisGameState &game, MorrisGameState &result) {
  AiAction action;
  return chooseAiAction(game, action, result);
}

bool chooseAiAction(const MorrisGameState &game, AiAction &action, MorrisGameState &result) {
  return chooseAiAction(game, AI_EASY, action, result);
}

bool chooseAiAction(const MorrisGameState &game, AiDifficulty difficulty,
                    AiAction &action, MorrisGameState &result) {
  if (game.phase == PHASE_GAME_OVER) {
    return false;
  }

  Player aiPlayer = game.currentPlayer;
  AiAction bestAction = {NO_POINT, game.cursor, game.actionMode};
  if (!chooseRankedAction(game, aiPlayer, difficulty, bestAction)) {
    return false;
  }

  action = bestAction;
  return applyAiAction(game, bestAction, result);
}
