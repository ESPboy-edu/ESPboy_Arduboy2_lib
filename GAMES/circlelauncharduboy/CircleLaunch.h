#pragma once

#include "src/app/App.h"
#include "CircleLaunchData.h"

#define CIRCLE_LAUNCH_PARTICLES                   48

#define CIRCLE_LAUNCH_DIRECTION_COUNT             56 * 2
#define CIRCLE_LAUNCH_PARTICLES_DIRECTIONS_COUNT  16
#define CIRCLE_LAUNCH_CIRCLE_SIZE                 7
#define CIRCLE_LAUNCH_CIRCLE_RADIUS               3
#define CIRCLE_LAUNCH_COLUMNS                     11
#define CIRCLE_LAUNCH_MAX_SELECTABLE_LEVEL        3
#define CIRCLE_LAUNCH_MIN_COMBO                   3

#define CIRCLE_LAUNCH_TITLE_ANIMATION_FRAMES      60

#define CIRCLE_LAUNCH_SHIFT_EVERY_MILLIS          2750

#define CIRCLE_LAUNCH_SHIFT_TITLE_MILLIS          625

#define CIRCLE_LAUNCH_BORDER_X                    38 // 8 * 5 - 2

#define CIRCLE_LAUNCH_NEXT_CIRCLE_Y               60 // SCREEN_HEIGHT - 4
#define CIRCLE_LAUNCH_START_CIRCLE_Y              31 // SCREEN_HEIGHT / 2 - 1

#define CIRCLE_LAUNCH_TIME_BONUS_EACH             5
#define CIRCLE_LAUNCH_TIME_BONUS_SOUND_FRAMES     10

#define CIRCLE_LAUNCH_BASE_SCORE                  5

#define CIRCLE_LAUNCH_X_PADDING                   40 // SCREEN_WIDTH - CIRCLE_LAUNCH_COLUMNS * (CIRCLE_LAUNCH_CIRCLE_SIZE + 1);

static const unsigned char PROGMEM solidSprite[8] = { 255, 255, 255, 255, 255, 255, 255, 255 };

const char circleLaunchNewHighScoreText[] PROGMEM = "NEW HIGH SCORE!";

class CircleLaunch : public App {
  public:
    CircleLaunch() {
      setDatastream(CIRCLE_LAUNCH_DATA);
      // engine.debugMode = true;
    }

    struct particle_t {
      s16 x, y;
      s16 xSpeed, ySpeed;
      uint8_t framesRemaining = 0;
    } particles[CIRCLE_LAUNCH_PARTICLES];
    
    int selectedDirection, direction;
        
    int8_t circleBoard[88] = { };
    
    int8_t columnCount;
    const int8_t colorCount = 7;

    // s32 circleX, circleY;
    int32_t cX, cY;
    int16_t circleXInt, circleYInt;
    
    int32_t currentNextCircleCy;
    bool isMoving;
    int8_t currentColor, nextColor;
    
    // const uint8_t xPadding = SCREEN_WIDTH - CIRCLE_LAUNCH_COLUMNS * (CIRCLE_LAUNCH_CIRCLE_SIZE + 1);

    int8_t circleRow, circleCol;

    uint8_t removedCount;
    int totalCountRemoved;
    
    uint16_t score;
    uint16_t highScores[2] = { };
    bool newHighScore;
    
    int8_t gameMode = -1;
    int8_t level;
    int8_t levelSelected = 0;
    
    int shiftEveryMillis;
    
    int wobbleFrames;
    
    int selectedMenuItem = 0;

    unsigned long millisElapsed;
    uint32_t flashStartAt;

    bool showTitleAnimation;
    // int titleAnimationSpeed;
    long titleOffsetCy;

    bool gameOver;
    // bool active;

    int timeBonus;
    
    uint8_t countRemaining;

#define MAGIC_NUMBER 0xC1A0

    void setup() {
      uint16_t magic;
      EEPROM.get(100, magic);
      
      if (magic == MAGIC_NUMBER) { 
        EEPROM.get(102, highScores[0]);
        EEPROM.get(104, highScores[1]);
      } else {
        magic = MAGIC_NUMBER;
        EEPROM.put(100, magic);
        EEPROM.put(102, (uint16_t)0);
        EEPROM.put(104, (uint16_t)0);
        EEPROM.commit();
        }
        
      engine.bootLogoCircleOpen();

      if (firstRun)
        App::setState(APP_STATE_MENU);
    }

    void menuSetup() {
      score = 0;
      newHighScore = false;
      level = levelSelected;
      showTitleAnimation = false;
      // titleAnimationSpeed = 0;
      titleOffsetCy = 0;
      gameOver = false;
      // active = false;
    }

    void menuLoop() {
      engine.controllerDefaultButtonMode = false;
      int maxMenuItem = 1 + (gameMode >= 0 ? CIRCLE_LAUNCH_MAX_SELECTABLE_LEVEL + 1 : 0);

      selectedMenuItem += engine.buttonPressed(CONTROLLER_BUTTON_DPAD_RIGHT) - engine.buttonPressed(CONTROLLER_BUTTON_DPAD_LEFT);
      engine.clampBetween(selectedMenuItem, 0, maxMenuItem);

      if (hasClicked) {
        if (hasClicked > 0)
          selectedMenuItem = hasClicked - 1;
        hasClicked = false;
      }

      bool buttonPressed = engine.buttonPressed(CONTROLLER_BUTTON_A);
      
      if (buttonPressed) {
        if (selectedMenuItem < 2) {
          gameMode = selectedMenuItem;
          shiftEveryMillis = CIRCLE_LAUNCH_SHIFT_EVERY_MILLIS / (1 + gameMode);
          selectedMenuItem = 2 + 0;
        } else {
          level = selectedMenuItem - 2;
          reset();
          levelSelected = level;
          showTitleAnimation = true;
          delayFrames = CIRCLE_LAUNCH_TITLE_ANIMATION_FRAMES;
        }
      }

      if (delayFrames == 0 && showTitleAnimation) {
        delayFrames = 90;
        App::setState(APP_STATE_PRIMARY, APP_TRANSITION_SLIDE_LEFT);
      }

      updateParticles();

      engine.clearDisplay();
      drawTitleMenu();
      drawParticles();
            
      showDisplay();
    }

