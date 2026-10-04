// Pocket Stage - the pages between the title and the play, after Guitar
// Hero's: the main menu (PLAY, SETTINGS), the settings, the difficulty, the
// setlist with every song's stars and best score; and the results after a
// song. Up and Down move the pick, B takes what it is on, A goes back; on
// the setlist Left and Right change the difficulty on the spot. The riff of
// the title page goes on underneath.
#pragma once
#include "ui.h"
#include "version.h"
#include "save.h"

#define PAGE_TITLE 0
#define PAGE_MAIN 1
#define PAGE_SETTINGS 2
#define PAGE_LEVEL 3
#define PAGE_SETLIST 4
#define PAGE_PLAY 5
#define PAGE_RESULTS 6
#define PAGE_PAUSE 7
#define PAGE_FAILED 8
#define PAGE_CREDITS 9

#define LIST_Y 14                       // the setlist's rows start under the bar and a row of black...
#define LIST_ROW 13                     // ...a row of white under the letters of the one picked...
#define LIST_ROWS 2                     // ...two of them (a third leaves no air for the small print)...
#define DETAIL_Y 41                     // ...then a line, and two lines of small print about the song picked
#define PAUSE_ROW 13                    // the pause menu's items, boxes with a row below the letters too

static uint8_t page;
static uint8_t level = 2;               // 0 easy, 1 medium, 2 hard, 3 expert; Joel tests on hard
static uint8_t song_index;
static char song_text[44];              // title, NUL, who wrote it, NUL
static uint8_t remote_b;                // "b" came over USB: as if B were pressed
static uint8_t remote_btn;              // "u", "d", "l", "r" came over USB: as if pressed

// the buttons as the library reads them, and those pressed over USB
static void poll() {
  arduboy.pollButtons();
  arduboy.currentButtonState |= remote_btn;
  remote_btn = 0;
}
static uint8_t pick;                    // the item the cursor is on
static uint8_t list_first;              // the setlist's first row
static uint8_t clear_asked;             // CLEAR SCORES pressed once
static uint8_t new_best, song_stars;    // the results
static uint8_t fail_pct;                // how far the song was when the crowd had enough
static uint8_t fail_frame;              // frames the booed-off page has been up (to 255)
static uint16_t credits_frame;          // frames the credits have rolled
static uint16_t roll_height;            // the whole roll, in rows (worked out once)

static const char LEVEL_0[] PROGMEM = "EASY";
static const char LEVEL_1[] PROGMEM = "MEDIUM";
static const char LEVEL_2[] PROGMEM = "HARD";
static const char LEVEL_3[] PROGMEM = "EXPERT";
static const char *const LEVEL_NAMES[LEVELS] PROGMEM = {LEVEL_0, LEVEL_1, LEVEL_2, LEVEL_3};
static const char MAIN_0[] PROGMEM = "PLAY";
static const char MAIN_1[] PROGMEM = "SETTINGS";
static const char SETTING_0[] PROGMEM = "SOUND";
static const char SETTING_1[] PROGMEM = "LEFTY FLIP";
static const char SETTING_2[] PROGMEM = "DEMO";
static const char SETTING_3[] PROGMEM = "CLEAR SCORES";
static const char SETTING_4[] PROGMEM = "CREDITS";
#define SETTING_COUNT 5
#define SETTING_ROWS 4                  // four in sight: the list moves up by one for the fifth
static const char *const SETTING_NAMES[SETTING_COUNT] PROGMEM = {SETTING_0, SETTING_1, SETTING_2, SETTING_3, SETTING_4};
static const char TXT_ON[] PROGMEM = "ON";
static const char TXT_OFF[] PROGMEM = "OFF";
static const char TXT_SURE[] PROGMEM = "SURE?";
static const char HDR_MAIN[] PROGMEM = "POCKET STAGE";
static const char HDR_SETTINGS[] PROGMEM = "SETTINGS";
static const char HDR_LEVEL[] PROGMEM = "DIFFICULTY";
static const char HDR_SETLIST[] PROGMEM = "SETLIST";
static const char TXT_NEW_BEST[] PROGMEM = "NEW BEST!";
static const char TXT_PRESS_B[] PROGMEM = "PRESS B";
static const char HDR_PAUSED[] PROGMEM = "PAUSED";
static const char PAUSE_0[] PROGMEM = "RESUME";
static const char PAUSE_1[] PROGMEM = "RESTART";
static const char PAUSE_2[] PROGMEM = "QUIT";
static const char *const PAUSE_NAMES[3] PROGMEM = {PAUSE_0, PAUSE_1, PAUSE_2};

