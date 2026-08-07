#pragma once

#define EngineForArduboy

#include <Arduino.h>
#include <Print.h>
#include <Arduboy2.h>
#include "EngineFonts.h"
#include "../fr_math/FR_math.h"

extern Arduboy2 arduboy; // Объявляем глобальный объект

#define WIDTH 128
#define HEIGHT 64

#define CLEAR_BUFFER true

// Регистры для FR_math
#define R7 7
#define R8 8
#define R9 9
#define R10 10
#define R11 11
#define R12 12
#define R13 13
#define R14 14
#define R15 15
#define R16 16
#define R8_ONE_HALF (u16)(FR_NUM(0, 5, 1, R8))

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdint.h>
#include <string.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DEFAULT_FRAME_RATE 60

#define INCREMENT_TO_NUMERATOR 7
#define INCREMENT_TO_DENOMINATOR -44
#define DITHER_PATTERNS_COUNT 7
#define SCROLL_STEP 9
#define MARQUEE_RECT_FRAMES 4
#define MARQUEE_RECT_DELAY_MS 166

#define BUTTONS_ARRAY_SIZE 3

// Константы кнопок
#define BUTTON_BACK 0
#define BUTTON_LEFT 1
#define BUTTON_RIGHT 2
#define CONTROLLER_BUTTON_B 8
#define CONTROLLER_BUTTON_A 9
#define CONTROLLER_BUTTON_X 10
#define CONTROLLER_BUTTON_Y 11
#define CONTROLLER_BUTTON_BACK 12
#define CONTROLLER_BUTTON_GUIDE 13
#define CONTROLLER_BUTTON_START 14
#define CONTROLLER_BUTTON_LEFTSTICK 15
#define CONTROLLER_BUTTON_RIGHTSTICK 16
#define CONTROLLER_BUTTON_LEFTSHOULDER 17
#define CONTROLLER_BUTTON_RIGHTSHOULDER 18
#define CONTROLLER_BUTTON_DPAD_UP 19
#define CONTROLLER_BUTTON_DPAD_DOWN 20
#define CONTROLLER_BUTTON_DPAD_LEFT 21
#define CONTROLLER_BUTTON_DPAD_RIGHT 22
#define CONTROLLER_BUTTON_MAX 23

#define CONTROLLER_DPAD_REPEAT 6
#define CONTROLLER_DPAD_REPEAT_MS 100
#define CONTROLLER_DPAD_REPEAT_DELAY 15
#define CONTROLLER_DPAD_REPEAT_DELAY_MS 250

#define INPUT_BUTTONS 1
#define INPUT_ENCODER 2
#define INPUT_TAG 3
#define INPUT_MOUSE 4
#define INPUT_CONTROLLER 5
#define INPUT_KEYBOARD 6

#ifndef INVERSE
#define BLACK 0
#define WHITE 1
#define INVERSE 2
#endif

#ifndef UNSIGNED_SHORT_MAX
#define UNSIGNED_SHORT_MAX 65535
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

using namespace std;

enum font_style_t { FONT_DEFAULT, FONT_SYSTEM, FONT_TINY };

int16_t MULHI(int16_t a, int16_t b);
int16_t sinpi(int16_t x);
int16_t cospi(int16_t x);

#define HORIZONTAL_MENU_CLOSE -1
#define HORIZONTAL_MENU_HEIGHT 12
#define HORIZONTAL_MENU_RECT_PADDING 2
#define HORIZONTAL_MENU_RECT_TEXT_HEIGHT 5

class horizontal_menu_t {
public:
  horizontal_menu_t(const char* labelsString);
  const char* labels;
  s32 xMenuOffset = 0;
  int selectedItem;
  int rectX, rectWidth, rectLabelX, selectedLeftX, selectedRightX;
  int8_t minItem = 0;
  int8_t maxItem;
  int maxX = 0;
  int y;

  void drawBegin(int topY);
  void drawMenuItem(int i, const char* labelsStringData, int selected, int status);
  void drawCloseMenuItem();
  int drawMenuRect(int x, const char* stringData, int i, bool isLabel, bool isSelected = false);
  void drawMenuLine();
  void updateOffset();
  int8_t minItemSelected();
};

class Engine {
  public:
  
    uint16_t frameRate = DEFAULT_FRAME_RATE;
    uint16_t frameRateActualInt;
    uint16_t speedFactorInt;

    static unsigned long frameCount;
    long lastFrameStart;
    long nextFrameStart = 0;
    static uint8_t lastFrameDurationMs;
    uint8_t lastFrameRenderDurationMs;
    static uint8_t eachFrameMillis;

    static unsigned long thisFrameStart;
    static bool justRendered;
    constexpr static uint8_t lastActiveInput = INPUT_CONTROLLER;
    unsigned long lastActiveAt = 0;

    uint8_t buttons[BUTTONS_ARRAY_SIZE] = { 0 };
    uint8_t lastButtons[BUTTONS_ARRAY_SIZE] = { 0 };

