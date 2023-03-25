/**
 * @file Arduboy2.cpp
 * \brief
 * The Arduboy2Base and Arduboy2 classes and support objects and definitions.
 */

#include "Arduboy2.h"
#include "ab_logo.c"
#include "nbSPI.h"

extern ESPboyInit myESPboy;

#if (HEIGHT == 64) 
  #define VERT_OFFSET 20
#else 
  #define VERT_OFFSET 0
#endif

ArduboySettings arduboyDefaultSettings;
ArduboySettings arduboySaveLoadSettings;


//========================================
//========== class Arduboy2Base ==========
//========================================


Arduboy2Base::Arduboy2Base(){
  currentButtonState = 0;
  previousButtonState = 0;
  // frame management
  setFrameDuration(16);
  frameCount = 0;
  justRendered = false;
}

// functions called here should be public so users can create their
// own init functions if they need different behavior than `begin`
// provides by default


void Arduboy2Base::start(){
  boot();
  EEPROM.get(EEPROM_STORAGE_SPACE_START, arduboySaveLoadSettings);
  if (arduboySaveLoadSettings.arduboyID != ARDUBOY_ID){
    EEPROM.put(EEPROM_STORAGE_SPACE_START, arduboyDefaultSettings);
    EEPROM.commit();
    arduboySaveLoadSettings = arduboyDefaultSettings;
    myESPboy.tft.setTextSize(1);
    myESPboy.tft.setTextColor(TFT_YELLOW, TFT_BLACK); 
    myESPboy.tft.fillScreen(TFT_BLACK);
    myESPboy.tft.drawString(F("start with"), 33, 30); 
    myESPboy.tft.drawString(F("UP or DOWN"), 33, 45); 
    myESPboy.tft.drawString(F("buttons pressed"), 18, 60);
    myESPboy.tft.drawString(F("for settings menu"), 12, 75);
    delay(5000);
    myESPboy.tft.fillScreen(TFT_BLACK);
    }
  clear();  
  display();
  systemButtons(); 
  audio.begin();
  drawLogoBitmap();
  waitNoButtons();
}

void Arduboy2Base::begin(){
  start();
}


void Arduboy2Base::flashlight(){
  digitalWriteRGB(RGB_ON, RGB_ON, RGB_ON);
  myESPboy.tft.fillScreen(TFT_WHITE);
  while (true) {
    delay(1000);
  }
}


#define SYS_BUTTONS_DELAY 10000 

void Arduboy2Base::systemButtons(){
    uint8_t keys;
  
  if (!pressed(UP_BUTTON) && !pressed(DOWN_BUTTON) && !pressed(LEFT_BUTTON) && !pressed(RIGHT_BUTTON))
    return;
  
  delayShort(200);
  
  myESPboy.tft.setTextSize(1);

  while (1){    
    if (keys&PAD_UP) {arduboySaveLoadSettings.arduboyAudioOnOff = !arduboySaveLoadSettings.arduboyAudioOnOff;}
    if (keys&PAD_DOWN) {arduboySaveLoadSettings.arboyLogo = !arduboySaveLoadSettings.arboyLogo;}
    if (keys&PAD_LEFT) {arduboySaveLoadSettings.arduboyLeds = !arduboySaveLoadSettings.arduboyLeds;}
    if (keys&PAD_RGT) {arduboySaveLoadSettings.arduboyBackground++; if(arduboySaveLoadSettings.arduboyBackground>18)arduboySaveLoadSettings.arduboyBackground=0;}
    if (keys&PAD_LFT) {arduboySaveLoadSettings.arduboyForeground++; if(arduboySaveLoadSettings.arduboyForeground>18)arduboySaveLoadSettings.arduboyForeground=0;}
    if (keys&PAD_ACT) flashlight();
    if (keys&PAD_ESC) break;
    
    myESPboy.tft.setTextColor(TFT_GREEN, TFT_BLACK);  
    myESPboy.tft.fillScreen(TFT_BLACK);
    if (arduboySaveLoadSettings.arduboyAudioOnOff) myESPboy.tft.drawString(F("SOUND ON"),40,20);
    else myESPboy.tft.drawString(F("SOUND OFF"),36,20);
    if(arduboySaveLoadSettings.arboyLogo)myESPboy.tft.drawString(F("SHOW LOGO ON"),28,30);
    else myESPboy.tft.drawString(F("SHOW LOGO OFF"),24,30);
    if(arduboySaveLoadSettings.arduboyLeds)myESPboy.tft.drawString(F("SHOW LED ON"),31,40);
    else myESPboy.tft.drawString(F("SHOW LED OFF"),28,40);
    
    myESPboy.tft.setTextColor(Arduboy2Core::colors[arduboySaveLoadSettings.arduboyForeground], Arduboy2Core::colors[arduboySaveLoadSettings.arduboyBackground]);
    myESPboy.tft.drawString(F("FOREGRND / BACKGRND"), 6, 50);
      
    myESPboy.tft.setTextColor(TFT_YELLOW, TFT_BLACK);  
    myESPboy.tft.drawString(F("UP for sound"),28,70);
    myESPboy.tft.drawString(F("DOWN for logo"),24,80);
    myESPboy.tft.drawString(F("LEFT for LEDs"),25,90);
    myESPboy.tft.drawString(F("LFT/RGT for colors"),9,100);
    myESPboy.tft.drawString(F("A for flashlight"),16,110);
    myESPboy.tft.drawString(F("B to exit"),37,120);
   
    while(myESPboy.getKeys())delay(10);
    while(!(keys=myESPboy.getKeys()))delay(10);
  }
  
  EEPROM.put(EEPROM_STORAGE_SPACE_START, arduboySaveLoadSettings);
  EEPROM.commit();
  myESPboy.myLED.setRGB(0,0,0);
  myESPboy.tft.fillScreen(TFT_BLACK);
};



