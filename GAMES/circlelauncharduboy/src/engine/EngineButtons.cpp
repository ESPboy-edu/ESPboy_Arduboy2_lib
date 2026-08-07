#include "Engine.h"

bool Engine::buttonIsDown(int i) {
  return bitRead(buttons[i / 8], i % 8);
}

bool Engine::buttonWasDown(int i) {
  return bitRead(lastButtons[i / 8], i % 8);
}

bool Engine::allButtonsUp() {
  return !buttons[0] && !buttons[1] && !buttons[2];
}

bool Engine::anyButtonChanged(bool controllerOnly) {
  if (ignoreButtonInput)
    return false;

  return (!controllerOnly && lastButtons[0] != buttons[0]) || lastButtons[1] != buttons[1] || lastButtons[2] != buttons[2];
}

bool Engine::buttonPressed(int i) {
  return buttonIsDown(i) && !buttonWasDown(i);
}

bool Engine::buttonUp(int i) {
  return !buttonIsDown(i) && buttonWasDown(i);
}

bool Engine::controllerLeftButtonPressed() {
  return (controllerDefaultButtonMode || backButtonIsDown()) && (buttonPressed(CONTROLLER_BUTTON_LEFTSHOULDER) || buttonPressed(CONTROLLER_BUTTON_B) || buttonPressed(CONTROLLER_BUTTON_X));
}

bool Engine::controllerRightButtonPressed() {
  return (controllerDefaultButtonMode || backButtonIsDown()) && (buttonPressed(CONTROLLER_BUTTON_RIGHTSHOULDER) || buttonPressed(CONTROLLER_BUTTON_A) || buttonPressed(CONTROLLER_BUTTON_Y));
}

bool Engine::controllerLeftButtonIsDown() {
  return (controllerDefaultButtonMode || backButtonIsDown()) && (buttonIsDown(CONTROLLER_BUTTON_LEFTSHOULDER) || buttonIsDown(CONTROLLER_BUTTON_B) || buttonIsDown(CONTROLLER_BUTTON_X));
}

bool Engine::controllerRightButtonIsDown() {
  return (controllerDefaultButtonMode || backButtonIsDown()) && (buttonIsDown(CONTROLLER_BUTTON_RIGHTSHOULDER) || buttonIsDown(CONTROLLER_BUTTON_A) || buttonIsDown(CONTROLLER_BUTTON_Y));
}

bool Engine::controllerLeftButtonUp() {
  return (controllerDefaultButtonMode || backButtonIsDown()) && (buttonUp(CONTROLLER_BUTTON_LEFTSHOULDER) || buttonUp(CONTROLLER_BUTTON_B) || buttonUp(CONTROLLER_BUTTON_X));
}

bool Engine::controllerRightButtonUp() {
  return (controllerDefaultButtonMode || backButtonIsDown()) && (buttonUp(CONTROLLER_BUTTON_RIGHTSHOULDER) || buttonUp(CONTROLLER_BUTTON_A) || buttonUp(CONTROLLER_BUTTON_Y));
}

bool Engine::leftButtonPressed() {
  if (ignoreButtonInput || backButtonIsDown())
    return false;

  if (buttonPressed(BUTTON_LEFT) || controllerLeftButtonPressed()) {
    lastActiveAt = millisNow();
    return true;
  } else {
    return false;
  }
}

bool Engine::rightButtonPressed() {
  if (ignoreButtonInput || backButtonIsDown())
    return false;

  if (buttonPressed(BUTTON_RIGHT) || controllerRightButtonPressed()) {
    lastActiveAt = millisNow();
    return true;
  } else {
    return false;
  }
}

bool Engine::eitherButtonPressed() {
  if (leftButtonPressed() || rightButtonPressed()) {
    // cout << "Engine::eitherButtonPressed() returned true" << endl;
    return true;
  } else {
    return false;
  }
}

bool Engine::backButtonPressed() {
  return false;
}

bool Engine::leftButtonIsDown() {
  if (ignoreButtonInput)
    return false;

  if (buttonIsDown(BUTTON_LEFT) || controllerLeftButtonIsDown()) {
    lastActiveAt = millisNow();
    return true;
  } else {
    return false;
  }
}

bool Engine::rightButtonIsDown() {
  if (ignoreButtonInput)
    return false;

  if (buttonIsDown(BUTTON_RIGHT) || controllerRightButtonIsDown()) {
    lastActiveAt = millisNow();
    return true;
  } else {
    return false;
  }
}

bool Engine::eitherButtonIsDown() {
  return leftButtonIsDown() || rightButtonIsDown();
}

bool Engine::backButtonIsDown() {
  return false;
}

bool Engine::otherButtonIsDown(int button) {
  if (ignoreButtonInput)
    return false;

  if (buttonIsDown(button)) {
    lastActiveAt = millisNow();
    return true;
  } else {
    return false;
  }
}

bool Engine::leftButtonUp() {
  if (ignoreButtonInput)
    return false;

  if (buttonUp(BUTTON_LEFT) || controllerLeftButtonUp()) {
    lastActiveAt = millisNow();
    return true;
  } else {
    return false;
  }
}

bool Engine::rightButtonUp() {
  if (ignoreButtonInput)
    return false;

  if (buttonUp(BUTTON_RIGHT) || controllerRightButtonUp()) {
    lastActiveAt = millisNow();
    return true;
  } else {
    return false;
  }
}

bool Engine::eitherButtonUp() {
  if (backButtonIsDown())
    return false;

  return leftButtonUp() || rightButtonUp();
}

bool Engine::backButtonUp() {
  return false;
}