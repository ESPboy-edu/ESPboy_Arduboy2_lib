#include "intro.h"
#include "load.h"
#include "game.h"
#include "solved.h"
#include "over.h"
#include "memory.h"
#include "settings.h"

typedef void (*FunctionPointer)();

const FunctionPointer PROGMEM gameStates[] = {
  gameIntro,
  loadLevel,
  gamePlay,
  levelSolved,
  gameOver,
  settings
};

void setup()
{
  //Serial.begin(9600);  // For debugging and screenshotting
  arduboy.begin();
  
//  arduboy.boot(); // raw hardware
//  arduboy.display(); // blank the display (sBuffer is global, so cleared automatically)
//  arduboy.flashlight(); // light the RGB LED and screen if UP button is being held.
  // check for and handle buttons held during start up for system control
//  arduboy.systemButtons();
//  arduboy.audio.begin();
//  arduboy.waitNoButtons(); // wait for all buttons to be released
  arduboy.initRandomSeed();
  arduboy.setFrameRate(60);
  initEEPROM();
  level = getLevel();
}

void loop()
{
  if(!arduboy.nextFrame())
    return;

  arduboy.pollButtons();
  arduboy.clear();
  ((FunctionPointer)pgm_read_ptr(&gameStates[gameState]))();

#if !DEBUG
  // For screenshots
  //Serial.write(arduboy.getBuffer(), 128 * 64 / 8);
#endif
  arduboy.display();

  frame++;
}