void Arduboy2Base::drawLogoBitmap(){
  uint8_t cntr=0;
  if(arduboySaveLoadSettings.arboyLogo){
    for (int16_t y = -16; y <= 24; y++) {  
      Arduboy2Base::display(CLEAR_BUFFER);
      drawBitmap(20, y, arduboy_logo, 88, 16);
      Arduboy2Base::display();
      if(arduboySaveLoadSettings.arduboyLeds){		
         if(cntr==1) myESPboy.myLED.setRGB(30,0,0);
         if(cntr==13) myESPboy.myLED.setRGB(0,30,0);
         if(cntr==26) myESPboy.myLED.setRGB(0,0,30);
      }
      delayShort(25);
      cntr++;
    }
  myESPboy.myLED.setRGB(0,0,0); 
  delay(1500); 
  myESPboy.tft.fillScreen(TFT_BLACK);
  } 
};



// wait for all buttons to be released
void Arduboy2Base::waitNoButtons() {
  do {
    delayShort(25); // simple button debounce
  } while (buttonsState());
}


/* Frame management */
void Arduboy2Base::setFrameRate(uint8_t rate){
  eachFrameMillis = 1000 / rate;
}


void Arduboy2Base::setFrameDuration(uint8_t duration){
  eachFrameMillis = duration;
}


bool Arduboy2Base::everyXFrames(uint8_t frames){
  return frameCount % frames == 0;
}


bool Arduboy2Base::nextFrame(){
  uint32_t now = millis();
  uint32_t frameDurationMs = now - thisFrameStart;

  if (justRendered == true){
    justRendered = false;
    return false;
  }
  
  if (frameDurationMs > eachFrameMillis) {
    justRendered = true;
    lastFrameDurationMs =  frameDurationMs;
    thisFrameStart = now;
    frameCount++;
    return true;
  }
  else 
    return false;
}


int Arduboy2Base::cpuLoad(){
  return lastFrameDurationMs*100 / eachFrameMillis;
}

void Arduboy2Base::initRandomSeed(){
  randomSeed(ESP8266_DREG(0x20E44));
}

/* Graphics */


void Arduboy2Base::drawPixel(int16_t x, int16_t y, uint8_t color){
  #ifdef PIXEL_SAFE_MODE
  if (x < 0 || x > (WIDTH-1) || y < 0 || y > (HEIGHT-1)){
    return;
  }
  #endif

  uint8_t row = (uint8_t)y / 8;
  
  if (color){
    sBuffer[(row*WIDTH) + (uint8_t)x] |=   _BV((uint8_t)y % 8);
  }
  else{
    sBuffer[(row*WIDTH) + (uint8_t)x] &= ~ _BV((uint8_t)y % 8);
  }
}


