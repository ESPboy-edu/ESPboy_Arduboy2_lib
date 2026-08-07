#include "CircleLaunch.h"

// Drawing
///////////////////////////////////////////////////////////////////////////////

void CircleLaunch::draw() {
  drawCircles();
  drawWall();
  drawBorder();
  drawScore(score, 0, 0);

  if (appState != APP_STATE_SECONDARY || gameOver)
    drawCurrentCircle();
  
  if (!delayFrames) {
    drawCurrentLine();
    if (appState != APP_STATE_SECONDARY || gameOver)
      drawNextCircle();
  }

  if (!gameOver || !delayFrames)
    drawAlert();

  drawParticles();   
  
  // cout << "(" << circleXInt - FR2I(circleX, R8) + circleYInt - FR2I(circleY, R8) << ") circleXInt: " << circleXInt << ", circleX: " << FR2I(circleX, R8) << "; circleYInt: " << circleYInt << ", circleY: " << FR2I(circleY, R8) << endl;
}

void CircleLaunch::drawAlert() {
  if (appState == APP_STATE_PRIMARY && !delayFrames)
    return;

  int len = 7 + (level >= 10) + 7 * (appState == APP_STATE_SECONDARY && !gameOver);
  
  int rectWidth = 32 + (level >= 10) * 5 + 32 * (appState == APP_STATE_SECONDARY && !gameOver);
  if (gameOver)
    rectWidth += 12;
  int rectHeight = 6 + 12 * (appState == APP_STATE_SECONDARY && !gameOver);
  engine.drawModal(rectWidth, rectHeight);
  
  engine.setFontTiny(1);
  engine.setTextColor(WHITE);

  int x = engine.modalX + (engine.modalWidth - len * 5 - 4 + 2 * (appState == APP_STATE_SECONDARY)) / 2;
  if (gameOver)
    x = engine.modalX;
  engine.setCursor(x, engine.modalY);

  if (!gameOver) {
    engine.print(F("Level "));
    engine.printInt(level+1);
  } else {
    engine.print(F("Game Over"));
    drawHighScore();
    EEPROM.put(102 + gameMode * 2, highScores[gameMode]);
    EEPROM.commit();
    return;
  }
  
  if (appState == APP_STATE_SECONDARY) {
    engine.print(F(" Clear!"));
    
    engine.setFontTiny(2);
    x = engine.modalX + (engine.modalWidth - 4 - (12 + engine.digitsInNumber(timeBonus * CIRCLE_LAUNCH_TIME_BONUS_EACH)) * 4) / 2 - 1;
    engine.setCursor(x, engine.modalY + 12);
    engine.print(F("TIME BONUS: "));
    int timeBonusScore = timeBonus * CIRCLE_LAUNCH_TIME_BONUS_EACH;
    if (timeBonusScore)
      engine.printInt(timeBonusScore);
    // else
    //   engine.printChar('-');

    if (delayFrames)
      return;

    if (!isReady()) {
      timeBonus += 1;
      score += 1 * CIRCLE_LAUNCH_TIME_BONUS_EACH;
      if (engine.frameCount % CIRCLE_LAUNCH_TIME_BONUS_SOUND_FRAMES == 0)
        engine.addSound(CAUGHT_SOUND, SOUNDS_DATA, sizeof(SOUNDS_DATA));
    }
      
  }
}

