#include "GameStateSplashScreen.h"

#include "../Game.h"

void GameStateEndScreen::update(Game &game)
{
  auto &arduboy = game.getArduboy();
  if (arduboy.justPressed(A_BUTTON))
  {
    game.setGameState(GameState::MainMenu);
  }
}

void GameStateEndScreen::render(Game &game)
{
  auto &arduboy = game.getArduboy();
  
  int f = 0;
  uint8_t h = 0;
  
  f = arduboy.frameCount%600;
    
  arduboy.fillScreen();
  //PHASE 1
  if(f<150){
	  h = 1;
	  arduboy.drawLine(128+15,h+8,0,h+8,BLACK);
	  arduboy.drawLine(128+15,h+12,0,h+12,BLACK);
	  h = 48;
	  Sprites::drawOverwrite(128-f, h, endCat, 0);
	  arduboy.drawLine(128+15,h+8,128-f+15,h+8,BLACK);
	  arduboy.drawLine(128+15,h+12,128-f+15,h+12,BLACK);
	  h = 16;
	  Sprites::drawOverwrite(128-f, h, endCat, 1);
	  arduboy.drawLine(0,h+8,128-f+15,h+8,BLACK);
	  arduboy.drawLine(0,h+12,128-f+15,h+12,BLACK);
  }
  if(f>=150 and f<300){
	  f=f-150;
  //PHASE 2
	  h = 48;
	  arduboy.drawLine(128+15,h+8,0,h+8,BLACK);
	  arduboy.drawLine(128+15,h+12,0,h+12,BLACK);
	  h = 32;
	  Sprites::drawOverwrite(f-16, h, endCat, 2);
	  arduboy.drawLine(0,h+8,f-15,h+8,BLACK);
	  arduboy.drawLine(0,h+12,f-15,h+12,BLACK);
	  h = 1;
	  Sprites::drawOverwrite(128-f, h, endCat, 1);
	  arduboy.drawLine(0,h+8,128-f+15,h+8,BLACK);
	  arduboy.drawLine(0,h+12,128-f+15,h+12,BLACK);
  }
  if(f>=300 and f<450){
	  f=f-300;
  //PHASE 3
	  h = 32;
	  arduboy.drawLine(128+15,h+8,0,h+8,BLACK);
	  arduboy.drawLine(128+15,h+12,0,h+12,BLACK);
	  h = 16;
	  Sprites::drawOverwrite(128-f, h, endCat, 0);
	  arduboy.drawLine(128+15,h+8,128-f+15,h+8,BLACK);
	  arduboy.drawLine(128+15,h+12,128-f+15,h+12,BLACK);
	  h = 48;
	  Sprites::drawOverwrite(128-f, h, endCat, 1);
	  arduboy.drawLine(0,h+8,128-f+15,h+8,BLACK);
	  arduboy.drawLine(0,h+12,128-f+15,h+12,BLACK);
  }
  if(f>=450 and f<600){
	  f=f-450;
  //PHASE 4
	  h = 16;
	  arduboy.drawLine(128+15,h+8,0,h+8,BLACK);
	  arduboy.drawLine(128+15,h+12,0,h+12,BLACK);
	  h = 1;
	  Sprites::drawOverwrite(f-16, h, endCat, 2);
	  arduboy.drawLine(0,h+8,f-15,h+8,BLACK);
	  arduboy.drawLine(0,h+12,f-15,h+12,BLACK);
	  h = 32;
	  Sprites::drawOverwrite(128-f, h, endCat, 1);
	  arduboy.drawLine(0,h+8,128-f+15,h+8,BLACK);
	  arduboy.drawLine(0,h+12,128-f+15,h+12,BLACK);
  }
  if(f%60<30)
  {
  Sprites::drawErase(20,17,endTitle,0);
  }
}