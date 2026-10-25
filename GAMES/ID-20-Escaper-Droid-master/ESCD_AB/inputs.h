#ifndef INPUTS_H
#define INPUTS_H

#include "globals.h"
#include "player.h"
#include "collision.h"

#define BUTTON_SCHEME_A  0
#define BUTTON_SCHEME_B  4

PROGMEM const unsigned char buttonScheme[] = {
  UP_BUTTON, RIGHT_BUTTON, DOWN_BUTTON, LEFT_BUTTON,
  LEFT_BUTTON, UP_BUTTON, RIGHT_BUTTON, DOWN_BUTTON
};

void checkInputs()
{
  if (arduboy.everyXFrames(2))
  {
    byte testingTile = tileFromXY(player.x, player.y - currentRoomY);
    if (testingTile < 25) player.isOnTile = testingTile;

    byte dir = 255;

    if      (arduboy.pressed(pgm_read_byte(&buttonScheme[NORTH + buttonSchemeOffset]))) dir = NORTH;
    else if (arduboy.pressed(pgm_read_byte(&buttonScheme[EAST  + buttonSchemeOffset]))) dir = EAST;
    else if (arduboy.pressed(pgm_read_byte(&buttonScheme[SOUTH + buttonSchemeOffset]))) dir = SOUTH;
    else if (arduboy.pressed(pgm_read_byte(&buttonScheme[WEST  + buttonSchemeOffset]))) dir = WEST;

    if (dir != 255)
    {
      player.characteristics = (player.characteristics & 0b11111100) | dir;

      if (!checkborderHit(player.x, player.y, dir))
      {
        if (hitObjects(player.x, player.y - currentRoomY, dir, PLAYER, 0))
          decideOnCollision();
        else
          walkThroughDoor();
      }
    }
  }

  if (arduboy.justPressed(A_BUTTON))
  {
    statePrepForPause();
    ATM.pause();
    gameState = STATE_GAME_PAUSE;
  }

  if (arduboy.justPressed(B_BUTTON))
  {
    byte dir = player.characteristics & 0b00000011;
    byte objType = elements[OBJECT].characteristics & 0b00000111;
    byte objTile = (elements[OBJECT].characteristics & 0b11111000) >> 3;
    int8_t neededTile = player.isOnTile + (int8_t)pgm_read_byte(&dirTileOffset[dir]);

    // 1. move block
    byte occ = 255;
    if (neededTile >= 0 && neededTile < 25)
      occ = itemsOrder[neededTile + ITEMS_ORDER_TILES_START];
    if (occ >= FLOOR_ONE && occ <= FLOOR_FIVE && floorKind(occ) == FLOOR_BOX)
    {
      tryPushBox(occ, dir);
      return;
    }

    if (objTile == neededTile)
    {
      // 2. transport
      if (objType == TELEPORT)
      {
        bitSet(player.characteristics, DROID_TRANSPORTING_AT_BIT_7);
        gameState = STATE_GAME_TRANSPORTING;
        play_SFX(SFX_BEAMMEUPSCOTTY);
        return;
      }
      // 3. switch
      if (objType > 5)
      {
        bitToggle(elements[OBJECT].characteristics, 0);
        bitToggle(stageRoom[currentRoom].roomNumberInfluencing, 7);
        byte targetRoom = stageRoom[currentRoom].roomNumberInfluencing & 0b00111111;
        byte mask       = stageRoom[currentRoom].elementsInfluenced;
        stageRoom[targetRoom].elementsActive ^= mask;
        play_SFX(SFX_SWITCH);
        return;
      }
    }

    // 4. open door
    if (player.isOnTile == pgm_read_byte(&doorFrontTile[dir]))
    {
      playerChecksAndOpensDoor(dir);
      return;
    }

    // 5. next level (the shot below is the fallback, so this has to come before it)
    bool onExitDecal = false;
    for (byte f = FLOOR_ONE; f <= FLOOR_FIVE; f++)
    {
      if (floorKind(f) == FLOOR_LEVEL_UP
          && ((elements[f].characteristics >> 3) == player.isOnTile))
        onExitDecal = true;
    }
    if ((currentRoom == exitRoomLocation && player.isOnTile == TILE_IN_MIDDLE) || onExitDecal)
    {
      ATM.pause();
      ATM.playSfx(levelUp,0);
      gameState = STATE_GAME_NEXT_LEVEL;
      return;
    }

    // 6. shoot
    spawnPlayerShot();
  }
}

void enemySetDir(byte enemy, byte dir)
{
  elements[enemy].characteristics = (elements[enemy].characteristics & 0b11100111) | ((dir & 3) << 3);
}

bool enemyCanMove(byte enemy, byte dir)
{
  return !hitBorders(elements[enemy].x, elements[enemy].y, dir, ENEMY) &&
         !hitObjects(elements[enemy].x, elements[enemy].y, dir, ENEMY, enemy);
}

byte directionTowardPlayer(byte enemy)
{
  byte et = tileFromXY(elements[enemy].x, elements[enemy].y);
  if (et >= 25) return (elements[enemy].characteristics & 0b00011000) >> 3;
  int8_t dRow = (int8_t)(player.isOnTile / 5) - (int8_t)(et / 5);
  int8_t dCol = (int8_t)(player.isOnTile % 5) - (int8_t)(et % 5);
  if (abs(dRow) >= abs(dCol))
    return (dRow < 0) ? NORTH : SOUTH;
  return (dCol < 0) ? EAST : WEST;
}

void updateEnemies()
{
  if (!arduboy.everyXFrames(6)) return;

  for (byte i = 0; i < 2; i++)
  {
    if (!bitRead(stageRoom[currentRoom].elementsActive, 7 - i))
      continue;

    byte dir = (elements[i].characteristics & 0b00011000) >> 3;
    byte type = elements[i].characteristics & 0b00000111;
    if (type > ENEMY_SHOOTER) continue;

    if (enemyCanMove(i, dir))
    {
      isoStep(elements[i].x, elements[i].y, dir);
      if (checkIfOnCenterTile(elements[i].x, elements[i].y))
      {
        byte want = dir;
        if (type == ENEMY_FLYER) want = (dir - 1) & 3;
        else if (type == ENEMY_MOVER) want = directionTowardPlayer(i);
        if (want != dir && enemyCanMove(i, want)) enemySetDir(i, want);
      }
    }
    else
    {
      enemySetDir(i, (dir + ((type == ENEMY_MOVER) ? 2 : 1)) & 3);
    }

    if (type == ENEMY_SHOOTER && arduboy.everyXFrames(48)) spawnEnemyShot(i);
  }
}

#endif