void CircleLaunch::drawTitleMenu() {
  for (int r = 0; r < 6; r++) {
    for (int c = 0; c <= SCREEN_WIDTH / 8; c++) {
      int x = c * 8 - engine.millisNow()/CIRCLE_LAUNCH_SHIFT_TITLE_MILLIS % 8;
      int y = titleOffsetCy / 100 + SCREEN_HEIGHT - (r + 1) * 8 + 4;
      engine.drawBitmapAt(x, y, d(CIRCLE_LAUNCH_BACKGROUNDS), min(4, r));
    }
  }

  engine.setFontTiny(1);
  engine.setTextColor(BLACK);

  int spacing = 9;
  // int spacing = 8;
  int appNameLen = strlen_P(appName);
  int titleWidth = appNameLen * spacing - 1;
  
  int yOffset = 7;
  int xOffset = 2 + (SCREEN_WIDTH - titleWidth) / 2;

  for (int i = 0; i < appNameLen; i++) {
    char c = pgm_read_byte(&appName[i]);
    if (c == ' ')
      continue;
    
    int x = xOffset + i * spacing;
    int y = yOffset;
    
    if (!showTitleAnimation) {
      engine.drawBitmapAt(x - 2, y - 1, d(CIRCLE_LAUNCH_TITLE_CIRCLE));
      engine.setCursor(x, y);
      engine.printChar(c >= 'a' ? c - 'a' + 'A' : c);
      // drawCircleAt(x + 1, y + 2, 2);
      // engine.setCursor(x, y);
      // engine.printChar(c >= 'a' ? c - 'a' + 'A' : c);
    } else if (delayFrames == CIRCLE_LAUNCH_TITLE_ANIMATION_FRAMES) {
      // int freeParticles = 0;
      // for (int i = 0; i < CIRCLE_LAUNCH_PARTICLES; i++)
      //   freeParticles += (particles[i].framesRemaining <= 0);
      
      showParticlesAt(x + 1, y + 2, CIRCLE_LAUNCH_PARTICLES / 12);
    }
  }
  
  // if (showTitleAnimation && titleOffsetCy < 48 * 100 + 50) {
  //   titleAnimationSpeed += engine.timeScaledDelay(67);
  //   titleOffsetCy += titleAnimationSpeed;
  // }

  if (showTitleAnimation) {
    engine.incrementTo(titleOffsetCy, 48 * 100 + 50, false);
  }

  engine.setTextColor(WHITE);
  
  yOffset = 26 + titleOffsetCy / 100;
  
  static long offsetCx = 38 * 100 + 50;
  
  if (gameMode >= 0)
    engine.incrementTo(offsetCx, 0);

  const int size = 14;
  const int perRow = 4;
  
  
  int gameModeXOffset = 14 + offsetCx / 100;
  
  
  // engine.fillRect(gameModeXOffset - 4, yOffset - 2, 36, 35, BLACK);
  for (int i = 0; i < 25; i++) {
    int x = gameModeXOffset - 4 + min((i % 5) * 8, 36 - 8);
    int y = yOffset - 2 + min((i / 5) * 8, 35 - 8);
    engine.drawBitmap(x, y, solidSprite, 8, 8, BLACK);
  }
  // fastBlackRect(gameModeXOffset - 4, yOffset - 2, 36, 35);
  
  engine.setFontTiny(2);
  engine.setCursor(gameModeXOffset + 6, yOffset);
  engine.print(F("MODE"));

  yOffset += 8;
  
  for (int i = 0; i < 2; i++) {
    int x = gameModeXOffset + size * (i % 5);
    int y = yOffset;
    
    engine.setFontTiny(1);
    engine.setCursor(x + 5, y + 4);
    engine.printChar('A'+i);
    if (selectedMenuItem == i)
      engine.drawMarqueeRect(x, y, size, size, WHITE);
    if (i == gameMode)
      engine.fillRect(x, y, size, size, INVERSE);
  }

  engine.setFontTiny(2);
  engine.setCursor(gameModeXOffset, yOffset + 14 + 3);
  engine.print(F("MATCH "));
  engine.printInt(2 + 1 - (selectedMenuItem < 2 ? selectedMenuItem : gameMode));
  

  yOffset -= 8;
  
  int startingLevelXOffset = 56 + offsetCx / 50;

  // engine.fillRect(startingLevelXOffset - 4, yOffset - 2, 64, 28, BLACK);
  for (int i = 0; i < 32; i++) {
    int x = startingLevelXOffset - 4 + min((i % 8) * 8, 64 - 8);
    int y = yOffset - 2 + min((i / 8) * 8, 28 - 8);
    engine.drawBitmap(x, y, solidSprite, 8, 8, BLACK);
  }
  // fastBlackRect(startingLevelXOffset - 4, yOffset - 2, 64, 28);

  engine.setFontTiny(2);
  engine.setCursor(startingLevelXOffset + 1, yOffset);
  engine.print(F("START AT LEVEL"));

  yOffset += 8;

  for (int i = 0; i < 4; i++) {
    int x = startingLevelXOffset + size * (i % 5);
    int y = yOffset;
    
    engine.setFontTiny(1);
    engine.setCursor(x + 5, y + 4);
    engine.printInt(i+1);
    if (selectedMenuItem - 2 == i) {
      engine.drawMarqueeRect(x, y, size, size, WHITE);
    }

  }
  
  if (gameMode < 0)
    engine.drawShadedRect(startingLevelXOffset, yOffset, 14 * 4, 14, BLACK);

  // if (!hasClicked && engine.mouseClickedBetween())
  //   hasClicked = -1;

  if (gameMode >= 0 && highScores[gameMode] > 0) {
    int width = (12 + 6) * 4;
    int x = SCREEN_WIDTH - width;
    int y = yOffset + 25;

    engine.fillRect(x - 4, y - 3, width + 4, 9, BLACK);
    engine.setCursor(x, y - 1);

    engine.setFontTiny(2);
    engine.print(F("HIGH SCORE: "));

    yOffset += 8;

    drawScore(highScores[gameMode], engine.cursorX, y - 1);
    // engine.printInt(highScores[gameMode]);
  }
}