static const char *song_by() {
  return song_text + strlen(song_text) + 1;
}

static const char *level_name(uint8_t l) {
  return (const char *)pgm_read_dword(&LEVEL_NAMES[l]);
}

// a song's entry and its texts, off the chip
// a song's entry and its texts, off the chip
static void song_read(uint8_t i, Song &s, char *text) {
  fx_read(FX_SONG_TABLE + (uint16_t)i * SONG_ENTRY_BYTES, &s, sizeof s);
  fx_read((uint32_t)s.text, text, sizeof song_text);
  text[sizeof song_text - 1] = 0;
}

// stars for a score: how much of the perfect score it is
static uint8_t stars_for(uint32_t score, uint32_t perfect) {
  if (!score || !perfect) return 0;
  uint32_t pct = score * 100 / perfect;
  return pct >= 75 ? 5 : pct >= 55 ? 4 : pct >= 30 ? 3 : pct >= 15 ? 2 : 1;
}

// the menus' song, unless it is playing already
static void riff_ensure() {
  if (!music_loop || music_loop_addr != FX_TITLE_MUSIC) riff_start();
}

// the preview of the song picked in the setlist, unless it is playing already
static void preview_ensure() {
  uint32_t at = 0; 
  fx_read(FX_SONG_TABLE + (uint16_t)song_index * SONG_ENTRY_BYTES + offsetof(Song, preview), &at, 3);
  if (!music_loop || music_loop_addr != at) loop_start(at);
}

// Up and Down through n items, round and round
static void pick_move(uint8_t n) {
  if (arduboy.justPressed(UP_BUTTON)) pick = pick ? pick - 1 : n - 1;
  if (arduboy.justPressed(DOWN_BUTTON)) pick = pick + 1 == n ? 0 : pick + 1;
}

static inline bool pressed_b() {
  if (!arduboy.justPressed(B_BUTTON) && !remote_b) return false;
  remote_b = 0;
  return true;
}

// ---------------------------------------------------------------- opening the pages
static void title_open() {
  page = PAGE_TITLE;
  title_frame = 0;
  riff_start();
}

static void main_open() {
  page = PAGE_MAIN;
  pick = 0;
  riff_ensure();
}

static void settings_open() {
  page = PAGE_SETTINGS;
  pick = 0;
  clear_asked = 0;
}

static void level_open() {
  page = PAGE_LEVEL;
  riff_ensure();                         // (back from the setlist: its preview stops)
}

static void setlist_open() {
  page = PAGE_SETLIST;
  if (best_level != level) best_fill(level);
  preview_ensure();
}

static void play_open() {
  page = PAGE_PLAY;
  song_read(song_index, song, song_text);
  autoplay = (settings & SET_DEMO) != 0;
  play_start(level);
}

// the rock meter ran out: the song stops, the crowd boos
static void failed_open() {
  page = PAGE_FAILED;
  fail_frame = 0;
  title_frame = 0;                      // the booing starts from its first frame
  fail_pct = (uint8_t)(now_ms() * 100 / song.length);
  audio_stop();
  noise_start(3000);
}

static void results_open() {
  page = PAGE_RESULTS;
  audio_stop();
  song_stars = stars_for(score, song.perfect[level]);
  new_best = 0;
  // a song the game played by itself (DEMO) shows how it went, but it is
  // nobody's best
    if (!autoplay && score > (uint32_t)best[song_index]) {
      best[song_index] = score;
      new_best = 1;
      save_score(level, song_index, score); // Передаем индекс текущей песни
    }
}

