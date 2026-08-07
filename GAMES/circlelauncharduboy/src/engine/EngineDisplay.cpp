#include "Engine.h"

uint8_t * Engine::displayBuffer;



void Engine::setCurrentBuffer(uint8_t * buffer, int width, int height) {
  currentBuffer = buffer;
  currentBufferWidth = width;
  currentBufferHeight = height;
}

void Engine::drawCurrentBufferAt(int xOffset, int yOffset, int scale) {
  if (currentBuffer == nullptr)
    return;

  uint8_t * buffer = currentBuffer;

  if (xOffset == -1)
    xOffset = (SCREEN_WIDTH - currentBufferWidth * scale) / 2;
  if (yOffset == -1)
    yOffset = (SCREEN_HEIGHT - currentBufferHeight * scale) / 2;

  currentBuffer = nullptr;

  for (int x = 0; x < currentBufferWidth; x++) {
    for (int y = 0; y < currentBufferHeight; y++) {
      if (readXYBufferAt(x, y, buffer, currentBufferWidth)) {
        if (scale > 4) {
          // drawCircle(x * scale + xOffset, y * scale + yOffset, (scale - 2) / 2, WHITE);
          // drawRectCornered(x * scale + xOffset, y * scale + yOffset, scale - 1, scale - 1, WHITE);

          // fillRect(x * scale + xOffset, y * scale + yOffset, scale - 1, scale - 1, WHITE);

          drawRectNoCorners(x * scale + xOffset, y * scale + yOffset, scale - 1, scale - 1, WHITE);
          fillRect(x * scale + 1 + xOffset, y * scale + 1 + yOffset, scale - 1 - 2, scale - 1 - 2, WHITE);
        } else {
          fillRect(x * scale + xOffset, y * scale + yOffset, scale - 1, scale - 1, WHITE);
          if (scale == 4)
            drawPixel(x * scale + xOffset + 1, y * scale + yOffset + 1, INVERSE);
        }
      }
    }
  }
}

void Engine::clearCurrentBuffer() {
  memset(currentBuffer, 0, currentBufferWidth * currentBufferHeight / 8);
}

uint8_t Engine::getPixel(uint8_t x, uint8_t y) {
  // uint8_t rowCount = displayBufferHeight / 8;
  // uint8_t row = y / rowCount;
  // uint8_t bitPosition = y % rowCount;
  // return (displayBuffer[(row*WIDTH) + x] & _BV(bitPosition)) >> bitPosition;

  uint8_t row = y / 8;
  uint8_t bitPosition = y % 8;
  return (displayBuffer[(row*WIDTH) + x] & _BV(bitPosition)) >> bitPosition;
}

uint8_t Engine::getPixelFrom(uint8_t x, uint8_t y, uint8_t *buffer, uint8_t w, uint8_t h) {
  uint8_t rowCount = h / 8;
  uint8_t row = y / 8;
  uint8_t bitPosition = y % 8;
  uint8_t byteValue = buffer[(row*w) + x];
  return (byteValue & _BV(bitPosition)) >> bitPosition;
}

void Engine::drawPixel(int16_t x, int16_t y, int8_t color) {

  if (currentBuffer == nullptr) {
    if (color == INVERSE) {
      color = 1 - getPixel(x, y);
    }

    writePixel(x, y, color);  
  } else if (x >= 0 && x < currentBufferWidth && y >= 0 && y < currentBufferHeight) {
    writeXYBufferAt(x, y, color, currentBuffer, currentBufferWidth);
  }
}


void Engine::writePixel(int16_t x, int16_t y, int8_t color) {
  // Перенаправляем на стандартный метод вашего порта
  arduboy.drawPixel(x, y, color); 
}



void Engine::drawCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color) {

  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  writePixel(x0, y0+r, color);
  writePixel(x0, y0-r, color);
  writePixel(x0+r, y0, color);
  writePixel(x0-r, y0, color);

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

    writePixel(x0 + x, y0 + y, color);
    writePixel(x0 - x, y0 + y, color);
    writePixel(x0 + x, y0 - y, color);
    writePixel(x0 - x, y0 - y, color);
    writePixel(x0 + y, y0 + x, color);
    writePixel(x0 - y, y0 + x, color);
    writePixel(x0 + y, y0 - x, color);
    writePixel(x0 - y, y0 - x, color);
  }
}

