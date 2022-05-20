#include <Arduino.h>
#include <Sprites.h>
#include <Print.h>
#include "Font4x6.h"

#define USE_LOWER_CASE

#define FONT4x6_WIDTH 4
#define FONT4x6_HEIGHT 7

#define CHAR_EXCLAMATION 33
#define CHAR_PERIOD 46
#define CHAR_LETTER_A 65
#define CHAR_LETTER_Z 90
#define CHAR_LETTER_A_LOWER 97
#define CHAR_LETTER_Z_LOWER 122
#define CHAR_NUMBER_0 48
#define CHAR_NUMBER_9 57

#ifdef USE_LOWER_CASE
  #define FONT_EXCLAMATION_INDEX 62
  #define FONT_PERIOD_INDEX 63
  #define FONT_NUMBER_INDEX 52
#else
  #define FONT_EXCLAMATION_INDEX 36
  #define FONT_PERIOD_INDEX 37
  #define FONT_NUMBER_INDEX 26
#endif

const uint8_t PROGMEM font_images[] = {
  4, 8,

  // #65 Letter 'A'.
  0x3E,  // ░░▓▓▓▓▓░
  0x09,  // ░░░░▓░░▓
  0x09,  // ░░░░▓░░▓
  0x3E,  // ░░▓▓▓▓▓░

  // #66 Letter 'B'.
  0x3F,  // ░░▓▓▓▓▓▓
  0x25,  // ░░▓░░▓░▓
  0x25,  // ░░▓░░▓░▓
  0x1A,  // ░░░▓▓░▓░

  // #67 Letter 'C'.
  0x1E,  // ░░░▓▓▓▓░
  0x21,  // ░░▓░░░░▓
  0x21,  // ░░▓░░░░▓
  0x12,  // ░░░▓░░▓░

  // #68 Letter 'D'.
  0x3F,  // ░░▓▓▓▓▓▓
  0x21,  // ░░▓░░░░▓
  0x21,  // ░░▓░░░░▓
  0x1E,  // ░░░▓▓▓▓░

  // #69 Letter 'E'.
  0x3F,  // ░░▓▓▓▓▓▓
  0x25,  // ░░▓░░▓░▓
  0x25,  // ░░▓░░▓░▓
  0x21,  // ░░▓░░░░▓

  // #70 Letter 'F'.
  0x3F,  // ░░▓▓▓▓▓▓
  0x05,  // ░░░░░▓░▓
  0x05,  // ░░░░░▓░▓
  0x01,  // ░░░░░░░▓

  // #71 Letter 'G'.
  0x1E,  // ░░░▓▓▓▓░
  0x21,  // ░░▓░░░░▓
  0x29,  // ░░▓░▓░░▓
  0x3A,  // ░░▓▓▓░▓░

  // #72 Letter 'H'.
  0x3F,  // ░░▓▓▓▓▓▓
  0x04,  // ░░░░░▓░░
  0x04,  // ░░░░░▓░░
  0x3F,  // ░░▓▓▓▓▓▓

  // #73 Letter 'I'.
  0x21,  // ░░▓░░░░▓
  0x3F,  // ░░▓▓▓▓▓▓
  0x21,  // ░░▓░░░░▓
  0x00,  // ░░░░░░░░

  // #74 Letter 'J'.
  0x10,  // ░░░▓░░░░
  0x21,  // ░░▓░░░░▓
  0x21,  // ░░▓░░░░▓
  0x1F,  // ░░░▓▓▓▓▓

  // #75 Letter 'K'.
  0x3F,  // ░░▓▓▓▓▓▓
  0x04,  // ░░░░░▓░░
  0x0A,  // ░░░░▓░▓░
  0x31,  // ░░▓▓░░░▓

  // #76 Letter 'L'.
  0x3F,  // ░░▓▓▓▓▓▓
  0x20,  // ░░▓░░░░░
  0x20,  // ░░▓░░░░░
  0x20,  // ░░▓░░░░░

  // #77 Letter 'M'.
  0x3F,  // ░░▓▓▓▓▓▓
  0x02,  // ░░░░░░▓░
  0x02,  // ░░░░░░▓░
  0x3F,  // ░░▓▓▓▓▓▓

  // #78 Letter 'N'.
  0x3F,  // ░░▓▓▓▓▓▓
  0x02,  // ░░░░░░▓░
  0x04,  // ░░░░░▓░░
  0x3F,  // ░░▓▓▓▓▓▓

  // #79 Letter 'O'.
  0x1E,  // ░░░▓▓▓▓░
  0x21,  // ░░▓░░░░▓
  0x21,  // ░░▓░░░░▓
  0x1E,  // ░░░▓▓▓▓░

  // #80 Letter 'P'.
  0x3F,  // ░░▓▓▓▓▓▓
  0x09,  // ░░░░▓░░▓
  0x09,  // ░░░░▓░░▓
  0x06,  // ░░░░░▓▓░

  // #81 Letter 'Q'.
  0x1E,  // ░░░▓▓▓▓░
  0x21,  // ░░▓░░░░▓
  0x11,  // ░░░▓░░░▓
  0x2E,  // ░░▓░▓▓▓░

  // #82 Letter 'R'.
  0x3F,  // ░░▓▓▓▓▓▓
  0x09,  // ░░░░▓░░▓
  0x09,  // ░░░░▓░░▓
  0x36,  // ░░▓▓░▓▓░

  // #83 Letter 'S'.
  0x22,  // ░░▓░░░▓░
  0x25,  // ░░▓░░▓░▓
  0x25,  // ░░▓░░▓░▓
  0x19,  // ░░░▓▓░░▓

  // #84 Letter 'T'.
  0x01,  // ░░░░░░░▓
  0x3F,  // ░░▓▓▓▓▓▓
  0x01,  // ░░░░░░░▓
  0x01,  // ░░░░░░░▓

  // #85 Letter 'U'.
  0x1F,  // ░░░▓▓▓▓▓
  0x20,  // ░░▓░░░░░
  0x20,  // ░░▓░░░░░
  0x1F,  // ░░░▓▓▓▓▓

  // #86 Letter 'V'.
  0x0F,  // ░░░░▓▓▓▓
  0x10,  // ░░░▓░░░░
  0x20,  // ░░▓░░░░░
  0x1F,  // ░░░▓▓▓▓▓

  // #87 Letter 'W'.
  0x3F,  // ░░▓▓▓▓▓▓
  0x10,  // ░░░▓░░░░
  0x10,  // ░░░▓░░░░
  0x3F,  // ░░▓▓▓▓▓▓

  // #88 Letter 'X'.
  0x3B,  // ░░▓▓▓░▓▓
  0x04,  // ░░░░░▓░░
  0x04,  // ░░░░░▓░░
  0x3B,  // ░░▓▓▓░▓▓

  // #89 Letter 'Y'.
  0x03,  // ░░░░░░▓▓
  0x04,  // ░░░░░▓░░
  0x38,  // ░░▓▓▓░░░
  0x07,  // ░░░░░▓▓▓

  // #90 Letter 'Z'.
  0x31,  // ░░▓▓░░░▓
  0x2D,  // ░░▓░▓▓░▓
  0x23,  // ░░▓░░░▓▓
  0x21,  // ░░▓░░░░▓

  #ifdef USE_LOWER_CASE

    // #97 Letter 'a'.
    0x10,  // ░░░▓░░░░
    0x2A,  // ░░▓░▓░▓░
    0x2A,  // ░░▓░▓░▓░
    0x3C,  // ░░▓▓▓▓░░

    // #98 Letter 'b'.
    0x3F,  // ░░▓▓▓▓▓▓
    0x24,  // ░░▓░░▓░░
    0x24,  // ░░▓░░▓░░
    0x18,  // ░░░▓▓░░░

    // #99 Letter 'c'.
    0x1C,  // ░░░▓▓▓░░
    0x22,  // ░░▓░░░▓░
    0x22,  // ░░▓░░░▓░
    0x14,  // ░░░▓░▓░░

    // #100 Letter 'd'.
    0x18,  // ░░░▓▓░░░
    0x24,  // ░░▓░░▓░░
    0x24,  // ░░▓░░▓░░
    0x3F,  // ░░▓▓▓▓▓▓

    // #101 Letter 'e'.
    0x1C,  // ░░░▓▓▓░░
    0x2A,  // ░░▓░▓░▓░
    0x2A,  // ░░▓░▓░▓░
    0x2C,  // ░░▓░▓▓░░

    // #102 Letter 'f'.
    0x04,  // ░░░░░▓░░
    0x7E,  // ░▓▓▓▓▓▓░
    0x05,  // ░░░░░▓░▓
    0x01,  // ░░░░░░░▓

    // #103 Letter 'g'.
    0x4C,  // ░▓░░▓▓░░
    0x52,  // ░▓░▓░░▓░
    0x52,  // ░▓░▓░░▓░
    0x3E,  // ░░▓▓▓▓▓░

    // #104 Letter 'h'.
    0x3F,  // ░░▓▓▓▓▓▓
    0x04,  // ░░░░░▓░░
    0x04,  // ░░░░░▓░░
    0x38,  // ░░▓▓▓░░░

    // #105 Letter 'i'.
    0x24,  // ░░▓░░▓░░
    0x3D,  // ░░▓▓▓▓░▓
    0x20,  // ░░▓░░░░░
    0x00,  // ░░░░░░░░

    // #106 Letter 'j'.
    0x40,  // ░▓░░░░░░
    0x40,  // ░▓░░░░░░
    0x44,  // ░▓░░░▓░░
    0x3D,  // ░░▓▓▓▓░▓

    // #107 Letter 'k'.
    0x3F,  // ░░▓▓▓▓▓▓
    0x08,  // ░░░░▓░░░
    0x14,  // ░░░▓░▓░░
    0x22,  // ░░▓░░░▓░

    // #108 Letter 'l'.
    0x21,  // ░░▓░░░░▓
    0x3F,  // ░░▓▓▓▓▓▓
    0x20,  // ░░▓░░░░░
    0x00,  // ░░░░░░░░

    // #109 Letter 'm'.
    0x3E,  // ░░▓▓▓▓▓░
    0x04,  // ░░░░░▓░░
    0x04,  // ░░░░░▓░░
    0x3E,  // ░░▓▓▓▓▓░

    // #110 Letter 'n'.
    0x3E,  // ░░▓▓▓▓▓░
    0x04,  // ░░░░░▓░░
    0x02,  // ░░░░░░▓░
    0x3C,  // ░░▓▓▓▓░░

    // #111 Letter 'o'.
    0x1C,  // ░░░▓▓▓░░
    0x22,  // ░░▓░░░▓░
    0x22,  // ░░▓░░░▓░
    0x1C,  // ░░░▓▓▓░░

    // #112 Letter 'p'.
    0x7E,  // ░▓▓▓▓▓▓░
    0x22,  // ░░▓░░░▓░
    0x22,  // ░░▓░░░▓░
    0x1C,  // ░░░▓▓▓░░

    // #113 Letter 'q'.
    0x1C,  // ░░░▓▓▓░░
    0x22,  // ░░▓░░░▓░
    0x22,  // ░░▓░░░▓░
    0x7E,  // ░▓▓▓▓▓▓░

    // #114 Letter 'r'.
    0x3E,  // ░░▓▓▓▓▓░
    0x04,  // ░░░░░▓░░
    0x02,  // ░░░░░░▓░
    0x04,  // ░░░░░▓░░

    // #115 Letter 's'.
    0x24,  // ░░▓░░▓░░
    0x2A,  // ░░▓░▓░▓░
    0x2A,  // ░░▓░▓░▓░
    0x12,  // ░░░▓░░▓░

    // #116 Letter 't'.
    0x02,  // ░░░░░░▓░
    0x1F,  // ░░░▓▓▓▓▓
    0x22,  // ░░▓░░░▓░
    0x20,  // ░░▓░░░░░

    // #117 Letter 'u'.
    0x1E,  // ░░░▓▓▓▓░
    0x20,  // ░░▓░░░░░
    0x20,  // ░░▓░░░░░
    0x1E,  // ░░░▓▓▓▓░

    // #118 Letter 'v'.
    0x0E,  // ░░░░▓▓▓░
    0x10,  // ░░░▓░░░░
    0x20,  // ░░▓░░░░░
    0x1E,  // ░░░▓▓▓▓░

    // #119 Letter 'w'.
    0x3E,  // ░░▓▓▓▓▓░
    0x10,  // ░░░▓░░░░
    0x10,  // ░░░▓░░░░
    0x3E,  // ░░▓▓▓▓▓░

    // #120 Letter 'x'.
    0x36,  // ░░▓▓░▓▓░
    0x08,  // ░░░░▓░░░
    0x08,  // ░░░░▓░░░
    0x36,  // ░░▓▓░▓▓░

    // #121 Letter 'y'.
    0x4E,  // ░▓░░▓▓▓░
    0x50,  // ░▓░▓░░░░
    0x50,  // ░▓░▓░░░░
    0x3E,  // ░░▓▓▓▓▓░

    // #122 Letter 'z'.
    0x32,  // ░░▓▓░░▓░
    0x2A,  // ░░▓░▓░▓░
    0x26,  // ░░▓░░▓▓░
    0x22,  // ░░▓░░░▓░

  #endif

  // #48 Number '0'.
  0x1E,  // ░░░▓▓▓▓░
  0x29,  // ░░▓░▓░░▓
  0x25,  // ░░▓░░▓░▓
  0x1E,  // ░░░▓▓▓▓░

  // #49 Number '1'.
  0x22,  // ░░▓░░░▓░
  0x3F,  // ░░▓▓▓▓▓▓
  0x20,  // ░░▓░░░░░
  0x00,  // ░░░░░░░░

  // #50 Number '2'.
  0x32,  // ░░▓▓░░▓░
  0x29,  // ░░▓░▓░░▓
  0x29,  // ░░▓░▓░░▓
  0x26,  // ░░▓░░▓▓░

  // #51 Number '3'.
  0x12,  // ░░░▓░░▓░
  0x21,  // ░░▓░░░░▓
  0x25,  // ░░▓░░▓░▓
  0x1A,  // ░░░▓▓░▓░

  // #52 Number '4'.
  0x0C,  // ░░░░▓▓░░
  0x0A,  // ░░░░▓░▓░
  0x3F,  // ░░▓▓▓▓▓▓
  0x08,  // ░░░░▓░░░

  // #53 Number '5'.
  0x17,  // ░░░▓░▓▓▓
  0x25,  // ░░▓░░▓░▓
  0x25,  // ░░▓░░▓░▓
  0x19,  // ░░░▓▓░░▓

  // #54 Number '6'.
  0x1E,  // ░░░▓▓▓▓░
  0x25,  // ░░▓░░▓░▓
  0x25,  // ░░▓░░▓░▓
  0x18,  // ░░░▓▓░░░

  // #55 Number '7'.
  0x01,  // ░░░░░░░▓
  0x39,  // ░░▓▓▓░░▓
  0x05,  // ░░░░░▓░▓
  0x03,  // ░░░░░░▓▓

  // #56 Number '8'.
  0x1A,  // ░░░▓▓░▓░
  0x25,  // ░░▓░░▓░▓
  0x25,  // ░░▓░░▓░▓
  0x1A,  // ░░░▓▓░▓░

  // #57 Number '9'.
  0x06,  // ░░░░░▓▓░
  0x29,  // ░░▓░▓░░▓
  0x29,  // ░░▓░▓░░▓
  0x1E,  // ░░░▓▓▓▓░

  // #33 Symbol '!'.
  0x00,  // ░░░░░░░░
  0x2F,  // ░░▓░▓▓▓▓
  0x00,  // ░░░░░░░░
  0x00,  // ░░░░░░░░

  // #46 Symbol '.'.
  0x00,  // ░░░░░░░░
  0x20,  // ░░▓░░░░░
  0x00,  // ░░░░░░░░
  0x00   // ░░░░░░░░

};


