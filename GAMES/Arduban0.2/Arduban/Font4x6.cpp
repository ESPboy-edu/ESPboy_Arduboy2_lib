/*

BSD 3-Clause License

Copyright (c) 2018, Filmote
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

From https://github.com/filmote/Font4x6

*/#include <Arduino.h>
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
#define CHAR_COLON 58
#define CHAR_LESS_THAN 60
#define CHAR_GREATER_THAN 62
#define CHAR_QUESTION_MARK 63

#ifdef USE_LOWER_CASE
  #define FONT_EXCLAMATION_INDEX 62
  #define FONT_PERIOD_INDEX 63
  #define FONT_LESS_THAN_INDEX 64
  #define FONT_GREATER_THAN_INDEX 65
  #define FONT_QUESTION_MARK_INDEX 66
  #define FONT_COLON_INDEX 67
  #define FONT_NUMBER_INDEX 52
#else
  #define FONT_EXCLAMATION_INDEX 36
  #define FONT_PERIOD_INDEX 37
  #define FONT_LESS_THAN_INDEX 38
  #define FONT_GREATER_THAN_INDEX 39
  #define FONT_QUESTION_MARK_INDEX 40
  #define FONT_COLON_INDEX 41
  #define FONT_NUMBER_INDEX 26
#endif

const uint8_t PROGMEM font_images[] = {
4, 8,
0x3E,	0x09,	0x09,	0x3E,	//A
0x3F,	0x25,	0x25,	0x1A,
0x1E,	0x21,	0x21,	0x12,
0x3F,	0x21,	0x21,	0x1E,
0x3F,	0x25,	0x25,	0x21,
0x3F,	0x05,	0x05,	0x01,
0x1E,	0x21,	0x29,	0x3A,
0x3F,	0x04,	0x04,	0x3F,
0x21,	0x3F,	0x21,	0x00,
0x10,	0x21,	0x21,	0x1F,
0x3F,	0x04,	0x0A,	0x31,
0x3F,	0x20,	0x20,	0x20,
0x3F,	0x02,	0x02,	0x3F,
0x3F,	0x02,	0x04,	0x3F,
0x1E,	0x21,	0x21,	0x1E,
0x3F,	0x09,	0x09,	0x06,
0x1E,	0x21,	0x11,	0x2E,
0x3F,	0x09,	0x09,	0x36,
0x22,	0x25,	0x25,	0x19,
0x01,	0x3F,	0x01,	0x01,
0x1F,	0x20,	0x20,	0x1F,
0x0F,	0x10,	0x20,	0x1F,
0x3F,	0x10,	0x10,	0x3F,
0x3B,	0x04,	0x04,	0x3B,
0x03,	0x04,	0x38,	0x07,
0x31,	0x2D,	0x23,	0x21, // Z
#ifdef USE_LOWER_CASE
0x10,	0x2A,	0x2A,	0x3C,	//a
0x3F,	0x24,	0x24,	0x18,
0x1C,	0x22,	0x22,	0x14,
0x18,	0x24,	0x24,	0x3F,
0x1C,	0x2A,	0x2A,	0x2C,
0x04,	0x7E,	0x05,	0x01,
0x4C,	0x52,	0x52,	0x3E,
0x3F,	0x04,	0x04,	0x38,
0x24,	0x3D,	0x20,	0x00,
0x40,	0x40,	0x44,	0x3D,
0x3F,	0x08,	0x14,	0x22,
0x21,	0x3F,	0x20,	0x00,
0x3E,	0x04,	0x04,	0x3E,
0x3E,	0x04,	0x02,	0x3C,
0x1C,	0x22,	0x22,	0x1C,
0x7E,	0x22,	0x22,	0x1C,
0x1C,	0x22,	0x22,	0x7E,
0x3E,	0x04,	0x02,	0x04,
0x24,	0x2A,	0x2A,	0x12,
0x02,	0x1F,	0x22,	0x20,
0x1E,	0x20,	0x20,	0x1E,
0x0E,	0x10,	0x20,	0x1E,
0x3E,	0x10,	0x10,	0x3E,
0x36,	0x08,	0x08,	0x36,
0x4E,	0x50,	0x50,	0x3E,
0x32,	0x2A,	0x26,	0x22, //z
#endif
0x1E,	0x29,	0x25,	0x1E,	//0
0x22,	0x3F,	0x20,	0x00,
0x32,	0x29,	0x29,	0x26,
0x12,	0x21,	0x25,	0x1A,
0x0C,	0x0A,	0x3F,	0x08,
0x17,	0x25,	0x25,	0x19,
0x1E,	0x25,	0x25,	0x18,
0x01,	0x39,	0x05,	0x03,
0x1A,	0x25,	0x25,	0x1A,
0x06,	0x29,	0x29,	0x1E,	//9
0x00,	0x2F,	0x00,	0x00,	//!
0x00,	0x20,	0x00,	0x00, //.
0x00, 0x04, 0x0A, 0x00, // <
0x00, 0x0A, 0x04, 0x00, // >
0x02, 0x01, 0x29, 0x06, // ?
0x00, 0x12, 0x00, 0x00, // :
};


Font4x6::Font4x6(uint8_t lineSpacing) {

  _lineHeight = lineSpacing;
  _letterSpacing = 1;

  _cursorX = _cursorY = _baseX = 0;
  _textColor = 1;

}

size_t Font4x6::write(uint8_t c) {

  if (c == '\n')      { _cursorX = _baseX; _cursorY += _lineHeight; }
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

    case CHAR_LESS_THAN:
      idx = FONT_LESS_THAN_INDEX;
      break;

    case CHAR_GREATER_THAN:
      idx = FONT_GREATER_THAN_INDEX;
      break;

    case CHAR_QUESTION_MARK:
      idx = FONT_QUESTION_MARK_INDEX;
      break;

    case CHAR_COLON:
      idx = FONT_COLON_INDEX;
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