void Engine::drawCircleHelper(int16_t x0, int16_t y0, uint8_t r, uint8_t corners, uint8_t color) {
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
      writePixel(x0 + x, y0 + y, color);
      writePixel(x0 + y, y0 + x, color);
    }
    if (corners & 0x2) // upper right
    {
      writePixel(x0 + x, y0 - y, color);
      writePixel(x0 + y, y0 - x, color);
    }
    if (corners & 0x8) // lower left
    {
      writePixel(x0 - y, y0 + x, color);
      writePixel(x0 - x, y0 + y, color);
    }
    if (corners & 0x1) // upper left
    {
      writePixel(x0 - y, y0 - x, color);
      writePixel(x0 - x, y0 - y, color);
    }
  }
}

void Engine::fillCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color) {
  drawFastVLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

void Engine::fillCircleHelper(int16_t x0, int16_t y0, uint8_t r, uint8_t sides, int16_t delta, uint8_t color) {
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

void Engine::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {

  writeLine(x0, y0, x1, y1, color);
}

void Engine::writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  if (x0 > x1) {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      writePixel(y0, x0, color);
    } else {
      writePixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void Engine::drawRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color) {
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);
}

void Engine::drawRectCornered(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color) {
  drawRect(x, y, w, h, color);

  writePixel(x, y, BLACK);
  writePixel(x + w - 1, y, BLACK);
  writePixel(x + w - 1, y + h - 1, BLACK);
  writePixel(x, y + h - 1, BLACK);
}

void Engine::drawRectNoCorners(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color) {
  drawFastHLine(x+1, y, w-2, color);
  drawFastHLine(x+1, y+h-1, w-2, color);
  drawFastVLine(x, y+1, h-2, color);
  drawFastVLine(x+w-1, y+1, h-2, color);
}

void Engine::drawFastVLine(int16_t x, int16_t y, uint8_t h, uint8_t color) {
// #ifndef EngineForArduboy
//   x += xDisplayOffsetInt;
//   y += yDisplayOffsetInt;
// #endif

  if (x < 0 || x > (WIDTH-1)) {
    return;
  }

  int end = y+h;
  for (int a = max(0,(int)y); a < min(end,HEIGHT); a++) {
    drawPixel(x, a, color);
    // writePixel(x,a,color);
  }
}

void Engine::drawFastHLine(int16_t x, int16_t y, uint8_t w, uint8_t color) {

  if (y < 0 || y > (HEIGHT-1)) {
    return;
  }

  if (currentBuffer != nullptr) {
    for (int i = 0; i < x; i++) {
      drawPixel(x, x+i, color);
    }
    return;
  }

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
  uint8_t *pBuf = displayBuffer + ((y / 8) * WIDTH) + x;

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

void Engine::fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, int8_t color) {
  if (color == INVERSE) {
    for (int i = 0; i < w * h; i++) {
      int col = x + i % w;
      int row = y + i / w;
      drawPixel(col, row, INVERSE);
    }
  } else {
    for (int16_t i = x; i < x + w; i++) {
      drawFastVLine(i, y, h, color);
    }
  }
}

void Engine::fillScreen(uint8_t color) {
  // Перенаправляем на стандартный метод вашего порта
  arduboy.fillScreen(color); 
}


void Engine::drawRoundRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color) {
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

void Engine::fillRoundRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color) {
  // smarter version
  fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r, y+r, r, 2, h-2*r-1, color);
}

void Engine::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color) {
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}

void Engine::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color) {
  int16_t a, b, y, last;
  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1)
  {
    _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
  }
  if (y1 > y2)
  {
    _swap_int16_t(y2, y1); _swap_int16_t(x2, x1);
  }
  if (y0 > y1)
  {
    _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
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
      _swap_int16_t(a,b);
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
      _swap_int16_t(a,b);
    }

    drawFastHLine(a, y, b-a+1, color);
  }
}

void Engine::clearDisplay() {
  arduboy.clear();
}

void Engine::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color) {
  int height = h + (h % 8 ? 8 - h % 8 : 0);
  
  drawFastBitmap(x, y, bitmap, w, height, color);
}

void Engine::drawBitmapTransformed(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color, bool flipX, bool flipY, int scale) {
  if (!flipX && !flipY) {
    drawBitmap(x, y, bitmap, w, h, color);
    return;
  }

  int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t b = 0;

  for (int16_t j = 0; j < h; j++) {
    for (int16_t i = 0; i < w; i++) {
      if (i & 7)
        b <<= 1;
      else
        b = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);
      if (b & 0x80) {
        int displayX = x + i;
        if (flipX)
          displayX = x + w - 1 - i;

        int displayY = y + j;
        if (flipY)
          displayY = y + h - 1 - j;

        drawPixel(displayX, displayY, color);
      }
    }
  }
}

