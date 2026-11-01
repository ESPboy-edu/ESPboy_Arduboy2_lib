/*
  Why Did the Chicken Cross the Street
  Arduboy version 0.0.1
  
  TEAM a.r.g.
  2026 - JO3RI

  Game License: MIT : https://opensource.org/licenses/MIT

*/

//determine the game
#define GAME_ID 39

#include "globals.h"
#include "menu.h"
#include "game.h"
#include "inputs.h"


typedef void (*FunctionPointer) ();
const FunctionPointer PROGMEM  mainGameLoop[] =
{
  stateMenuMain,
  stateMenuConf,
  stateMenuSdfx,
  stateMenuInfo,
  stateMenuPlay,
  stateMenuIntro,
  stateGamePlaying,
  stateGamePause,
  stateGameOver,
};


void setup()
{
  arduboy.boot();
  arduboy.audio.on();
  if (arduboy.audio.enabled()) ATM.play(silentSong);   // an empty song keeps the playroutine running (music.h)
  arduboy.setFrameRate(45);
  
};

void loop() {
  if (!(arduboy.nextFrame())) return;
  arduboy.pollButtons();
  musicUpdate();
  arduboy.clear();
  ((FunctionPointer) pgm_read_dword (&mainGameLoop[gameState]))();
  arduboy.display();
};

