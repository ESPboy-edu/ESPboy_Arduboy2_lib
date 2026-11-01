#ifndef COLLISION_H
#define COLLISION_H

#include "globals.h"
#include "elements.h"
#include "player.h"
#include "room.h"

void isoStep(int &x, int &y, byte dir)
{
  dir &= 3;
  x += (dir == EAST || dir == SOUTH) ? 2 : -2;
  y += (dir > EAST) ? 1 : -1;
}

boolean hitBorders(int objectX, int objectY, byte directionFacing, bool playerOrEnemy)
{
  directionFacing &= 3;
  switch (directionFacing)
  {
    case NORTH:
      if (objectX + (2 * objectY) > 89 + (playerOrEnemy * (2 * currentRoomY))) return false;
      break;
    case EAST:
      if (objectX - (2 * objectY) < 15 - (playerOrEnemy * (2 * currentRoomY))) return false;
      break;
    case SOUTH:
      if (objectX + (2 * objectY) < 183 + (playerOrEnemy * (2 * currentRoomY))) return false;
      break;
    case WEST:
      if (objectX - (2 * objectY) > -81 - (playerOrEnemy * (2 * currentRoomY))) return false;
      break;
  }
  return true;
}

void playerChecksAndOpensDoor(byte direction)
{
  // Black card: the designated level door, or the closed door that leads into the exit room.
  // White cards only open ordinary closed doors.
  byte destRoom = (roomByte(currentRoom, 1 + direction) >> 2) & 0x3f;
  bool isLevel = (checkIfLevelDoor() == direction) || (destRoom == exitRoomLocation);
  byte need = isLevel ? 0b00100000 : 0b00001000;
  if (player.assets & (isLevel ? 0b00100000 : 0b00011000))
  {
    player.assets -= need;
    scorePlayer += isLevel ? SCORE_LEVEL_DOOR : SCORE_OPEN_DOOR;
    play_SFX(SFX_DOOROPEN);
    bitClear(stageRoom[currentRoom].doorsClosedActive, direction);
  }
  else
  {
    play_SFX(SFX_DOOR);
    loadAndFillMessage(isLevel ? 9 : 8);
    setTicker = TEXT_BLINK_SCROLL_RIGHT;
    showTicker = TRUE;
  }
}

void setPlayerWalkingThroughDoor()
{
  byte dir = player.characteristics & 0b00000011;
  byte tile = pgm_read_byte(&doorFrontTile[dir]);
  if ((player.isOnTile == tile) &&
      (bitRead(stageRoom[currentRoom].doorsClosedActive, dir + UPPERBIT_OFFSET)) &&
      (!bitRead(stageRoom[currentRoom].doorsClosedActive, dir)))
  {
    bitSet(player.characteristics, DROID_GOES_THROUGH_DOOR_AT_BIT_5);
    player.x = translateTileToX(tile);
    player.y = translateTileToY(tile) + currentRoomY;
  }
}

boolean checkborderHit(int objectX, int objectY, byte directionFacing)
{
  if (!hitBorders(objectX, objectY, directionFacing, PLAYER)) return false;
  else setPlayerWalkingThroughDoor();
  return true;
}

byte floorKind(byte floorSlot)
{
  return elements[floorSlot].characteristics & 0b00000111;
}

byte tileIsOccupied(byte tileTesting, bool playerOrEnemy, bool enemyTwo)
{
  if (tileTesting < 25)
  {
    currentlyOnTestingTile = itemsOrder[tileTesting + ITEMS_ORDER_TILES_START];
    if (currentlyOnTestingTile == EMPTY_PLACE) return false;
    // LEVEL UP is only a floor decal — the droid must be able to stand on it
    if (currentlyOnTestingTile >= FLOOR_ONE && currentlyOnTestingTile <= FLOOR_FIVE
        && floorKind(currentlyOnTestingTile) == FLOOR_LEVEL_UP)
      return false;
    if (playerOrEnemy)
    {
      if (currentlyOnTestingTile == PLAYER_DROID) return false;
    }
    else
    {
      if (!enemyTwo && currentlyOnTestingTile == ENEMY_ONE) return false;
      else if (enemyTwo && currentlyOnTestingTile == ENEMY_TWO) return false;
      else if (currentlyOnTestingTile == PLAYER_DROID) return PLAYER_DROID;
    }
    return true;
  }
  else return false;
}

