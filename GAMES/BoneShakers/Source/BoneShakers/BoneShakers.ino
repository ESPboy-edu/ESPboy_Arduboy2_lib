#include <Arduboy2.h>
#include "Game.h"

Arduboy2Base arduboy;
Sprites sprites;

uint8_t GetInput()
{
  uint8_t result = 0;
  
  if(arduboy.pressed(A_BUTTON))
  {
    result |= INPUT_A;  
  }
  if(arduboy.pressed(B_BUTTON))
  {
    result |= INPUT_B;  
  }
  if(arduboy.pressed(UP_BUTTON))
  {
    result |= INPUT_UP;  
  }
  if(arduboy.pressed(DOWN_BUTTON))
  {
    result |= INPUT_DOWN;  
  }
  if(arduboy.pressed(LEFT_BUTTON))
  {
    result |= INPUT_LEFT;  
  }
  if(arduboy.pressed(RIGHT_BUTTON))
  {
    result |= INPUT_RIGHT;  
  }

  return result;
}

void SetLED(uint8_t r, uint8_t g, uint8_t b)
{
  arduboy.digitalWriteRGB(r ? RGB_ON : RGB_OFF, g ? RGB_ON : RGB_OFF, b ? RGB_ON : RGB_OFF);
}

void PutPixel(uint8_t x, uint8_t y, uint8_t colour)
{
  arduboy.drawPixel(x, y, colour);
}

void DrawSprite(int16_t x, int16_t y, const uint8_t *bitmap,
  const uint8_t *mask, uint8_t frame, uint8_t mask_frame)
{
  sprites.drawExternalMask(x, y, bitmap, mask, frame, mask_frame);
}

void DrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap)
{
  uint8_t w = pgm_read_byte(&bitmap[0]);
  uint8_t h = pgm_read_byte(&bitmap[1]);
  arduboy.drawBitmap(x, y, bitmap + 2, w, h);
}

void DrawSolidBitmap(int16_t x, int16_t y, const uint8_t *bitmap)
{
  uint8_t w = pgm_read_byte(&bitmap[0]);
  uint8_t h = pgm_read_byte(&bitmap[1]);
  arduboy.fillRect(x, y, w, h, BLACK);
  arduboy.drawBitmap(x, y, bitmap + 2, w, h);
}

void FillScreen(uint8_t colour)
{
  arduboy.fillScreen(colour);
}

/*
void DrawFilledRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t colour)
{
  arduboy.fillRect(x, y, w, h, colour);
}

void DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t colour)
{
  arduboy.drawRect(x, y, w, h, colour);
}
*/
/*--------------- CUT HERE FOR EXCEPTION DECODER ---------------

Exception (3):
epc1=0x40201fe1 epc2=0x00000000 epc3=0x00000000 excvaddr=0x40256901 depc=0x00000000

>>>stack>>>

ctx: cont
sp: 3ffffe00 end: 3fffffd0 offset: 0150
3fffff50:  00000000 00000001 00000010 40202f1c  
3fffff60:  40100300 3ffeedec 00000040 402028ef  
3fffff70:  00000000 000043e3 00000000 4020106b  
3fffff80:  00000000 00000078 3ffee947 3ffef24c  
3fffff90:  00000002 3ffee948 3ffee947 4020240b  
3fffffa0:  3fffdad0 00000000 3ffef220 402011e6  
3fffffb0:  00000000 00000000 00000001 402105cc  
3fffffc0:  feefeffe feefeffe 3fffdab0 401017ed  
<<<stack<<<

--------------- CUT HERE FOR EXCEPTION DECODER ---------------
⸮B⸮r-⸮P⸮M'⸮⸮X⸮⸮s⸮~⸮-xh
void DrawBitmap(const uint8_t* bmp, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  arduboy.drawBitmap(x, y, bmp, w, h, WHITE);
}*/

void setup()
{
  arduboy.begin();
  //arduboy.flashlight();
  //arduboy.systemButtons();
//  arduboy.bootLogo();
  arduboy.setFrameRate(30);

  Serial.begin(74880);

  InitGame();
}

void loop()
{
  if(arduboy.nextFrame())
  {
    TickGame();
    
    //Serial.write(arduboy.getBuffer(), 128 * 64 / 8);
    arduboy.display(true);
  }
}