void Engine::drawBitmapAt(int x, int y, const unsigned char *bitmap, int frame, int color) {
  int width = pgm_read_byte(&bitmap[0]);
  int height = pgm_read_byte(&bitmap[1]);
  int totalFrames = pgm_read_byte(&bitmap[2]);
  int byteLength = (height / 8 + (height % 8 > 0)) * width;

  int i = 3;
  if (totalFrames > 127) {
    totalFrames = 256 - totalFrames;
    i += frame * byteLength * 2;
    drawBitmap(x, y, bitmap+i, width, height, (color != INVERSE ? BLACK : WHITE));
    i += byteLength;
  } else {
    i += frame * byteLength;
  }

  drawBitmap(x, y, bitmap+i, width, height, (color == WHITE ? WHITE : BLACK));
}

void Engine::drawBitmapTransformedAt(int x, int y, const unsigned char *bitmap, int frame, int color, bool flipX, bool flipY, int scale) {
  int width = pgm_read_byte(&bitmap[0]);
  int height = pgm_read_byte(&bitmap[1]);
  int totalFrames = pgm_read_byte(&bitmap[2]);
  int byteLength = (height / 8 + (height % 8 > 0)) * width;

  int i = 3;
  if (totalFrames > 127) {
    totalFrames = 256 - totalFrames;
    i += frame * byteLength * 2;
    drawFastBitmapTransformed(x, y, bitmap+i, width, height, (color != INVERSE ? BLACK : WHITE), flipX, flipY);
    i += byteLength;
  } else {
    i += frame * byteLength;
  }

  drawFastBitmapTransformed(x, y, bitmap+i, width, height, (color == WHITE ? WHITE : BLACK), flipX, flipY);
}

void Engine::drawFastBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color) {

  // no need to draw at all if we're offscreen
  if (x + w <= 0 || x > WIDTH - 1 || y + h <= 0 || y > HEIGHT - 1)
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
        if (iCol + x > (WIDTH-1))
          break;

        if (iCol + x >= 0) {
          int byteCol = iCol;
          uint8_t byteValue = pgm_read_byte(bitmap+(a*w)+byteCol);

          if (bRow >= 0) {
            int bufferIndex = (bRow*WIDTH) + x + iCol;
            if (color == WHITE)
              displayBuffer[bufferIndex] |= byteValue << yOffset;
            else if (color == BLACK)
              displayBuffer[bufferIndex] &= ~(byteValue << yOffset);
            else
              displayBuffer[bufferIndex] ^= byteValue << yOffset;
          }

          if (yOffset && bRow<(HEIGHT/8)-1 && bRow > -2) {
            int bufferIndex = ((bRow+1)*WIDTH) + x + iCol;
            if (color == WHITE)
              displayBuffer[bufferIndex] |= byteValue >> (8-yOffset);
            else if (color == BLACK)
              displayBuffer[bufferIndex] &= ~(byteValue >> (8-yOffset));
            else
              displayBuffer[bufferIndex] ^= byteValue >> (8-yOffset);
          }
        }
      }
    }
  }
}
 