uint8_t Arduboy2Base::getPixel(uint8_t x, uint8_t y){
  uint8_t row = y / 8;
  uint8_t bit_position = y % 8;

  return (sBuffer[(row*WIDTH) + x] & _BV(bit_position)) >> bit_position;
}


void Arduboy2Base::drawCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color){
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawPixel(x0, y0+r, color);
  drawPixel(x0, y0-r, color);
  drawPixel(x0+r, y0, color);
  drawPixel(x0-r, y0, color);

  while (x<y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;

    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
  }
}

void Arduboy2Base::drawCircleHelper(int16_t x0, int16_t y0, uint8_t r, uint8_t corners, uint8_t color){
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x<y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;

    if (corners & 0x4) // lower right
    {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    }
    if (corners & 0x2) // upper right
    {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (corners & 0x8) // lower left
    {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (corners & 0x1) // upper left
    {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}


void Arduboy2Base::fillCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color){
  drawFastVLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}


void Arduboy2Base::fillCircleHelper(int16_t x0, int16_t y0, uint8_t r, uint8_t sides, int16_t delta, uint8_t color){
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;

    if (sides & 0x1) // right side
    {
      drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }

    if (sides & 0x2) // left side
    {
      drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}


void Arduboy2Base::drawLine (int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color){
  // bresenham's algorithm - thx wikpedia
  bool steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int8_t ystep;

  if (y0 < y1)
  {
    ystep = 1;
  }
  else
  {
    ystep = -1;
  }

  for (; x0 <= x1; x0++)
  {
    if (steep)
    {
      drawPixel(y0, x0, color);
    }
    else
    {
      drawPixel(x0, y0, color);
    }

    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}


void Arduboy2Base::drawRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color){
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);
}

void Arduboy2Base::drawFastVLine
(int16_t x, int16_t y, uint8_t h, uint8_t color)
{	
  int16_t end = y+h;
  for (int16_t a = max(0,y); a < min(end,HEIGHT); a++)
  {
    drawPixel(x,a,color);
  }
}


void Arduboy2Base::drawFastHLine(int16_t x, int16_t y, uint8_t w, uint8_t color){
  int16_t xEnd; // last x point + 1

  // Do y bounds checks
  if (y < 0 || y >= HEIGHT)
    return;

  xEnd = x + w;

  // Check if the entire line is not on the display
  if (xEnd <= 0 || x >= WIDTH)
    return;

  // Don't start before the left edge
  if (x < 0)
    x = 0;

  // Don't end past the right edge
  if (xEnd > WIDTH)
    xEnd = WIDTH;

  // calculate actual width (even if unchanged)
  w = xEnd - x;

  // buffer pointer plus row offset + x offset
  uint8_t *pBuf = sBuffer + ((y / 8) * WIDTH) + x;	  

  // pixel mask
  uint8_t mask = 1 << (y & 7);

  switch (color)
  {
    case WHITE:
      while (w--)
      {
        *pBuf++ |= mask;
      }
      break;

    case BLACK:
      mask = ~mask;
      while (w--)
      {
        *pBuf++ &= mask;
      }
      break;
  }
}


void Arduboy2Base::fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color){
  // stupidest version - update in subclasses if desired!
  for (int16_t i=x; i<x+w; i++)
  {
    drawFastVLine(i, y, h, color);
  }
}


void Arduboy2Base::fillScreen(uint8_t color){
	if (color == BLACK) memset(sBuffer, 0x00, HEIGHT*WIDTH/8);
    else memset(sBuffer, 0xff, HEIGHT*WIDTH/8);
}


void Arduboy2Base::drawRoundRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color){
  // smarter version
  drawFastHLine(x+r, y, w-2*r, color); // Top
  drawFastHLine(x+r, y+h-1, w-2*r, color); // Bottom
  drawFastVLine(x, y+r, h-2*r, color); // Left
  drawFastVLine(x+w-1, y+r, h-2*r, color); // Right
  // draw four corners
  drawCircleHelper(x+r, y+r, r, 1, color);
  drawCircleHelper(x+w-r-1, y+r, r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r, y+h-r-1, r, 8, color);
}


void Arduboy2Base::fillRoundRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color){
  // smarter version
  fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r, y+r, r, 2, h-2*r-1, color);
}


void Arduboy2Base::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color){
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}


