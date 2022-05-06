#include "src/Utils/Arduboy2Ext.h"
#ifdef HIGH_SCORES

uint8_t clearScores = 0;


// --------------------------------------------------------------------------------------
//  Render High Score screen ..
//
#define HIGH_SCORE_DIST_BETWEEN_ROWS 9
void renderHighScore(HighScore &highScore) {

  uint8_t xOffset = 75;
  uint8_t yOffset = 4;


  Sprites::drawOverwrite(4, 7, flying_aces, 0);
  Sprites::drawOverwrite(0, 48, splash_lower, 0);

  arduboy.drawHorizontalDottedLine(2, 67, 3);
  arduboy.drawHorizontalDottedLine(2, 67, 32);

  Sprites::drawOverwrite(6, 40, flying_aces_stars, 0);
  Sprites::drawOverwrite(40, 40, flying_aces_stars, 0);
  Sprites::drawOverwrite(25, 31, flying_aces_p38, 0);

  for (uint8_t x = 0; x < MAX_NUMBER_OF_SCORES; x++) {

    xOffset = 76;
    Slot slot = EEPROM_Utils::getSlot(x);

    Sprites::drawOverwrite(xOffset, yOffset, font4x6_Full, slot.getChar0());
    Sprites::drawOverwrite(xOffset + 6, yOffset, font4x6_Full, slot.getChar1());
    Sprites::drawOverwrite(xOffset + 12, yOffset, font4x6_Full, slot.getChar2());


    // Score ..
    {
      uint8_t digits[6] = {};
      extractDigits(digits, slot.getScore());
    
      for (uint8_t i = 0, x2 = xOffset + 47; i < 6; ++i, x2 -= 5) {
        Sprites::drawSelfMasked(x2, yOffset, font4x6_Full, (digits[i] + 26));
      }
      
    }

    yOffset = yOffset + HIGH_SCORE_DIST_BETWEEN_ROWS;

  }

  if (highScore.getSlotNumber() != DO_NOT_EDIT_SLOT) {

    xOffset = 76;
    yOffset = 4;
    alternate++;
  
    if (alternate < 15) {

      //Sprites::drawOverwrite(xOffset - (highScore.getSlotNumber() * HIGH_SCORE_DIST_BETWEEN_ROWS) - 1, yOffset - 4, arrow, 0);
      //Sprites::drawOverwrite(xOffset - (highScore.getSlotNumber() * HIGH_SCORE_DIST_BETWEEN_ROWS) - 1, yOffset + 59, arrow, 0);

    }
    else if (alternate > 30) {

      alternate = 0;

    }

    if (highScore.getCharIndex() == 0) {
      arduboy.fillRect(xOffset - 1, yOffset + (highScore.getSlotNumber() * HIGH_SCORE_DIST_BETWEEN_ROWS) - 1, 6, 8, WHITE);
      Sprites::drawErase(xOffset, yOffset + (highScore.getSlotNumber() * HIGH_SCORE_DIST_BETWEEN_ROWS), font4x6_Full, highScore.getChar(0));
    }
    else {
      Sprites::drawOverwrite(xOffset, yOffset + (highScore.getSlotNumber() * HIGH_SCORE_DIST_BETWEEN_ROWS), font4x6_Full, highScore.getChar(0));
    }

    if (highScore.getCharIndex() == 1) {
      arduboy.fillRect(xOffset + 5, yOffset + (highScore.getSlotNumber() * HIGH_SCORE_DIST_BETWEEN_ROWS) - 1, 6, 8, WHITE);
      Sprites::drawErase(xOffset + 6, yOffset + (highScore.getSlotNumber() * HIGH_SCORE_DIST_BETWEEN_ROWS), font4x6_Full, highScore.getChar(1));
    }
    else {
      Sprites::drawOverwrite(xOffset + 6, yOffset + (highScore.getSlotNumber() * HIGH_SCORE_DIST_BETWEEN_ROWS), font4x6_Full, highScore.getChar(1));
    }

    if (highScore.getCharIndex() == 2) {
      arduboy.fillRect(xOffset + 11, yOffset + (highScore.getSlotNumber() * HIGH_SCORE_DIST_BETWEEN_ROWS) - 1, 6, 8, WHITE);
      Sprites::drawErase(xOffset + 12, yOffset + (highScore.getSlotNumber() * HIGH_SCORE_DIST_BETWEEN_ROWS), font4x6_Full, highScore.getChar(2));
    }
    else {
      Sprites::drawOverwrite(xOffset + 12, yOffset + (highScore.getSlotNumber() * HIGH_SCORE_DIST_BETWEEN_ROWS), font4x6_Full, highScore.getChar(2));
    }

    // if (highScore.getCharIndex() == 1) {
    //   arduboy.fillRect(xOffset - (highScore.getSlotNumber() * HIGH_SCORE_DIST_BETWEEN_ROWS) - 1, yOffset + 5, 8, 6, WHITE);
    //   Sprites::drawErase(xOffset - (highScore.getSlotNumber() * HIGH_SCORE_DIST_BETWEEN_ROWS), yOffset + 6, font4x6_Full, highScore.getChar(1));
    // }
    // else {
    //   Sprites::drawOverwrite(xOffset - (highScore.getSlotNumber() * HIGH_SCORE_DIST_BETWEEN_ROWS), yOffset + 6, font4x6_Full, highScore.getChar(1));
    // }

    // if (highScore.getCharIndex() == 2) {
    //   arduboy.fillRect(xOffset - (highScore.getSlotNumber() * HIGH_SCORE_DIST_BETWEEN_ROWS) - 1, yOffset + 11, 8, 6, WHITE);
    //   Sprites::drawErase(xOffset - (highScore.getSlotNumber() * HIGH_SCORE_DIST_BETWEEN_ROWS), yOffset + 12, font4x6_Full, highScore.getChar(2));
    // }
    // else {
    //   Sprites::drawOverwrite(xOffset - (highScore.getSlotNumber() * HIGH_SCORE_DIST_BETWEEN_ROWS), yOffset + 12, font4x6_Full, highScore.getChar(2));
    // }


    // Handle buttons ..

    {
      uint8_t justPressed = arduboy.justPressedButtons();
      uint8_t charIndex = highScore.getCharIndex();

      if (justPressed & DOWN_BUTTON)          { highScore.decChar(charIndex); }
      if (justPressed & UP_BUTTON)            { highScore.incChar(charIndex); }
      if (justPressed & LEFT_BUTTON)          { highScore.decCharIndex(); } 
      if (justPressed & RIGHT_BUTTON)         { highScore.incCharIndex(); } 

      if (justPressed & A_BUTTON) { 
        
        EEPROM_Utils::writeChars(highScore.getSlotNumber(), highScore);
        highScore.disableEditting(); 
        
      }

    }

  }
  else {

    uint8_t pressed = arduboy.pressedButtons();


    // Clear scores ..

    if ((pressed & UP_BUTTON) && (pressed & DOWN_BUTTON)) {

      clearScores++;

      switch (clearScores) {

        case 21 ... 60:
          #ifdef USE_LEDS             
          arduboy.setRGBled(128 - (clearScores * 2), 0, 0);
          #endif
          break;

        case 61:
          clearScores = 0;
          #ifdef USE_LEDS             
          arduboy.setRGBled(0, 0, 0);
          #endif
          EEPROM_Utils::initEEPROM(true);
          sound.tone(NOTE_C6, 100);
          return;

      }

    }
    else {

      if (clearScores > 0) {
      
        #ifdef USE_LEDS             
        arduboy.setRGBled(0, 0, 0);
        #endif
        clearScores = 0;

      }
      
    }

    {
      uint8_t justPressed = arduboy.justPressedButtons();

      if ((justPressed & A_BUTTON) || (justPressed & B_BUTTON)) { 
        #ifdef USE_LEDS             
        arduboy.setRGBled(0, 0, 0);
        #endif
        gameState = GameState::Intro_Init; 
      }

    }
    
  }

  //arduboy.display(true);

}

#endif