void Engine::drawFastBitmapTransformed(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color, bool flipX, bool flipY) {

  if (flipY)
    y -= 8 % h;
    // cout << "flipY height: " << (int)h << endl;

  // no need to draw at all if we're offscreen
  if (x + w <= 0 || x > WIDTH - 1 || y + h <= 0 || y > HEIGHT - 1)
    return;

  int yOffset = abs(y) % 8;
  int sRow = y / 8;
  if (y < 0) {
    sRow--;
    yOffset = 8 - yOffset;
  }
  int rows = h/8 + (h%8!=0);

  for (int a = 0; a < rows; a++) {
    int bRow = sRow + a;
    if (bRow > (HEIGHT/8)-1)
      break;
    if (bRow <= -2)
      continue;

    for (int iCol = 0; iCol<w; iCol++) {
      if (iCol + x > (WIDTH-1))
        break;

      if (iCol + x >= 0) {
        int byteCol = iCol;
        if (flipX)
          byteCol = w - 1 - iCol;
        uint8_t byteValue = pgm_read_byte(bitmap+(a*w)+byteCol);
        if (flipY) {
          // https://stackoverflow.com/a/2602885
          byteValue = (byteValue & 0xF0) >> 4 | (byteValue & 0x0F) << 4;
          byteValue = (byteValue & 0xCC) >> 2 | (byteValue & 0x33) << 2;
          byteValue = (byteValue & 0xAA) >> 1 | (byteValue & 0x55) << 1;
        }

        if (bRow >= 0) {
          int bufferIndex = (bRow*WIDTH) + x + iCol;
          if (color == WHITE)
            displayBuffer[bufferIndex] |= byteValue << yOffset;
          else if (color == BLACK)
            displayBuffer[bufferIndex] &= ~(byteValue << yOffset);
          else
            displayBuffer[bufferIndex] ^= byteValue << yOffset;
        }

        if (yOffset && bRow<(HEIGHT/8)-1 && bRow > -2) {
          int bufferIndex = ((bRow+1)*WIDTH) + x + iCol;
          if (color == WHITE)
            displayBuffer[bufferIndex] |= byteValue >> (8-yOffset);
          else if (color == BLACK)
            displayBuffer[bufferIndex] &= ~(byteValue >> (8-yOffset));
          else
            displayBuffer[bufferIndex] ^= byteValue >> (8-yOffset);
        }
      }
    }
  }
}

void Engine::drawSlowXYBitmapTransformed(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color, bool flipX, bool flipY, int scale) {

  // no need to draw at all of we're offscreen
  if (x + w <= 0 || x > WIDTH - 1 || y + h <= 0 || y > HEIGHT - 1)
    return;

  int columns = w / 8 + (w % 8 > 0) * 1;

  for (int yi = 0; yi < h; yi++) {
    for (int column = 0; column < columns; column++) {
      unsigned char byteValue = bitmap[yi * columns + column];
      for (int bitIndex = 7; bitIndex >= 0; bitIndex--) {
        if (byteValue & 0x01) {
          int xi = column * 8 + bitIndex;
          if (flipX)
            xi = w - xi - 1;
          int displayY = yi;
          if (flipY)
            displayY = h - displayY - 1;
          if (scale == 1)
            writePixel(x + xi, y + yi, color);
          else
            fillRect(x + xi * scale, y + displayY * scale, scale, scale, color);

        } else {
//                    printf("bit %d is 0\n", i);
        }

        byteValue = byteValue >> 1;
      }
    }
  }

  // int16_t xi, yi, byteWidth = (w + 7) / 8;
  // for(yi = 0; yi < h; yi++) {
  //   for(xi = 0; xi < w; xi++ ) {
  //     if(pgm_read_byte(bitmap + yi * byteWidth + xi / 8) & (128 >> (xi & 7))) {
  //       writePixel(x + xi, y + yi, color);
  //     }
  //   }
  // }
}

void Engine::drawSlowXYBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color) {

  // no need to draw at all of we're offscreen
  if (x + w <= 0 || x > WIDTH - 1 || y + h <= 0 || y > HEIGHT - 1)
    return;

  int16_t xi, yi, byteWidth = (w + 7) / 8;
  for(yi = 0; yi < h; yi++) {
    for(xi = 0; xi < w; xi++ ) {
      if(pgm_read_byte(bitmap + yi * byteWidth + xi / 8) & (128 >> (xi & 7))) {
        writePixel(x + xi, y + yi, color);
      }
    }
  }
}

void Engine::drawDitherPixel(int x, int y, int pattern, int patternColor, int transparent) {
  int color = (y % 2 == x % 2) ? !patternColor : patternColor;

  switch (pattern) {
    // 13%
    case 0:
    if (x % 2 == 1 || (y % 4 == (x+1) % 4))
      color = !patternColor;
    break;
    // 25%
    case 1:
    if (x % 2 == 1)
      color = !patternColor;
    break;
    // 38%
    case 2:
    if (x % 2 == 1 && (y % 4 == ((x+1) % 4 == 0 ? 2 : 0)))
      color = !patternColor;
    break;
    // 50%
    // case 3:
    // break;
    // 63%
    case 4:
    if (x % 2 == 0 && (y % 4 == (x % 4 == 0 ? 2 : 0)))
      color = patternColor;
    break;
    // 75%
    case 5:
    if (x % 2 == 0)
      color = patternColor;
    break;
    // 88%
    case 6:
    if (x % 2 == 0 || (y % 4 == x % 4))
      color = patternColor;
    break;
    // 100%
    case 7:
    color = patternColor;
    break;
  }

  if (transparent && color != patternColor)
    return;

  drawPixel(x, y, color);
}

