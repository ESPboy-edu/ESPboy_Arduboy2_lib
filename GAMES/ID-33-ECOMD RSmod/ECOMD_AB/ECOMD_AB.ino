/*
 EPIC CRATES OF MASS DESTRUCTION : http://www.team-arg.org/ecomd-manual.html

 Arduboy version 1.1:  http://www.team-arg.org/ecomd-downloads.html

 MADE by TEAM a.r.g. : http://www.team-arg.org/more-about.html

 2015 - DRAGULA96 - JO3RI  (Firepit animation by @JUSTIN_CYR)

 Game License: MIT : https://opensource.org/licenses/MIT

*/

//determine the game
#define GAME_ID 33

#include "globals.h"
#include "Rects.h"
#include "Player.h"
#include "Weapons.h"
#include "Crate.h"
#include "Enemy.h"
#include "score.h"
#include "inputs.h"
#include "stage.h"
#include "menu.h"
#include "game.h"

typedef void (*FunctionPointer) ();

const FunctionPointer PROGMEM mainGameLoop[] = {
  stateMenuIntro,
  stateMenuMain,
  stateMenuHelp,
  stateMenuPlay,
  stateMenuInfo,
  stateMenuSoundfx,
  stateGameInitLevel,
  stateGamePlaying,
  stateGamePause,
  stateGameOver,
};


void setup()
{
  arduboy.begin();
  arduboy.setFrameRate(60);
  arduboy.initRandomSeed();
}

void loop() {
  if (!(arduboy.nextFrame())) return;
  arduboy.pollButtons();
  arduboy.clear();
  ((FunctionPointer) pgm_read_ptr (&mainGameLoop[gameState]))();
  arduboy.display();
}
