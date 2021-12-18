#include "Onychophora.hpp"
#include "Bitmaps.hpp"
#include "Levels.hpp"

void Onychophora::init() {
  // initiate arduboy instance
  arduboy.begin();

  // set the framerate to 16 to save battery
  arduboy.setFrameRate(16);

  uint8_t o = EEPROM.read(EEPROM_STORAGE_SPACE_START + 0);
  uint8_t n = EEPROM.read(EEPROM_STORAGE_SPACE_START + 1);
  uint8_t y = EEPROM.read(EEPROM_STORAGE_SPACE_START + 2);
  if (o == 'o' && n == 'n' && y == 'y') {
    lastLevel = EEPROM.read(EEPROM_STORAGE_SPACE_START + 3);
  } else {
    lastLevel = 0;

    EEPROM.write(EEPROM_STORAGE_SPACE_START + 0, 'o');
    EEPROM.write(EEPROM_STORAGE_SPACE_START + 1, 'n');
    EEPROM.write(EEPROM_STORAGE_SPACE_START + 2, 'y');
    EEPROM.write(EEPROM_STORAGE_SPACE_START + 3, 0);
    EEPROM.commit();
  }
}

void Onychophora::loop() {
  // pause render until it's time for the next frame
  if (!(arduboy.nextFrame())) {
    return;
  }
  update();
  render();
}

void Onychophora::update() {
  arduboy.pollButtons();

  switch (currentMenu) {
    case Menus::main:
      if (arduboy.justPressed(UP_BUTTON)) {
        switch (currentMainSelection) {
          case MainSelections::play:
            currentMainSelection = MainSelections::help;
            break;
          case MainSelections::credits:
            currentMainSelection = MainSelections::play;
            break;
          case MainSelections::help:
            currentMainSelection = MainSelections::credits;
            break;
        }
      }
      if (arduboy.justPressed(DOWN_BUTTON)) {
        switch (currentMainSelection) {
          case MainSelections::play:
            currentMainSelection = MainSelections::credits;
            break;
          case MainSelections::credits:
            currentMainSelection = MainSelections::help;
            break;
          case MainSelections::help:
            currentMainSelection = MainSelections::play;
            break;
        }
      }
      if (arduboy.justPressed(A_BUTTON)) {
        switch (currentMainSelection) {
          case MainSelections::play:
            level.init(lastLevel);
            currentMenu = Menus::game;
            break;
          case MainSelections::credits:
            currentMenu = Menus::credits;
            break;
          case MainSelections::help:
            currentMenu = Menus::help;
            break;
        }
      }

      break;
    case Menus::game:
      level.update();
      if (level.getCurrentLevel() >= Levels::levelsCount) {
        currentMenu = Menus::over;
        lastLevel = 0;
        level.init(lastLevel);
      } else if (arduboy.justPressed(UP_BUTTON)) {
        level.onInput(Direction::up);
      } else if (arduboy.justPressed(DOWN_BUTTON)) {
        level.onInput(Direction::down);
      } else if (arduboy.justPressed(RIGHT_BUTTON)) {
        level.onInput(Direction::right);
      } else if (arduboy.justPressed(LEFT_BUTTON)) {
        level.onInput(Direction::left);
      } else if (arduboy.justPressed(A_BUTTON)) {
        level.init(level.getCurrentLevel());
      } else if (arduboy.justPressed(B_BUTTON)) {
        currentMenu = Menus::main;
      } else if (level.getCurrentLevel() > lastLevel) {
        lastLevel = level.getCurrentLevel();
        EEPROM.write(EEPROM_STORAGE_SPACE_START + 3, lastLevel);
        EEPROM.commit();
      }

      break;
    case Menus::over:
      if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
        lastLevel = 0;
        currentMenu = Menus::main;
      }
      break;
    case Menus::credits:
      if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
        currentMenu = Menus::main;
      }
      break;
    case Menus::help:
      if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
        currentMenu = Menus::main;
      }
      break;
  }
}

void Onychophora::render() {
  // clear the screen to black
  arduboy.clear();

  switch (currentMenu) {
    case Menus::main:

      arduboy.drawBitmap(0, 0, bmpLogo, 128, 16);

      tinyfont.setCursor(44, 25);
      tinyfont.print(F(" PLAY"));
      tinyfont.setCursor(44, 35);
      tinyfont.print(F("CREDITS"));
      tinyfont.setCursor(44, 45);
      tinyfont.print(F(" HELP"));
      switch (currentMainSelection) {
        case MainSelections::play:
          arduboy.drawBitmap(30, 25 - 2, bmpArrow, 8, 8);
          break;
        case MainSelections::credits:
          arduboy.drawBitmap(30, 35 - 2, bmpArrow, 8, 8);
          break;
        case MainSelections::help:
          arduboy.drawBitmap(30, 45 - 2, bmpArrow, 8, 8);
          break;
      }
      break;
    case Menus::game:
      level.render();
      break;
    case Menus::over:
      tinyfont.setCursor(10, 10);
      tinyfont.print(F("CONGRATULATIONS!\nYOU COMPLETED ALL THE\nLEVELS IN THE GAME!"));
      break;
    case Menus::credits:
      tinyfont.setCursor(2, 10);
      tinyfont.print(F("ONYCHOPHORA WAS MADE BY"));
      tinyfont.setCursor(2, 20);
      tinyfont.print(F("\t\tRENATO GROTTESI"));
      break;
    case Menus::help:
      tinyfont.setCursor(2, 0);
      tinyfont.print(
          F("USE THE D-PAD TO MOVE THE\n" /**/
            "WORM TO REACH THE EXIT.\n"   /**/
            "EAT ROOTS TO GET LONGER.\n"  /**/
            "JUNK MAKES YOU SHORTER.\n"   /**/
            "ROCKS CAN'T BE EATEN.\n"     /**/
            "YOU CAN DIG SOIL.\n"         /**/
            "SOIL NOT CONNECTED TO\n"     /**/
            "ROCKS WILL FALL DOWN.\n"     /**/
            "DON'T GET BURIED!\n"         /**/
            "PRESS (A) TO RESTART.\n"     /**/
            "\nGOOD LUCK!\n"              /**/
            ));
      break;
  }

  // tell the arduboy to swap buffers
  arduboy.display();
}
