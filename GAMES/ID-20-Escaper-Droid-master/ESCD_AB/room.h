#ifndef ROOM_H
#define ROOM_H

#include "globals.h"
#include "levels.h"
#include "player.h"
#include "elements.h"
#include "font.h"

#define UPPERBIT_OFFSET               4
#define LEVEL_OFFSET                  1 

#define TILE_INFRONT_DOOR_NORTH       2
#define TILE_INFRONT_DOOR_EAST        10
#define TILE_INFRONT_DOOR_SOUTH       22
#define TILE_INFRONT_DOOR_WEST        14
#define TILE_IN_MIDDLE                12

#define NORTH_DOOR_EXISTS             NORTH + UPPERBIT_OFFSET
#define NORTH_DOOR_IS_CLOSSED         NORTH
#define NORTH_LINTEL                  11
#define NORTH_BIG_POST                12
#define NORTH_SMALL_POST              13
#define NORTH_DOOR_CLOSSED            14

#define EAST_DOOR_EXISTS              EAST + UPPERBIT_OFFSET
#define EAST_DOOR_IS_CLOSSED          EAST
#define EAST_LINTEL                   15
#define EAST_BIG_POST                 16
#define EAST_SMALL_POST               17
#define EAST_DOOR_CLOSSED             18

#define SOUTH_DOOR_EXISTS             SOUTH + UPPERBIT_OFFSET
#define SOUTH_DOOR_IS_CLOSSED         SOUTH
#define SOUTH_LINTEL                  19
#define SOUTH_BIG_POST                20
#define SOUTH_SMALL_POST              21
#define SOUTH_DOOR_CLOSSED            22

#define WEST_DOOR_EXISTS              WEST + UPPERBIT_OFFSET
#define WEST_DOOR_IS_CLOSSED          WEST
#define WEST_LINTEL                   23
#define WEST_BIG_POST                 24
#define WEST_SMALL_POST               25
#define WEST_DOOR_CLOSSED             26

#define EMPTY_PLACE                   27

PROGMEM const byte doorFrontTile[] = {
  TILE_INFRONT_DOOR_NORTH,
  TILE_INFRONT_DOOR_EAST,
  TILE_INFRONT_DOOR_SOUTH,
  TILE_INFRONT_DOOR_WEST
};

byte levelUpAnimation = 0;

struct Room {
  public:
    byte doorsClosedActive;
    byte elementsActive;
    byte roomToTransportTo;
    byte roomNumberInfluencing;
    byte elementsInfluenced;