void CircleLaunch::drawCurrentLine() {
  if (isMoving || circleYInt != CIRCLE_LAUNCH_START_CIRCLE_Y || appState != APP_STATE_PRIMARY)
    return;

  s32 deltaX = (s32)(engine.xDirectionsN16(selectedDirection, CIRCLE_LAUNCH_DIRECTION_COUNT));
  s32 deltaY = (s32)(engine.yDirectionsN16(selectedDirection, CIRCLE_LAUNCH_DIRECTION_COUNT));

  // s32 divisor = UNSIGNED_SHORT_MAX / 2;
  // int x0 = circleXInt + 1 + (s32)(deltaX * 3 * 2) / divisor;
  // int y0 = circleYInt + (s32)(deltaY * 3 * 2) / divisor;
  // int x1 = circleXInt + 1 + (s32)(deltaX * 3 * 9) / divisor;
  // int y1 = circleYInt + (s32)(deltaY * 3 * 9) / divisor;

  float dist1 = 3.0;
  // int x0, y0, x1, y1;

  for (int i = 2; i < 10; i++) {
    int frame = (engine.frameCount / 8) % (int)dist1;
    // int frame = 0;
    float r = dist1 * i + frame;
    int x = circleXInt + 1 + (deltaX * r) / (UNSIGNED_SHORT_MAX / 2);
    int y = circleYInt + 1 + (deltaY * r) / (UNSIGNED_SHORT_MAX / 2);
    
    // if (i == 2) {
    //   x0 = x;
    //   y0 = y;
    // } else if (i == 9) {
    //   x1 = x;
    //   y1 = y;
    // }
    engine.drawPixel(x, y, WHITE);
  }

  // engine.drawLineMarquee(x0, y0, x1, y1, WHITE, 3);
}

void CircleLaunch::drawCurrentCircle() {
  drawCircleAt(circleXInt, circleYInt, currentColor);
}

void CircleLaunch::drawNextCircle() {
  if (nextColor < 0 || appState != APP_STATE_PRIMARY )
    return;
  
  drawCircleAt(CIRCLE_LAUNCH_CIRCLE_SIZE / 2, currentNextCircleCy / 100, nextColor);
}

void CircleLaunch::drawCircles() {
  for (int col = 0; col < CIRCLE_LAUNCH_COLUMNS; col++) {
    for (int row = 0; row < rowsInColumn(col); row++) {
      int color = getColorInBounds(col, row, false);
      if (color == 0)
        continue;
      
      int yOffset = CIRCLE_LAUNCH_CIRCLE_SIZE / 2;
      if (col % 2)
        yOffset += CIRCLE_LAUNCH_CIRCLE_SIZE / 2 + 1;
      
      int x = xOffset() + col * (CIRCLE_LAUNCH_CIRCLE_SIZE + 1);
      if (color < 0 && wobbleFrames > 0 && engine.frameCount % 2 && appState == APP_STATE_PRIMARY)
        x += engine.randomInt(0, 2) ? 1 : -1;
      int y = yOffset + row * (CIRCLE_LAUNCH_CIRCLE_SIZE + 1);

      drawCircleAt(x, y, abs(color));
    }
  }
}

void CircleLaunch::drawBorder() {
  if (appState == APP_STATE_PRIMARY || !gameOver)
    engine.drawShadedRect(CIRCLE_LAUNCH_BORDER_X, -1, 1, SCREEN_HEIGHT, WHITE);
}

void CircleLaunch::drawScore(int value, int x, int y) {
  engine.setFontTiny(2);
  engine.setCursor(x, y);
  long factor = 100000;
  for (int d = 0; d < 5; d++) {
    if (value >= factor)
      break;
    factor = factor / 10;
    engine.printChar('0');
  }
  engine.printInt(value);
  
  if (!delayFrames && appState != APP_STATE_SECONDARY)
    return;
}

void CircleLaunch::drawHighScore() {
  if (!newHighScore || engine.modalOffsetCy)
    return;

  engine.setFontTiny(2);
  int width = 15 * 4 - 1;
  int x = (SCREEN_WIDTH - width) / 2;
  int y = 50;

  engine.setCursor(x, y);
  engine.printSine(circleLaunchNewHighScoreText, 3, 30);
}

