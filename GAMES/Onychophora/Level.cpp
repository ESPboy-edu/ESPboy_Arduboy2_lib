#include "Level.hpp"
#include "Bitmaps.hpp"
#include "Levels.hpp"

void Level::init(uint8_t level) {
  Cell head, body, tail;
  Levels::setupCells(&goal, &head, &body, &tail, level);
  worm.reset(head);
  worm.addPiece(body);
  worm.addPiece(tail);

  Levels::setupTiles(rock, soil, food, poop, nail, level);

  tutorial = Levels::tutorial(level);
  currentLevel = level;
}

void Level::onInput(Direction dir) {
  if (skips == 0) {
    Cell newHead = worm.getHead();

    switch (dir) {
      case Direction::up:
        if (newHead.x == 0) {
          return;
        };
        newHead.x--;
        break;
      case Direction::down:
        if (newHead.x == 7) {
          return;
        };
        newHead.x++;
        break;
      case Direction::left:
        if (newHead.y == 0) {
          return;
        };
        newHead.y--;
        break;
      case Direction::right:
        if (newHead.y == 15) {
          return;
        };
        newHead.y++;
        break;
    }

    updateWorm(newHead);
  }
}

void Level::updateWorm(Cell newHead) {
  if (newHead.intersects(rock)) {
    return;
  }

  bool enlarge = false;
  bool shrink = false;

  if (newHead.intersects(food)) {
    newHead.resetTilemap(food);
    enlarge = true;
  }

  if (newHead.intersects(poop)) {
    newHead.resetTilemap(poop);
    shrink = true;
  }

  worm.moveTo(newHead, enlarge, shrink);

  if (newHead.intersects(soil)) {
    newHead.resetTilemap(soil);
  }
}

void Level::update() {
  if (skips > 0) {
    skips--;
    return;
  }

  uint16_t solids[8];
  for (uint8_t row = 0; row < 8; row++) {
    solids[row] = rock[row] | soil[row] | food[row] | poop[row];
  }

  if (worm.fall(solids)) {
    skips = 2;
    updateWorm(worm.getHead());
    return;
  }

  if (worm.getHead().intersects(goal)) {
    currentLevel++;
    if (currentLevel < Levels::levelsCount) {
      init(currentLevel);
    }
  }

  uint16_t stable[8];
  for (uint8_t row = 0; row < 8; row++) {
    stable[row] = rock[row];
  }

  uint16_t oldStable = 0;
  uint16_t newStable = 1;

  while (newStable != oldStable) {
    oldStable = newStable;
    newStable = 0;
    for (uint8_t row = 0; row < 8; row++) {
      for (uint8_t col = 0; col < 16; col++) {
        if (soil[row] & (1 << (15 - col))) {
          if ((row < 7 && (stable[row + 1] & (1 << (15 - col)))) ||
              (row > 0 && (stable[row - 1] & (1 << (15 - col)))) ||
              (col < 15 && (stable[row] & (1 << (15 - col + 1)))) ||
              (col > 0 && (stable[row] & (1 << (15 - col - 1)))) ||
              (row == 7 || row == 0 || col == 15 || col == 0) /* Borders are safe. */
          ) {
            stable[row] |= (1 << (15 - col));
            newStable++;
          }
        }
      }
    }
  }

  for (uint8_t row = 6; row != 0; row--) {
    for (uint8_t col = 0; col < 16; col++) {
      if ((soil[row] & (1 << (15 - col))) && !(stable[row] & (1 << (15 - col)))) {
        soil[row] &= ~(1 << (15 - col));
        soil[row + 1] |= (1 << (15 - col));
        skips = 2;
      }
    }
  }

  if (skips == 2 && worm.intersects(soil)) {
    skips = 8;
  }

  if (skips == 0 && worm.intersects(soil)) {
    init(currentLevel);
  }

  if (worm.intersects(nail)) {
    init(currentLevel);
  }
}

void Level::render() {
  for (uint8_t row = 0; row < 8; row++) {
    for (uint8_t col = 0; col < 16; col++) {
      if (rock[row] & (1 << (15 - col))) {
        arduboy.drawBitmap(col * 8, row * 8, bmpRock, 8, 8);
      }
      if (soil[row] & (1 << (15 - col))) {
        arduboy.drawBitmap(col * 8, row * 8, bmpSoil, 8, 8);
      }
      if (food[row] & (1 << (15 - col))) {
        arduboy.drawBitmap(col * 8, row * 8, bmpFood, 8, 8);
      }
      if (poop[row] & (1 << (15 - col))) {
        arduboy.drawBitmap(col * 8, row * 8, bmpPoop, 8, 8);
      }
      if (nail[row] & (1 << (15 - col))) {
        arduboy.drawBitmap(col * 8, row * 8, bmpNail, 8, 8);
      }
    }
  }
  worm.render();
  arduboy.drawLine(0, 0, 127, 0);
  arduboy.drawLine(127, 0, 127, 63);
  arduboy.drawLine(127, 63, 0, 63);
  arduboy.drawLine(0, 63, 0, 0);
  arduboy.drawBitmap(goal.y * 8, goal.x * 8, bmpGoal, 8, 8);
  tinyfont.setCursor(2, 2);
  tinyfont.setTextColor(BLACK);
  tinyfont.print((reinterpret_cast<const __FlashStringHelper*>(tutorial)));
  tinyfont.setTextColor(WHITE);
}