    // doorsClosedActive — this byte holds all the 4 doors characteristics for each room
    //                    ||||||||
    //                    |||||||└->  0  DOOR NORTH  IS CLOSED (0 = false / 1 = true)
    //                    ||||||└-->  1  DOOR EAST   IS CLOSED (0 = false / 1 = true)
    //                    |||||└--->  2  DOOR SOUTH  IS CLOSED (0 = false / 1 = true)
    //                    ||||└---->  3  DOOR WEST   IS CLOSED (0 = false / 1 = true)
    //                    |||└----->  4  DOOR NORTH  EXISTS    (0 = false / 1 = true)
    //                    ||└------>  5  DOOR EAST   EXISTS    (0 = false / 1 = true)
    //                    |└------->  6  DOOR SOUTH  EXISTS    (0 = false / 1 = true)
    //                    └-------->  7  DOOR WEST   EXISTS    (0 = false / 1 = true)
    //
    // elementsActive
    //                 ||||||||
    //                 |||||||└->  7 => 0 FLOOR  5 EXISTS (0 = false / 1 = true)
    //                 ||||||└-->  6 => 1 FLOOR  4 EXISTS (0 = false / 1 = true)
    //                 |||||└--->  5 => 2 FLOOR  3 EXISTS (0 = false / 1 = true)
    //                 ||||└---->  4 => 3 FLOOR  2 EXISTS (0 = false / 1 = true)
    //                 |||└----->  3 => 4 FLOOR  1 EXISTS (0 = false / 1 = true)
    //                 ||└------>  2 => 5 OBJECT 3 EXISTS (0 = false / 1 = true)
    //                 |└------->  1 => 6 ENEMY  2 EXISTS (0 = false / 1 = true)
    //                 └-------->  0 => 7 ENEMY  1 EXISTS (0 = false / 1 = true)
    //
    // roomToTransportTo
    //                    |||||||└->  \
    //                    ||||||└-->   |
    //                    |||||└--->   | these 6 bits are used for the roomnumber you'll go to
    //                    ||||└---->   |
    //                    |||└----->   |
    //                    ||└------>  /
    //                    |└-------> NOT USED
    //                    └--------> NOT USED
    //
    // roomNumberInfluencing
    //                        |||||||└->0  \
    //                        ||||||└-->1   |
    //                        |||||└--->2   | bits 0-5: room whose elements this switch toggles
    //                        ||||└---->3   | (the old separate "from room" level byte is gone)
    //                        |||└----->4   |
    //                        ||└------>5  /
    //                        |└------->6 NOT USED
    //                        └-------->7 switch is ON (remembered when you leave the room)
    //
    // elementsInfluenced
    //                     ||||||||
    //                     |||||||└->0 FLOOR  1 INFLUENCED (0 = false / 1 = true)
    //                     ||||||└-->1 FLOOR  2 INFLUENCED (0 = false / 1 = true)
    //                     |||||└--->2 FLOOR  3 INFLUENCED (0 = false / 1 = true)
    //                     ||||└---->3 FLOOR  4 INFLUENCED (0 = false / 1 = true)
    //                     |||└----->4 FLOOR  5 INFLUENCED (0 = false / 1 = true)
    //                     ||└------>5 ENEMY  1 INFLUENCED (0 = false / 1 = true)
    //                     |└------->6 ENEMY  2 INFLUENCED (0 = false / 1 = true)
    //                     └-------->7 OBJECT 3 INFLUENCED (0 = false / 1 = true)
};

Room stageRoom[MAX_AMOUNT_OF_ROOMS];

// one PROGMEM lookup for the current 1-based level
// levels[] is itself in PROGMEM, so first read the pointer, then the byte
byte lvByte(byte idx)
{
  const unsigned char *lv =
    (const unsigned char *)pgm_read_dword(&levels[level - LEVEL_OFFSET]);
  return pgm_read_byte(lv + idx);
}

byte roomByte(byte roomNumber, byte offset)
{
  return lvByte(ROOMS_DATA_START_AT_BYTE + (BYTES_USED_FOR_EVERY_ROOM * roomNumber) + offset);
}

void buildRooms()
{
  dropRoom = 0xFF;
  // let's read out in witch room the exit to the next level is
  exitRoomLocation = lvByte(LEVEL_ROOM_DATA_START_AT_BYTE) & 0b00111111;

  byte amountOfRooms = lvByte(AMOUNT_OF_ROOMS_AT_BYTE);
  int transportDataAtByte = ROOMS_DATA_START_AT_BYTE + (BYTES_USED_FOR_EVERY_ROOM * amountOfRooms);
  int influenceDataAtByte = transportDataAtByte + lvByte(AMOUNT_OF_TRANSPORTERS_AT_BYTE);
  byte transporterCounter = 0;
  byte influenceDataCounter = 0;
  // start reading the data out off PROGMEM
  for (byte roomNumber = 0; roomNumber < amountOfRooms; roomNumber++)
  {
    memset(&stageRoom[roomNumber], 0, sizeof(Room));

    // now lets set all the data for each room in the current level from the datasheet
    // first set all the doors and if those are closed or open
    stageRoom[roomNumber].doorsClosedActive = roomByte(roomNumber, 0);

    // Second thing to do is to set the 8 elements active or inactive in each room (2 enemies, an object and 5 special floor tiles)
    for (byte i = 0; i < 8; i++)
    {
      if (roomByte(roomNumber, 5 + i))
      { //0b76543210
        bitSet (stageRoom[roomNumber].elementsActive, 7 - i);    //0b12345678
      }
    }

    byte objType = roomByte(roomNumber, 5 + OBJECT) & 0b00000111;

    // Third thing to do is to set the transporter data in the correct room
    if (objType == TELEPORT)
    {
      stageRoom[roomNumber].roomToTransportTo = lvByte(transportDataAtByte + transporterCounter);
      transporterCounter++;
    }
    // Fourth thing to do is to set in which room an element is influenced, where the influencer is and what elementes are influenced
    if (objType > TELEPORT)
    {
      // influence record is 2 bytes: target room, element mask
      // (the old middle "from room" byte was unused and has been removed)
      stageRoom[roomNumber].roomNumberInfluencing = lvByte(influenceDataAtByte + influenceDataCounter);
      stageRoom[roomNumber].elementsInfluenced = lvByte(influenceDataAtByte + influenceDataCounter + 1);
      influenceDataCounter += 2;
      if (objType == SWITCH_ON)
        bitSet(stageRoom[roomNumber].roomNumberInfluencing, 7);
    }
  }
}


