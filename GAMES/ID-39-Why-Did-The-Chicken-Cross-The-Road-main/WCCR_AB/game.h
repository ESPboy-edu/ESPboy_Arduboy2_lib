#ifndef GAME_H
#define GAME_H

#include "globals.h"
#include "inputs.h"
#include "playfield.h"

// the screen scrolls forward all the time (the world slides down the screen): stand still too long and you are lost
#define DEAD_START   40       // world units the scroll line starts behind the chicken (5 rows)
#define DEAD_LAG     80       // the scroll line is never more than this far (10 rows) behind the furthest row reached
byte    scrollSub = 0;        // 1/16 px

void resetGame()
{
  worldInit();
  player.set();
  camX = player.x;
  camY = player.y;
  scrollY = player.y - DEAD_START;
  scrollSub = 0;
}

// camera: glide towards the chicken, always moving at least 1 px
static int16_t camStep(int16_t d)
{
  if (d > 0) return (d + 3) >> 2;
  return -((-d + 3) >> 2);
}

void updateCamera()
{
  if (!(player.characteristics & CH_DEAD) && player.bestRow > 0 && player.hourglass == 0)     // scrolling only starts after the first hop forward
  {
    byte spd = 2 + (player.bestRow >= 40 ? 2 : (player.bestRow >= 20 ? 1 : 0));   // 1/16 px per frame, a little faster later on
    scrollSub += spd;
    scrollY += scrollSub >> 4;
    scrollSub &= 15;
  }
  int16_t bestY = player.bestRow * 8;
  if (scrollY < bestY - DEAD_LAG) scrollY = bestY - DEAD_LAG;   // runs ahead faster than the scroll: the line only trails 10 rows (the lane ring keeps those)
  camX += camStep(player.x - camX);                             // the camera stays on the chicken
  camY += camStep(player.y - camY);
}

void stateMenuPlay()
{

}

void stateGamePlaying()
{
  if (newGame)
  {
    resetGame();
    newGame = false;
  }

  checkInputs();
  int16_t cr = camY >> 3;                                    // the camera can be ahead of the chicken now
  if (player.row() > cr) cr = player.row();
  extendWorld(cr);
  updateLanes(cr);
  player.update();
  updateCamera();
  if (!(player.characteristics & CH_DEAD) && player.row() < deadRow()) player.dieFeathers();   // touched the black: bursts into feathers

  drawPlayField();
}





void stateGamePause()
{
  drawPlayField();                       // show the frozen scene
  if (arduboy.justPressed(A_BUTTON | B_BUTTON))
  {
    gameState = STATE_GAME_PLAYING;
  }
}


void stateGameOver()
{
  drawPlayField();                       // show where it went wrong
  sprites.drawPlusMask(49, 20, gameOver_plus_mask, 0);
  if (arduboy.justPressed(A_BUTTON))     // A: back to the title screen
  {
    newGame = true;
    gameState = STATE_MENU_MAIN;
  }
  else if (arduboy.justPressed(B_BUTTON))   // B: play again
  {
    newGame = true;
    gameState = STATE_GAME_PLAYING;
  }
}

void stateGameFinished()
{

  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) 
  {
  }
}


#endif