// ---------------------------------------------------------------- the title
static void title_update() {
  music_fill();
  poll();
  if (pressed_b() || arduboy.justPressed(A_BUTTON)) main_open();
}

// ---------------------------------------------------------------- the main menu
static void main_update() {
  music_fill();
  poll();
  pick_move(2);
  if (pressed_b()) {
    if (pick == 0) level_open();
    else settings_open();
  } else if (arduboy.justPressed(A_BUTTON)) {
    title_open();
  }
}

// the title page's crowd and spotlights under the menu (so they stay put
// going from the one to the other)
static void main_draw() {
  crowd_scene();
  ui_header(HDR_MAIN);
  ui_item(18, MAIN_0, true, pick == 0, false, UI_ITEM_H + 1);
  ui_item(33, MAIN_1, true, pick == 1, false, UI_ITEM_H + 1);
}

// ---------------------------------------------------------------- the settings
static void credits_open();

static void settings_update() {
  music_fill();
  poll();
  uint8_t was = pick;
  pick_move(SETTING_COUNT);
  if (pick != was) clear_asked = 0;
  if (pressed_b()) {
    switch (pick) {
      case 0:
        audio_set_sound(!sound_on);
        break;
      case 1:
        settings ^= SET_LEFTY;
        lanes_flipped = (settings & SET_LEFTY) != 0;
        save_settings();
        break;
      case 2:
        settings ^= SET_DEMO;
        save_settings();
        break;
      case 3:
        if (clear_asked) save_clear();
        clear_asked = !clear_asked;
        break;
      default:
        credits_open();
        break;
    }
  } else if (arduboy.justPressed(A_BUTTON)) {
    main_open();
  }
}

static void settings_draw() {
  arduboy.clear();
  ui_header(HDR_SETTINGS);
  uint8_t first = pick >= SETTING_ROWS ? pick - (SETTING_ROWS - 1) : 0;
  for (uint8_t k = 0; k < SETTING_ROWS; k++) {
    uint8_t i = first + k;
    uint8_t y = 14 + k * UI_ITEM_H;
    bool on = pick == i;
    ui_item(y, (const char *)pgm_read_dword(&SETTING_NAMES[i]), true, on);
    const char *v = 0;
    if (i == 0) v = sound_on ? TXT_ON : TXT_OFF;
    else if (i == 1) v = settings & SET_LEFTY ? TXT_ON : TXT_OFF;
    else if (i == 2) v = settings & SET_DEMO ? TXT_ON : TXT_OFF;
    else if (i == 3 && clear_asked) v = TXT_SURE;
    if (v) ui_value(y, v, on);
  }
}

// ---------------------------------------------------------------- the credits
// The credits roll up from behind the crowd at the bottom, a row every third
// frame (every frame while B is held), round and round, the crowd jumping to
// the menus' song (Joel picked
// the roll of samples 8 on 2026-10-02, and its words): the logo, the
// version, who made the game, the songs with their bands (off the chip), a
// word about them, thanks. A script of records, each its kind and, for
// text, the text and a NUL; a gap is so many rows of nothing.
#define ROLL_END 0
#define ROLL_LOGO 1
#define ROLL_GAP 2                      // then the rows
#define ROLL_SMALL 3                    // then the text: the small print
#define ROLL_BIG 4                      // then the text: the menu font
#define ROLL_SONGS 5                    // every song: its title big, its band small
#define ROLL_TOP CROWD_TOP              // the crowd stands in front of the rows from here down
#define ROLL_BIG_H 13
#define ROLL_SMALL_H 10
#define ROLL_SONG_H (ROLL_BIG_H + ROLL_SMALL_H + 6)
static const char ROLL[] PROGMEM =
  "\x01" "\x02\x06"
  "\x03" "VERSION " GAME_VERSION "\0" "\x02\x10"
  "\x03" "A GAME BY\0" "\x02\x02"
  "\x04" "JOEL COSTER\0" "\x02\x10"
  "\x03" "THE SONGS\0" "\x02\x06"
  "\x05" "\x02\x0A"
  "\x03" "ALL THE SONGS WERE\0"
  "\x03" "WRITTEN FOR THE GAME\0"
  "\x03" "AND THE BANDS\0"
  "\x03" "ARE FICTIONAL\0" "\x02\x10"
  "\x04" "THANKS FOR PLAYING\0" "\x02\x30";

