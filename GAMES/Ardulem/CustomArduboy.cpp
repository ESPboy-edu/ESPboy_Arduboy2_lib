#include "Ardulem.h"
#include "CustomArduboy.h"
//#include "Arduboy_glcdfont.c"


void CustomArduboy::invertBufferCharWithSpecifiedChar(int16_t x, int16_t yRow, uint8_t inversionChar)
{
  // no need to dar at all of we're offscreen
  if (x < 0 || x > WIDTH-1 /* || yRow < 0 || yRow > (HEIGHT/8)-1 */) // no need to check the row, I know it is correct
    return;

  // invert the specificed buffer char
  this->sBuffer[ (yRow*WIDTH) + x ] ^= inversionChar;
}



void CustomArduboy::drawBitmapExt(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t color, bool xMirrored, uint8_t mask){
  // no need to dare at all of we're offscreen
  if (x+w < 0 || x > WIDTH-1 || y+8 < 0 || y > HEIGHT-1)
  return;

  int16_t yOffset = abs(y) % 8;
  int16_t sRow = y / 8;
  if (y < 0) 
  {
    sRow--;
    yOffset = 8 - yOffset;
  }
  int16_t yOffsetComplement = 8-yOffset;
  int16_t bufferRowShift = sRow*WIDTH;
  int16_t nextBufferRowShift = (sRow+1)*WIDTH;

    if ((sRow > -2) && (sRow < (HEIGHT/8)))
  {
    // compute the start and end X (clamp if outside the screen)
    int16_t startX = (x<0) ? -x : 0;
    int16_t endX = (x+w > (WIDTH-1)) ? WIDTH-x : w;

    // inverse the horizontal iteration inside the bitmap if we are mirrored on x
    int16_t iCol = startX;
    int16_t iColDirection = 1;
    if (xMirrored)
    {
      iCol = w-1;
      iColDirection = -1;
    }

    // iterate along the width of the bitmap to draw
    for (int xCol = startX; xCol < endX; xCol++, iCol += iColDirection)
    {
      // compute the current x
      int16_t currentX = x + xCol;

      // get the byte to write from the progmem
      uint8_t unshiftedByteToWrite = pgm_read_byte(bitmap+iCol) & mask;
      // write the top part of the byte on the screen row
      if (sRow >= 0) 
      {
        int16_t bufferPosition = bufferRowShift + currentX;
        uint8_t byteToWrite = unshiftedByteToWrite << yOffset;
        if      (color == WHITE) this->sBuffer[ bufferPosition ] |= byteToWrite;
        else if (color == BLACK) this->sBuffer[ bufferPosition ] &= ~byteToWrite;
        else                     this->sBuffer[ bufferPosition ] ^= byteToWrite;
      }
      // write the bottom part of the byte on the next screen row
      if (yOffset && sRow<(HEIGHT/8)-1) 
      {
        int16_t bufferPosition = nextBufferRowShift + currentX;
        uint8_t byteToWrite = unshiftedByteToWrite >> yOffsetComplement;
        if      (color == WHITE) this->sBuffer[ bufferPosition ] |= byteToWrite;
        else if (color == BLACK) this->sBuffer[ bufferPosition ] &= ~byteToWrite;
        else                     this->sBuffer[ bufferPosition ] ^= byteToWrite;
      }
    }
    }
}
 