void CircleLaunch::drawCircleAt(int x, int y, int t) {
  engine.drawBitmapAt(x - 3, y - 3, d(CIRCLE_LAUNCH_CIRCLES), t - 1);
}

void CircleLaunch::drawWall() {
  int startX = SCREEN_WIDTH - CIRCLE_LAUNCH_CIRCLE_SIZE / 2 - 2 + 5 - millisElapsed/shiftEveryMillis - timeBonus;
  int wallWidth = SCREEN_WIDTH - startX + 8;
  const int patternCount = 4;

  for (int i = 0; i < 8; i++) {
    int y = i * 8;
    for (int j = 0; j < wallWidth / 8; j++) {
      int x = startX + j * 8;
      engine.drawBitmapAt(x, y, d(CIRCLE_LAUNCH_BACKGROUNDS), max(patternCount - j, 0));
    }
  }
}

// Actions
///////////////////////////////////////////////////////////////////////////////

void CircleLaunch::getInput() {
  if (delayFrames > 0 || isMoving) {
    return;
  }

  engine.nextControllerDPadRepeatDelay = 5;
  engine.nextControllerDPadRepeat = 4;

  if (circleYInt != CIRCLE_LAUNCH_START_CIRCLE_Y) {
    currentNextCircleCy = (CIRCLE_LAUNCH_NEXT_CIRCLE_Y + CIRCLE_LAUNCH_CIRCLE_SIZE) * 100;
    engine.incrementTo(cY, CIRCLE_LAUNCH_START_CIRCLE_Y * 100 + 50);
    circleYInt = cY / 100;
    // engine.incrementTo(circleY, I2FR(CIRCLE_LAUNCH_START_CIRCLE_Y, R8) + R8_ONE_HALF);
    return;
  } else {
    engine.incrementTo(currentNextCircleCy, CIRCLE_LAUNCH_NEXT_CIRCLE_Y * 100 + 50);
  }

  int min = CIRCLE_LAUNCH_DIRECTION_COUNT / 14;
  int max = CIRCLE_LAUNCH_DIRECTION_COUNT / 2 - CIRCLE_LAUNCH_DIRECTION_COUNT / 14;

  selectedDirection -= (engine.buttonPressed(CONTROLLER_BUTTON_DPAD_UP) + engine.buttonPressed(CONTROLLER_BUTTON_DPAD_LEFT));
  selectedDirection += (engine.buttonPressed(CONTROLLER_BUTTON_DPAD_DOWN) + engine.buttonPressed(CONTROLLER_BUTTON_DPAD_RIGHT));
  engine.clampBetween(selectedDirection, min, max);
  
  bool buttonPressed = engine.buttonPressed(CONTROLLER_BUTTON_A) || engine.buttonPressed(CONTROLLER_BUTTON_B);
  if (buttonPressed) {
    unmarkAll();
    isMoving = true;
    direction = selectedDirection;
  }
}

void CircleLaunch::updateCurrentCircle() {
  if (!isMoving)
    return;
  
  int deltaCx = engine.xDirectionsN(direction, CIRCLE_LAUNCH_DIRECTION_COUNT);
  int deltaCy = engine.yDirectionsN(direction, CIRCLE_LAUNCH_DIRECTION_COUNT);
  int cXNext = cX + deltaCx * 5.0;
  int cYNext = cY + deltaCy * 5.0;
  
  // s16 deltaX = engine.xDirectionsN16(direction, CIRCLE_LAUNCH_DIRECTION_COUNT);
  // s16 deltaY = engine.yDirectionsN16(direction, CIRCLE_LAUNCH_DIRECTION_COUNT);
  // s32 nextCircleX = circleX + (deltaX * 5 >> R7);
  // s32 nextCircleY = circleY + (deltaY * 5 >> R7);
  
  if (!willCollideAt(cXNext / 100, cYNext / 100)) {
    cX = cXNext;
    cY = cYNext;

    // circleX = nextCircleX;
    // circleY = nextCircleY;

    circleXInt = cX / 100;
    circleYInt = cY / 100;

    // TODO: rewrite to bounce correctly off the sides and not clip out of the screen
    if (cY < (CIRCLE_LAUNCH_CIRCLE_SIZE / 2) * 100 || cY > (SCREEN_HEIGHT - CIRCLE_LAUNCH_CIRCLE_SIZE / 2 - 2) * 100) {
      direction = (direction - CIRCLE_LAUNCH_DIRECTION_COUNT / 4) * -1 + CIRCLE_LAUNCH_DIRECTION_COUNT / 4;
      deltaCx = engine.xDirectionsN(direction, CIRCLE_LAUNCH_DIRECTION_COUNT);
      deltaCy = engine.yDirectionsN(direction, CIRCLE_LAUNCH_DIRECTION_COUNT);
      // deltaX = engine.xDirectionsN16(direction, CIRCLE_LAUNCH_DIRECTION_COUNT);
      // deltaY = engine.yDirectionsN16(direction, CIRCLE_LAUNCH_DIRECTION_COUNT);
    }
  } else {
    while (!willCollideAt(cX / 100, cY / 100)) {
      cX += deltaCx * 0.4;
      cY += deltaCy * 0.4;
      // circleX += (deltaX * 4 / 10) >> R7;
      // circleY += (deltaY * 4 / 10) >> R7;
    }

    circleXInt = cX / 100;
    circleYInt = cY / 100;
    
    dropCircle();
  }
}