byte checkIfLevelDoor()
{
  byte test = lvByte(LEVEL_DOOR_DATA_START_AT_BYTE);
  if (currentRoom == (test >> 2)) return (test & 0b00000011);
  return 255;
}

byte tileFromXY(byte x, byte y)
{
  unsigned int x45 = x * 170;
  unsigned int y45 = y * 341;
  x = (((y45 - x45) >> 8) + 18);
  y = (((y45 + x45) >> 8) - 49);
  return (y >> 4) * 5 + (x >> 4);
}


int translateTileToX (byte currentTile)
{
  return ((((((currentTile / 5) * 6) + 4) - currentTile) * 12) + 3);
}


int translateTileToY (byte currentTile)
{
  return (18 + (currentTile * 6) - ((currentTile / 5) * 24));
}

bool checkIfOnCenterTile (byte coX, byte coY)
{
  // same 5×5 centres as the loop above, closed form
  byte dx = coX - 3;
  byte dy = coY - 18;
  if ((dx % 12) || (dy % 6)) return false;
  int8_t a = dx / 12;
  int8_t b = dy / 6;
  int8_t row = a + b - 4;
  if (row & 1) return false;
  row >>= 1;
  int8_t col = b - row;
  return (row >= 0 && row < 5 && col >= 0 && col < 5);
}


/*
bool checkIfOnCenterTile(byte coX, byte coY)
{
  byte tx = 51;   // starting X for y=0, x=0
  byte ty = 18;   // starting Y

  for (byte y = 0; y < 5; y++)
  {
    byte cx = tx;
    byte cy = ty;

    for (byte x = 0; x < 5; x++)
    {
      if (coX == cx && coY == cy)
        return true;

      cx -= 12;
      cy += 6;
    }

    tx += 12;
    ty += 6;
  }

  return false;
}
*/

void enterRoom(byte roomNumber)
{
 
  playerShot.active = false;
  enemyBulletActive = false;
  objectHiddenThisVisit = false;
  for (byte i = 0; i < 8; i++)
  {
    elements[i].characteristics = 0;
    if (bitRead (stageRoom[roomNumber].elementsActive, 7 - i))
    {
      byte b = roomByte(roomNumber, 5 + i);
      byte currentTile = b >> 3;
      if (currentTile > 24) currentTile = 0;
      //if (currentTile > 24) elements[i].characteristics = 0;
      elements[i].characteristics = b; 
      elements[i].x = translateTileToX(currentTile);
      elements[i].y = translateTileToY(currentTile);
    }
  }
  if ((elements[OBJECT].characteristics & 0b00000111) >= SWITCH_OFF)
  {
    if (bitRead(stageRoom[roomNumber].roomNumberInfluencing, 7))
      bitSet(elements[OBJECT].characteristics, 0);
    else
      bitClear(elements[OBJECT].characteristics, 0);
  }
  if (dropRoom == roomNumber)
  {
    byte s = dropInfo >> 5;
    byte t = dropInfo & 31;
    bitSet(stageRoom[roomNumber].elementsActive, 7 - s);
    elements[s].characteristics = (t << 3) | ENEMY_BATTERY;
    elements[s].x = translateTileToX(t);
    elements[s].y = translateTileToY(t);
  }
}

byte transportToRoom (byte roomNumber)
{
  return stageRoom[roomNumber].roomToTransportTo;
}