// a line of the roll, centred, at y (above the screen is fine)
static void roll_text(uint8_t kind, const char *s, bool pm, int16_t y) {
  if (kind == ROLL_BIG) {
    ui_text_center(64, y, s, pm);
    return;
  }
  ui_small(64 - 3 * (pm ? strlen_P(s) : strlen(s)), y);
  if (pm) arduboy.print((const __FlashStringHelper *)s);
  else arduboy.print(s);
}

// the roll with its top at y0, drawing what is in sight above the crowd
// (or nothing, to measure it): how tall it is
static uint16_t roll_draw(int16_t y0, bool draw) {
  const char *p = ROLL;
  int16_t y = y0;
  for (;;) {
    uint8_t kind = pgm_read_byte(p++);
    if (kind == ROLL_END) break;
    if (kind == ROLL_GAP) {
      y += pgm_read_byte(p++);
    } else if (kind == ROLL_LOGO) {
      if (draw && y < ROLL_TOP && y + LOGO_H > 0) Sprites::drawExternalMask(LOGO_X, y, LOGO, LOGO_MASK, 0, 0);
      y += LOGO_H;
    } else if (kind == ROLL_SONGS) {
      for (uint8_t i = 0; i < SONG_COUNT; i++, y += ROLL_SONG_H) {
        if (!draw || y >= ROLL_TOP || y + ROLL_SONG_H <= 0) continue;
        Song s;
        char text[sizeof song_text];
        song_read(i, s, text);
        roll_text(ROLL_BIG, text, false, y);
        roll_text(ROLL_SMALL, text + strlen(text) + 1, false, y + ROLL_BIG_H);
      }
    } else {
      if (draw && y < ROLL_TOP && y + ROLL_BIG_H > 0) roll_text(kind, p, true, y);
      y += kind == ROLL_BIG ? ROLL_BIG_H : ROLL_SMALL_H;
      while (pgm_read_byte(p++)) {}       // past the text and its NUL
    }
  }
  return y - y0;
}

static void credits_open() {
  page = PAGE_CREDITS;
  credits_frame = 0;
  if (!roll_height) roll_height = roll_draw(0, false);
}

// A: back to the settings, CREDITS still picked (B held rolls them faster)
static void credits_update() {
  music_fill();
  poll();
  if (arduboy.justPressed(A_BUTTON)) page = PAGE_SETTINGS;
}

static void credits_draw() {
  arduboy.clear();
  roll_draw(ROLL_TOP - (int16_t)(credits_frame / 3), true);
  uint8_t step = (uint8_t)(now_ms() * 8 / TITLE_BEAT_MS) & 15;
  fx_crowd(FX_TITLE_CROWD + (uint24_t)step * (TITLE_CROWD_PAGES * 128));
  // a row every third frame; B held, a row every frame (Joel, 2026-10-02)
  credits_frame += arduboy.pressed(B_BUTTON) ? 3 : 1;
  if (credits_frame >= roll_height * 3) credits_frame -= roll_height * 3;   // gone over the top: again from behind the crowd
}

// ---------------------------------------------------------------- the difficulty
static void level_update() {
  music_fill();
  poll();
  pick = level;
  pick_move(LEVELS);
  level = pick;
  if (pressed_b()) setlist_open();
  else if (arduboy.justPressed(A_BUTTON)) main_open();
}