    static uint8_t buttonsState();
    static bool pressed(uint8_t buttons);

    uint8_t controllerDefaultEncoderMode = true;
    uint8_t controllerDefaultButtonMode = true;
    int nextControllerDPadRepeat = CONTROLLER_DPAD_REPEAT;
    int nextControllerDPadRepeatDelay = CONTROLLER_DPAD_REPEAT_DELAY;
    int controllerDPadRepeatIn[4] = { -1 };

    bool ignoreButtonInput = false;
    bool ignoringCurrentInput = false;
    bool willStopIgnoringCurrentInput = false;

    uint8_t fontTiny = 0;
    int cursorX, cursorY;
    bool textOutline = false;
    int textColor = WHITE;

    uint8_t modalX, modalY, modalWidth, modalHeight, modalPadding;
    long modalOffsetCx = 0;
    long modalOffsetCy = 0;

    s32 xDisplayOffset = 0;
    static int xDisplayOffsetInt;
    s32 yDisplayOffset = 0;
    static int yDisplayOffsetInt;

    const int16_t *soundData = nullptr;
    int soundFrame = 0;
    int frameTotal;

    void addSound(uint8_t index, const int16_t* data, int length, int loopStartAt = -1);
    void setup(const char * title = nullptr);
    void setFrameRate(uint8_t rate);
    bool isNextFrame();
    void waitUntilNextFrame();
    void pollInput();
    int cpuLoad();
    void resetController();
    void ignoreCurrentInput();
    bool inactiveForAtLeast(int millis);

    static void delayMillis(uint16_t millis);
    static unsigned long millisNow();

    static bool readXYBufferAt(uint8_t x, uint8_t y, uint8_t * buffer = nullptr, uint8_t width = SCREEN_WIDTH);
    static void writeXYBufferAt(uint8_t x, uint8_t y, bool value, uint8_t * buffer = nullptr, uint8_t width = SCREEN_WIDTH);

    static int getStringCount(const char* stringData, char delimeter = '|');
    static int getStringPos(const char* stringData, int i, char delimeter = '|');
    static int getStringLen(const char* stringData, int i, char delimeter = '|', uint8_t charLimit = 0);
    int getStringWidthTiny(const char* stringData, int i, char delimeter = '|', uint8_t charLimit = 0);
    static void addNumberToString(char * charArray, int number, char separator = '\0');
    static int strStartsWith(const char *pre, const char *str);
    void printStringAt(const char* stringData, int pos, char delimeter = '|', uint8_t charLimit = 0);
    void printString(const char* stringData, int i, char delimeter = '|', uint8_t charLimit = 0);

    bool buttonPressed(int i);
    bool allButtonsUp();
    bool anyButtonChanged(bool controllerOnly = false);
    bool controllerLeftButtonPressed();
    bool controllerRightButtonPressed();
    bool controllerLeftButtonIsDown();
    bool controllerRightButtonIsDown();
    bool controllerLeftButtonUp();
    bool controllerRightButtonUp();
    bool leftButtonPressed();
    bool rightButtonPressed();
    bool eitherButtonPressed();
    bool backButtonPressed();
    bool buttonIsDown(int i);
    bool buttonWasDown(int i);
    bool leftButtonIsDown();
    bool rightButtonIsDown();
    bool eitherButtonIsDown();
    bool backButtonIsDown();
    bool otherButtonIsDown(int button);
    bool buttonUp(int i);
    bool leftButtonUp();
    bool rightButtonUp();
    bool eitherButtonUp();
    bool backButtonUp();

    static uint8_t *displayBuffer;
    static uint8_t *currentBuffer;
    static uint8_t currentBufferWidth, currentBufferHeight;

    static void setCurrentBuffer(uint8_t * buffer, int width, int height);
    static void clearCurrentBuffer();
    static void drawCurrentBufferAt(int xOffset = -1, int yOffset = -1, int scale = 1);
    
