#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include "ATMconfig.h"
#include <ATMlib.h>
#include <Arduboy2.h>
#include <Sprites.h>
#include "bitmaps.h"


// constants /////////////////////////////////////////////////////////////////

//define menu states (on main menu)

#define STATE_MENU_MAIN              0

#define STATE_MENU_CONF              1
#define STATE_MENU_SDFX              2
#define STATE_MENU_INFO              3
#define STATE_MENU_PLAY              4

#define STATE_MENU_INTRO             5


//define game states (on main menu)
#define STATE_GAME_PLAYING           6
#define STATE_GAME_PAUSE             7   // must match the position in mainGameLoop[] (WCCS_AB.ino)
#define STATE_GAME_SPLASH            10
#define STATE_GAME_OVER              8   // idem

//define facing directions
#define EAST                          0
#define NORTH                         1
#define WEST                          2
#define SOUTH                         3


// globals ///////////////////////////////////////////////////////////////////

Arduboy2Base arduboy;
Sprites sprites;
ATMsynth ATM;

byte gameState = STATE_MENU_INTRO;   // start the game with the intro logo
byte menuSelection = STATE_MENU_PLAY; // PLAY menu item is pre-selected

byte introFrame = 0;                   // frame counter of the intro (menu.h)
byte test = 0;
bool newGame = true;                   // set to true to rebuild the world when the game starts


// this game runs at 45 fps (22 ms per frame): 11 frames = 242 ms = 6 ATM ticks
#define INTRO_STEP   11
#define INTRO_TICKS  6
#define INTRO_SLIDE  15
#define INTRO_SPEED  8
#define INTRO_TAIL   15
#include "arduboy_intro.h"

#include "music.h"

#endif