void Engine::drawDitherOverlay(int pattern, int patternColor) {
  for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
    int x = i % SCREEN_WIDTH;
    int y = i / SCREEN_WIDTH;
    drawDitherPixel(x, y, pattern, patternColor);
  }
}

void Engine::drawShadedRect(int x, int y, int width, int height, int color) {
  bool willDraw = true;

  for (int yPos = y; yPos < y + height; yPos++) {
    bool isFirst = true;
    for (int xPos = x; xPos < x + width; xPos++) {
      if (isFirst && width % 2 == 0) {
        isFirst = false;
      } else {
        willDraw = !willDraw;
      }

      if (willDraw)
        drawPixel(xPos, yPos, color);
    }
  }
}

#define MARQUEE_RECT_DELAY_MS  166
#define MARQUEE_RECT_FRAMES    4

void Engine::drawMarqueeRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  int frame = millisNow() / MARQUEE_RECT_DELAY_MS % MARQUEE_RECT_FRAMES;
  int offset = MARQUEE_RECT_FRAMES - frame;
  int pixels = (w + h - 2) * 2;

  for (int i = 0; i < pixels; i++) {
    bool color = (offset + i) % 4 / 2;

    drawPixel(x, y, color);

    if (i < w - 1) {
      x++;
    } else if (i < w + h - 2) {
      y++;
    } else if (i < w * 2 + h - 3) {
      x--;
    } else {
      y--;
    }
  }
}

void Engine::drawButtonRect(int x, int y, int width, int height) {
  drawRectCornered(x, y, width, height, WHITE);
  fillRect(x+1, y+1, width-2, height-2, BLACK);      
}

void Engine::drawModal(int16_t w, int16_t h, uint8_t padding, uint8_t borderWidth, int yOffset) {
  modalWidth = w + padding * 2;
  modalHeight = h + padding * 2;
  int x = (SCREEN_WIDTH - modalWidth) / 2 + modalOffsetCx / 100;
  int y = (SCREEN_HEIGHT - modalHeight) / 2 + yOffset + modalOffsetCy / 100;
  modalX = x + padding;
  modalY = y + padding;
  modalPadding = padding;

  for (int i = 0; i < borderWidth; i++) {
    if (i == (borderWidth - 1))
      drawRectCornered(x - i, y - i, modalWidth + i * 2, modalHeight + i * 2, WHITE);
    else
      drawRect(x - i, y - i, modalWidth + i * 2, modalHeight + i * 2, WHITE);
  }
  fillRect(x + 1, y + 1, modalWidth - 2, modalHeight - 2, BLACK);
  
  // draw black outline
  drawFastHLine(x - borderWidth + 2, y - borderWidth, modalWidth + borderWidth * 2 - 4, BLACK);
  drawFastHLine(x - borderWidth + 2, y - borderWidth + modalHeight + borderWidth * 2 - 1, modalWidth + borderWidth * 2 - 4, BLACK);
  drawFastVLine(x - borderWidth, y - borderWidth + 2, modalHeight + borderWidth * 2 - 4, BLACK);
  drawFastVLine(x - borderWidth + modalWidth + borderWidth * 2 - 1, y - borderWidth + 2, modalHeight + borderWidth * 2 - 4, BLACK);
}

void Engine::drawSprite(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame, uint16_t color) {
  int16_t width = pgm_read_byte(&bitmap[0]);
  int16_t height = pgm_read_byte(&bitmap[1]);

  int byteWidth = height / 8 + (height % 8 > 0 ? 8 - height % 8 : 0);
  int startIndex = 2 + width * frame;

  if (currentBuffer == nullptr) {
    drawBitmap(x, y, bitmap+startIndex, width, height, color);
    return;
  }

  for (int i = startIndex; i < startIndex + width; i++) {
    int displayX = x + i - startIndex;
    unsigned char c = pgm_read_byte(&bitmap[i]);

    for (int bitIndex = 7; bitIndex >= 0; bitIndex--) {
      if (c & 0x01) {
        int displayY = y + height - bitIndex - 1;
        drawPixel(displayX, displayY, color);
      } else {
//        printf("bit %d is 0\n", i);
      }

      c = c >> 1;
    }
  }
}