void Arduboy2Base::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color){
  int16_t a, b, y, last;
  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1)
  {
    swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2)
  {
    swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1)
  {
    swap(y0, y1); swap(x0, x1);
  }

  if(y0 == y2)
  { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)
    {
      a = x1;
    }
    else if(x1 > b)
    {
      b = x1;
    }
    if(x2 < a)
    {
      a = x2;
    }
    else if(x2 > b)
    {
      b = x2;
    }
    drawFastHLine(a, y0, b-a+1, color);
    return;
  }

  int16_t dx01 = x1 - x0,
      dy01 = y1 - y0,
      dx02 = x2 - x0,
      dy02 = y2 - y0,
      dx12 = x2 - x1,
      dy12 = y2 - y1,
      sa = 0,
      sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (y1 == y2)
  {
    last = y1;   // Include y1 scanline
  }
  else
  {
    last = y1-1; // Skip it
  }


  for(y = y0; y <= last; y++)
  {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;

    if(a > b)
    {
      swap(a,b);
    }

    drawFastHLine(a, y, b-a+1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);

  for(; y <= y2; y++)
  {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;

    if(a > b)
    {
      swap(a,b);
    }

    drawFastHLine(a, y, b-a+1, color);
  }
}


void Arduboy2Base::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color){
  // no need to draw at all if we're offscreen
  if (x+w < 0 || x > WIDTH-1 || y+h < 0 || y > HEIGHT-1)
    return;

  int yOffset = abs(y) % 8;
  int sRow = y / 8;
  if (y < 0) {
    sRow--;
    yOffset = 8 - yOffset;
  }
  int rows = h/8;
  if (h%8!=0) rows++;
  for (int a = 0; a < rows; a++) {
    int bRow = sRow + a;
    if (bRow > (HEIGHT/8)-1) break;
    if (bRow > -2) {
      for (int iCol = 0; iCol<w; iCol++) {
        if (iCol + x > (WIDTH-1)) break;
        if (iCol + x >= 0) {
          if (bRow >= 0) {	  
            if (color == WHITE)
              sBuffer[(bRow*WIDTH) + x + iCol] |= pgm_read_byte(bitmap+(a*w)+iCol) << yOffset;
            else if (color == BLACK)
              sBuffer[(bRow*WIDTH) + x + iCol] &= ~(pgm_read_byte(bitmap+(a*w)+iCol) << yOffset);
            else
              sBuffer[(bRow*WIDTH) + x + iCol] ^= pgm_read_byte(bitmap+(a*w)+iCol) << yOffset;
          }
          if (yOffset && bRow<(HEIGHT/8)-1 && bRow > -2) {
            if (color == WHITE)
              sBuffer[((bRow+1)*WIDTH) + x + iCol] |= pgm_read_byte(bitmap+(a*w)+iCol) >> (8-yOffset);
            else if (color == BLACK)
              sBuffer[((bRow+1)*WIDTH) + x + iCol] &= ~(pgm_read_byte(bitmap+(a*w)+iCol) >> (8-yOffset));
            else
              sBuffer[((bRow+1)*WIDTH) + x + iCol] ^= pgm_read_byte(bitmap+(a*w)+iCol) >> (8-yOffset);
          }
        }
      }
    }
  }
}


void Arduboy2Base::drawSlowXYBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color){
  // no need to draw at all of we're offscreen
  if (x+w < 0 || x > WIDTH-1 || y+h < 0 || y > HEIGHT-1)
    return;
  int16_t xi, yi, byteWidth = (w + 7) / 8;

  for(yi = 0; yi < h; yi++) {
    for(xi = 0; xi < w; xi++ ) {
      if((pgm_read_byte(bitmap + yi * byteWidth + xi / 8) & (128 >> (xi & 7)))) {		
        drawPixel(x + xi, y + yi, color);
      }
    }
  }
}



// Helper for drawCompressed()
struct BitStreamReader{
  const uint8_t *source;
  uint16_t sourceIndex;
  uint8_t bitBuffer;
  uint8_t byteBuffer;

  BitStreamReader(const uint8_t *source)
    : source(source), sourceIndex(), bitBuffer(), byteBuffer()
  {
  }

