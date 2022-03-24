#include "GameStateSplashScreen.h"

#include "../Game.h"

void GameStateSplashScreen::update(Game &game)
{
  auto &arduboy = game.getArduboy();
  if (arduboy.justPressed(A_BUTTON))
  {
    game.setGameState(GameState::MainMenu);
  }
}

void GameStateSplashScreen::render(Game &game)
{
  auto &arduboy = game.getArduboy();
  arduboy.drawBitmap(0,0,splash,128,64,WHITE);
}