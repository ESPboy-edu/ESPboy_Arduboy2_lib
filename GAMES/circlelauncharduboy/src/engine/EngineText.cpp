#include "Engine.h"

void Engine::setFontStyle(font_style_t style, int size, int color) {
  textOutline = false;
  setTextSize(size);
  setTextColor(color);
  if (style == FONT_DEFAULT) {
    setFontTiny(1);
  } else if (style == FONT_TINY) {
    setFontTiny(2);
  } else {
    setTextSize(1);
  }
}

void Engine::print(const char* string, int maxCursorX) {
  int i = 0;
  do {
    if (cursorX >= maxCursorX)
      return;
    printChar(string[i]);
    i++;
  } while (string[i] != '\0');
}

void Engine::printSine(const char* string, int radius, int frameMillis) {
  int w = getStringWidthTiny(string, 0);
  int y = cursorY;
  int i = 0;

  int totalFrames = 60;
  int frame = (engine.millisNow() / frameMillis) % totalFrames;
  // int yDelta = FR2I(radius * (s32)(sinpi(frame * UNSIGNED_SHORT_MAX / totalFrames)), R15);
  //
  // cursorY = y + yDelta;

  // engine.fillRect(cursorX - 1, cursorY - 1, w + 1, 8, BLACK);
  
  int len = strlen(string);
  
  char c;
  while (true) {
    int yDelta = FR2I(radius * (s32)(sinpi(frame * UNSIGNED_SHORT_MAX / totalFrames + i * UNSIGNED_SHORT_MAX / len)), R15);  
    cursorY = y + yDelta;

    c = pgm_read_byte_near(&string[i++]);
    if (c == '\0')
      break;

    engine.fillRect(cursorX - 1, cursorY - 1, 4, 7, BLACK);
    printChar(c);
  }
}

void Engine::printInt(int n) {
  char buffer[9] = { };

  if (n < 0) {
    printChar('-');
    n *= -1;
  }

  int len = 0;
  do {
    buffer[len++] = '0' + n % 10;
    n /= 10;
  } while (n > 0);

  for (int i = len - 1; i >= 0; i--) {
    printChar(buffer[i]);
  }

  // sprintf(buffer, "%d", n);
  //
  // for (int i = 0; i < strlen(buffer); i++) {
  //   printChar(buffer[i]);
  // }
}

void Engine::printSeconds(int n) {
  int seconds = n % 60;
  int minutes = n / 60 % 60;
  // int hours = n / 60 / 60 % 60;
  printInt(minutes);
  printChar(':');
  if (seconds < 10)
    printInt(0);
  printInt(seconds);
}

void Engine::setFontTiny(int8_t newFontTiny) {
  fontTiny = newFontTiny;
}

void Engine::printCharTiny(char c, int8_t font) {
  if (cursorX >= SCREEN_WIDTH)
    return;

  int charHeight, asciiStart, asciiEnd;
  if (font == 1) {
    charHeight = 7;
    asciiStart = FONT4x6_ASCII_START;
    asciiEnd =   FONT4x6_ASCII_END;
  } else if (font == 2) {
    charHeight = 6;
    asciiStart = FONT3x5_ASCII_START;
    asciiEnd =   FONT3x5_ASCII_END;
  } else {
    return;
  }

  int i = -1;

  if ((int)c >= asciiStart && (int)c <= asciiEnd) {
    i = (int)c - asciiStart;
  // } else if (c == '•') {
  //   cout << "degree sign: " << c << endl;
  //   i = asciiEnd - asciiStart + DEGREE_SIGN_OFFSET;
  } else if (c != ' ') {
    // cout << "unrecognized: " << c << " (" << (int)c << ")" << endl;
  }

  if (i >= 0) {
    // TODO: DRY this up into a single call
    const uint8_t *bitmap;
    if (font == 1)
      bitmap = font4x6_images;
    else
      bitmap = font3x5_images;

    int y = cursorY;
// #ifdef EngineForArduboy
    if (font == 2)
      y -= 2;
// #endif

    drawSprite(cursorX, y, bitmap, i, textColor);

    if (font == 1) {
      if (c == '#') {
        drawPixel(cursorX + 4, cursorY + 2, textColor);
        drawPixel(cursorX + 4, cursorY + 4, textColor);
      } else if (c == 'w' || c == 'm' || c == 'W' || c == 'M') {
        drawFastVLine(cursorX + 4, cursorY + (c >= 'a' ? 1 : 0), 5 + (c >= 'a' ? 0 : 1), textColor);
      } else if (c == 'T') {
        drawPixel(cursorX + 4, cursorY, textColor);
      }
    } else if (font == 2) {
      if (c == '%') {
        drawPixel(cursorX + 3, cursorY + 1, textColor);
        drawPixel(cursorX + 3, cursorY + 4, textColor);
      } else if (c == '#') {
        drawSprite(cursorX + 2, y, bitmap, i, textColor);
      }
    }
  }

  cursorX += charWidth(c, font);
}

int Engine::charWidth(char c, int8_t font) {
  if (font == -1) {
    font = fontTiny;
  }

  if (font == 1) {
    if (c == '#' || c == 'w' || c == 'm' || c == 'W' || c == 'M' || c == 'T')
      return 6;
    else if (c == 'I' || c == 'i' || c == 'l' || c == '.' || c == ',' || c == ':' || c == ' ' || c == '`' || c == '\'')
      return 4;
    else
      return 5;
  } else if (font == 2) {
    if (c == 'l' || c == '`')
      return 3;
    else if (c == '%' || c == '#')
      return 5;
    else
      return 4;
  }

  return 0;
}

int Engine::maxCharWidth() {
  return 4 + (fontTiny < 2);
}

