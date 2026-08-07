#include "EngineForArduboy.h"

// Создаем глобальный объект ESPboy_Arduboy2
Arduboy2 arduboy;

uint8_t rawDisplayBuffer[(HEIGHT*WIDTH)/8];

void Engine::setup(const char * title) {
  arduboy.begin();
  arduboy.setFrameRate(DEFAULT_FRAME_RATE); 
  setFrameRate(DEFAULT_FRAME_RATE);
  displayBuffer = arduboy.getBuffer(); 

  bootLogo();
}

void Engine::delayMillis(uint16_t millis) {
  delay(millis);
}

unsigned long Engine::millisNow() {
  return millis();
}

uint8_t Engine::buttonsState() {
  return arduboy.buttonsState(); 
}

bool Engine::pressed(uint8_t buttons) {
  return arduboy.pressed(buttons);
}

void Engine::initRandomSeed() {
  randomSeed(millis()); // Используем встроенную функцию вместо чтения аналогового пина
}

bool Engine::bootLogoShell(void (&drawLogo)(int16_t)) {
  /*
  for (int16_t y = -15; y <= 24; y++) {
    if (pressed(RIGHT_BUTTON)) return false;

    clearDisplay();
    (*drawLogo)(y);
    showDisplay();
    delayMillis(15);
  }
  delayMillis(400);
  */
  return true;
}

// Заглушки для эффектов логотипа
void Engine::bootLogoFadeOut() {}
void Engine::bootLogoSineOut() {}
void Engine::bootLogoCircleOpen() {}
void Engine::bootLogoGridOut() {}
void Engine::bootLogoDotsOut() {}
void Engine::bootLogoFlickerOut() {}
void Engine::bootLogoExplode() {}

const PROGMEM uint8_t arduboy_logo[] = {
  0xF0, 0xF8, 0x9C, 0x8E, 0x87, 0x83, 0x87, 0x8E, 0x9C, 0xF8,
  0xF0, 0x00, 0x00, 0xFE, 0xFF, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x07, 0x0E, 0xFC, 0xF8, 0x00, 0x00, 0xFE, 0xFF, 0x03, 0x03,
  0x03, 0x03, 0x03, 0x07, 0x0E, 0xFC, 0xF8, 0x00, 0x00, 0xFF,
  0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
  0x00, 0x00, 0xFE, 0xFF, 0x83, 0x83, 0x83, 0x83, 0x83, 0xC7,
  0xEE, 0x7C, 0x38, 0x00, 0x00, 0xF8, 0xFC, 0x0E, 0x07, 0x03,
  0x03, 0x03, 0x07, 0x0E, 0xFC, 0xF8, 0x00, 0x00, 0x3F, 0x7F,
  0xE0, 0xC0, 0x80, 0x80, 0xC0, 0xE0, 0x7F, 0x3F, 0xFF, 0xFF,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF, 0xFF, 0x00,
  0x00, 0xFF, 0xFF, 0x0C, 0x0C, 0x0C, 0x0C, 0x1C, 0x3E, 0x77,
  0xE3, 0xC1, 0x00, 0x00, 0x7F, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0,
  0xC0, 0xE0, 0x70, 0x3F, 0x1F, 0x00, 0x00, 0x1F, 0x3F, 0x70,
  0xE0, 0xC0, 0xC0, 0xC0, 0xE0, 0x70, 0x3F, 0x1F, 0x00, 0x00,
  0x7F, 0xFF, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xE3, 0x77, 0x3E,
  0x1C, 0x00, 0x00, 0x1F, 0x3F, 0x70, 0xE0, 0xC0, 0xC0, 0xC0,
  0xE0, 0x70, 0x3F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00
};

void Engine::drawLogoBitmap(int16_t y) {
  drawFastBitmap(20, y, arduboy_logo, 88, 16, WHITE);
}

// Добавьте этот код в конец EngineForArduboy.cpp