void CircleLaunch::reset() {
  startTransition(APP_TRANSITION_RECT_SLIDE_DOWN);
  
  // active = true;
  wobbleFrames = 0;
  
  millisElapsed = 0;
  timeBonus = 0;

  columnCount = min(level + 2, CIRCLE_LAUNCH_COLUMNS - 1);
#ifdef CIRCLE_LAUNCH_TEST_MODE
  columnCount--;
#endif

  countRemaining = 0;

  memset(circleBoard, 0, sizeof(circleBoard));

  Engine::initRandomSeed();

  for (int col = 0; col < CIRCLE_LAUNCH_COLUMNS; col++) {
    for (int row = 0; row < rowsInColumn(col); row++) {
#ifdef CIRCLE_LAUNCH_TEST_MODE
      if (row > 1)
        break;
#endif
      int color = 0;
      if (col > CIRCLE_LAUNCH_COLUMNS - columnCount - 1)
        color = engine.randomInt(1, colorCount + 1);
      setColor(col, row, color);
      countRemaining++;
    }
  }

  nextColor = -1;

  getNextCircle();
}

void CircleLaunch::dropCircle() {
  int nearestDistance = 9999;

  for (int col = CIRCLE_LAUNCH_COLUMNS - 1; col >= 0; col--) {
    for (int row = 0; row < rowsInColumn(col); row++) {
      // int i = col + row * CIRCLE_LAUNCH_COLUMNS;
      if (getColorInBounds(col, row) != 0)
        continue;
      
      int yOffset = CIRCLE_LAUNCH_CIRCLE_SIZE / 2 + (col % 2) * (CIRCLE_LAUNCH_CIRCLE_SIZE / 2 + 1);
      // int yOffset = CIRCLE_LAUNCH_CIRCLE_SIZE / 2;
      // if (col % 2)
      //   yOffset += CIRCLE_LAUNCH_CIRCLE_SIZE / 2 + 1;
      
      int x = xOffset() + col * (CIRCLE_LAUNCH_CIRCLE_SIZE + 1);
      int y = yOffset + row * (CIRCLE_LAUNCH_CIRCLE_SIZE + 1);
      
      float distance = sqrt(pow(circleXInt - x, 2) + pow(circleYInt - y, 2));
      if (distance < nearestDistance) {
        circleCol = col;
        circleRow = row;
        nearestDistance = distance;
      }
    }
  }

  setColor(circleCol, circleRow, currentColor);

  int count = markAdjacentCirclesFrom(circleCol, circleRow, currentColor);
  if (count >= CIRCLE_LAUNCH_MIN_COMBO - gameMode) {
    score += CIRCLE_LAUNCH_BASE_SCORE * count;
    totalCountRemoved = count;
    removeMarked();
    int strayCount = markStrays();
    
    score += CIRCLE_LAUNCH_BASE_SCORE * 2 * strayCount + (strayCount - 1) * CIRCLE_LAUNCH_BASE_SCORE * strayCount;
    removeMarked();
  } else if (!gameMode && count >= CIRCLE_LAUNCH_MIN_COMBO - 1) {
    wobbleFrames = 15;
  }
  
  countRemaining = circleCount();
  
  if (countRemaining == 0) {
    engine.addSound(READY_SOUND, SOUNDS_DATA, sizeof(SOUNDS_DATA));
    getNextCircle();
    App::setState(APP_STATE_SECONDARY);
  } else {
    if (removedCount > 0) {
      // cout << "removedCount - 2: " << removedCount - 2 << endl;
      engine.addSound(BOUNCE_SOUND, SOUNDS_DATA, sizeof(SOUNDS_DATA));
    } else {
      engine.addSound(CAUGHT_SOUND, SOUNDS_DATA, sizeof(SOUNDS_DATA));
    }
    
    getNextCircle();
  }
}

