#include "App.h"

int8_t App::hasClicked = false;

void App::run() {
  engine.resetController();

  engine.initRandomSeed();

  willGoBack = false;

  setup();

  firstRun = false;

  while (true) {
    runLoop();
    yield();
  }
}

void App::runLoop() {
  static int8_t lastAppState = -1;
  if (lastAppState != appState) {
    appStateFrameCount = 0;
    lastAppState = appState;
  }

  engine.waitUntilNextFrame();

  loop();

  appStateFrameCount++;
  frameCount++;

  if (delayFrames > 0)
    delayFrames--;
  
  if (delayMillis > 0)
    delayMillis -= min(delayMillis, (int)engine.lastFrameDurationMs);
}

void App::showDisplay() {
  if (engine.modalOffsetCy && engine.frameCount > 2)
      engine.incrementTo(engine.modalOffsetCy, 0);

  engine.showDisplay();
}

void App::init() { }

void App::setup() { }

void App::menuSetup() { }
void App::submenuSetup() { }
void App::primarySetup() { }
void App::secondarySetup() { }
void App::tertiarySetup() { }

void App::loop() {
  if (appState == APP_STATE_MENU)
    menuLoop();
  else if (appState == APP_STATE_SUBMENU)
    submenuLoop();
  else if (appState == APP_STATE_PRIMARY)
    primaryLoop();
  else if (appState == APP_STATE_SECONDARY)
    secondaryLoop();
  else if (appState == APP_STATE_TERTIARY)
    tertiaryLoop();
}

void App::menuLoop() { }
void App::submenuLoop() { }
void App::primaryLoop() { }
void App::secondaryLoop() { }
void App::tertiaryLoop() { }

void App::reset() { }

void App::setState(int newAppState, int transition) {
  // if (transition > 0)
  //   startTransition(transition);

  appState = newAppState;
  hasClicked = false;

  engine.resetController();

  willGoBack = false;
  engine.ignoreCurrentInput();
  switchedToAppState(newAppState);
  
  if (appState == APP_STATE_MENU)
    menuSetup();
  else if (appState == APP_STATE_PRIMARY)
    primarySetup();
  else if (appState == APP_STATE_SECONDARY)
    secondarySetup();
  else if (appState == APP_STATE_TERTIARY)
    tertiarySetup();
}

void App::switchedToAppState(int newAppState) {
  
}

void App::startTransition(int transition) {
  if (transition == APP_TRANSITION_RECT_SLIDE_UP)
    engine.modalOffsetCy = SCREEN_HEIGHT * 100;
  else if (transition == APP_TRANSITION_RECT_SLIDE_DOWN)
    engine.modalOffsetCy = SCREEN_HEIGHT * -100;
}

void App::setDatastream(const unsigned char *d) {
  datastream = d;
  appIcon = d;
  appName = (char*)(d+24);
}

const unsigned char *App::d(int offset) {
  return datastream + offset;
}

const char *App::dChar(int offset, const unsigned char *d) {
  if (d == nullptr)
    return (char*)(datastream+offset);
  else
    return (char*)(d+offset);
}