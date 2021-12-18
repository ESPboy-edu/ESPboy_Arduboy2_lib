#pragma once

#include "Level.hpp"
#include "Utility.hpp"

class Onychophora {
 public:
  Onychophora()
      : tinyfont(arduboy.sBuffer, Arduboy2::width(), Arduboy2::height()),
        level(arduboy, tinyfont) {}
  void init();
  void loop();

 private:
  void update();
  void render();

 private:
  enum class Menus : uint8_t { main, game, credits, help, over };
  enum class MainSelections : uint8_t { play, credits, help };

  Arduboy2 arduboy;
  Tinyfont tinyfont;
  Level level;
  uint8_t lastLevel = 0;
  Menus currentMenu = Menus::main;
  MainSelections currentMainSelection = MainSelections::play;
};
