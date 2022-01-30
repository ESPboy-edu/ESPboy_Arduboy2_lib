/*----------------------------------
               NumberPuzzle


    Created by:takaiwa@gmail.com
    Game Copyright (C) 2017 takaiwa@gmail.com

  ----------------------------------*/

#include "globals.h"

#include "utility.h"

#include "map.h"

#include "boxes.h"
#include "player.h"

#include "init.h"
#include "menu.h"


/*-------------------------
    setup
  ------------------------ */
void setup() {
  arduboy.begin();

// for screen shot..
// Serial.begin(9600);

  displayTitle();

  lTime = millis();
  lTimeNext = millis();

  arduboy.setFrameRate(40);
  arduboy.initRandomSeed();
}


/*-------------------------
    loop
  ------------------------ */
void loop() {

  if (pWait) {
    return;
  }

  if (!(arduboy.nextFrame())){
    return;
  }

  lTime = millis();

  arduboy.clear();

  drawHeader();

  moveBox();

  movePlayer();
  drawPlayer();

  drawBox();


  displayStageClear();
  displayGameOver();

  arduboy.display();

// for screen shot..
// Serial.write(arduboy.getBuffer(), 128 * 64 / 8);
}
