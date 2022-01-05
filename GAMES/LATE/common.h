#pragma once
#include <Arduboy2.h>

#define MENU_EXIT 0
#define MENU_NEW 1
#define MENU_RESUME 2

const uint8_t digitsBmp[][3] PROGMEM = {
  {0b11111, 0b10001, 0b11111},
  {0b10010, 0b11111, 0b10000},
  {0b11101, 0b10101, 0b10111},
  {0b10001, 0b10101, 0b11111},
  {0b00111, 0b00100, 0b11111},
  {0b10111, 0b10101, 0b11101},
  {0b11111, 0b10101, 0b11101},
  {0b00001, 0b11101, 0b00111},
  {0b11111, 0b10101, 0b11111},
  {0b10111, 0b10101, 0b11111}
};

const uint8_t cupBmp[] PROGMEM = {
	0b00000110,
	0b01001001,
	0b01011111,
	0b01111111,
	0b01111111,
	0b01011111,
	0b01001001,
	0b00000110
};

const uint8_t dPadBmp[] PROGMEM = {
	0b00011100,
	0b00011100,
	0b01101011,
	0b01110111,
	0b01101011,
	0b00011100,
	0b00011100
};

const uint8_t aBmp[] PROGMEM = {
	0b00011100,
	0b00111110,
	0b01000011,
	0b01110101,
	0b01000011,
	0b00111110,
	0b00011100
};

const uint8_t bBmp[] PROGMEM = {
	0b00011100,
	0b00111110,
	0b01000001,
	0b01010101,
	0b01101011,
	0b00111110,
	0b00011100
};

void drawNumber(Arduboy2 *gr, uint8_t x, uint8_t y, uint32_t number, uint8_t color, uint8_t padding);

int8_t buttonPressed(Arduboy2 *gr);
int8_t buttonReleased(Arduboy2 *gr);