  uint16_t readBits(uint16_t bitCount)
  {
    uint16_t result = 0;
    for (uint16_t i = 0; i < bitCount; i++)
    {
      if (this->bitBuffer == 0)
      {
        this->bitBuffer = 0x1;
        this->byteBuffer = pgm_read_byte(&this->source[this->sourceIndex]);
        ++this->sourceIndex;
      }

      if ((this->byteBuffer & this->bitBuffer) != 0)
        result |= (1 << i); // result |= bitshift_left[i];

      this->bitBuffer <<= 1;
    }
    return result;
  }
};


void Arduboy2Base::drawCompressed(int16_t sx, int16_t sy, const uint8_t *bitmap, uint8_t color){
  // set up decompress state
  BitStreamReader cs = BitStreamReader(bitmap);

  // read header
  int width = (int)cs.readBits(8) + 1;
  int height = (int)cs.readBits(8) + 1;
  uint8_t spanColour = (uint8_t)cs.readBits(1); // starting colour

  // no need to draw at all if we're offscreen
  if ((sx + width < 0) || (sx > WIDTH - 1) || (sy + height < 0) || (sy > HEIGHT - 1))
    return;

  // sy = sy - (frame * height);
  int yOffset = abs(sy) % 8;
  int startRow = sy / 8;
  if (sy < 0) {
    startRow--;
    yOffset = 8 - yOffset;
  }
  int rows = height / 8;
  if ((height % 8) != 0)
    ++rows;

  int rowOffset = 0; // +(frame*rows);
  int columnOffset = 0;

  uint8_t byte = 0x00;
  uint8_t bit = 0x01;
  while (rowOffset < rows) // + (frame*rows))
  {
    uint16_t bitLength = 1;
    while (cs.readBits(1) == 0)
      bitLength += 2;

    uint16_t len = cs.readBits(bitLength) + 1; // span length

    // draw the span
    for (uint16_t i = 0; i < len; ++i)
    {
      if (spanColour != 0)
        byte |= bit;
      bit <<= 1;

      if (bit == 0) // reached end of byte
      {
        // draw
        int bRow = startRow + rowOffset;

        //if (byte) // possible optimisation
        if ((bRow <= (HEIGHT / 8) - 1) && (bRow > -2) &&
            (columnOffset + sx <= (WIDTH - 1)) && (columnOffset + sx >= 0))
        {
          int16_t offset = (bRow * WIDTH) + sx + columnOffset;
          if (bRow >= 0)
          {
            int16_t index = offset;
            uint8_t value = byte << yOffset;
            if (color != 0)
              sBuffer[index] |= value;
            else
              sBuffer[index] &= ~value;
          }
          if ((yOffset != 0) && (bRow < (HEIGHT / 8) - 1))
          {
            int16_t index = offset + WIDTH;
            uint8_t value = byte >> (8 - yOffset);

            if (color != 0)
              sBuffer[index] |= value;
            else
              sBuffer[index] &= ~value;
          }
        }

        // iterate
        ++columnOffset;
        if (columnOffset >= width)
        {
          columnOffset = 0;
          ++rowOffset;
        }

        // reset byte
        byte = 0x00;
        bit = 0x01;
      }
    }

    spanColour ^= 0x01; // toggle colour bit (bit 0) for next span
  }
}


void Arduboy2Base::clear(){
  memset(sBuffer, 0, HEIGHT*WIDTH/8);
}

void Arduboy2Base::clearDisplay(){
  clear();
}

#define SWPLH(x) ((x>>8)|(x<<8))