void CircleLaunch::clearCircleAt(int col, int row) {
  if (outOfBounds(col, row) || getColor(col, row) == 0)
    return;

  setColor(col, row, 0);
  removedCount++;
  
  int yOffset = CIRCLE_LAUNCH_CIRCLE_SIZE / 2;
  if (col % 2)
    yOffset += CIRCLE_LAUNCH_CIRCLE_SIZE / 2 + 1;
  
  int x = xOffset() + col * (CIRCLE_LAUNCH_CIRCLE_SIZE + 1);
  int y = yOffset + row * (CIRCLE_LAUNCH_CIRCLE_SIZE + 1);
  showParticlesAt(x, y, min(10, CIRCLE_LAUNCH_PARTICLES / totalCountRemoved));
}

int CircleLaunch::markAdjacentCirclesFrom(int col, int row, int color) {
  if (outOfBounds(col, row))
    return 0;

  setColor(col, row, color * -1);
  uint8_t count = 1;

  const int8_t neighbors[] = {
    0, -1,
    0, 1,
    -1, 0,
    1, 0,
    -1, (int8_t)(col % 2 ? 1 : -1),
    1, (int8_t)(col % 2 ? 1 : -1)
  };
  
  for (int i = 0; i < 6; i++) {
    int8_t c = col + neighbors[i*2];
    int8_t r = row + neighbors[i*2+1];
    if (getColor(c, r, false) == color)
      count += markAdjacentCirclesFrom(c, r, color);
  }
  
  return count;
}

int CircleLaunch::markStrays() {
  int count = 0;
  // flip colors to negative values except for last column
  for (int col = CIRCLE_LAUNCH_COLUMNS - 2; col > 0; col--) {
    for (int row = 0; row < rowsInColumn(col); row++) {
      int color = getColorInBounds(col, row);
      if (color == 0)
        continue;

      setColor(col, row, color * -1);
      count++;
    }
  }

  // scan each column row by row down and up, flipping each color to positive it is adjacent to another positive color
  for (int col = CIRCLE_LAUNCH_COLUMNS - 2; col > 0; col--) {
    for (int directionRight = 0; directionRight < 2; directionRight++) {
      int c = (directionRight % 2 ? CIRCLE_LAUNCH_COLUMNS - 1 - col : col);
      int rowCount = rowsInColumn(c);
      for (int row = 0; row < rowCount; row++) {
        for (int directionUp = 0; directionUp < 2; directionUp++) {
          int r = (directionUp % 2 ? rowCount - 1 - row : row);
        
          int color = getColorInBounds(c, r, false);
          if (color >= 0)
            continue;
        
          int colSide = (directionRight % 2 ? c-1 : c+1);
          if (getColor(c, r+1, false) > 0 ||
            getColor(c, r-1, false) > 0 ||
            getColor(colSide, r, false) > 0 ||
            getColor(colSide, r + (c % 2 ? 1 : -1), false) > 0) {
              setColor(c, r, color * -1);
              count--;
          }
        }
      }
      
    }
  }

  int additionalMarked;
  do {
    additionalMarked = 0;
    for (int col = CIRCLE_LAUNCH_COLUMNS - 2; col > 0; col--) {
      int rowOffset = -1 + (col % 2) * 2;

      for (int row = 0; row < rowsInColumn(col); row++) {
        int color = getColor(col, row, false);
        if (color >= 0)
          continue;

        if (getColor(col, row+1, false) > 0 ||
          getColor(col, row-1, false) > 0 ||
          getColor(col+1, row, false) > 0 ||
          getColor(col+1, row + rowOffset, false) > 0 ||
          getColor(col-1, row, false) > 0 ||
          getColor(col-1, row + rowOffset, false) > 0) {
            setColor(col, row, color * -1);
            count--;
            // cout << "added at col, row: " << col << ", " << row << endl;
            additionalMarked++;
        }
      }
    }
  } while (additionalMarked);

  totalCountRemoved += count;      
  freeParticles(count);
  
  return count;
}

int CircleLaunch::removeMarked() {
  int count = 0;
  // remove any remaining negative values
  for (int col = CIRCLE_LAUNCH_COLUMNS - 1; col > 0; col--) {
    for (int row = 0; row < rowsInColumn(col); row++) {
      int color = getColorInBounds(col, row, false);
      if (color >= 0)
        continue;

      clearCircleAt(col, row);
      count++;
    }
  }

  return count;
}

