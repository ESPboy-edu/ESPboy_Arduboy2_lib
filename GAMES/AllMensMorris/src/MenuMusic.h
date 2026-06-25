#pragma once

#include <Arduino.h>

constexpr uint8_t MENU_MUSIC_THEME_COUNT = 3;
constexpr uint8_t MENU_MUSIC_MIN_NOTE = 55;
constexpr uint8_t MENU_MUSIC_MAX_NOTE = 72;
constexpr uint8_t MENU_MUSIC_TICK_MS = 110;

extern const uint8_t MenuMusicThemeEventCounts[] PROGMEM;
extern const uint8_t MenuMusicTheme0Notes[] PROGMEM;
extern const uint8_t MenuMusicTheme0Durations[] PROGMEM;
extern const uint8_t MenuMusicTheme1Notes[] PROGMEM;
extern const uint8_t MenuMusicTheme1Durations[] PROGMEM;
extern const uint8_t MenuMusicTheme2Notes[] PROGMEM;
extern const uint8_t MenuMusicTheme2Durations[] PROGMEM;
extern const uint8_t *const MenuMusicNotesByTheme[] PROGMEM;
extern const uint8_t *const MenuMusicDurationsByTheme[] PROGMEM;
extern const uint16_t MenuMusicFrequencies[] PROGMEM;
