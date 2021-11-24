#ifndef PLAYER_H
#define PLAYER_H

//define player facing states (in game)
#define PLAYER_FACING_RIGHT       0
#define PLAYER_FACING_LEFT        1
#define PLAYER_FACING_FRONT       2

#define STAGE_LEFT                      0
#define STAGE_RIGHT                     1
#define STAGE_TOP                       2
#define STAGE_BOTTOM                    3

extern byte walkingFrame;
extern byte waitingFrame;

byte playerFacing = PLAYER_FACING_RIGHT;
boolean isGrounded = false;
const byte gravity = 1;
int playerYVelocity = 0;

PROGMEM const byte playerWalks[] = { 1, 2, 3, 2, 4, 5, 6, 5,};
PROGMEM const byte playerWaiting[] = {0, 7, 0, 7, 0, 0, 0, 0, 0, 8, 9, 8, 0, 0, 0, 0, 1, 1, 1, 1, 0, 4, 4, 4, 4, 0, 0, 0, 7, 0, 7, 0, 0, 0, 0, 0};


class GameObject {
  public:
    byte x, y;
    int8_t xSpeed, ySpeed;
};

GameObject player;



boolean checkCollisions()
{
  playerRect.x = player.x;
  playerRect.y = player.y - 1 ;
  playerRect.width = 8;
  playerRect.height = 8;
  for (byte i = 0; i < 4; i++)
  {
    if (arduboy.collide(playerRect, wall[i])) return true;
  }
  return false;
}


boolean checkIfGrounded()
{
  playerRect.x = player.x + 1;
  playerRect.y = player.y + 1 ;
  playerRect.width = 6;
  playerRect.height = 8;
  for (byte i = 0; i < 4; i++)
  {
    if (arduboy.collide(playerRect, wall[i]))
    {
      isGrounded = true;
      return true;
    }
    if (player.y + 8 > 62)
    {
      isGrounded = true;
      player.y = 63 - 8;
      return true;
    }
  }
  return false;
}


void gravityEffect()
{
  playerYVelocity -= gravity;
  if (playerYVelocity < -2) playerYVelocity = -2;


  if (playerYVelocity > 0) //if jumping up
    for (int i = 0; i < playerYVelocity; i++) {
      if (!checkCollisions() && (player.y > 2)) player.y --;
    }



  if (playerYVelocity < 0) //if Falling
    for (int i = 0; i > playerYVelocity; i--) {
      if (!checkIfGrounded())player.y++;
    }
}


void drawPlayer()
{
  if (player.x < 8) player.x = 8;
  if (player.x > 112) player.x = 112;
  switch (playerFacing)
  {
    case PLAYER_FACING_FRONT:
      sprites.drawPlusMask(player.x, player.y, player_plus_mask, pgm_read_byte(&playerWaiting[waitingFrame]));
      break;
    default:
      sprites.drawPlusMask(player.x, player.y, player_plus_mask, pgm_read_byte(&playerWalks[walkingFrame + (4 * playerFacing)]));
      break;
  }
}


#endif