void Engine::drawLineMarquee(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color, uint8_t drawEvery) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  bool xSwapped = false;
  if (x0 > x1) {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
    xSwapped = true;
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  int count = -1;
  for (; x0 <= x1; x0++) {    
    count++;
    int offset = millisNow() / MARQUEE_RECT_DELAY_MS % drawEvery;
    if (!xSwapped)
      offset = drawEvery - 1 - offset;
    if ((count + offset) % drawEvery == 0) {
      if (steep) {
        drawPixel(y0, x0, color);
      } else {
        drawPixel(x0, y0, color);
      }
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }      
}

void Engine::drawLineWithArrow(int x0, int y0, int x1, int y1, int degreesOffset, int radius, int color) {
  int points = 8;
  int cX = x1 * 100 + 50;
  int cY = y1 * 100 + 50;

  for (int i = 0; i < points; i++) {
    if (i % 2 == 0)
      continue;

    int degrees = (int)(360 * (1.0 * i / points) + 270) % 360;
    float radians = deg2rad(degrees + (360 - degreesOffset) + 1);

    int x = ((radius * 100 - 50) * cos(radians) + cX) / 100;
    int y = ((radius * 100 - 50) * sin(radians) + cY) / 100;

    if (i/2 > 0 && i/2 < 3)
      drawLine(cX / 100, cY / 100, x, y, color);
    drawLine(x0, y0, x1, y1, color);
  }      
}

void Engine::drawDebugInfo(bool outlined, int yOffset, bool showCpu) {

  engine.setFontTiny(2);
  engine.setTextColor(WHITE);

  static int lastValues[2] = { -1, -1 };
  int value, x, len;
  for (int i = 0; i < 1 + showCpu; i++) {
    if (i == 0) {
      value = FR2I(engine.frameRateActualInt, R8);
      x = 0;
    } else {
      value = engine.cpuLoad();
    }

    // NOTE: takes too long to rise to the next value?
    // value = (lastValues[i] + value) / 2;

    if (lastValues[i] < 0 || frameCount % 20 == 0)
      lastValues[i] = value;

    len = (lastValues[i] >= 100) + (lastValues[i] >= 10) + 2 + (i == 0) * 2;
    int width = len * 4;

    if (i > 0)
      x = SCREEN_WIDTH - width;

    if (outlined)
      fillRect(x - 2, yOffset, width + 3, 7, BLACK);

    engine.setCursor(x, yOffset);

    engine.printInt(lastValues[i]);
    if (i == 0) {
      engine.print(F("fps"));
    } else {
      engine.printChar('%');      
    }
  }

//   int value;
//   if (debugPointer == nullptr)
//     value = FR2I(frameRateActualInt, R8);
//   else
//     value = *debugPointer;
//   int digits = digitsInNumber(value);
//
//   setCursor(0, 0);
//   setFontTiny(2);
//   setTextColor(WHITE);
//   fillRect(0, 0, digits * 4 + 1, 7, BLACK);
//   static int lastValue = value;
//
//   if (debugPointer != nullptr || frameCount % 20 == 0)
//     lastValue = value;
//   printInt(lastValue);
}

// void Engine::plotCircle(int16_t x0, int16_t y0, uint16_t r, uint16_t color, int points, int degreesOffset, uint8_t * pattern, bool patternInvert) {
//   int cX = x0 * 100 + 50;
//   int cY = y0 * 100 + 50;
//   for (int i = 0; i < points; i++) {
//     int patternIndex = i/2 % 16 >= 8;
//     int patternBitIndex = i/2 % 8;
//     bool patternOn = (pattern == nullptr) || bitRead(pattern[patternIndex], patternBitIndex);
//     if (pattern != nullptr && (patternInvert ? patternOn : !patternOn))
//       continue;
//
//     int degrees = (int)(360 * (1.0 * i / points) + 270) % 360;
//     float radians = deg2rad(degrees + (360 - degreesOffset) + 1);
//
//     int x = ((r * 100 - 50) * cos(radians) + cX) / 100;
//     int y = ((r * 100 - 50) * sin(radians) + cY) / 100;
//
//     // if (degrees > 180)
//     //   y += 1;
//     // if (degrees > 90 && degrees < 270)
//     //   x += 1;
//
//     if (frameCount/2 % 2 || patternIndex || patternBitIndex)
//       drawPixel(x, y, color);
//   }
//
//   // draw center point (for debugging)
//   // drawPixel(x0, y0, color);
// }