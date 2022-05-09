#ifndef CUSTOM_ARDUBOY_H
#define CUSTOM_ARDUBOY_H

#include "Arduboy2.h"

class CustomArduboy : public Arduboy2{
public:
  void drawBitmapExt(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t color, bool xMirrored = false, uint8_t mask = 0xFF);
  void invertBufferCharWithSpecifiedChar(int16_t x, int16_t yRow, uint8_t inversionChar);
};

#endif
