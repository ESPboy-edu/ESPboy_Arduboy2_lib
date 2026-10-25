/*
  Escaper Droid
  Arduboy version 1.0.1
  
  STARTED by TEAM a.r.g.
  2016 - JO3RI - STG

  CONTINUED by
  2026 - JO3RI

  Game License: MIT : https://opensource.org/licenses/MIT

*/

//determine the game
#define GAME_ID 20

#include "globals.h"
#include "menu.h"
#include "game.h"
#include "room.h"
#include "elements.h"
#include "inputs.h"
#include "player.h"
#include "song.h"


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
  stateGameNextRoom,
  stateGameNextLevel,
  stateGamePause,
  stateGameOver,
  stateGameTransporting,
  stateGameFinished,
  stateGameNew,
  stateGameContinue,
};


void setup()
{
  Serial.begin(115200);
  arduboy.boot();
  arduboy.audio.begin();
  arduboy.setFrameRate(45);
  if (arduboy.audio.enabled()) ATM.play(silentSong);   // an empty song keeps the playroutine running for the intro tune (sfx.h)
  if ((EEPROM.read(OFFSET_ESCD_START) == GAME_ID) && (EEPROM.read(OFFSET_ESCD_END) == GAME_ID))
  {
    buttonSchemeOffset = EEPROM.read(OFFSET_BUTTONS);
  }
}

void loop() {
  if (!(arduboy.nextFrame())) return;
  arduboy.pollButtons();
  arduboy.clear();
  ((FunctionPointer) pgm_read_dword (&mainGameLoop[gameState]))();
  arduboy.display();
};
