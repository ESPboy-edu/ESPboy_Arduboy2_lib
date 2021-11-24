#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include "globals.h"

void stateMenuIntro()
{
  globalCounter++;
  sprites.drawSelfMasked(34, 4, T_arg, 0);
  if (globalCounter > 100)
  {
    globalCounter = 0;
    gameState = STATE_MENU_MAIN;
  }
};

void stateMenuMain()
{
  arduboy.drawCompressed(0, 0, title_bitmap, WHITE);
  sprites.drawSelfMasked(20, 56, mainmenu_is_mask, menuSelection - 2);
  if (arduboy.justPressed(RIGHT_BUTTON) && (menuSelection < 5))menuSelection++;
  if (arduboy.justPressed(LEFT_BUTTON) && (menuSelection > 2))menuSelection--;
  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) gameState = menuSelection;
}

void stateMenuHelp()
{
  arduboy.drawCompressed(32, 0, qrcode_bitmap, WHITE);
  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) gameState = STATE_MENU_MAIN;
}

void stateMenuInfo()
{
  arduboy.drawCompressed(20, 0, info_bitmap, WHITE);
  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) gameState = STATE_MENU_MAIN;
}

void stateMenuSoundfx()
{
  arduboy.drawCompressed(0, 0, title_bitmap, WHITE);
  sprites.drawSelfMasked(22, 56, soundYesNo_is_mask, arduboy.audio.enabled());
  if (arduboy.justPressed(RIGHT_BUTTON)) arduboy.audio.on();
  if (arduboy.justPressed(LEFT_BUTTON)) arduboy.audio.off();
  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON))
  {
    arduboy.audio.saveOnOff();
    gameState = STATE_MENU_MAIN;
  }
}


void stateMenuPlay()
{
  gameState = STATE_GAME_INIT_LEVEL;
}




#endif