void Engine::waitUntilNextFrame() {
  while (!isNextFrame()) {
    yield(); // Обязательно кормим WDT и отдаем такты ESP8266!
  }

  for (int i = 0; i < sizeof(buttons); i++) {
    lastButtons[i] = buttons[i];
    buttons[i] = 0;
  }

  pollInput();

  if (nextControllerDPadRepeat > 0) {
    for (int i = 0; i < 4; i++) {
      int button = CONTROLLER_BUTTON_DPAD_UP + i;
      if (buttonIsDown(button)) {
        // add nextControllerDPadRepeatDelay to first repeat if button was just pressed
        int delayFrames = (controllerDPadRepeatIn[i] < 0) ? engine.timeScaledDelay(nextControllerDPadRepeatDelay) : 0;

        // set this button in lastButtons to false to make button pressed be true
        if (controllerDPadRepeatIn[i]-- == 0) {
          bitWrite(lastButtons[button/8], button%8, false);
        }

        if (controllerDPadRepeatIn[i] < 0)
          controllerDPadRepeatIn[i] = engine.timeScaledDelay(nextControllerDPadRepeat) + delayFrames;
      } else {
        controllerDPadRepeatIn[i] = -1;
      }
    }
  }

  if (anyButtonChanged() && !ignoringCurrentInput) {
    lastActiveAt = millisNow();
  }

  if (ignoringCurrentInput && willStopIgnoringCurrentInput) {
    ignoreButtonInput = false;
    ignoringCurrentInput = false;
    willStopIgnoringCurrentInput = false;
  }

  if (ignoringCurrentInput && allButtonsUp())
    willStopIgnoringCurrentInput = true;

  // Воспроизведение звука для ESP8266 (заменяет старый beep.tone)
  if (soundData != nullptr) {
    int i = 0;
    frameTotal = 0;
    int freq, toneFrames;
    do {
      freq = (int16_t)pgm_read_word(&soundData[i * 2]);

      if (freq < 0) {
        soundData = nullptr;
        soundFrame = 0;
        break;
      }

      toneFrames = (int16_t)pgm_read_word(&soundData[i * 2 + 1]) + 1;

      if (soundFrame == frameTotal) {
        // Вывод звука на пин 0 (GPIO0 / D3 на ESPboy). Длительность в миллисекундах.
        tone(0, freq, toneFrames * (1000 / engine.frameRate)); 
      }

      i++;
      frameTotal += toneFrames;
    } while (soundFrame >= frameTotal);

    if (soundData != nullptr)
      soundFrame++;
  }
}

void Engine::pollInput() {
  for (int i = 0; i < sizeof(buttons) * 8; i++) {
    int buttonsIndex = i / 8;
    int bitIndex = i % 8;
    int value = -1;

    switch (i) {
      case BUTTON_LEFT:
        value = engine.controllerDefaultButtonMode && pressed(A_BUTTON);
        break;
      case BUTTON_RIGHT:
        value = engine.controllerDefaultButtonMode && pressed(B_BUTTON);
        break;
    }

    switch (i) {
      case CONTROLLER_BUTTON_DPAD_UP:
        value = pressed(UP_BUTTON);
        break;
      case CONTROLLER_BUTTON_DPAD_DOWN:
        value = pressed(DOWN_BUTTON);
        break;
      case CONTROLLER_BUTTON_DPAD_LEFT:
        value = pressed(LEFT_BUTTON);
        break;
      case CONTROLLER_BUTTON_DPAD_RIGHT:
        value = pressed(RIGHT_BUTTON);
        break; 
    }

    switch (i) {
      case CONTROLLER_BUTTON_A:
        value = pressed(B_BUTTON);
        break;
      case CONTROLLER_BUTTON_B:
        value = pressed(A_BUTTON);
        break;
      }
      
    if (value >= 0)
      bitWrite(buttons[buttonsIndex], bitIndex, value);
  }
}


// ==========================================
// НЕДОСТАЮЩИЕ ФУНКЦИИ ВЫВОДА ТЕКСТА И ЭКРАНА
// ==========================================

void Engine::showDisplay() {
  arduboy.display();
}

void Engine::bootLogo() {
  bootLogoShell(drawLogoBitmap);
}

void Engine::setCursor(int x, int y) {
  cursorX = x;
  cursorY = y;
  arduboy.setCursor(x, y);
}

void Engine::setTextColor(int color) {
  textColor = color;
  arduboy.setTextColor(color);
}

void Engine::setTextSize(int newTextSize) {
  fontTiny = 0;
  arduboy.setTextSize(newTextSize);
}

void Engine::printChar(char character) {
  if (!fontTiny) {
    arduboy.print(character);
  } else {
    printCharTiny(character, fontTiny);
  }
}

void Engine::print(const __FlashStringHelper *ifsh) {
  if (!fontTiny) {
    arduboy.print(ifsh);
  } else {
    PGM_P p = reinterpret_cast<PGM_P>(ifsh);
    while (true) {
      unsigned char c = pgm_read_byte(p++);
      if (c == 0) break;
      printChar(c);
    }
  }
}

void Engine::println(const __FlashStringHelper *ifsh) {
  if (!fontTiny) {
    arduboy.println(ifsh);
  } else {
    print(ifsh);
    printChar('\n');
  }
}