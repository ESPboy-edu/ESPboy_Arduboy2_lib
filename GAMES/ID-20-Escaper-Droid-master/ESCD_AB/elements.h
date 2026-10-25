#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "globals.h"

#define ENEMY_ONE                 0
#define ENEMY_TWO                 1
#define OBJECT                    2
#define FLOOR_ONE                 3
#define FLOOR_TWO                 4
#define FLOOR_THREE               5
#define FLOOR_FOUR                6
#define FLOOR_FIVE                7
#define ENEMY_BULLET              8

#define ENEMY                     0

#define ENEMY_BOX                 0   // enemies_plus_mask frames 0-3  (N,E,S,W)
#define ENEMY_FLYER               1   // enemies_plus_mask frames 4-7  (N,E,S,W)
#define ENEMY_MOVER               2   // enemies_plus_mask frames 8-11 (N,E,S,W)
#define ENEMY_SHOOTER             3   // enemies_plus_mask frames 12-15(N,E,S,W)
#define ENEMY_BATTERY             4   // dead shooter; object battery frames

#define PICKUP_BLACK_CARD         0
#define PICKUP_WHITE_CARD         1
#define PICKUP_BATTERY            2
#define PICKUP_BULLET             3
#define PICKUP_CHIP               4
#define TELEPORT                  5
#define SWITCH_OFF                6
#define SWITCH_ON                 7

#define FLOOR_NORMAL              0
#define FLOOR_BOX                 1     // pushable by player - bullet can NOT pass
#define FLOOR_SPIKE               2
#define FLOOR_PIRAMIDE            3
#define FLOOR_PIT                 4
#define FLOOR_LEVEL_UP            7   // exit tile; walkable decal, frames 5-7 of floorTile

#define SCORE_BLACK_CARD          100
#define SCORE_WHITE_CARD          10
#define SCORE_LIFE                20
#define SCORE_TO_MUCH_LIFE        50
#define SCORE_BULLET              30
#define SCORE_CHIP                200
#define SCORE_OPEN_DOOR           50
#define SCORE_LEVEL_DOOR          1000

struct Element
{
  public:
    int x, y;
    byte characteristics = 0b00000000;
    byte frame = 0;
};

Element elements[9];
bool enemyBulletActive = false;

byte dropRoom = 0xFF;   // 0xFF = no unpicked shooter battery
byte dropInfo;          // bits 0-4 tile, bit 5 slot

void drawShot(int sx, int sy);

void drawEnemies(bool i)
{
  byte ch = elements[i].characteristics;
  if ((ch & 7) == ENEMY_BATTERY)
  {
    if (arduboy.everyXFrames(8) && ++elements[i].frame > 5) elements[i].frame = 0;
    sprites.drawPlusMask(elements[i].x + 4, elements[i].y + currentRoomY + 6, elements_plus_mask,
                         12 + elements[i].frame);
  }
  else
    drawFlip(elements[i].x, elements[i].y + currentRoomY, enemies_plus_mask,
                         ((ch & 7) << 2) | ((ch & 0b00011000) >> 3));
}

void drawObject()
{
  if (objectHiddenThisVisit) return;
  if (arduboy.everyXFrames(8)) elements[OBJECT].frame = (++elements[OBJECT].frame) % 6;
  byte type = elements[OBJECT].characteristics & 0b00000111;
  byte fr = elements[OBJECT].frame;
  // types 0-5 have 6 frames each; the two switches only have 2 frames each (frames 36-39), held for 3 steps
  fr = (type >= SWITCH_OFF) ? (6 * SWITCH_OFF + 2 * (type - SWITCH_OFF) + fr / 3) : (fr + 6 * type);
  sprites.drawPlusMask(elements[OBJECT].x + 4, elements[OBJECT].y + currentRoomY + 6, elements_plus_mask, fr);
}

void drawFloor(byte floor)
{
  if (elements[floor].characteristics > 0b00000000)
  {
    byte fr = elements[floor].characteristics & 0b00000111;
    if (fr == FLOOR_LEVEL_UP) fr = 5 + ((arduboy.frameCount >> 3) % 3);
    sprites.drawPlusMask(elements[floor].x - 3, elements[floor].y + currentRoomY + 9, floorTile_plus_mask, fr);
  }
}

void drawBulletEnemy()
{
  if (enemyBulletActive) drawShot(elements[ENEMY_BULLET].x, elements[ENEMY_BULLET].y);
}

#endif