byte goToRoom(byte roomNumber)
{
  // we know which door the player goes through by the direction the droid is facing
  byte door = player.characteristics & 0b00000011;
  return roomByte(roomNumber, 1 + door) >> 2;
};


byte goToTile(byte roomNumber)
{
  // we know which door the player goes through by the direction the droid is facing
  byte door = player.characteristics & 0b00000011;
  return pgm_read_byte(&doorFrontTile[roomByte(roomNumber, 1 + door) & 0b00000011]);
}

int setCurrentRoomY(byte currentTile)
{
  // SOUTH or WEST → -35, otherwise -9
  return (currentTile == TILE_INFRONT_DOOR_SOUTH ||
          currentTile == TILE_INFRONT_DOOR_WEST) ? -35 : -9;
}

int offsetXAfterDoor(byte currentTile)
{
  // EAST or SOUTH → 10, otherwise -10
  return (currentTile == TILE_INFRONT_DOOR_EAST ||
          currentTile == TILE_INFRONT_DOOR_SOUTH) ? 10 : -10;
}

int offsetYAfterDoor(byte currentTile)
{
  // SOUTH or WEST → 5, otherwise -5
  return (currentTile == TILE_INFRONT_DOOR_SOUTH ||
          currentTile == TILE_INFRONT_DOOR_WEST) ? 5 : -5;
}

/////////////////  DRAW ROOM    ///////////////////
///////////////////////////////////////////////////
void drawFloor()
{
  bool menu = gameState < STATE_GAME_PLAYING;

  for (byte y = 0; y < 5; y++)
  {
    for (byte x = 0; x < 5; x++)
    {
      byte tile = y * 5 + x;
      if (menu && ((tile > 5 && tile <= 9) || tile == 11 || tile == 16 || tile == 21))
        continue;

      byte fr = 0;
      if (!menu && x == 2 && y == 2 && currentRoom == exitRoomLocation)
      {
        if (arduboy.everyXFrames(8)) levelUpAnimation = (++levelUpAnimation % 3);
        fr = 5 + levelUpAnimation;
      }
      sprites.drawPlusMask(48 - 12 * x + 12 * y,
                           currentRoomY + 27 + 6 * x + 6 * y,
                           floorTile_plus_mask, fr);
    }
  }

  if (!menu) return;

  for (byte i = 0; i < 5; i++)
    sprites.drawSelfMasked(39 + (i << 3), currentRoomY + 46, escaperDroid, i);
  for (byte i = 0; i < 9; i++)
    sprites.drawSelfMasked(23 + (i << 3), currentRoomY + 54, escaperDroid, 5 + i);
  for (byte i = 0; i < 4; i++)
  {
    sprites.drawSelfMasked(15 + (i << 3), currentRoomY + 62, escaperDroid, 14 + i);
    sprites.drawSelfMasked(71 + (i << 3), currentRoomY + 62, escaperDroid, 17 + i);
  }
}

void drawWallSegments()
{
  for (byte x = 0; x < 6; x++)
  {
    sprites.drawSelfMasked( -2 + (10 * x), currentRoomY + 25 - (5 * x), wallPartsV3, NORTH);
    sprites.drawSelfMasked(60 + (10 * x), currentRoomY + (5 * x), wallPartsV3, EAST);
  }
}

void drawTicker(byte setTicker)
{
  if (setTicker && showTicker)
  {
    byte y = 0;
    byte x = 0;
    for (byte w = 0; w < 59; w++)
    {
      for (byte z = 0; z < 2; z++)
      {
        sprites.drawSelfMasked(x, currentRoomY + 38 - y, fnt, charBox[x]);
        x++;
      }
      (w < 29) ? y++ : y--;
    }
  }

  if (arduboy.everyXFrames(20)&& (bitRead(setTicker,5))) bitToggle(showTicker,0);

  if (arduboy.everyXFrames(4))
  {
    switch (setTicker)
    {
      case TEXT_SCROLL_LEFT:
      case TEXT_BLINK_SCROLL_LEFT:
        {
          byte tempChar = charBox[0];
          for (int i = 0; i < 119; i++) charBox[i] = charBox[i + 1];
          charBox[119] = tempChar;
        }
        break;
      case TEXT_SCROLL_RIGHT:
      case TEXT_BLINK_SCROLL_RIGHT:
        {
          byte tempChar = charBox[119];
          for (int i = 119; i > 0; i--) charBox[i] = charBox[i - 1];
          charBox[0] = tempChar;
        }
        break;
    }
  }

  // blink: hide the text every other 16 frames
  if (bitRead(setTicker, 5) && showMask) { /* drawn already; mask is separate */ }
}

