#include "Engine.h"

uint8_t Engine::eachFrameMillis;
unsigned long Engine::thisFrameStart;
uint8_t Engine::lastFrameDurationMs;
bool Engine::justRendered = false;

uint8_t *Engine::currentBuffer = nullptr;
uint8_t Engine::currentBufferWidth, Engine::currentBufferHeight;

unsigned long Engine::frameCount = 0;

bool Engine::isNextFrame() {
  unsigned long now = millisNow();
  long frameDurationMs = now - thisFrameStart;
  uint8_t remaining;

  // post render
  if (justRendered) {
    lastFrameRenderDurationMs = frameDurationMs;
    justRendered = false;
    return false;
  }

  // if it's not time for the next frame yet
  // else if (now < nextFrameStart) {
  else if (frameDurationMs < eachFrameMillis) {
    remaining = nextFrameStart - now;
    // if we have more than 1ms to spare, lets sleep
    // we should be woken up by timer0 every 1ms, so this should be ok
    // if (remaining > 1)
    if (++frameDurationMs < eachFrameMillis)
      delayMillis(1);
      // idle();
    return false;
  }

  // cout << "now: " << now << "; thisFrameStart: " << thisFrameStart << "; frameDurationMs: " << (int)frameDurationMs << "; eachFrameMillis: " << (int)eachFrameMillis << endl;

  long lastTwo = lastFrameDurationMs + (now - lastFrameStart);
  if (lastTwo <= 0) lastTwo = 1;
  float actualFps = 2000.0f / lastTwo;
  frameRateActualInt = (uint16_t)(actualFps * 256.0f);
  speedFactorInt = (uint16_t)((actualFps / frameRate) * 256.0f);
  if (speedFactorInt == 0) speedFactorInt = 256;

  lastFrameDurationMs = now - lastFrameStart;

  // pre-render

  // technically next frame should be last frame + each frame but if we're
  // running a slow render we would constnatly be behind the clock
  // keep an eye on this and see how it works.  If it works well the
  // lastFrameStart variable could be eliminated completely
  nextFrameStart = now + eachFrameMillis;
  lastFrameStart = now;
  thisFrameStart = now;
  justRendered = true;
  frameCount++;
  return justRendered;
}

int Engine::cpuLoad() {
  return lastFrameRenderDurationMs*100 / eachFrameMillis;
}

void Engine::setFrameRate(uint8_t rate) {
  frameRate = rate;
  eachFrameMillis = 1000 / rate;
}

bool Engine::inactiveForAtLeast(int millis) {
  return millisNow() - lastActiveAt > millis;
}

void Engine::resetController() {
  controllerDefaultEncoderMode = true;
  controllerDefaultButtonMode = true;
  nextControllerDPadRepeat = CONTROLLER_DPAD_REPEAT;
  nextControllerDPadRepeatDelay = CONTROLLER_DPAD_REPEAT_DELAY;
}

void Engine::ignoreCurrentInput() {
  ignoreButtonInput = true;
  ignoringCurrentInput = true;
  willStopIgnoringCurrentInput = false;
}

bool Engine::readXYBufferAt(uint8_t x, uint8_t y, uint8_t * buffer, uint8_t width) {
  uint16_t i = x + width * y;
  return bitRead(buffer[i / 8], i % 8);
}

void Engine::writeXYBufferAt(uint8_t x, uint8_t y, bool value, uint8_t * buffer, uint8_t width) {
  uint16_t i = x + width * y;
  bitWrite(buffer[i / 8], i % 8, value);
}

// String helpers
int Engine::getStringCount(const char* stringData, char delimeter) {
  int count = 1;
  char c;

  int i = 0;
  do {
    c = pgm_read_byte_near(stringData + i++);
    if (c == delimeter)
      count++;
  } while (c != '\0');

  return count;
}

int Engine::getStringPos(const char* stringData, int i, char delimeter) {
  int pos = 0;
  int currentItem = 0;
  char c;

  while (currentItem < i) {
    c = pgm_read_byte_near(stringData + pos);
    if (c == delimeter)
      currentItem++;
    else if (c == '\0')
      return -1;
    pos++;
  }
  return pos;
}

int Engine::getStringLen(const char* stringData, int i, char delimeter, uint8_t charLimit) {
  int pos = getStringPos(stringData, i, delimeter);
  if (pos < 0)
    return 0;

  char c;
  int len = 0;

  while (1) {
    c = pgm_read_byte_near(stringData + pos++);
    if (c == delimeter || c == '\0')
      break;
    len++;
  }

  return len;
}

int Engine::getStringWidthTiny(const char* stringData, int i, char delimeter, uint8_t charLimit) {
  int pos = getStringPos(stringData, i, delimeter);
  if (pos < 0)
    return 0;
  // todo: store last stringData, i, delimeter, and related info in engine public vars

  char c;
  int width = 0;

  while (1) {
    c = pgm_read_byte_near(stringData + pos++);
    if (c == delimeter || c == '\0')
      break;
    width += charWidth(c, fontTiny);
  }

  return width - 1;
}