void CircleLaunch::unmarkAll() {
  for (int col = CIRCLE_LAUNCH_COLUMNS - 1; col > 0; col--) {
    for (int row = 0; row < rowsInColumn(col); row++) {
      int color = getColorInBounds(col, row, false);
      if (color >= 0)
        continue;

      setColor(col, row, color * -1);
    }
  }
}

void CircleLaunch::getNextCircle() {
  if (!countRemaining)
    return;

  circleXInt = CIRCLE_LAUNCH_CIRCLE_SIZE / 2;
  circleYInt = CIRCLE_LAUNCH_NEXT_CIRCLE_Y;
  // circleX = I2FR(circleXInt, R8);
  // circleY = I2FR(circleYInt, R8);
  cX = CIRCLE_LAUNCH_CIRCLE_SIZE / 2 * 100;
  cY = CIRCLE_LAUNCH_NEXT_CIRCLE_Y * 100;
  isMoving = false;
  
  if (nextColor == -1)
    nextColor = engine.randomInt(1, colorCount + 1);
  
  currentColor = nextColor;
  
  bool boardHasColor;
  do {
    nextColor = engine.randomInt(1, colorCount + 1);
    boardHasColor = false;

    for (int col = 0; col < CIRCLE_LAUNCH_COLUMNS; col++) {
      for (int row = 0; row < rowsInColumn(col); row++) {
        if (getColorInBounds(col, row) == nextColor) {
          boardHasColor = true;
          break;
        }
      }
    }
  } while (countRemaining <= colorCount * 1.25 && !boardHasColor);

  removedCount = 0;
  totalCountRemoved = 0;
}

// Helpers
///////////////////////////////////////////////////////////////////////////////

bool CircleLaunch::isReady() {
  return !gameOver && millisElapsed/shiftEveryMillis + timeBonus >= 80;
}

int CircleLaunch::xOffset() {
  return CIRCLE_LAUNCH_X_PADDING + CIRCLE_LAUNCH_CIRCLE_SIZE / 2 - millisElapsed/shiftEveryMillis - timeBonus;
}

int CircleLaunch::yOffset() {
  return CIRCLE_LAUNCH_CIRCLE_SIZE / 2;
}

int CircleLaunch::rowsInColumn(int col) {
  return 8 - col % 2;
}

bool CircleLaunch::outOfBounds(int col, int row) {
  return (row < 0 || col < 0 || col >= CIRCLE_LAUNCH_COLUMNS || row >= rowsInColumn(col));
}

int CircleLaunch::getColor(int col, int row, bool absoluteValue) {
  if (outOfBounds(col, row))
    return 0;

  return getColorInBounds(col, row, absoluteValue);
}

int CircleLaunch::getColorInBounds(int col, int row, bool absoluteValue) {
  int color = circleBoard[col + row * CIRCLE_LAUNCH_COLUMNS];
  return absoluteValue ? abs(color) : color;
}

void CircleLaunch::setColor(int col, int row, int color) {
  if (outOfBounds(col, row))
    return;
  
  // cout << "setting color..." << endl;
  circleBoard[col + row * CIRCLE_LAUNCH_COLUMNS] = color;
  // cout << "color set!" << endl;
}

bool CircleLaunch::isGameOver() {
  int col = (millisElapsed/shiftEveryMillis - 2) / 8;
  for (int row = 0; row < rowsInColumn(col); row++) {
    if (getColor(col, row) > 0)
      return true;
  }
  
  return false;
}

int CircleLaunch::circleCount() {
  int count = 0;
  
  for (int col = 0; col < CIRCLE_LAUNCH_COLUMNS; col++) {
    for (int row = 0; row < rowsInColumn(col); row++) {
      count += getColor(col, row) > 0;
    }
  }
  
  return count;
}

bool CircleLaunch::willCollideAt(int playerX, int playerY) {
  if (playerX >= SCREEN_WIDTH - CIRCLE_LAUNCH_CIRCLE_SIZE / 2 - 2 - millisElapsed/shiftEveryMillis)
    return true;
  
  for (int col = 0; col < CIRCLE_LAUNCH_COLUMNS; col++) {
    for (int row = 0; row < rowsInColumn(col); row++) {
      if (getColor(col, row) == 0)
        continue;
      
      int xOffset = CIRCLE_LAUNCH_X_PADDING + CIRCLE_LAUNCH_CIRCLE_SIZE / 2 - millisElapsed/shiftEveryMillis;
      int yOffset = CIRCLE_LAUNCH_CIRCLE_SIZE / 2;
      if (col % 2)
        yOffset += CIRCLE_LAUNCH_CIRCLE_SIZE / 2 + 1;
      
      int x = xOffset + col * (CIRCLE_LAUNCH_CIRCLE_SIZE + 1);
      int y = yOffset + row * (CIRCLE_LAUNCH_CIRCLE_SIZE + 1);
      
      if (sqrt(pow(playerX - x, 2) + pow(playerY - y, 2)) < CIRCLE_LAUNCH_CIRCLE_SIZE)
        return true;
    }
  }

  return false;      
}