bool kickPlayer(byte dir);

boolean hitObjects (int objectX, int objectY, byte directionFacing, bool playerOrEnemy, bool enemy)
{
  directionFacing &= 3;
  switch (directionFacing)
  {
    case NORTH:
      testingTile = tileFromXY(objectX - 8, objectY - 4);
      break;
    case EAST:
      testingTile = tileFromXY(objectX + 8, objectY - 4);
      break;
    case SOUTH:
      testingTile = tileFromXY(objectX + 6, objectY + 3);
      break;
    case WEST:
      testingTile = tileFromXY(objectX - 6, objectY + 3);
      break;
  }
  byte test = tileIsOccupied(testingTile, playerOrEnemy, enemy);
  if (test > 0)
  {
    if (test == PLAYER_DROID)
    {
      // enemy is the slot (0/1). A successful shove frees the tile so the boxer can step.
      if ((elements[enemy].characteristics & 7) == ENEMY_BOX)
        return !kickPlayer(directionFacing);
      playerLosesLife();
    }
    return true;
  }
  else return false;
}

void collectBattery()
{
  play_SFX(SFX_PICKUP);
  songSpeedChange();
  if (player.life < 3) { player.life++; scorePlayer += SCORE_LIFE; }
  else scorePlayer += SCORE_TO_MUCH_LIFE;
}

void clearElement()
{
  bitClear(stageRoom[currentRoom].elementsActive, 5);
}

void checkObjectTypeAndAct()
{
  switch ((elements[2].characteristics & 0b00000111))
  {
    case PICKUP_BLACK_CARD:
      if (bitRead(player.assets,5) == 0)
      {
        play_SFX(SFX_PICKUP);
        bitSet(player.assets,5);
        clearElement();
        scorePlayer += SCORE_BLACK_CARD;
      }
      break;
    case PICKUP_WHITE_CARD:
      if ((player.assets & 0b00011000) < 0b00011000)
      {
        play_SFX(SFX_PICKUP);
        player.assets += 0b00001000;
        clearElement();
        scorePlayer += SCORE_WHITE_CARD;
      }
      break;
    case PICKUP_BATTERY:
      collectBattery();
      clearElement();
      break;
    case PICKUP_BULLET:
      if ((player.assets & 0b00000111) < 0b00000111)
      {
        play_SFX(SFX_PICKUP);   // only when really picked up (full ammo: no sound, no score)
        player.assets++;
        objectHiddenThisVisit = true;   // gone until you leave the room
        scorePlayer += SCORE_BULLET;
      }
      break;
    case PICKUP_CHIP:
      play_SFX(SFX_PICKUP);
      clearElement();
      scorePlayer += SCORE_CHIP;
      break;
  }
}

// N,E,S,W tile step. Shared by box push, B-action neighbor, boxer kick.
PROGMEM const int8_t dirTileOffset[] = { -5, -1, 5, 1 };

int8_t tileInDir(byte src, byte dir)
{
  dir &= 3;
  if (src >= 25) return -1;
  byte col = src % 5;
  if ((dir == NORTH && src < 5) ||
      (dir == SOUTH && src >= 20) ||
      (dir == EAST  && col == 0) ||
      (dir == WEST  && col == 4))
    return -1;
  return (int8_t)src + (int8_t)pgm_read_byte(&dirTileOffset[dir]);
}

boolean tryPushBox(byte slot, byte dir)
{
  if (floorKind(slot) != FLOOR_BOX) return false;
  if (!checkIfOnCenterTile(elements[slot].x, elements[slot].y)) return false;
  int8_t dest = tileInDir(tileFromXY(elements[slot].x, elements[slot].y), dir);
  if (dest < 0) return false;
  if (itemsOrder[dest + ITEMS_ORDER_TILES_START] != EMPTY_PLACE) return false;
  elements[slot].x = translateTileToX(dest);
  elements[slot].y = translateTileToY(dest);
  play_SFX(SFX_KICK);
  return true;
}

