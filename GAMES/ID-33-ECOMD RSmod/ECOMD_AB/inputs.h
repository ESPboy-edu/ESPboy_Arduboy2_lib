#ifndef INPUTS_H
#define INPUTS_H

#define STAGE_LEFT                      0
#define STAGE_RIGHT                     1
#define STAGE_TOP                       2
#define STAGE_BOTTOM                    3

extern unsigned char gameState;
extern boolean isGrounded;
extern int playerYVelocity;

byte waitingTimer;

void checkInputs()
{
  if (arduboy.justPressed(UP_BUTTON)) gameState = STATE_GAME_PAUSE;

  if (arduboy.pressed(RIGHT_BUTTON))
  {
    playerFacing = PLAYER_FACING_RIGHT;
    waitingTimer = 0;
    waitingFrame = 0;
    if (arduboy.everyXFrames(6))
    {
      walkingFrame++;
      if (walkingFrame > 3) walkingFrame = 0;
    }
    for (byte i  = 0; i < player.xSpeed; i++)
    {
      if (!checkCollisions()) player.x ++;
    }
  }

  if (arduboy.pressed(LEFT_BUTTON))
  {
    playerFacing = PLAYER_FACING_LEFT;
    waitingTimer = 0;
    waitingFrame = 0;
    if (arduboy.everyXFrames(6))
    {
      walkingFrame++;
      if (walkingFrame > 3) walkingFrame = 0;
    }
    for (byte i  = 0; i < player.xSpeed; i++)
    {
      if (!checkCollisions()) player.x --;
    }
  }

  if (arduboy.notPressed(LEFT_BUTTON && RIGHT_BUTTON))
  {
    if (arduboy.everyXFrames(60)) waitingTimer++;
    if (waitingTimer > 5)
    {
      playerFacing = PLAYER_FACING_FRONT;
      waitingTimer = 6;
      if (arduboy.everyXFrames(15))
      {
        waitingFrame++;
        if (waitingFrame > 35) waitingFrame = 0;
      }
    }
  }

  if (arduboy.pressed(A_BUTTON))
  {
    waitingTimer = 0;             //resetwaiting timer becauause you are shooting
    shoot();                      //BANG! BANG!
  }

  if (arduboy.justPressed(B_BUTTON) && isGrounded)
  {
    if (player.y > 4) player.y --;
    playerYVelocity = 7;
    isGrounded = false;
    sound.tone(60, 100);
  }
}

#endif