void IRAM_ATTR Arduboy2Base::display(){ 
//WARNING! flip_horizontal and flip_vertical control and render not implemented
//but you can do it checking global 
//bool flip_vertical_flag;
//bool flip_horizontal_flag;
  static uint16_t oBuffer1[WIDTH*16] __attribute__ ((aligned));
  static uint16_t oBuffer2[WIDTH*16] __attribute__ ((aligned));
  static uint16_t* oBuffer = oBuffer1;
  static uint16_t currentDataByte, currentDataAddr;
  static uint16_t foregroundColor, backgroundColor, xPos, yPos, kPos, kkPos, addr;
  bool flipBuf;
   
   myESPboy.tft.setAddrWindow(0, VERT_OFFSET, WIDTH, HEIGHT);
  if(!invert_flag){
    foregroundColor = SWPLH(colors[arduboySaveLoadSettings.arduboyForeground]);
    backgroundColor = SWPLH(colors[arduboySaveLoadSettings.arduboyBackground]);
  }
  else{
    backgroundColor = SWPLH(colors[arduboySaveLoadSettings.arduboyForeground]);
    foregroundColor = SWPLH(colors[arduboySaveLoadSettings.arduboyBackground]);
  }

if(!allpixelson_flag){
  for(kPos = 0; kPos<4*(HEIGHT/64); kPos++){
    kkPos = kPos<<1;
    for (xPos = 0; xPos < WIDTH; xPos++) {
            currentDataAddr = xPos + kkPos * WIDTH;
            currentDataByte = sBuffer[currentDataAddr] + (sBuffer[currentDataAddr+128]<<8);
      for (yPos = 0; yPos < 16; yPos++) {		
		    //if (!(yPos % 8)) currentDataByte = sBuffer[xPos + ((yPos>>3)+kkPos) * WIDTH];
		    addr = 	yPos*WIDTH+xPos;
            if (currentDataByte & 0x01) oBuffer[addr] = foregroundColor;
            else oBuffer[addr] = backgroundColor;
			currentDataByte = currentDataByte >> 1;
	  }
    }
    while(nbSPI_isBusy()); 
    nbSPI_writeBytes((uint8_t*)oBuffer, WIDTH*16*2);  
    flipBuf = !flipBuf;
    oBuffer = flipBuf?oBuffer1:oBuffer2;
    //myESPboy.tft.pushColors(oBuffer, WIDTH*16);
  }
}
else {
  while(nbSPI_isBusy()); 
  myESPboy.tft.fillRect(0, VERT_OFFSET, WIDTH, HEIGHT,foregroundColor);
  }
}


void Arduboy2Base::display(bool clear){
  Arduboy2Base::display();
  if (clear) Arduboy2Base::clear();
}


uint8_t* Arduboy2Base::getBuffer(){
  return sBuffer;
}


bool Arduboy2Base::pressed(uint8_t buttons){
  return (buttonsState() & buttons) == buttons;
}


bool Arduboy2Base::notPressed(uint8_t buttons){
  return (buttonsState() & buttons) == 0;
}


void Arduboy2Base::pollButtons(){
  previousButtonState = currentButtonState;
  currentButtonState = buttonsState();
}


bool Arduboy2Base::justPressed(uint8_t button){
  return (!(previousButtonState & button) && (currentButtonState & button));
}


bool Arduboy2Base::justReleased(uint8_t button){
  return ((previousButtonState & button) && !(currentButtonState & button));
}


bool Arduboy2Base::collide(Point point, Rect rect){
  return ((point.x >= rect.x) && (point.x < rect.x + rect.width) &&
      (point.y >= rect.y) && (point.y < rect.y + rect.height));
}


bool Arduboy2Base::collide(Rect rect1, Rect rect2){
  return !(rect2.x                >= rect1.x + rect1.width  ||
           rect2.x + rect2.width  <= rect1.x                ||
           rect2.y                >= rect1.y + rect1.height ||
           rect2.y + rect2.height <= rect1.y);
}


uint16_t Arduboy2Base::readUnitID(){
  return ARDUBOY_ID;
}


void Arduboy2Base::writeUnitID(uint16_t id){
}


uint8_t Arduboy2Base::readUnitName(char* name){
  return ('A');
}


void Arduboy2Base::writeUnitName(char* name){
}


bool Arduboy2Base::readShowBootLogoFlag(){
  return arduboySaveLoadSettings.arboyLogo;
}


void Arduboy2Base::writeShowBootLogoFlag(bool val){
}


bool Arduboy2Base::readShowUnitNameFlag(){
  return 0;
}


void Arduboy2Base::writeShowUnitNameFlag(bool val){
}


bool Arduboy2Base::readShowBootLogoLEDsFlag(){
  return arduboySaveLoadSettings.arduboyLeds;
}


void Arduboy2Base::writeShowBootLogoLEDsFlag(bool val){
}


void Arduboy2Base::swap(int16_t& a, int16_t& b){
  int16_t temp = a;
  a = b;
  b = temp;
}


//====================================
//========== class Arduboy2 ==========
//====================================

Arduboy2::Arduboy2()
{
  cursor_x = 0;
  cursor_y = 0;
  textColor = 1;
  textBackground = 0;
  textSize = 1;
  textWrap = 0;
  textRaw = 0;
}



