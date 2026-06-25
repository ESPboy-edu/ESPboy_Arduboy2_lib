#pragma once

#include <Arduino.h>

#include "Rules.h"

struct AiAction {
  uint8_t from;
  uint8_t to;
  TurnActionMode mode;
};

enum AiDifficulty : uint8_t {
  AI_EASY,
  AI_HARD,
};

bool chooseAiAction(const MorrisGameState &game, MorrisGameState &result);
bool chooseAiAction(const MorrisGameState &game, AiAction &action, MorrisGameState &result);
bool chooseAiAction(const MorrisGameState &game, AiDifficulty difficulty,
                    AiAction &action, MorrisGameState &result);