    void primarySetup() {
      selectedDirection = CIRCLE_LAUNCH_DIRECTION_COUNT / 4;
    }

    void primaryLoop() {
      getInput();
      
      if (engine.buttonIsDown(CONTROLLER_BUTTON_DPAD_UP) && engine.buttonIsDown(CONTROLLER_BUTTON_DPAD_DOWN)) {
        App::setState(APP_STATE_MENU);
        return;
      }

      if (gameOver) {
        engine.addSound(GAME_OVER_SOUND, SOUNDS_DATA, sizeof(SOUNDS_DATA));
        App::setState(APP_STATE_SECONDARY);
      }

      if (delayFrames > 0) {
        // do nothing
      } else {
        updateCurrentCircle();
        
        // if (countRemaining == 0) {
        //   bool hasParticles = false;
        //   for (int i = 0; i < CIRCLE_LAUNCH_PARTICLES; i++) {
        //     if (particles[i].framesRemaining > 0) {
        //       hasParticles = true;
        //       break;
        //     }
        //   }
        //
        //   if (!hasParticles) {
        //     engine.addSound(READY_SOUND, SOUNDS_DATA, sizeof(SOUNDS_DATA));
        //     getNextCircle();
        //     App::setState(APP_STATE_SECONDARY);
        //   }
        // }
        
        updateParticles();
        gameOver = isGameOver();
        millisElapsed += engine.lastFrameDurationMs;
      }
      
      if (wobbleFrames > 0)
        wobbleFrames--;
      
      engine.clearDisplay();
      draw();
      // engine.drawDebugInfo(true, 0, true);
      showDisplay();
    }

    void secondarySetup() {
      if (gameOver) {
        for (int y = -1; y <= SCREEN_HEIGHT; y += 2) {
          showParticlesAt(CIRCLE_LAUNCH_BORDER_X, y, CIRCLE_LAUNCH_PARTICLES / (SCREEN_HEIGHT / 2), 25);
        }
        
        if (highScores[gameMode] < score) {
          highScores[gameMode] = score;
          newHighScore = true;
        }
      }

      delayFrames = 90;
    }

    void secondaryLoop() {      
      if (gameOver && delayFrames)
        engine.modalOffsetCy = SCREEN_HEIGHT * -100;

      if (engine.modalOffsetCy != 0)
        flashStartAt = engine.millisNow();
      
      // if (delayFrames > 60)
      //   engine.modalOffsetCy = SCREEN_HEIGHT * -100;
      // else if (!gameOver)
      //   engine.modalOffsetCy = 0;

      if (delayFrames > 0 || (!gameOver && !isReady())) {
        // do nothing
      } else {
        willGoBack |= engine.buttonPressed(CONTROLLER_BUTTON_A) || engine.buttonPressed(CONTROLLER_BUTTON_B) || (!gameOver && appStateFrameCount > 60 * 6);
        
        if (willGoBack) {
          if (gameOver) {
            App::setState(APP_STATE_MENU, APP_TRANSITION_SLIDE_RIGHT);
            return;
          } else {
            level++;
            reset();
            delayFrames = 120;
            App::setState(APP_STATE_PRIMARY);
          }
        }
      }

      updateParticles();
      
      engine.clearDisplay();  
      draw();
      showDisplay();
    }

// Drawing
///////////////////////////////////////////////////////////////////////////////

    void draw();
    void drawAlert();
    void drawTitleMenu();
    void drawCurrentLine();
    void drawCurrentCircle();
    void drawNextCircle();
    void drawCircles();
    void drawBorder();
    void drawScore(int value, int x, int y);
    void drawHighScore();
    void drawCircleAt(int x, int y, int t);
    void drawWall();

// Actions
///////////////////////////////////////////////////////////////////////////////

    void getInput();
    void updateCurrentCircle();
    void reset();
    void dropCircle();
    void clearCircleAt(int col, int row);
    int markAdjacentCirclesFrom(int col, int row, int color);
    int markStrays();
    int removeMarked();
    void unmarkAll();
    void getNextCircle();
    
    
// Helpers
///////////////////////////////////////////////////////////////////////////////

    bool isReady();
    int xOffset();
    int yOffset();
    int rowsInColumn(int col);
    bool outOfBounds(int col, int row);
    int getColor(int col, int row, bool absoluteValue = true);
    int getColorInBounds(int col, int row, bool absoluteValue = true);
    void setColor(int col, int row, int color);
    bool isGameOver();
    int circleCount();
    bool willCollideAt(int playerX, int playerY);
    void showParticlesAt(int x, int y, int count = 10, int speed = 50);
    void updateParticles();
    void freeParticles(int circleCount);
    void drawParticles();
    void fastFillRect(int x, int y, int w, int h, int color);
};
