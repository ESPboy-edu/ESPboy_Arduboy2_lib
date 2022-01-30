#ifndef PLAYER_H
#define PLAYER_H

/*-------------------------
    _movablePlayer
  ------------------------ */
void _movablePlayer()
{
  if (pX < 0) {
    pX = 0;
  } else if (pX > pWidth - pCharSize) {
    pX = pWidth - pCharSize;
  } else if (pY < 16) {
    pY = 16;
  } else if (pY > pHeight - pCharSize) {
    pY = pHeight - pCharSize;
  }
}

/*-------------------------
    movePlayer
  ------------------------ */
void movePlayer() {

  int save_pX = pX;
  int save_pY = pY;

  if (pX % 8 == 0 && pY % 8 == 0) {
    if (arduboy.pressed(UP_BUTTON)) {
      pArrow = pLastPress = UP_BUTTON;
      pY -= pSpeed;
    } else if (arduboy.pressed(DOWN_BUTTON)) {
      pArrow = pLastPress = DOWN_BUTTON;
      pY += pSpeed;
    } else if (arduboy.pressed(LEFT_BUTTON)) {
      if (pX == 0) {
        return;
      }
      pArrow = pLastPress = LEFT_BUTTON;
      pX -= pSpeed;
    } else if (arduboy.pressed(RIGHT_BUTTON)) {
      pArrow = pLastPress = RIGHT_BUTTON;
      pX += pSpeed;
    }
  } else {
    if (pLastPress == UP_BUTTON) pY -= pSpeed;
    if (pLastPress == DOWN_BUTTON) pY += pSpeed;
    if (pLastPress == LEFT_BUTTON) pX -= pSpeed;
    if (pLastPress == RIGHT_BUTTON) pX += pSpeed;
  }

  _movablePlayer();

  // giveup
  if (arduboy.pressed(A_BUTTON) || arduboy.pressed(B_BUTTON)) {
    arduboy.clear();
    arduboy.display();
    delay(500);
    pLives--;
    if (pLives<=0) {
      pIsGameOver = true;
    }
    pX = pWidth - pCharSize;
    pY = pHeight - pCharSize;
    initBox(concurrent_box_max);
  }

  // collision
  for (int i = 0; i < box.concurrent_box_max; i++) {
    if (boxes[i].active) {
      if (abs(boxes[i].x - pX) < pCharSize / 2 + pCharSize / 2 &&
          abs(boxes[i].y - pY) < pCharSize / 2 + pCharSize / 2 )
      {
        arduboy.tunes.tone(NOTE_G3, 80);
        boxes[i].arrow = pArrow;
        pX = save_pX;
        pY = save_pY;
      }
    }
  }
}

/*-------------------------
    Draw Player
  ------------------------ */
void drawPlayer()
{
  arduboy.drawSlowXYBitmap(pX , pY - pYAdjust, bMan, 8, 8, 1);
}

#endif