int Engine::loadText(char * output, int length, int item) {
  int row = 0;
  int rowCharCount = 0;
  int textWidth = 0;
  char lastChar;

  if (length < 0)
    length = sizeof(output);

  int currentItem = 0;

  for (int i = 0; i < length; i++) {
    char c = output[i];

    if (c == '|') {
      currentItem += 1;
      if (item >= 0)
        continue;
    }

    if (item >= 0 && currentItem != item)
      continue;

    // render return characters
    if (c == '\n') {
      row++;
      textWidth = 0;
      lastChar = '\n';
      continue;
    }

    if (c == ' ' && lastChar == ' ')
      continue;

    // determine if there is enough space for the next word and go to new line if needed
    if (textWidth > 0 && lastChar == ' ') {
      int nextWidth = 0;

      for (int j = i; j < length; j++) {
        if (output[j] == ' ' || output[j] == '\n' || (item >= 0 && output[j] == '|'))
          break;

        nextWidth += charWidth(c, 1);

        if (textWidth + nextWidth > SCREEN_WIDTH) {
          row++;
          textWidth = 0;
          lastChar = '\n';
          // setCursor(0, row * lineHeight + yOffset);
          break;
        }
      }
    }

    // skip leading spaces
    if (textWidth == 0 && c == ' ') {
      continue;
    }

    // printChar(c);
    textWidth += charWidth(c, 1);

    // lastTextHeight = (row + 1) * lineHeight;
    lastChar = c;
  }

  // cout << "rows: " << row << endl;

  // return lastTextHeight;

  return 0;
}

int Engine::drawText(const char *output, int length, int xOffset, int yOffset, int lineHeight, int item, int alignCenter, int maxWidth) {
  if (xOffset >= SCREEN_WIDTH || xOffset <= (SCREEN_WIDTH - 1) * -1) {
    return 0;
  }

  // loadText(output, length);

  setTextColor(WHITE);

  int y = 0;
  int row = 0;
  int textWidth = 0;
  char lastChar;

  setCursor(xOffset, yOffset);
  
  if (length < 0)
    length = sizeof(output);
  
  int currentItem = 0;
  int lineStart = -1;
  int lineEnd = -1;
  int emptyLineCount = 0;
  
  for (int i = 0; i <= length; i++) {
    char c = pgm_read_byte(&output[i]);
    bool endOfLine = false;
    bool endOfCurrentItem = false;
    
    if (c == '\0') {
      endOfLine = true;
      lineEnd = i - 1;
    }
    
    else if (item >= 0 && c == '|') {
      if (currentItem == item) {
        endOfCurrentItem = endOfLine = true;
      }

      currentItem += 1;
      
      if (currentItem == item) {
        lineStart = i+1;
      }
    }

    // skip text for other items
    if (!endOfCurrentItem && item >= 0 && currentItem != item)
      continue;

    // render return characters
    if (c == '\n')
      endOfLine = true;

    // skip multiple spaces
    else if (c == ' ' && lastChar == ' ')
      continue;

    // skip leading spaces
    else if (textWidth == 0 && c == ' ') {
      continue;
    }

    // line starts here if textWidth is zero
    if (lineStart < 0 && textWidth == 0)
      lineStart = i;      

    // determine if there is enough space for the next word and go to new line if needed
    if (!endOfLine && textWidth > 0 && lastChar == ' ') {
      int nextWidth = 0;
      
      for (int j = i; j < length; j++) {
        char cc = pgm_read_byte(&output[j]);
        if (cc == ' ' || cc == '\n' || (item >= 0 && cc == '|') || cc == '\0')
          break;

        nextWidth += charWidth(cc);

        if (textWidth + nextWidth > (SCREEN_WIDTH - 1)) {
          endOfLine = true;
          lineEnd = i - 1;
        }
      }
    }

    if (endOfLine) {
      if (lineEnd < 0)
        lineEnd = i - 1;
      
      if (textWidth > 0) {
        if (lastChar == ' ')
          textWidth -= charWidth(lastChar);
        int x = xOffset + (alignCenter ? (SCREEN_WIDTH - 1 - textWidth)/2 : 0);
        drawTextLine(output, lineStart, lineEnd, x, row * lineHeight + yOffset);
        emptyLineCount = 0;        
      } else {
        emptyLineCount++;
      }

      row++;
      textWidth = 0;
      lastChar = '\n';
      lineStart = lineEnd = -1;
    }

    if (c == '\0') {
      break;
    }
      
    else if (c != '\n' && c != '|') {
      textWidth += charWidth(c);
      lastChar = c;
      if (lineStart < 0)
        lineStart = i;
    }
  }
  
  int height = (row - emptyLineCount) * lineHeight;
  // cout << "height: " << height << "; row: " << row << "; emptyLineCount: " << emptyLineCount << endl;
  
  return height;
}

void Engine::drawTextLine(const char * output, int startIndex, int endIndex, int x, int y) {
  setCursor(x, y);
  
  char lastChar = '\0';
  
  for (int i = startIndex; i <= endIndex; i++) {
    char c = pgm_read_byte(&output[i]);

    // skip multiple spaces
    if (c == ' ' && lastChar == ' ')
      continue;
    
    printChar(c);
  }
}

void Engine::printStringAt(const char* stringData, int pos, char delimeter, uint8_t charLimit) {
  char c;

  while (1) {
    c = pgm_read_byte_near(stringData + pos++);
    if (c == delimeter || c == '\0')
      break;
    printChar(c);
  }
}

void Engine::printString(const char* stringData, int i, char delimeter, uint8_t charLimit) {
  int pos = getStringPos(stringData, i, delimeter);
  if (pos < 0)
    return;
  printStringAt(stringData, pos, delimeter);
}