void drawWalls()
{
  drawTicker(setTicker);
  drawWallSegments();
  
}




// Door piece coords: for each dir N,E,S,W × part lintel, big-post, small-post, closed
// pairs are (x, y relative to currentRoomY). Same pixels as the original 16 functions.
PROGMEM const unsigned char doorPieceXY[] = {
  16,  5,  24, 21,  16, 21,  24, 15,   // NORTH
  80,  5,  80, 21,  95, 21,  85, 15,   // EAST
  81, 38,  89, 54,  81, 54,  89, 48,   // SOUTH
  14, 38,  14, 54,  29, 54,  19, 48    // WEST
};

void drawDoorPiece(byte id)
{
  byte piece = id - NORTH_LINTEL;          // 0..15
  byte dir   = piece >> 2;                 // N E S W
  byte part  = piece & 3;                  // 0 lintel 1 big 2 small 3 closed
  byte idx   = piece << 1;
  byte x     = pgm_read_byte(&doorPieceXY[idx]);
  int  y     = currentRoomY + pgm_read_byte(&doorPieceXY[idx + 1]);
  const unsigned char *bmp = doorLintel_plus_mask;
  if (part == 1) bmp = doorPostBig_plus_mask;
  else if (part == 2) bmp = doorPostSmall_plus_mask;
  else if (part == 3) bmp = doorClossed_plus_mask;
  drawFlip(x, y, bmp, dir);
  if (part == 3 && checkIfLevelDoor() == dir)
    drawFlip(x, y + 1, bmp, dir);   // level door drawn twice
}







// itemsOrder is the z-buffer. Do NOT shrink it or move the door-gap slots.
// painter order (back → front):
//   [ 0] N lintel  [ 1] N big post  [ 2] GAP (droid N in / S out)
//   [ 3] N small   [ 4] N closed
//   [ 5] E lintel  [ 6] E big post  [ 7] GAP (droid E in / W out)
//   [ 8] E small   [ 9] E closed
//   [10..34] 25 floor tiles (ITEMS_ORDER_TILES_START = 10)
//   [35] S lintel  [36] S big post  [37] GAP (droid S in / N or E out)
//   [38] S small   [39] S closed
//   [40] W lintel  [41] W big post  [42] GAP (droid W in)
//   [43] W small   [44] W closed
// Putting the droid in those GAP slots is what draws him BETWEEN the two posts.

void drawRoom()
{
  drawWalls();
  drawFloor();
  for (byte i = 0; i < SIZE_OF_ITEMSORDER; i++)
  {
    if (i == ITEMS_ORDER_TILES_START + 25)   // after the floor tiles, before the S and W doors: shots fly behind those posts
    {
      drawBulletPlayer();
      drawBulletEnemy();
    }
    byte id = itemsOrder[i];
    if (id == EMPTY_PLACE) continue;
    if (id <= ENEMY_TWO)         drawEnemies(id);
    else if (id == OBJECT)       drawObject();
    else if (id <= FLOOR_FIVE)   drawFloor(id);
    else if (id == PLAYER_DROID) drawPlayer();
    else if (id >= NORTH_LINTEL && id <= WEST_DOOR_CLOSSED) drawDoorPiece(id);
  }
}


// door z-slots: lintel, big-post, GAP, small-post, closed  — do not move the GAP
PROGMEM const byte doorSlot[] = { 0, 5, 35, 40 };
PROGMEM const byte doorGapIn[]  = { 2, 7, 37, 42 };
PROGMEM const byte doorGapOut[] = { 37, 37, 2, 7 };

