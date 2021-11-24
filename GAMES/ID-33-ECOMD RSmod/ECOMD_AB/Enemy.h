#ifndef ENEMY_H
#define ENEMY_H

#define FRAME_DELAY_START         3
#define HP_MAX_SMALL              2
#define HP_MAX_MID                3
#define HP_MAX_BIG                5

#define SMALL_ENEMY               2
#define MID_ENEMY                 1
#define BIG_ENEMY                 0

extern int scorePlayer;
extern unsigned char gameState;

byte enemyWalkingFrame;


class Enemy
{
  public:
    static const byte startX = 64;
    static const byte startY = 0;

    byte enemyType = SMALL_ENEMY; //set enemyType to small at start
    byte frameDelay = 3;
    byte x, y;
    byte width = 6;  // small = 6 / middle = 8 and big = 12
    byte height = 6; // small = 6 / middle = 9 and big = 12
    boolean xSpeed;

    int HP;
    unsigned char spawnDelay;

    boolean isActive = false;
    boolean isAgro = false;  //when turned on Enemy xSpeed will double


    void setToTopOfSceen() {
      x = startX;
      y = startY;
    }
    void reset() {
      spawnDelay = random(60, 120);
      switch (enemyType)
      {
        case SMALL_ENEMY:
          HP = HP_MAX_SMALL;
          width = 6;
          height = 6;
          break;
        case MID_ENEMY:
          HP = HP_MAX_MID;
          width = 8;
          height = 9;
          break;
        case BIG_ENEMY:
          HP = HP_MAX_BIG;
          width = 12;
          height = 12;
          break;

      }
      isAgro = false;
      isActive = false;
      frameDelay = FRAME_DELAY_START;
      setToTopOfSceen();
      xSpeed = random(0, 2);
    }



    void takeDamage(byte _damage) {
      HP -= _damage;
      if (HP < 1)
      {
        scorePlayer ++;
        reset();
      }
    }

    void makeAgro() {
      if (!isAgro) {
        isAgro = true;
        frameDelay --;
      }
      setToTopOfSceen();
    }

    void reverseDirection() {
      xSpeed = !xSpeed;

    }

    void Update()
    {
      if (spawnDelay > 0)
      {
        spawnDelay --;
      }
      else
      {
        x =  x + 1 - (2 * xSpeed);
        isActive = true;
      }
    }
};

Enemy enemy[8];



void drawEnemies()
{
  for (byte i = 0; i < 8; i++)
  {
    if (enemy[i].isActive) switch (enemy[i].enemyType)
      {
        case 2: //small
          sprites.drawPlusMask(enemy[i].x, enemy[i].y - 2, enemySmall_plus_mask, enemyWalkingFrame + (enemy[i].xSpeed * 4));
          break;
        case 1: //medium
          sprites.drawPlusMask(enemy[i].x, enemy[i].y - 7, enemyMiddle_plus_mask, enemyWalkingFrame + (enemy[i].xSpeed * 4));
          break;
        case 0: //big
          sprites.drawPlusMask(enemy[i].x, enemy[i].y - 4, enemyBig_plus_mask, enemyWalkingFrame + (enemy[i].xSpeed * 4));
          break;
      }
  }
}

void checkEnemyCollisions()
{
  if ((player.y > 50) && (player.x > 52) &&(player.x < 68)) gameState = STATE_GAME_OVER;
  for (byte i = 0; i < 8; i++)
  {
    if (enemy[i].y > 63) enemy[i].makeAgro();
    if (enemy[i].isActive)
    {
      //check if enemy is grounded
      Rect enemyRect;
      enemyRect.x = enemy[i].x;
      enemyRect.y = enemy[i].y + enemy[i].height;
      enemyRect.width = enemy[i].width;
      enemyRect.height = 1;

      boolean enemyGrounded = false;
      for (byte i = 0; i < 4; i++)
      {
        if (arduboy.collide(enemyRect, wall[i]))
        {
          enemyGrounded = true;
        }
      }
      if (!enemyGrounded)
      {
        if (enemy[i].y + enemy[i].height < 63)
        {
          enemy[i].y += 1;  //drop enemy y position
        }
        else
        {
          if (enemy[i].x + 3 > 56 && enemy[i].x + 3 < 56 + 16)
          {
            enemy[i].y += 1;  //drop enemy y position
          }
        }
      }

      //reset enemy hitbox
      //Rect enemyRect2;
      enemyRect.x = enemy[i].x + 1;
      enemyRect.y = enemy[i].y + 1;
      enemyRect.width = enemy[i].width - 1;
      enemyRect.height = enemy[i].height - 1;

      //CHECK ENEMY COLLISIONS WITH PLAYER
      Rect playerRect;
      playerRect.x = player.x + 2;
      playerRect.y = player.y + 2;
      playerRect.width = 4;
      playerRect.height = 4;

      if (arduboy.collide(playerRect, enemyRect)) gameState = STATE_GAME_OVER;
      

      //check bullets
      enemyRect.x = enemy[i].x;
      enemyRect.y = enemy[i].y;
      enemyRect.width = enemy[i].width;
      enemyRect.height = enemy[i].height;
      
      Rect bulletRect;
      for (byte ii = 0; ii < weapon.maxBullets; ii++)
      {
        if (weapon.bullet[ii].isActive)
        {
          bulletRect.x = weapon.bullet[ii].x;
          bulletRect.y = weapon.bullet[ii].y;
          bulletRect.width = weapon.bullet[ii].width;
          bulletRect.height = weapon.bullet[ii].height;
          if (arduboy.collide(enemyRect, bulletRect))
          {
            enemy[i].takeDamage(weapon.bullet[ii].damage);
            if (CURRENT_WEAPON != SHOTGUN)
              weapon.bullet[ii].isActive = false;
          }
        }
      }
    }
  }
}


void updateEnemies()
{
  for (byte i = 0; i < 8; i++)
  {
    if (arduboy.everyXFrames(enemy[i].frameDelay))
    {
      enemy[i].Update();
      //check outOFBounds
      if (enemy[i].x > 120 - enemy[i].width) enemy[i].reverseDirection();
      if (enemy[i].x < 8 ) enemy[i].reverseDirection();
    }
  }
  if (arduboy.everyXFrames(6))enemyWalkingFrame++;
  if (enemyWalkingFrame > 3) enemyWalkingFrame = 0;
}

#endif
