#include "MenuMusic.h"

// Generated from TableTop Studio menu music data.
// Menu themes are compact monophonic Arduboy tone event streams.

const uint8_t MenuMusicTheme0Notes[] PROGMEM = {
  60, 64, 64, 60, 64, 60, 64, 64, 60, 64,
};

const uint8_t MenuMusicTheme0Durations[] PROGMEM = {
  4, 2, 2, 2, 4, 4, 4, 2, 4, 4,
};
const uint8_t MenuMusicTheme1Notes[] PROGMEM = {
  72, 72, 69, 72, 69, 72, 67, 0, 69, 69, 67, 69, 67, 69, 65, 0,
};

const uint8_t MenuMusicTheme1Durations[] PROGMEM = {
  6, 4, 2, 2, 2, 2, 6, 2, 6, 4, 2, 2, 2, 2, 6, 2,
};
const uint8_t MenuMusicTheme2Notes[] PROGMEM = {
  61, 60, 59, 58, 57, 56, 55, 57, 0, 64, 0, 63, 62, 61, 60, 59,
  58, 61, 0,
};

const uint8_t MenuMusicTheme2Durations[] PROGMEM = {
  6, 2, 2, 2, 2, 2, 6, 6, 2, 4, 2, 2, 2, 2, 2, 2,
  6, 6, 2,
};

const uint8_t MenuMusicThemeEventCounts[] PROGMEM = { 10, 16, 19 };

const uint8_t *const MenuMusicNotesByTheme[] PROGMEM = { MenuMusicTheme0Notes, MenuMusicTheme1Notes, MenuMusicTheme2Notes };

const uint8_t *const MenuMusicDurationsByTheme[] PROGMEM = { MenuMusicTheme0Durations, MenuMusicTheme1Durations, MenuMusicTheme2Durations };

const uint16_t MenuMusicFrequencies[] PROGMEM = {
  196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466,
  494, 523,
};