// Boxer shove: one tile along facing. Hurt only on spikes or another enemy.
bool kickPlayer(byte dir)
{
  int8_t dest = tileInDir(player.isOnTile, dir);
  if (dest < 0) return false;
  byte occ = itemsOrder[dest + ITEMS_ORDER_TILES_START];
  bool foe = (occ <= ENEMY_TWO);
  bool spike = false;
  if (occ >= FLOOR_ONE && occ <= FLOOR_FIVE)
  {
    byte kind = floorKind(occ);
    if (kind == FLOOR_SPIKE) spike = true;
    else if (kind != FLOOR_LEVEL_UP && kind != FLOOR_PIT) return false;
  }
  else if (occ != EMPTY_PLACE && !foe) return false;

  player.isOnTile = dest;
  for (byte i = 0; i < 6; i++)   // the screen scrolls like it does when walking: 6 steps make one tile
  {
    if (dir > EAST) { if (currentRoomY > MAX_ROOM_Y_SCROLLING) currentRoomY--; }
    else if (currentRoomY < 0) currentRoomY++;
  }
  player.x = translateTileToX(dest);
  player.y = translateTileToY(dest) + currentRoomY;
  play_SFX(SFX_KICK);          // played first so the hurt sound takes over if the shove lands on a foe or spikes
  if (foe || spike) playerLosesLife();
  return true;
}

void decideOnCollision()
{
  switch (currentlyOnTestingTile)
  {
    case ENEMY_ONE:
    case ENEMY_TWO:
      if ((elements[currentlyOnTestingTile].characteristics & 7) == ENEMY_BATTERY)
      {
        collectBattery();
        bitClear(stageRoom[currentRoom].elementsActive, 7 - currentlyOnTestingTile);
        elements[currentlyOnTestingTile].characteristics = 0;
        dropRoom = 0xFF;
      }
      else if ((elements[currentlyOnTestingTile].characteristics & 7) == ENEMY_BOX)
      {
        byte dir = (elements[currentlyOnTestingTile].characteristics >> 3) & 3;
        byte et = tileFromXY(elements[currentlyOnTestingTile].x, elements[currentlyOnTestingTile].y);
        if (tileInDir(et, dir) == player.isOnTile) kickPlayer(dir);
      }
      else
        playerLosesLife();
      break;
    case OBJECT:
      checkObjectTypeAndAct();
      break;
    case FLOOR_ONE:
    case FLOOR_TWO:
    case FLOOR_THREE:
    case FLOOR_FOUR:
    case FLOOR_FIVE:
      {
        byte kind = floorKind(currentlyOnTestingTile);
        if (kind == FLOOR_BOX)
        {
          // walking into a box just stops you; kick it with B
        }
        else if (kind == FLOOR_SPIKE)
          playerLosesLife();
        // pits no longer hurt the droid
      }
      break;
  }
}

void killEnemy(byte enemySlot)
{
  scorePlayer += SCORE_ENEMY_HIT;
  play_SFX(SFX_KILL);
  if ((elements[enemySlot].characteristics & 7) == ENEMY_SHOOTER)
  {
    byte tile = tileFromXY(elements[enemySlot].x, elements[enemySlot].y);
    elements[enemySlot].characteristics = (tile << 3) | ENEMY_BATTERY;
    dropRoom = currentRoom;
    dropInfo = tile | (enemySlot << 5);
    return;
  }
  bitClear(stageRoom[currentRoom].elementsActive, 7 - enemySlot);
  elements[enemySlot].characteristics = 0;
}

byte tileOccupant(int ox, int oy)
{
  byte t = tileFromXY(ox, oy);
  if (t >= 25) return EMPTY_PLACE;
  return itemsOrder[t + ITEMS_ORDER_TILES_START];
}

bool shotHitsBlockingFloor(byte occupant)
{
  if (occupant < FLOOR_ONE || occupant > FLOOR_FIVE) return false;
  byte kind = floorKind(occupant);
  if (kind == FLOOR_PIT) return false;
  if (kind == FLOOR_PIRAMIDE)
  {
    play_SFX(SFX_PIRAMIDE);
    bitClear(stageRoom[currentRoom].elementsActive, 7 - occupant);
    elements[occupant].characteristics = 0;
    return true;
  }
  return (kind == FLOOR_BOX || kind == FLOOR_SPIKE);
}