static void level_draw() {
  arduboy.clear();
  ui_header(HDR_LEVEL);
  for (uint8_t i = 0; i < LEVELS; i++) ui_item(14 + i * UI_ITEM_H, level_name(i), true, level == i);
}

// ---------------------------------------------------------------- the setlist
static void setlist_update() {
  music_fill();
  poll();
  uint8_t was = song_index;
  if (arduboy.justPressed(UP_BUTTON)) song_index = song_index ? song_index - 1 : SONG_COUNT - 1;
  if (arduboy.justPressed(DOWN_BUTTON)) song_index = song_index + 1 == SONG_COUNT ? 0 : song_index + 1;
  if (song_index != was) preview_ensure();      // the newly picked song's preview
  if (arduboy.justPressed(RIGHT_BUTTON) || arduboy.justPressed(LEFT_BUTTON)) {
    if (arduboy.justPressed(RIGHT_BUTTON)) level = level == LEVELS - 1 ? 0 : level + 1;
    else level = level ? level - 1 : LEVELS - 1;
    best_fill(level);
  }
  // the row the cursor is on stays in sight; the rows keep their places
  if (song_index < list_first) list_first = song_index;
  if (song_index >= list_first + LIST_ROWS) list_first = song_index - LIST_ROWS + 1;
  if (pressed_b()) play_open();
  else if (arduboy.justPressed(A_BUTTON)) level_open();
}

static void setlist_draw() {
  arduboy.clear();
  // the bar: SETLIST, and the difficulty with the arrows that change it
  arduboy.fillRect(0, 0, 128, UI_BAR_H, WHITE);
  ui_text(2, 1, HDR_SETLIST, true, true);
  char line[24];
  strcpy_P(line, PSTR("< "));
  strcat_P(line, level_name(level));
  strcat_P(line, PSTR(" >"));
  ui_text_right(126, 1, line, false, true);
  // the rows: a title and its stars each, the one picked white (a row
  // taller, like a menu item's box, so its letters have white under them)
  Song s;
  char text[sizeof song_text];
  for (uint8_t k = 0; k < LIST_ROWS; k++) {
    uint8_t i = list_first + k;
    if (i >= SONG_COUNT) break;
    uint8_t y = LIST_Y + k * LIST_ROW;
    bool on = i == song_index;
    song_read(i, s, text);
    if (on) {
      arduboy.fillRect(0, y, 128, LIST_ROW, WHITE);
      Sprites::drawErase(1, y + 2, PICK, 0);
    }
    ui_text(9, y + 1, text, false, on);
    arduboy.fillRect(84, y + 1, 44, FONT_H, on ? WHITE : BLACK);    // the stars over the end of a long title
    ui_stars(87, y + 3, stars_for(best[i], s.perfect[level]), 8, on);
  }
  // under a line, about the song picked: who plays it and how long it is,
  // the best so far and how many notes; the two lines ten rows apart
  arduboy.drawFastHLine(0, DETAIL_Y, 128, WHITE);
  song_read(song_index, s, text);
  ui_small(2, DETAIL_Y + 4);
  arduboy.print(text + strlen(text) + 1);
  uint16_t secs = s.length / 1000;
  arduboy.setCursor(126 - 6 * 4, DETAIL_Y + 4);
  arduboy.print(secs / 60);
  arduboy.print(':');
  if (secs % 60 < 10) arduboy.print('0');
  arduboy.print(secs % 60);
  arduboy.setCursor(2, DETAIL_Y + 14);
  if (best[song_index]) {
    arduboy.print(F("BEST "));
    arduboy.print((uint32_t)best[song_index]);
  } else {
    arduboy.print(F("NOT PLAYED"));
  }
  utoa(s.notes[level], line, 10);
  strcat_P(line, PSTR(" NOTES"));
  arduboy.setCursor(126 - 6 * strlen(line), DETAIL_Y + 14);
  arduboy.print(line);
}

// ---------------------------------------------------------------- the play, and the pause
static void play_page_update() {
  poll();
  if (pressed_b()) {
    page = PAGE_PAUSE;
    pick = 0;
    audio_pause();
    return;
  }
  play_update();
}

