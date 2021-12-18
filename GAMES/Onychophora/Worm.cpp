#include "Worm.hpp"
#include "Bitmaps.hpp"

bool Worm::moveTo(Cell newHead, bool enlarge, bool shorten) {
  if (newHead.x > 7) {
    return false;
  }

  for (uint8_t i = 0; i < count; i++) {
    if (cells[i].intersects(newHead)) {
      return false;
    }
  }

  if (enlarge) {
    count++;
  }
  if (shorten) {
    count--;
  }
  if (count < 2) {
    count = 2;
  }
  if (count > 16) {
    count = 16;
  }

  for (uint8_t i = count - 1; i > 0; i--) {
    cells[i] = cells[i - 1];
  }

  cells[0] = newHead;

  return true;
}

uint8_t Worm::extrRotation(uint8_t f, uint8_t l) {
  if (cells[f].x > cells[l].x) {
    return U;
  } else if (cells[f].x < cells[l].x) {
    return D;
  } else {
    if (cells[f].y > cells[l].y) {
      return L;
    } else {
      return R;
    }
  }
}

/**
 * R _|      _
 *    _    U  |
 * L |
 *         D |_
 */

uint8_t Worm::joinRotation(uint8_t j) {
  if (cells[j].x > cells[j + 1].x) {
    if (cells[j].y > cells[j - 1].y) {
      return R;
    } else {
      return D;
    }
  } else if (cells[j].x < cells[j + 1].x) {
    if (cells[j].y > cells[j - 1].y) {
      return U;
    } else {
      return L;
    }
  } else if (cells[j].y < cells[j + 1].y) {
    if (cells[j].x > cells[j - 1].x) {
      return D;
    } else {
      return L;
    }
  } else {
    if (cells[j].x > cells[j - 1].x) {
      return R;
    } else {
      return U;
    }
  }
}

void Worm::render() {
  arduboy.drawBitmap(cells[0].y * 8, cells[0].x * 8, &bmpWormHead[extrRotation(1, 0)], 8, 8);
  for (uint8_t i = 1; i < count - 1; i++) {
    if (cells[i - 1].x == cells[i].x && cells[i + 1].x == cells[i].x) {
      arduboy.drawBitmap(cells[i].y * 8, cells[i].x * 8, &bmpWormBody[R], 8, 8);
    } else if (cells[i - 1].y == cells[i].y && cells[i + 1].y == cells[i].y) {
      arduboy.drawBitmap(cells[i].y * 8, cells[i].x * 8, &bmpWormBody[U], 8, 8);
    } else {
      arduboy.drawBitmap(cells[i].y * 8, cells[i].x * 8, &bmpWormJoin[Worm::joinRotation(i)], 8,
                         8);
    }
  }
  arduboy.drawBitmap(cells[count - 1].y * 8, cells[count - 1].x * 8,
                     &bmpWormTail[extrRotation(count - 1, count - 2)], 8, 8);
}

void Worm::reset(Cell c) {
  count = 0;
  addPiece(c);
}

void Worm::addPiece(Cell c) {
  cells[count] = c;
  count++;
}

bool Worm::fall(uint16_t solids[8]) {
  for (uint8_t i = 0; i < count; i++) {
    /* Can't go outside the bottom of the screen. */
    if (cells[i].x == 7) {
      return false;
    }
    if (solids[cells[i].x + 1] & (1 << (15 - cells[i].y))) {
      return false;
    }
  }

  for (uint8_t i = 0; i < count; i++) {
    cells[i].x++;
  }
  return true;
}

bool Worm::intersects(uint16_t solids[8]) {
  for (uint8_t i = 0; i < count; i++) {
    if (cells[i].intersects(solids)) {
      return true;
    }
  }
  return false;
}