void CircleLaunch::showParticlesAt(int x, int y, int count, int speed) {
  int remaining = count;

  speed += engine.randomInt(0, 41) - 20;

  for (int i = 0; i < CIRCLE_LAUNCH_PARTICLES; i++) {
    if (particles[i].framesRemaining > 0)
      continue;

    int direction = engine.randomInt(0, CIRCLE_LAUNCH_PARTICLES_DIRECTIONS_COUNT);

    particles[i].x = I2FR(x, R8);
    particles[i].y = I2FR(y, R8);

    s16 xDelta = engine.xDirectionsN(direction, CIRCLE_LAUNCH_PARTICLES_DIRECTIONS_COUNT);
    s16 yDelta = engine.yDirectionsN(direction, CIRCLE_LAUNCH_PARTICLES_DIRECTIONS_COUNT);
    particles[i].xSpeed = (speed * xDelta) / 100;
    particles[i].ySpeed = (speed * yDelta) / 100;
    // particles[i].xSpeed = D2FR(xDelta / 100.0 * (speed / 100.0), R8);
    // particles[i].ySpeed = D2FR(yDelta / 100.0 * (speed / 100.0), R8);

    // particles[i].cX = x * 100;
    // particles[i].cY = y * 100;
    // particles[i].speedCx = engine.xDirectionsN(direction, CIRCLE_LAUNCH_PARTICLES_DIRECTIONS_COUNT) * (speed / 100.0);
    // particles[i].speedCy = engine.yDirectionsN(direction, CIRCLE_LAUNCH_PARTICLES_DIRECTIONS_COUNT) * (speed / 100.0);

    particles[i].framesRemaining = 40 + engine.randomInt(0, 25) - 15;
    
    remaining--;
    if (remaining == 0)
      return;
  }

}

void CircleLaunch::updateParticles() {
  for (int i = 0; i < CIRCLE_LAUNCH_PARTICLES; i++) {
    if (particles[i].framesRemaining <= 0)
      continue;

    
    particles[i].x += particles[i].xSpeed;
    particles[i].y += particles[i].ySpeed;
    particles[i].ySpeed += FR_NUM(0, 3, 2, R8);

    // particles[i].cX += particles[i].speedCx;
    // particles[i].cY += particles[i].speedCy;
    // particles[i].speedCy += 3;

    particles[i].framesRemaining--;
  }
}

void CircleLaunch::freeParticles(int circleCount) {
  int count = min(CIRCLE_LAUNCH_PARTICLES, circleCount * min(10, CIRCLE_LAUNCH_PARTICLES / totalCountRemoved));
  
  int available = CIRCLE_LAUNCH_PARTICLES;
  for (int i = 0; i < CIRCLE_LAUNCH_PARTICLES; i++) {
    available -= (particles[i].framesRemaining > 0);
  }
  
  do {
    int i = engine.randomInt(0, CIRCLE_LAUNCH_PARTICLES);
    if (particles[i].framesRemaining > 0) {
      particles[i].framesRemaining = 0;
      available++;
    }
  } while (available < count);
}

void CircleLaunch::drawParticles() {
  const int flickerAtFrame = 20;
  const int shrinkAtFrame = 10;
  for (int i = 0; i < CIRCLE_LAUNCH_PARTICLES; i++) {
    if (particles[i].framesRemaining <= 0 || (particles[i].framesRemaining <= flickerAtFrame && (engine.frameCount % 2)))
      continue;
    
    int x = FR2I(particles[i].x, R8);
    int y = FR2I(particles[i].y, R8);

    if (i % 2 || particles[i].framesRemaining <= shrinkAtFrame)
      engine.drawPixel(x, y, INVERSE);
    else
      engine.fillRect(x, y, 2, 2, INVERSE);
  }
}

void CircleLaunch::fastFillRect(int x, int y, int w, int h, int color) {
  int cols = w / 8 + (w % 8 > 0);
  int rows = h / 8 + (h % 8 > 0);
  
  for (int i = 0; i < cols * rows; i++) {
    int x1 = x + min((i % cols) * 8, w - 8);
    int y1 = y + min((i / cols) * 8, h - 8);
    engine.drawBitmap(x1, y1, solidSprite, 8, 8, color);
  }
}