void checkOrderOfObjects()
{
  // clear out the itemsOrder
  memset(itemsOrder, EMPTY_PLACE, SIZE_OF_ITEMSORDER);

  byte doors = stageRoom[currentRoom].doorsClosedActive;
  for (byte d = 0; d < 4; d++)
  {
    byte base = pgm_read_byte(&doorSlot[d]);
    byte id   = NORTH_LINTEL + (d << 2);
    if (bitRead(doors, d + 4))
    {
      itemsOrder[base    ] = id;
      itemsOrder[base + 1] = id + 1;
      itemsOrder[base + 3] = id + 2;
    }
    if (bitRead(doors, d)) itemsOrder[base + 4] = id + 3;
  }


  //******************************
  //determine what is on the tiles
  //******************************
  // check what tile the player is on (so that we can determine what order things need to be displayed)
  // elements first, then the droid so a box on the same tile cannot hide him
  for (byte i = 0; i < 8; i++)
  {
    if (i == OBJECT && objectHiddenThisVisit) continue;
    if (bitRead(stageRoom[currentRoom].elementsActive, 7 - i))itemsOrder[tileFromXY(elements[i].x, elements[i].y) + ITEMS_ORDER_TILES_START] = i;
  }

  if (!bitRead(player.characteristics, DROID_GOES_THROUGH_DOOR_AT_BIT_5) && !bitRead(player.characteristics, DROID_COMES_OUT_DOOR_AT_BIT_6))
  {
    itemsOrder[player.isOnTile + ITEMS_ORDER_TILES_START] = PLAYER_DROID;
  }
  else
  {
    byte d = player.characteristics & 0b00000011;
    if (bitRead(player.characteristics, DROID_GOES_THROUGH_DOOR_AT_BIT_5))
      itemsOrder[pgm_read_byte(&doorGapIn[d])] = PLAYER_DROID;
    if (bitRead(player.characteristics, DROID_COMES_OUT_DOOR_AT_BIT_6))
      itemsOrder[pgm_read_byte(&doorGapOut[d])] = PLAYER_DROID;
  }
}

void drawNumbers(byte x, byte y, byte numbers, byte width)
{
  // HUD digits use the same 3-column ticker font as the scrolling text
  byte digits[7];
  byte charLen = 0;
  if (numbers == 0)
    digits[charLen++] = 0;
  else
  {
    while (numbers && charLen < 7)
    {
      digits[charLen++] = numbers % 10;
      numbers /= 10;
    }
  }
  while (charLen < width && charLen < 7)
    digits[charLen++] = 0;
  for (byte i = 0; i < charLen; i++)
  {
    byte fr = digits[charLen - 1 - i] * 3 + 1;
    for (byte c = 0; c < 3; c++)
      sprites.drawSelfMasked(x + (4 * i) + c, y, fnt, fr + c);
  }
}

void drawHUD()
{
  //draw HUD mask
  for (byte y = 0; y < 8; y++) sprites.drawPlusMask(118, y * 8, hudMask_plus_mask, 0);

  //draw room number
  drawNumbers(121, 1, currentRoom, 2);

  //draw amount of bullets
  drawNumbers(123, 23, (player.assets & 0b00000111), 1);
  sprites.drawSelfMasked(122, 29, hudBullet, 0);

  //draw amount of white cards
  drawNumbers(123, 38, (player.assets & 0b00011000) >> 3, 1);
  sprites.drawSelfMasked(121, 44, hudWhiteCard, 0);

  //draw amount of black cards
  drawNumbers(123, 53,((bitRead(player.assets,DROID_HAS_BLACK_CARD_AT_BIT_5)) == 0) ? 0 : 1, 1);
  sprites.drawSelfMasked(121, 59, hudBlackCard, 0);

  //draw life (battery icon + count)
  if (bitRead(player.characteristics,DROID_DYING_AT_BIT_4) || (player.life > 1))bitSet(player.assets,DROID_BATTERY_VISIBLE_AT_BIT_6);
  else if (arduboy.everyXFrames(20) && (player.life < 2)) bitToggle(player.assets,DROID_BATTERY_VISIBLE_AT_BIT_6);
  if (bitRead(player.assets, DROID_BATTERY_VISIBLE_AT_BIT_6)) sprites.drawSelfMasked(122, 9, hudLife, player.life);
}


#endif
