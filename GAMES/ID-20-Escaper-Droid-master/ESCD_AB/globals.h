#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Arduboy2.h>
#include <Sprites.h>
#include "ATMconfig.h"
#include <ATMlib.h>
#include "bitmaps.h"
#include "sfx.h"
#include "song.h"
#include "text.h"

// constants /////////////////////////////////////////////////////////////////

//define EEPROM
#define OFFSET_ESCD_START             (EEPROM_STORAGE_SPACE_START + 73)
#define OFFSET_LEVEL                  (OFFSET_ESCD_START + sizeof(byte))
#define OFFSET_BUTTONS                (OFFSET_LEVEL + sizeof(byte))
#define OFFSET_ESCD_END               (OFFSET_BUTTONS + sizeof(byte))

//define menu states (on main menu)
#define STATE_MENU_INTRO             5
#define STATE_MENU_MAIN              0
#define STATE_MENU_PLAY              4
#define STATE_MENU_INFO              3
#define STATE_MENU_CONF              1
#define STATE_MENU_SDFX              2

//define menuSelection states
#define STATE_MENU_SELECT_CONF       0
#define STATE_MENU_SELECT_SDFX       1
#define STATE_MENU_SELECT_INFO       2
#define STATE_MENU_SELECT_PLAY       3

//define game states (for the game)
#define STATE_GAME_PLAYING           6
#define STATE_GAME_NEXT_ROOM         7
#define STATE_GAME_NEXT_LEVEL        8
#define STATE_GAME_PAUSE             9
#define STATE_GAME_OVER              10
#define STATE_GAME_TRANSPORTING      11
#define STATE_GAME_FINISHED          12

#define STATE_GAME_NEW               13
#define STATE_GAME_CONTINUE          14


//define facing directions
#define NORTH                        0
#define EAST                         1
#define SOUTH                        2
#define WEST                         3

#define NORTH_SOUTH                  0
#define EAST_WEST                    1

#define BIG_FONT                     0
#define THIN_FONT                    1
#define SMALL_FONT                   2

#define SIZE_OF_ITEMSORDER           46
#define ITEMS_ORDER_TILES_START      10

#define LEVEL_TO_START_WITH          1
#define TILE_GAME_STARTS_ON          12

#define FALSE                        0
#define TRUE                         1

#define ROOM_DRAWING_OFFSET          -6

// globals ///////////////////////////////////////////////////////////////////

Arduboy2Base arduboy;
Sprites sprites;
ATMsynth ATM;

byte introFrame = 0;                 // frame counter of the ARDUBOY intro + T-arg logo
byte gameState = STATE_MENU_INTRO;   // start the game with the intro logo
byte menuSelection = STATE_MENU_SELECT_PLAY; // PLAY menu item is pre-selected
byte level;
unsigned long scorePlayer;

byte itemsOrder[SIZE_OF_ITEMSORDER];
byte currentRoom;
byte currentlyOnTestingTile;
byte testingTile;
int currentRoomY;
byte buttonSchemeOffset = FALSE;
byte newGameOffset = FALSE;
byte exitRoomLocation = 0;
byte setTicker;
bool showTicker;
bool danceDroid = 0;
byte showMask = 0;
bool objectHiddenThisVisit;  // ammo pickup hides until you leave the room
byte gameTempo = 36;         // tempo of the song that is playing in the game (set by playGameMusic)
bool musicPending;           // a new level starts: play its song when the player leaves the level screen

void statePrepForPause()
{
  setTicker = TEXT_BLINK;
  loadAndFillMessage(5);
  addNumber(level,9,2);
  addNumber(scorePlayer,23,6);
}

void statePrepForRoom()
{
  loadAndFillMessage(10+(currentRoom%8));
  setTicker = TEXT_SCROLL_LEFT;
  showTicker = TRUE;
  gameState = STATE_GAME_PLAYING;
}

void play_SFX(byte idSfx)
{
  if (soundMode & 1) return;   // 1 = music only, 3 = mute
  const unsigned char *fx =
    (const unsigned char *)pgm_read_dword(&soundFX[idSfx]);
  ATM.playSfx(fx, 1);          // ch1: leave song pulse on ch0
}

// Draws frame f of a plus-mask sheet that only stores the even frames: stored frame f>>1, mirrored left/right when f is odd
// (N/E and S/W views are mirror images). Same result as sprites.drawPlusMask, clipped to the screen.
void drawFlip(int x, int y, const unsigned char *bmp, byte f)
{
  byte w = pgm_read_byte(bmp);
  byte pages = (pgm_read_byte(bmp + 1) + 7) >> 3;
  const unsigned char *p = bmp + 2 + (unsigned int)(f >> 1) * w * pages * 2;
  byte *buf = arduboy.getBuffer();
  byte sh = y & 7;
  int page = y >> 3;
  for (byte pg = 0; pg < pages; pg++)
    for (byte c = 0; c < w; c++)
    {
      int sx = x + c;
      if (sx < 0 || sx > 127) continue;
      const unsigned char *q = p + (((pg * w) + ((f & 1) ? w - 1 - c : c)) << 1);
      unsigned int s = (unsigned int)pgm_read_byte(q) << sh;
      unsigned int m = (unsigned int)pgm_read_byte(q + 1) << sh;
      int d = page + pg;
      byte *b = buf + d * 128 + sx;
      if (d >= 0 && d < 8) *b = (*b & ~(byte)m) | (byte)s;
      if (sh && d >= -1 && d < 7) { b += 128; *b = (*b & ~(byte)(m >> 8)) | (byte)(s >> 8); }
    }
}

void playMenuMusic()
{
  ATM.play(titleSong);
  if (soundMode >= 2) ATM.pause();   // SFX only: keep ISR, freeze score
}

#endif
