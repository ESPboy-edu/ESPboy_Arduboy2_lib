// PC stand-in for Arduboy2: just enough API to run the sketch headless.
#pragma once
#define ARDUBOY_NO_USB   /* the PC stand-in has no USB either way */
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#define PROGMEM
#define pgm_read_byte(p) (*(const uint8_t*)(p))
#define pgm_read_word(p) (*(const uint16_t*)(p))
#define pgm_read_ptr(p) ((void*)*(p))
#define memcpy_P memcpy
#define strlen_P strlen
#define strcpy_P strcpy
#define strcat_P strcat
#define PSTR(s) (s)
#define CLEAR_BUFFER true
class __FlashStringHelper;
#define F(s) (reinterpret_cast<const __FlashStringHelper*>(s))
inline long random(long n) { return rand() % n; }
enum { BLACK = 0, WHITE = 1 };
enum { RED_LED = 1, GREEN_LED = 2, BLUE_LED = 4, RGB_ON = 1, RGB_OFF = 0 };
enum { RIGHT_BUTTON = 64, LEFT_BUTTON = 32, UP_BUTTON = 128, DOWN_BUTTON = 16, A_BUTTON = 8, B_BUTTON = 4 };
#include "font.h"
struct Audio {
  static bool& st() { static bool on = true; return on; }
  static bool enabled() { return st(); }
  static void on() { st() = true; } static void off() { st() = false; } static void saveOnOff() {}
  static void begin() {}
};
#define EEPROM_STORAGE_SPACE_START 16
struct Arduboy2Base {
  Audio audio;
  void boot() {} void flashlight() {} void systemButtons() {} void waitNoButtons() {}
  uint8_t px[64][128], shown[64][128]; uint16_t frameCount = 0; uint8_t cur = 0, prev = 0, nextInput = 0;
  int16_t cx = 0, cy = 0; uint8_t tc = 1, tb = 0, ts = 1;
  void begin() {} void setFrameRate(int) {} void initRandomSeed() {} uint16_t generateRandomSeed() { return (uint16_t)rand(); }
  bool nextFrame() { frameCount++; return true; }
  void pollButtons() { prev = cur; cur = nextInput; }
  uint8_t buttonsState() { return cur; }
  bool justPressed(uint8_t b) { return !(prev & b) && (cur & b); }
  bool pressed(uint8_t b) { return (cur & b) == b; }
  void clear() { memset(px, 0, sizeof px); }
  void display() {}
  void drawPixel(int x, int y, uint8_t c = WHITE) { if (x >= 0 && y >= 0 && x < 128 && y < 64) px[y][x] = c; }
  void fillRect(int x, int y, int w, int h, uint8_t c = WHITE) { for (int j = 0; j < h; j++) for (int i = 0; i < w; i++) drawPixel(x + i, y + j, c); }
  void drawRect(int x, int y, int w, int h, uint8_t c = WHITE) { drawFastHLine(x, y, w, c); drawFastHLine(x, y + h - 1, w, c); drawFastVLine(x, y, h, c); drawFastVLine(x + w - 1, y, h, c); }
  void drawFastHLine(int x, int y, int w, uint8_t c = WHITE) { for (int i = 0; i < w; i++) drawPixel(x + i, y, c); }
  void drawFastVLine(int x, int y, int h, uint8_t c = WHITE) { for (int i = 0; i < h; i++) drawPixel(x, y + i, c); }
  void digitalWriteRGB(uint8_t, uint8_t) {}
  void drawLine(int x0, int y0, int x1, int y1, uint8_t c = WHITE) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1, dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1, e = dx + dy;
    for (;;) { drawPixel(x0, y0, c); if (x0 == x1 && y0 == y1) break; int e2 = 2 * e; if (e2 >= dy) { e += dy; x0 += sx; } if (e2 <= dx) { e += dx; y0 += sy; } }
  }
  void drawBitmap(int x, int y, const uint8_t* b, int w, int h, uint8_t c = WHITE) {
    for (int i = 0; i < w; i++) for (int j = 0; j < h; j++) if (b[(j / 8) * w + i] & (1 << (j & 7))) drawPixel(x + i, y + j, c);
  }
  void setCursor(int x, int y) { cx = x; cy = y; }
  void setTextColor(uint8_t c) { tc = c; } void setTextBackground(uint8_t c) { tb = c; } void setTextSize(uint8_t s) { ts = s; }
  void write(uint8_t ch) {
    for (int i = 0; i < 6; i++) { uint8_t col = i < 5 ? FONT5x7[ch * 5 + i] : 0;
      for (int j = 0; j < 8; j++) { uint8_t on = (col >> j) & 1; if (on || tb != tc) fillRect(cx + i * ts, cy + j * ts, ts, ts, on ? tc : tb); } }
    cx += 6 * ts;
  }
  void print(const char* s) { while (*s) write(*s++); }
  void print(const __FlashStringHelper* s) { print((const char*)s); }
  void print(char c) { write(c); }
  void print(unsigned v) { char b[8]; snprintf(b, 8, "%u", v); print(b); }
  void print(int v) { char b[8]; snprintf(b, 8, "%d", v); print(b); }
  void print(uint8_t v) { print((unsigned)v); }
};