    // Графические примитивы 
    static uint8_t getPixel(uint8_t x, uint8_t y);
    static uint8_t getPixelFrom(uint8_t x, uint8_t y, uint8_t *buffer, uint8_t w, uint8_t h);
    static void drawPixel(int16_t x, int16_t y, int8_t color);
    static void writePixel(int16_t x, int16_t y, int8_t color);
    static void drawCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color);
    static void drawCircleHelper(int16_t x0, int16_t y0, uint8_t r, uint8_t corners, uint8_t color);
    static void fillCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color);
    static void fillCircleHelper(int16_t x0, int16_t y0, uint8_t r, uint8_t sides, int16_t delta, uint8_t color);
    static void drawRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color);
    static void drawRectCornered(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color);
    static void drawRectNoCorners(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color);
    static void drawFastVLine(int16_t x, int16_t y, uint8_t h, uint8_t color);
    static void drawFastHLine(int16_t x, int16_t y, uint8_t w, uint8_t color);
    static void fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, int8_t color);
    static void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    static void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    static void fillScreen(uint8_t color);
    static void drawRoundRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color);
    static void fillRoundRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color);
    static void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
    static void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
    static void clearDisplay();
    static void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);
    static void drawBitmapTransformed(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color, bool flipX = false, bool flipY = false, int scale = 1);
    static void drawBitmapAt(int x, int y, const unsigned char *bitmap, int frame = 0, int color = WHITE);
    static void drawBitmapTransformedAt(int x, int y, const unsigned char *bitmap, int frame = 0, int color = WHITE, bool flipX = false, bool flipY = false, int scale = 1);
    static void drawFastBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color = WHITE);
    static void drawFastBitmapTransformed(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color = WHITE, bool flipX = false, bool flipY = false);
    static void drawSlowXYBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color = WHITE);
    static void drawSlowXYBitmapTransformed(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color = WHITE, bool flipX = false, bool flipY = false, int scale = 1);
    static void drawSprite(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame, uint16_t color = WHITE);
    static void drawLineMarquee(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color = WHITE, uint8_t drawEvery = 3);
    static void drawLineWithArrow(int x0, int y0, int x1, int y1, int degreesOffset, int radius = 6, int color = WHITE);
    static void drawDitherPixel(int x, int y, int pattern, int patternColor = WHITE, int transparent = true);
    static void drawDitherOverlay(int pattern, int patternColor = BLACK);
    static void drawShadedRect(int x, int y, int width, int height, int color);
    static void drawMarqueeRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    static void drawButtonRect(int x, int y, int width = 16, int height = 12);
    void drawModal(int16_t w, int16_t h, uint8_t padding = 4, uint8_t borderWidth = 1, int yOffset = 0);
    static void drawDebugInfo(bool outlined = true, int yOffset = 0, bool showCpu = false);
    
    static void showDisplay();

    // Numbers
    static int randomInt(int minValue, int maxValue);
    static int randomIntFromMillis(int maxValue);
    static int digitsInNumber(int n);
    static float deg2rad(int degrees);
    static int xDirectionsN(int i, int n);
    static int yDirectionsN(int i, int n);
    static int16_t xDirectionsN16(int i, int n);
    static int16_t yDirectionsN16(int i, int n);
    static int distanceBetween(int x0, int y0, int x1, int y1);
    static float distanceBetween(float x0, float y0, float x1, float y1);    
    int32_t timeScaledDelta(int32_t delta);
    int32_t timeScaledDelay(int32_t delay);
    
template <typename T, typename U>
    T incrementTo(T& currentValue, U futureValue, bool timeAdjusted = true, int numerator = INCREMENT_TO_NUMERATOR, int denominator = INCREMENT_TO_DENOMINATOR) {
      T targetValue = static_cast<T>(futureValue);
      if (currentValue == targetValue)
        return 0;

      if (timeAdjusted && frameCount > 0)
        numerator = timeScaledDelta(numerator);
      int delta = (currentValue - targetValue) * numerator / denominator;
      if (delta == 0) {
        if (currentValue < targetValue)
          delta = 1;
        else
          delta = -1;
      }
      currentValue += delta;
      return delta * -1;
    }
    
    static void clampBetween(int& value, int minValue, int maxValue, bool loopAround = false);
    static void wrapBetween(float& value, float minValue, float maxValue);
    
    static void initRandomSeed();

    // Text
    void setTextSize(int newTextSize);
    void setTextColor(int color);
    void setFontTiny(int8_t newFontTiny);
    void setFontStyle(font_style_t style = FONT_DEFAULT, int size = 1, int color = WHITE);
    void setCursor(int x, int y);
    int maxCharWidth();
    void print(const __FlashStringHelper *ifsh); 
    void println(const __FlashStringHelper *ifsh);
    void print(const char* string, int maxCursorX = SCREEN_WIDTH);
    void printSine(const char* string, int radius, int frameMillis);
    void printInt(int n);
    void printSeconds(int n);
    void printChar(char character);
    void printCharTiny(char c, int8_t font);
    int charWidth(char c, int8_t font = -1);
    void printTime();
    int loadText(char * output, int length, int item);
    int drawText(const char *output, int length, int xOffset, int yOffset = 0, int lineHeight = 9, int item = -1, int alignCenter = false, int maxWidth = SCREEN_WIDTH - 1);
    void drawTextLine(const char * output, int startIndex, int endIndex, int x, int y);

    // Boot & System 
    static void bootLogo();
    static void drawLogoBitmap(int16_t y);
    static bool bootLogoShell(void (&drawLogo)(int16_t));
    static void bootLogoFadeOut();
    static void bootLogoSineOut();
    static void bootLogoCircleOpen();
    static void bootLogoGridOut();
    static void bootLogoDotsOut();
    static void bootLogoExplode();
    static void bootLogoFlickerOut();

    static Engine& getInstance();

  private:
    Engine() {}
    Engine(Engine const&);
    void operator=(Engine const&);
};

extern Engine &engine;