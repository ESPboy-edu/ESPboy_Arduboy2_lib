#pragma once

#include <stdint.h>

#define TITLE_SCREEN_SOUND        0
#define READY_SOUND               1
#define CAUGHT_SOUND              2
#define EMPTY_SOUND               3
#define BOUNCE_SOUND              4
#define FLOOR_HIT_SOUND           5
#define GAME_OVER_SOUND           6
#define TEST_SOUND_1              7
#define HANGMAN_VICTORY_SOUND     8
#define LAST_SOUND                9
#define SOUND_COUNT               15

const int16_t SOUNDS_DATA[] PROGMEM = {
  // TITLE_SCREEN_SOUND
  262, 2,
  294, 2,
  330, 2,
  349, 2,
  392, 2,
  440, 2,
  494, 2,
  523, 2,
  -1,
  // READY_SOUND
  392, 7,
  330, 7,
  262, 7,
  523, 3,
  -1,
  // CAUGHT_SOUND
  400, 0,
  -1,
  // EMPTY_SOUND
  300, 0,
  -1,
  // BOUNCE_SOUND
  200, 2,
  262, 2,
  330, 2,
  392, 1,
  -1,
  // FLOOR_HIT_SOUND
  200, 2,
  200, 1,
  -1,
  // GAME_OVER_SOUND
  200, 8,
  150, 3,
  -1,
  // TEST_SOUND_1
  100, 1,
  110, 1,
  120, 1,
  130, 1,
  140, 1,
  150, 1,
  140, 1,
  200, 1,
  130, 1,
  120, 1,
  -1,
  // HANGMAN_VICTORY_SOUND
  392, 4,
  262, 6,
  523, 1,
  -1,
  // INSTRUMENT_SOUND
  262, 3,
  263, 3,
  264, 3,
  263, 3,
  262, 3,
  261, 3,
  260, 3,
  261, 3,
  -1,
  // INSTRUMENT_SOUND
  262, 1,
  264, 1,
  266, 1,
  264, 1,
  262, 1,
  260, 1,
  258, 1,
  260, 1,
  -1,
  // INSTRUMENT_SOUND_2
  262, 0,
  264, 0,
  266, 0,
  264, 0,
  262, 0,
  260, 0,
  258, 0,
  260, 0,
  -1,
  // CAUGHT_SOUND
  262, 0,
  -1,
  // FLOOR_HIT_SOUND
  262, 1,
  262, 0,
  -1,
  // GAME_OVER_SOUND
  262, 5,
  196, 2,
  -1,
};