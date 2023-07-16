#include <Arduboy2.h>
Arduboy2 arduboy;
#include <ArduboyTones.h>
ArduboyTones sound(arduboy.audio.enabled);
//#include <EEPROM.h>

int state = 0;

#include "bitmaps.h"
#include "Sound.h"
#include "Game.h"
#include "TitleScreen.h"
#include "TitleGame.h"

void setup() {
  randomSeed(analogRead(0));
  arduboy.begin();
  arduboy.setFrameRate(30);
}

void loop() {
  if (!(arduboy.nextFrame())) return;
  arduboy.clear();
  arduboy.pollButtons();
  
  if (state == 0)  { TitleScreen(); }
  else if (state == 1)  { TitleGame(); }
  else if (state == 2)  { Game(); }

  arduboy.display();
}
