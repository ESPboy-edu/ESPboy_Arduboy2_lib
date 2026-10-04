// Pocket Stage - the look of the menus: the bold font (font.h, from Impact),
// a bar across the top with the page's name, items in boxes with the pick
// beside the one the cursor is on, and stars.
#pragma once
#include "font.h"

#define UI_BAR_H 13                    // the header bar: the font and a row above and below it
#define UI_ITEM_H 12                   // an item's box: the font and a row above it; a page with
                                       // room gives it a row below as well (UI_ITEM_H + 1)

static const uint8_t *ui_glyph(char c) {
  if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
  uint16_t off = 0xFFFF;
  if (c >= FONT_FIRST && c <= FONT_LAST) off = pgm_read_word(&FONT_OFFSET[c - FONT_FIRST]);
  if (off == 0xFFFF) off = 0;                    // not in the font: a space
  return FONT_DATA + off;
}

static inline char ui_char(const char *s, bool pm) {
  return pm ? (char)pgm_read_byte(s) : *s;
}

static uint8_t ui_width(const char *s, bool pm) {
  uint8_t w = 0;
  for (char c; (c = ui_char(s, pm)); s++) w += pgm_read_byte(ui_glyph(c)) + 1;
  return w ? w - 1 : 0;
}

// the text in white, or in black on something white
static void ui_text(int16_t x, int16_t y, const char *s, bool pm, bool black = false) {
  for (char c; (c = ui_char(s, pm)); s++) {
    const uint8_t *g = ui_glyph(c);
    if (black) Sprites::drawErase(x, y, g, 0);
    else Sprites::drawSelfMasked(x, y, g, 0);
    x += pgm_read_byte(g) + 1;
  }
}

static void ui_text_center(int16_t cx, int16_t y, const char *s, bool pm, bool black = false) {
  ui_text(cx - ui_width(s, pm) / 2, y, s, pm, black);
}

static void ui_text_right(int16_t rx, int16_t y, const char *s, bool pm, bool black = false) {
  ui_text(rx - ui_width(s, pm), y, s, pm, black);
}

// the bar across the top with the page's name
static void ui_header(const char *s, bool pm = true) {
  arduboy.fillRect(0, 0, 128, UI_BAR_H, WHITE);
  ui_text_center(64, 1, s, pm, true);
}

// a menu item: a box of its own, white with the text in black and the pick
// beside it when the cursor is on it, else black with the text in white;
// the box across the screen (room for a value at the end) or just around
// the text, so the stage shows around it. The white box is h tall: with
// UI_ITEM_H + 1 the letters have a row of white under them too, so they do
// not run into the black below; four items under a bar have no room for
// that, there the box is UI_ITEM_H and the letters sit on its edge
static void ui_item_at(uint8_t x, uint8_t y, uint8_t w, const char *s, bool pm, bool picked, uint8_t h = UI_ITEM_H) {
  if (picked) {
    arduboy.fillRect(x, y, w, h, WHITE);
    Sprites::drawSelfMasked(x - 8, y + 2, PICK, 0);
  } else {
    arduboy.fillRect(x, y + 1, w, FONT_H, BLACK);
  }
  ui_text(x + 5, y + 1, s, pm, picked);
}

static void ui_item(uint8_t y, const char *s, bool pm, bool picked, bool wide = true, uint8_t h = UI_ITEM_H) {
  ui_item_at(10, y, wide ? 118 : ui_width(s, pm) + 10, s, pm, picked, h);
}

// what an item is set to, at its right end (ON, OFF...)
static void ui_value(uint8_t y, const char *s_P, bool picked) {
  ui_text_right(124, y + 1, s_P, true, picked);
}

// so many stars of five, the rest hollow
static void ui_stars(int16_t x, int16_t y, uint8_t n, uint8_t step = 8, bool black = false) {
  for (uint8_t i = 0; i < 5; i++) {
    const uint8_t *s = i < n ? STAR : STAR_HOLLOW;
    if (black) Sprites::drawErase(x, y, s, 0);
    else Sprites::drawSelfMasked(x, y, s, 0);
    x += step;
  }
}

// the small print: the library's font, white on black or the other way round
static void ui_small(int16_t x, int16_t y, bool black = false) {
  arduboy.setTextColor(black ? BLACK : WHITE);
  arduboy.setTextBackground(black ? WHITE : BLACK);
  arduboy.setCursor(x, y);
}
