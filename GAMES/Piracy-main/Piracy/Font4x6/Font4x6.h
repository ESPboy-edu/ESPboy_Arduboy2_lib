//
// Modified source
// Origin at https://github.com/filmote/Font4x6
//

#pragma once

#include <Arduboy2.h>
#include <Print.h>

#define FONT4x6_WIDTH 4
#define FONT4x6_HEIGHT 7

#define CHAR_AT 64
#define CHAR_COLON 58
#define CHAR_COMMA 44
#define CHAR_LESS 60
#define CHAR_GREATER 62
#define CHAR_MONEY 36
#define CHAR_MINUS 45
#define CHAR_PARENTH_OPEN 40
#define CHAR_PARENTH_CLOSE 41
#define CHAR_EXCLAMATION 33
#define CHAR_PERIOD 46
#define CHAR_LETTER_A 65
#define CHAR_LETTER_Z 90
#define CHAR_LETTER_A_LOWER 97
#define CHAR_LETTER_Z_LOWER 122
#define CHAR_NUMBER_0 48
#define CHAR_NUMBER_9 57

#define FONT_AT_INDEX 46
#define FONT_COLON_INDEX 45
#define FONT_COMMA_INDEX 44
#define FONT_LESS_INDEX 42
#define FONT_GREATER_INDEX 43
#define FONT_MONEY_INDEX 41
#define FONT_MINUS_INDEX 40
#define FONT_PARENTH_OPEN_INDEX 38
#define FONT_PARENTH_CLOSE_INDEX 39
#define FONT_EXCLAMATION_INDEX 36
#define FONT_PERIOD_INDEX 37
#define FONT_NUMBER_INDEX 26


const uint8_t PROGMEM font_images[] = 
{
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
    0x07,  // ░░░░░▓▓▓
    0x28,  // ░░▓░▓░░
    0x28,  // ░░▓ ▓░░░
    0x1f,  // ░░░▓▓▓▓▓

    // #90 Letter 'Z'.
    0x31,  // ░░▓▓░░░▓
    0x2D,  // ░░▓░▓▓░▓
    0x23,  // ░░▓░░░▓▓
    0x21,  // ░░▓░░░░▓

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
    0x00,  // ░░░░░░░░

    // #40 Symbol '('.
    0x00,  // ░░░░░░░░
    0x1E,  // ░░░▓▓▓▓░
    0x21,  // ░░▓░░░░▓
    0x00,  // ░░░░░░░░

    // #41 Symbol ')'.
    0x00,  // ░░░░░░░░
    0x21,  // ░░▓░░░░▓    
    0x1E,  // ░░░▓▓▓▓░
    0x00,  // ░░░░░░░░

    // #45 Symbol '-'.
    0x00,  // ░░░░░░░░
    0x08,  // ░░░░▓░░░
    0x08,  // ░░░░▓░░░
    0x00,  // ░░░░░░░░

    // #36 Symbol '$'.
    0x14,  // ░░░▓░▓░░
    0x3E,  // ░░▓▓▓▓▓░
    0x14,  // ░░░▓░▓░░
    0x00,  // ░░░░░░░░

    // #60 Symbol '<'.
    0x08,  // ░░░░▓░░░
    0x1c,  // ░░░▓▓▓░░
    0x3E,  // ░░▓▓▓▓▓░
    0x7F,  // ░▓▓▓▓▓▓▓

    // #62 Symbol '>'.
    0x7F,  // ░▓▓▓▓▓▓▓
    0x3E,  // ░░▓▓▓▓▓░
    0x1c,  // ░░░▓▓▓░░
    0x08,  // ░░░░▓░░░

    // #44 Symbol ','.
    0x00,  // ░░░░░░░░
    0x40,  // ░▓░░░░░░
    0x20,  // ░░▓░░░░░
    0x00,  // ░░░░░░░░

    // #58 Symbol ':'.
    0x00,  // ░░░░░░░░
    0x24,  // ░░▓░░▓░░
    0x00,  // ░░░░░░░░
    0x00,  // ░░░░░░░░

    // #64 Symbol '@'.
    0x26,  // ░░▓░░▓▓░
    0x3f,  // ░░▓▓▓▓▓▓
    0x3f,  // ░░▓▓▓▓▓▓
    0x26   // ░░▓░░▓▓░
};

const uint8_t glyph_mask[] = { 
    0xff, 0xff, 0xff, 0xff
};  


class Font4x6 : public Print 
{
    private:
        int8_t _cursorX;        // Default is 0.
        int8_t _baseX;          // needed for linebreak.
        int8_t _cursorY;        // Default is 0.
        int8_t _textColor;      // BLACK == 0, everything else is WHITE. Default is WHITE.
    
    public:
        Font4x6() {      
            _cursorX = _cursorY = _baseX = 0;
            _textColor = 1; 
        }

    virtual size_t write(uint8_t c) { // used by the Arduino Print class
        if (c == '\n') { 
            _cursorX = _baseX; 
            _cursorY += 11;
        }
        else if (c == 32) {
            _cursorX += 3;
        }
        else if (c == 95) {
            _cursorX += FONT4x6_WIDTH + 1;
        }
        else {       
            printChar(c, _cursorX, _cursorY);
            _cursorX += FONT4x6_WIDTH + 1; //_letterSpacing;       
        }      
        return 1;
    }
    
    void printChar(const char c, const int8_t x, int8_t y) {
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

            case CHAR_PARENTH_OPEN:
                idx = FONT_PARENTH_OPEN_INDEX;
                break;

            case CHAR_PARENTH_CLOSE:
                idx = FONT_PARENTH_CLOSE_INDEX;
                break;

            case CHAR_MINUS:
                idx = FONT_MINUS_INDEX;
                break;

            case CHAR_MONEY:
                idx = FONT_MONEY_INDEX;
                break;

            case CHAR_LESS:
                idx = FONT_LESS_INDEX;
                break;

            case CHAR_GREATER:
                idx = FONT_GREATER_INDEX;
                break;

            case CHAR_COMMA:
                idx = FONT_COMMA_INDEX;
                break;

            case CHAR_COLON:
                idx = FONT_COLON_INDEX;
                break;

            case CHAR_AT:
                idx = FONT_AT_INDEX;
                break;

            default: break;      
        }
        
        if (idx > -1) {       
            Sprites::drawExternalMask(x, y, font_images, glyph_mask, idx, 0);
        }
    }

    void setCursor(const int8_t x, const int8_t y) {
        _cursorX = _baseX = x;
        _cursorY = y;
    }

};