// itemsOrder is a z-buffer. The droid is written last, so it overwrites an
// enemy that shares the tile. Shots must test live positions, not that slot.
byte shotHitsEnemyAt(int sx, int sy)
{
  byte shotTile = tileFromXY(sx, sy);
  if (shotTile >= 25) return 255;
  for (byte i = 0; i < 2; i++)
  {
    if (!bitRead(stageRoom[currentRoom].elementsActive, 7 - i)) continue;
    if ((elements[i].characteristics & 7) == ENEMY_BATTERY) continue;
    if (tileFromXY(elements[i].x, elements[i].y) == shotTile) return i;
  }
  return 255;
}

bool shotHitsPlayerAt(int sx, int sy)
{
  byte shotTile = tileFromXY(sx, sy);
  if (shotTile >= 25) return false;
  return (tileFromXY(player.x, player.y - currentRoomY) == shotTile);
}

bool resolveShotOnTile(int sx, int sy, byte dir, bool fromPlayer)
{
  if (hitBorders(sx, sy, dir, ENEMY))
    return true;

  if (fromPlayer)
  {
    byte hit = shotHitsEnemyAt(sx, sy);
    if (hit < 2)
    {
      killEnemy(hit);
      return true;
    }
  }
  else if (shotHitsPlayerAt(sx, sy))
  {
    playerLosesLife();
    return true;
  }

  byte occupant = tileOccupant(sx, sy);
  if (occupant == PLAYER_DROID || occupant == ENEMY_ONE || occupant == ENEMY_TWO)
    occupant = EMPTY_PLACE;
  if (shotHitsBlockingFloor(occupant) || occupant == OBJECT)
    return true;

  return false;
}

void spawnEnemyShot(byte enemySlot)
{
  if (enemyBulletActive) return;
  play_SFX(SFX_SHOOT);
  enemyBulletActive = true;
  elements[ENEMY_BULLET].x = elements[enemySlot].x;
  elements[ENEMY_BULLET].y = elements[enemySlot].y;
  elements[ENEMY_BULLET].characteristics = elements[enemySlot].characteristics & 0b00011000;
  elements[ENEMY_BULLET].frame = 0;
}

void updateShot(bool fromPlayer)
{
  if (!(fromPlayer ? playerShot.active : enemyBulletActive)) return;
  if (!arduboy.everyXFrames(2)) return;

  int sx, sy;
  byte dir, steps;
  if (fromPlayer)
  {
    sx = playerShot.x;
    sy = playerShot.y;
    dir = playerShot.dir;
    steps = playerShot.steps;
  }
  else
  {
    sx = elements[ENEMY_BULLET].x;
    sy = elements[ENEMY_BULLET].y;
    dir = (elements[ENEMY_BULLET].characteristics & 0b00011000) >> 3;
    steps = elements[ENEMY_BULLET].frame;
  }

  isoStep(sx, sy, dir);
  steps++;

  bool done = false;
  if (fromPlayer)
  {
    byte hit = shotHitsEnemyAt(sx, sy);
    if (hit < 2)
    {
      killEnemy(hit);
      done = true;
    }
  }
  else if (shotHitsPlayerAt(sx, sy))
  {
    playerLosesLife();
    done = true;
  }

  if (!done && steps >= SHOT_STEPS_PER_TILE)
  {
    steps = 0;
    if (resolveShotOnTile(sx, sy, dir, fromPlayer)) done = true;
  }

  if (fromPlayer)
  {
    playerShot.x = sx;
    playerShot.y = sy;
    playerShot.steps = steps;
    if (done) playerShot.active = false;
  }
  else
  {
    elements[ENEMY_BULLET].x = sx;
    elements[ENEMY_BULLET].y = sy;
    elements[ENEMY_BULLET].frame = steps;
    if (done) enemyBulletActive = false;
  }
}

void updatePlayerShot()
{
  updateShot(true);
}

void updateEnemyShot()
{
  updateShot(false);
}

#endif