Font4x6::Font4x6(uint8_t lineSpacing) {

  _lineHeight = lineSpacing;
  _letterSpacing = 1;

  _cursorX = _cursorY = _baseX = 0;
  _textColor = 1;

}

size_t Font4x6::write(uint8_t c) {

  if (c == '\n')      { _cursorX = _baseX; _cursorY += _lineHeight; }
  else if (c == ' ')      { _cursorX += 2 + _letterSpacing; }
  else {

    printChar(c, _cursorX, _cursorY);
    _cursorX += FONT4x6_WIDTH + _letterSpacing;

  }

  return 1;

}

void Font4x6::printChar(const char c, const int8_t x, int8_t y) {

  int8_t idx = -1;

  ++y;

  switch (c) {
    
    case CHAR_LETTER_A ... CHAR_LETTER_Z:
      idx = c - CHAR_LETTER_A;
      break;

#ifdef USE_LOWER_CASE    
    case CHAR_LETTER_A_LOWER ... CHAR_LETTER_Z_LOWER:
      idx = c - CHAR_LETTER_A_LOWER + 26;
      break;
#endif

    case CHAR_NUMBER_0 ... CHAR_NUMBER_9:
      idx = c - CHAR_NUMBER_0 + FONT_NUMBER_INDEX;
      break;
      
    case CHAR_EXCLAMATION:
      idx = FONT_EXCLAMATION_INDEX;
      break;
      
    case CHAR_PERIOD:
      idx = FONT_PERIOD_INDEX;
      break;

  }

  if (idx > -1) {
    
    if (_textColor == WHITE) {
      Sprites::drawSelfMasked(x, y, font_images, idx);
    }
    else {
      Sprites::drawErase(x, y, font_images, idx);
    }

  }

}

void Font4x6::setCursor(const int8_t x, const int8_t y) {
  _cursorX = _baseX = x;
  _cursorY = y;
}

void Font4x6::setTextColor(const uint8_t color){
  _textColor = color;
}

void Font4x6::setHeight(const uint8_t color){
  _lineHeight = color;
}