size_t Arduboy2::write(uint8_t c)
{
  if ((c == '\r') && !textRaw)
  {
    return 1;
  }

  if (((c == '\n') && !textRaw) ||
      (textWrap && (cursor_x > (WIDTH - (characterWidth * textSize)))))
  {
    cursor_x = 0;
    cursor_y += fullCharacterHeight * textSize;
  }

  if ((c != '\n') || textRaw)
  {
    drawChar(cursor_x, cursor_y, c, textColor, textBackground, textSize);
    cursor_x += fullCharacterWidth * textSize;
  }

  return 1;
}

void Arduboy2::drawChar
  (int16_t x, int16_t y, uint8_t c, uint8_t color, uint8_t bg, uint8_t size)
{
// It is assumed that rendering characters fully off screen will be rare,
// so let drawPixel() handle off screen checks, to reduce code size at the
// expense of slower off screen character handling.
#if 0
  if ((x >= WIDTH) ||              // Clip right
      (y >= HEIGHT) ||             // Clip bottom
      ((x + characterWidth * size - 1) < 0) ||  // Clip left
      ((y + characterHeight * size - 1) < 0)    // Clip top
     )
  {
    return;
  }
#endif

  bool drawBackground = bg != color;
  const uint8_t* bitmap =
    &font5x7[c * characterWidth * ((characterHeight + 8 - 1) / 8)];

  for (uint8_t i = 0; i < fullCharacterWidth; i++)
  {
    uint8_t column;

    if (characterHeight <= 8)
    {
      column = (i < characterWidth) ? pgm_read_byte(bitmap++) : 0;
    }
    else
    {
      column = 0;
    }

    // draw the character by columns. Top to bottom, left to right
    // including character spacing on the right
    for (uint8_t j = 0; j < characterHeight; j++)
    {
      if (characterHeight > 8)
      {
        // at this point variable "column" will be 0, either from initialization
        // or by having eight 0 bits shifted in by the >>= operation below
        if ((j % 8 == 0) && (i < characterWidth))
        {
          column = pgm_read_byte(bitmap++);
        }
      }

      // pixelIsSet should be a bool but at the time of writing,
      // the GCC AVR compiler generates less code if it's a uint8_t
      uint8_t pixelIsSet = column & 0x01;

      if (pixelIsSet || drawBackground)
      {
        for (uint8_t a = 0; a < size; a++)
        {
          for (uint8_t b = 0; b < size; b++)
          {
            drawPixel(x + (i * size) + a, y + (j * size) + b,
                      pixelIsSet ? color : bg);
          }
        }
      }
      column >>= 1;
    }

    // draw the inter-line spacing pixels for this column if required
    if (drawBackground)
    {
      for (uint8_t j = characterHeight; j < fullCharacterHeight; j++)
      {
        for (uint8_t a = 0; a < size; a++)
        {
          for (uint8_t b = 0; b < size; b++)
          {
            drawPixel(x + (i * size) + a, y + (j * size) + b, bg);
          }
        }
      }
    }
  }
}


void Arduboy2::setCursor(int16_t x, int16_t y){
  cursor_x = x;
  cursor_y = y;
}


int16_t Arduboy2::getCursorX(){
  return cursor_x;
}


int16_t Arduboy2::getCursorY(){
  return cursor_y;
}

void Arduboy2::setTextColor(uint8_t color){
  textColor = color;
}


uint8_t Arduboy2::getTextColor(){
  return textColor;
}


void Arduboy2::setTextBackground(uint8_t bg){
  textBackground = bg;
}


uint8_t Arduboy2::getTextBackground(){
  return textBackground;
}


void Arduboy2::setTextSize(uint8_t s){
  // size must always be 1 or higher
  textSize = max(1, s);
}


uint8_t Arduboy2::getTextSize(){
  return textSize;
}


void Arduboy2::setTextWrap(bool w){
  textWrap = w;
}


bool Arduboy2::getTextWrap(){
  return textWrap;
}


void Arduboy2::setTextRawMode(bool raw)
{
  textRaw = raw;
}

bool Arduboy2::getTextRawMode()
{
  return textRaw;
}


void Arduboy2::clear(){
    Arduboy2Base::clear();
    cursor_x = cursor_y = 0;
}

