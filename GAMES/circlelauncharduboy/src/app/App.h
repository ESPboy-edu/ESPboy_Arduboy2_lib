#pragma once

#include "../engine/EngineForArduboy.h"
#include "AppSounds.h"

#define APP_STATE_MENU       0
#define APP_STATE_SUBMENU    1
#define APP_STATE_PRIMARY    10
#define APP_STATE_SECONDARY  11
#define APP_STATE_TERTIARY   12

#define APP_TRANSITION_NONE             0
#define APP_TRANSITION_SLIDE_LEFT       1
#define APP_TRANSITION_SLIDE_RIGHT      2
#define APP_TRANSITION_RECT_SLIDE_UP    3
#define APP_TRANSITION_RECT_SLIDE_DOWN  4

#define APP_MENU_NO_BACKGROUND      0
#define APP_MENU_SHADED_BACKGROUND  1

#define APP_LETTERS_CAPITAL_ONE_ONLY  0
#define APP_LETTERS_ALL               1

#define APP_SWITCHING_MARGIN  8

class App {
public:
  App() {
    init();
  }

  struct object_t {
    int cX, cY;
  };

  struct moving_object_t : object_t {
    int speedCx, speedCy;
  };

  struct particle_t : moving_object_t {
    int framesRemaining = 0;
    int millisRemaining = 0;
  };
  
  // static particle_t appParticles[APP_PARTICLES_MAX];

  static int8_t hasClicked;

  static unsigned long lastActiveAt;

  const char* appName;
  const unsigned char *appIcon;
  const unsigned char *datastream;

  uint8_t appState = APP_STATE_MENU;
  unsigned long appStateFrameCount = 0;
  unsigned long frameCount = 0;

  bool willGoBack = false;

  bool firstRun = true;

  uint8_t delayFrames = 0;
  int delayMillis = 0;

  static int selectedLetterGroup;
  int submenuSelection;
  
  // char * alertText = nullptr;
  // bool usesParticles = false;

// Code execution control
  void run();
  void runLoop();
  void showDisplay();
  
// App-defined functions
  virtual void init();
  virtual void setup();
  virtual void loop();

  virtual void menuSetup();
  virtual void submenuSetup();
  virtual void primarySetup();
  virtual void secondarySetup();
  virtual void tertiarySetup();

  virtual void menuLoop();
  virtual void submenuLoop();
  virtual void primaryLoop();
  virtual void secondaryLoop();
  virtual void tertiaryLoop();

  virtual void reset();

  virtual void setState(int newAppState, int transition = APP_TRANSITION_NONE);
  virtual void switchedToAppState(int newAppState);
  
  void startTransition(int transition);

  
  void drawTrianglePointer(uint16_t x, uint16_t y, uint8_t d);

// Datastream
  void setDatastream(const unsigned char *d);
  const unsigned char *d(int offset);
  const char *dChar(int offset, const unsigned char *d = nullptr);
};