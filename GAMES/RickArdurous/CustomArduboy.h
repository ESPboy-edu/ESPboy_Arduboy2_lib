#ifndef _CUSTOM_ARDUBOY_H_
#define _CUSTOM_ARDUBOY_H_

// include the Arduboy2
#include "Arduboy2.h"

// this function is defined in Arduboy_Audio
extern unsigned int NoteToFrequency(byte note);

// wrap the missing function in a derivated class
class Arduboy : public Arduboy2
{
public:
	void drawChar(uint8_t x, uint8_t y, unsigned char c, uint8_t color, uint8_t bg) { Arduboy2::drawChar(x, y, c, color, bg, 1); }
};


static constexpr uint8_t TRANSPARENT = 3;

class CustomArduboy : public Arduboy
{
public:
	unsigned int drawBitmapExtended(int8_t x, int8_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color, bool mirrorX);
	bool CheckWhitePixelsInRow(uint8_t x, uint8_t y, uint8_t w);
	uint8_t GetCursorX() { return cursor_x; }
	uint8_t GetCursorY() { return cursor_y; }
};

#endif
