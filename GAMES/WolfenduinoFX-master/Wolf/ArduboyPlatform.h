#pragma once

#include <avr/pgmspace.h>
#include <ArduboyFX.h>      

#include "Platform.h"

extern Arduboy2Base arduboy;

extern int y_lut[];

inline uint8_t* GetScreenBuffer() { return arduboy.sBuffer; }

[[gnu::always_inline]]
inline void drawPixel(uint8_t x, uint8_t y, uint8_t color)
{
  uint16_t row_offset;
  uint8_t bit;
/*
  asm volatile
  (
    // bit = 1 << (y & 7)
    "ldi  %[bit], 1                    \n" //bit = 1;
    "sbrc %[y], 1                      \n" //if (y & _BV(1)) bit = 4;
    "ldi  %[bit], 4                    \n"
    "sbrc %[y], 0                      \n" //if (y & _BV(0)) bit = bit << 1;
    "lsl  %[bit]                       \n"
    "sbrc %[y], 2                      \n" //if (y & _BV(2)) bit = (bit << 4) | (bit >> 4);
    "swap %[bit]                       \n"
    //row_offset = y / 8 * WIDTH + x;
    "andi %A[y], 0xf8                  \n" //row_offset = (y & 0xF8) * WIDTH / 8
    "mul  %[width_offset], %A[y]       \n"
    "movw %[row_offset], r0            \n"
    "clr  __zero_reg__                 \n"
    "add  %A[row_offset], %[x]         \n" //row_offset += x
#if WIDTH != 128
    "adc  %B[row_offset], __zero_reg__ \n" // only non 128 width can overflow
#endif
    : [row_offset]   "=&x" (row_offset),   // upper register (ANDI)
      [bit]          "=&d" (bit),          // upper register (LDI)
      [y]            "+d"  (y)             // upper register (ANDI), must be writable
    : [width_offset] "r"   ((uint8_t)(WIDTH/8)),
      [x]            "r"   ((uint8_t)x)
    :
  );
  uint8_t data = arduboy.sBuffer[row_offset] | bit;
  if (!(color & _BV(0))) data ^= bit;
  arduboy.sBuffer[row_offset] = data;
  */
	arduboy.drawPixel(x, y, color);

	//if(colour)
	//{
	//	_displayBuffer[y_lut[y] + x] &= ~(0x01 << (y & 7));
	//}
	//else
	//{
	//	_displayBuffer[y_lut[y] + x] |= (0x01 << (y & 7));
	//}
}

[[gnu::always_inline]]
inline void setPixel(uint8_t x, uint8_t y)
{
	 drawPixel(x, y, BLACK);
	//_displayBuffer[y_lut[y] + x] |= (0x01 << (y & 7));
}

[[gnu::always_inline]]
inline void clearPixel(uint8_t x, uint8_t y)
{
	 drawPixel(x, y, WHITE);
	//_displayBuffer[y_lut[y] + x] &= ~(0x01 << (y & 7));
}


inline void clearDisplay(uint8_t colour)
{
	uint8_t data = colour ? 0xff : 0;
	uint8_t* ptr = arduboy.sBuffer;
	int count = 128 * 64 / 8;
	while(count--)
		*ptr++ = data;
	//memset(_displayBuffer, data, LCDWIDTH * LCDHEIGHT / 8);
}

class ArduboyPlatform : public PlatformBase
{
public:
	void playSound(uint8_t id);

	void update();
};

void ERROR(const char* msg);

extern ArduboyPlatform Platform;

inline void diskRead(uint24_t address, uint8_t* buffer, int length) 	
{ 
	FX::readDataBytes(address, buffer, length); 
}

inline void writeSaveFile(uint8_t* buffer, int length)
{
	FX::saveGameState(buffer, length);
}

inline bool readSaveFile(uint8_t* buffer, int length)
{
	return FX::loadGameState(buffer, length) != 0;
}
