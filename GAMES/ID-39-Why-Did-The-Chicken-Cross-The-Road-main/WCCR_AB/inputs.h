#ifndef INPUTS_H
#define INPUTS_H

#include "globals.h"
#include "player.h"


void checkInputs()
{
  // a hop pressed while the chicken is still in the air is buffered (see queueHop)
  if (arduboy.justPressed(UP_BUTTON))    player.queueHop(HOP_UP);
  if (arduboy.justPressed(LEFT_BUTTON))  player.queueHop(HOP_LEFT);
  if (arduboy.justPressed(DOWN_BUTTON))  player.queueHop(HOP_DOWN);
  if (arduboy.justPressed(RIGHT_BUTTON)) player.queueHop(HOP_RIGHT);

  if (arduboy.justPressed(A_BUTTON | B_BUTTON) && !(player.characteristics & CH_DEAD))   // no pause once the chicken is dead
  {
    gameState = STATE_GAME_PAUSE;
  }
}


#endif