static void pause_resume() {
  page = PAGE_PLAY;
  audio_resume();
}

static void pause_update() {
  poll();
  pick_move(3);
  if (pressed_b()) {
    if (pick == 0) pause_resume();
    else if (pick == 1) play_open();
    else {
      audio_stop();
      setlist_open();
    }
  } else if (arduboy.justPressed(A_BUTTON)) {
    pause_resume();
  }
}

// the highway as it stands, and a box over it
static void pause_draw() {
  play_draw();
  arduboy.fillRect(22, 4, 84, 56, BLACK);
  arduboy.drawRect(22, 4, 84, 56, WHITE);
  arduboy.fillRect(23, 5, 82, UI_BAR_H, WHITE);
  ui_text_center(64, 6, HDR_PAUSED, true, true);
  for (uint8_t i = 0; i < 3; i++)
    ui_item_at(34, 19 + i * PAUSE_ROW, 68, (const char *)pgm_read_dword(&PAUSE_NAMES[i]), true, pick == i, PAUSE_ROW);
}

// ---------------------------------------------------------------- the results
static void results_update() {
  poll();
  if (pressed_b() || arduboy.justPressed(A_BUTTON)) setlist_open();
}

// NOTES 51/80, centred, into the line
static void notes_line(char *line) {
  strcpy_P(line, PSTR("NOTES "));
  utoa(hits, line + strlen(line), 10);
  strcat_P(line, PSTR("/"));
  utoa(total_notes, line + strlen(line), 10);
}

static void small_center(uint8_t y, const char *line) {
  ui_small(64 - 3 * strlen(line), y);
  arduboy.print(line);
}

// booed off: the title in the logo's style (off the chip), how far the song
// got, PRESS B halfway between that and the crowd, and the title page's
// crowd booing under the light at the front of the stage, as low as on the
// title page (the spotlights on as the page opens, then out one after the
// other). No score is kept.
static void failed_draw() {
  crowd_booing(fail_frame);
  fx_sprite(FX_BOOED, (128 - BOOED_W) / 2, 1, BOOED_W, BOOED_PAGES);
  char line[24];
  strcpy_P(line, PSTR("AT "));
  utoa(fail_pct, line + strlen(line), 10);
  strcat_P(line, PSTR("% OF THE SONG"));
  small_center(BOOED_H + 3, line);
  if ((title_frame & 31) < 24) {        // blinking: on for 24 frames, off for 8; halfway between the
    arduboy.setCursor(64 - 3 * 7, 39);  // line above and the crowd, seven rows of black over and under it
    arduboy.print((const __FlashStringHelper *)TXT_PRESS_B);
  }
  title_frame++;
  if (fail_frame < 255) fail_frame++;
}

static void results_draw() {
  arduboy.clear();
  title_frame++;
  ui_header(song_text, false);
  ui_stars(43, 15, song_stars, 9);
  char line[24];
  ultoa(score, line, 10);
  ui_text_center(64, 24, line, false);
  // NOTES 159/168 94%, centred
  notes_line(line);
  if (total_notes) {
    strcat_P(line, PSTR(" "));
    utoa((uint16_t)((uint32_t)hits * 100 / total_notes), line + strlen(line), 10);
    strcat_P(line, PSTR("%"));
  }
  small_center(37, line);
  // the streak under it, and NEW BEST! blinking or PRESS B under that: the
  // lines of small print ten rows apart
  strcpy_P(line, PSTR("STREAK "));
  utoa(best_streak, line + strlen(line), 10);
  small_center(47, line);
  if (new_best) {
    if ((title_frame & 31) < 24) {
      arduboy.setCursor(64 - 3 * 9, 57);
      arduboy.print((const __FlashStringHelper *)TXT_NEW_BEST);
    }
  } else {
    arduboy.setCursor(64 - 3 * 7, 57);
    arduboy.print((const __FlashStringHelper *)TXT_PRESS_B);
  }
}
