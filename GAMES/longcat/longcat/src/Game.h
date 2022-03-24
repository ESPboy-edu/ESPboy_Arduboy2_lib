#pragma once

#include <Arduboy2.h>

#include "States/GameState.h"

#include "GameContext/GameContext.h"

#include "GameMode/GameMode.h"

#include "States.h"

#include "Utils/SaveUtil.h"

class Game
{
public:
  void setup();
  void loop();

private:
  GameMode gameMode{GameMode::Fixed};
  GameState gameState{GameState::SplashScreen};
  GameContext gameContext;

private:
  Arduboy2 arduboy;
  GameStateSplashScreen splashScreenState;
  GameStateMainMenu mainMenuState;
  GameStateSelectLevel selectLevelState;
  GameStateSelectSeed selectSeedState;
  GameStateLoadLevel loadLevelState;
  GameStateGamePlay gamePlayState;
  GameStateSimulate simulateState;
  GameStateCampaignMenu campaignMenuState;
  GameStateRandomMenu randomMenuState;

public:
  void setGameMode(GameMode gameMode)
  {
    this->gameMode = gameMode;
  }

  GameMode getGameMode() const
  {
    return this->gameMode;
  }

  void setGameState(GameState gameState)
  {
    this->gameState = gameState;
  }

  GameState getGameState() const
  {
    return this->gameState;
  }

  GameContext &getGameContext()
  {
    return this->gameContext;
  }

  const GameContext &getGameContext() const
  {
    return this->gameContext;
  }

  Arduboy2 &getArduboy()
  {
    return this->arduboy;
  }

  const Arduboy2 &getArduboy() const
  {
    return this->arduboy;
  }
};