// TODO: make safe
void Engine::addNumberToString(char * charArray, int number, char separator) {
  int charIndex = strlen(charArray);

  if (separator > 0)
    charArray[charIndex++] = separator;

  // for (int i = 6; i >= 0; i--) {
  //   if (i == 0 || number >= pow(10, i-1)) {
  //     charArray[charIndex++] = '0' + ((number % pow(10, i)) / pow(10, i-1));
  //   }
  // }

  if (number >= 100000)
    charArray[charIndex++] = '0'+((number % 1000000) / 100000);
  if (number >= 10000)
    charArray[charIndex++] = '0'+((number % 100000) / 10000);
  if (number >= 1000)
    charArray[charIndex++] = '0'+((number % 10000) / 1000);
  if (number >= 100)
    charArray[charIndex++] = '0'+((number % 1000) / 100);
  if (number >= 10)
    charArray[charIndex++] = '0'+number % 100 / 10;
  charArray[charIndex++] = '0'+number % 10;

  charArray[charIndex] = '\0';
}

int Engine::strStartsWith(const char *pre, const char *str) {
  size_t lenpre = strlen(pre),
         lenstr = strlen(str);
  return lenstr < lenpre ? 0 : strncmp(pre, str, lenpre) == 0;
}

horizontal_menu_t::horizontal_menu_t(const char* labelsString) {
  labels = labelsString;
  maxItem = engine.getStringCount(labels) - 1;
}

void horizontal_menu_t::drawBegin(int topY) {
  y = topY;
  engine.drawFastHLine(0, y + HORIZONTAL_MENU_HEIGHT - 2, SCREEN_WIDTH, WHITE);
  rectLabelX = 0;
  maxX = 0;

  engine.setTextColor(WHITE);
  engine.setFontTiny(2);
    
}

void horizontal_menu_t::updateOffset() {
  if (engine.frameCount <= 1)
    return;
  
  int32_t nextOffsetX;
  int padding = 16;
  int leftX = selectedLeftX - padding + FR2I(xMenuOffset, R8);
  int rightX = selectedRightX + padding + FR2I(xMenuOffset, R8);

  if (rightX > SCREEN_WIDTH) {
    nextOffsetX = (selectedRightX - SCREEN_WIDTH + padding) * -1;
    int minValue = (maxX - SCREEN_WIDTH - 1) * -1;
    if (nextOffsetX < minValue)
      nextOffsetX = minValue;
  } else if (leftX < 0) {
    nextOffsetX = (selectedLeftX - padding) * -1;
    if (nextOffsetX > 0)
      nextOffsetX = 0;
  } else {
    return;
  }
  
  engine.incrementTo(xMenuOffset, I2FR(nextOffsetX, R8) + R8_ONE_HALF);
}

void horizontal_menu_t::drawMenuItem(int i, const char* labelsStringData, int selected, int status) {
  if (i > minItem && status == 1) {
    drawMenuLine();
  }
  
  int labelWidth = 0;
  labelWidth += drawMenuRect(maxX, labels, i, true);

  if (labelWidth >= 4 && status == 2)
    engine.drawFastHLine(maxX + FR2I(xMenuOffset, R8) + 2, y + 9, labelWidth - 2 - 2, WHITE);

  maxX += labelWidth;

  maxX += drawMenuRect(maxX, labelsStringData, selected, false, selectedItem == i) + 1;
}

int horizontal_menu_t::drawMenuRect(int x, const char* stringData, int i, bool isLabel, bool isSelected) {
  int offsetX = FR2I(xMenuOffset, R8);

  rectX = x;
  if (isLabel)
    rectLabelX = x;

  if (stringData == nullptr) {
    rectWidth = engine.digitsInNumber(i) * 4 - 1;
  } else {
    rectWidth = engine.getStringWidthTiny(stringData, i);
  }

  if (isLabel && rectWidth <= 0) {
    return maxX > 0;
  }

  rectWidth += HORIZONTAL_MENU_RECT_PADDING * 2;

  bool willPrint = (x + offsetX < SCREEN_WIDTH) || (rectX + rectWidth + offsetX > 0);

  if (willPrint) {
    engine.setCursor(x + HORIZONTAL_MENU_RECT_PADDING + offsetX, y + HORIZONTAL_MENU_RECT_PADDING);
    engine.setTextColor(WHITE);
    if (stringData == nullptr)
      engine.printInt(i);
    else
      engine.printString(stringData, i);

    x += rectWidth + 1;

    if (isSelected) {
      engine.drawMarqueeRect(rectX + offsetX, y, rectWidth, HORIZONTAL_MENU_RECT_TEXT_HEIGHT + HORIZONTAL_MENU_RECT_PADDING * 2, WHITE);
      if (engine.eitherButtonIsDown())
        engine.fillRect(rectX+1 + offsetX, y+1, rectWidth - 2, HORIZONTAL_MENU_RECT_TEXT_HEIGHT + HORIZONTAL_MENU_RECT_PADDING * 2 - 2, INVERSE);
      selectedLeftX = rectLabelX;
      selectedRightX = x;
    }
  }

  return rectWidth;
}

void horizontal_menu_t::drawMenuLine() {
  engine.drawFastVLine(maxX + FR2I(xMenuOffset, R8), y, 10, WHITE);
  maxX++;
}

int8_t horizontal_menu_t::minItemSelected() {
  return minItem;
}

Engine& Engine::getInstance() {
  static Engine instance; // Guaranteed to be destroyed.
  // Instantiated on first use.
  return instance;
}

Engine &engine = Engine::getInstance();