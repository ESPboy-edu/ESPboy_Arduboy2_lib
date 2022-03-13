#include "src/utils/Arduboy2Ext.h"
//#include <EEPROM.h>

void initEEPROM(bool forceOverwrite) {

    uint8_t c1 = EEPROM.read(Constants::EEPROM_Start_C1);
    uint8_t c2 = EEPROM.read(Constants::EEPROM_Start_C2);

    if (c1 != 'A' || c2 != 'T' || forceOverwrite) {

        EEPROM.write(Constants::EEPROM_Start_C1, 'A');
        EEPROM.write(Constants::EEPROM_Start_C2, 'T');  

        uint16_t score = 30;
        uint8_t sounds = 1;

        EEPROM.put(Constants::EEPROM_Sounds, sounds);

        for (uint8_t i = 0; i < 5; i++) {
            EEPROM.write(Constants::EEPROM_Seeds + i, 0);
        }

        for (uint8_t i = 0; i < Constants::NumberOfScores; i++) {

            EEPROM.put(Constants::EEPROM_Scores + (7 * i), score = score - 10);
            EEPROM.write(Constants::EEPROM_Scores + (7 * i) + 2, 0);
            EEPROM.write(Constants::EEPROM_Scores + (7 * i) + 3, 0);
            EEPROM.write(Constants::EEPROM_Scores + (7 * i) + 4, 0);
            EEPROM.write(Constants::EEPROM_Scores + (7 * i) + 5, 0);
            EEPROM.write(Constants::EEPROM_Scores + (7 * i) + 6, 0);

        }

    } 
    EEPROM.commit();
}


// Returns the new slot the provided high score should occupy ..
void checkHighScoreSlot(uint16_t score) {

    uint16_t tmpScore = 0;

    for (uint8_t i = 0; i < Constants::NumberOfScores; i++) {

        EEPROM.get(Constants::EEPROM_Scores + (7 * i), tmpScore);

        if (tmpScore < score) {
            
            highScoreVars.slot = i;
            highScoreVars.score = game.score + game.player.x / Constants::TileSize;

            for (uint8_t i = 0; i < 5; i++) {
                highScoreVars.seeds[i] = seedVars.seed[i];
            }
            
            writeHighScoreEntry(highScoreVars);
            return;

        }

    }

    highScoreVars.slot = Constants::EEPROM_No_Slot;

}

void writeHighScoreEntry(HighScoreVars &highScoreVars) {


    // Move the scores down one ..

    for (uint8_t j = Constants::NumberOfScores - 1; j > highScoreVars.slot; j--) {          

        for (uint8_t k = 0; k < 7; k++) {

            EEPROM.write(Constants::EEPROM_Scores + (7 * j) + k, (char)EEPROM.read(Constants::EEPROM_Scores + (7 * (j - 1)) + k));

        }

    }


    // Write score and initials to the nominated slot ..

    EEPROM.put(Constants::EEPROM_Scores + (7 * highScoreVars.slot), game.score + game.player.x / Constants::TileSize);

    for (uint8_t i = 0; i < 5; i++) {

        EEPROM.write(Constants::EEPROM_Scores + (7 * highScoreVars.slot) + 2 + i, highScoreVars.seeds[i]);

    }
 EEPROM.commit();
}

void highScores() {

    uint16_t score;

    // Rnadomly spin a rune ..

    if (gameState == GameState::HighScore_NoFlash && highScoreVars.spinCountdown == 0) {

        highScoreVars.index = hash(game.seed) % 14;
        highScoreVars.spinIndex = 16;
        highScoreVars.spinCountdown = 100;

    }


    // Render screen ..

    Sprites::drawOverwrite(1, 0, Images::Underground_Chain, 0);
    Sprites::drawOverwrite(9, 1, Images::TopThree, 0);
    Sprites::drawOverwrite(65, 0, Images::Underground_Chain, 0);

    for (uint8_t x = 74; x < 128; x = x + 4) {

        Sprites::drawOverwrite(x, 1, Images::HighScore_Pattern, 0);

    }

    for (uint8_t i = 0; i < Constants::NumberOfScores; i++) {

        // If we are not editing this slot, the render it normally ..
        
        if (gameState == GameState::HighScore_NoFlash || i != highScoreVars.slot || arduboy.getFrameCountHalf(32)) {

            EEPROM.get(Constants::EEPROM_Scores + (7 * i), score);

            for (uint8_t j = 0; j < 5; j++) {

                if ((i * 5) + j == highScoreVars.index && highScoreVars.spinIndex > 0) {
                    Sprites::drawOverwrite(1 + (j * 14), 15 + (i * 17), Images::Rune_Frame, 16 - highScoreVars.spinIndex);
                }
                else {
                    Sprites::drawOverwrite(1 + (j * 14), 15 + (i * 17), Images::Rune_Frame, 0);
                    Sprites::drawOverwrite(4 + (j * 14), 15 + (i * 17) + 4, Images::Runes, EEPROM.read(Constants::EEPROM_Scores + (7 * i) + 2 + j));
                }

            }

            // Render score always ..

            uint8_t digits[6] = {};
            extractDigits(digits, score);
            
            for (uint8_t j = 0; j < 6; j++) {
                Sprites::drawSelfMasked(69 + ((6 - j) * 5), 15 + (i * 17) + 4, Images::Numbers, digits[j]);
            }

        }

    }

    Sprites::drawOverwrite(105, 16, Images::Outside_Exit_00, 0);
    Sprites::drawOverwrite(108, 53, Images::HighScore_Pattern_2, 0);


    // Handle player input ..

    uint8_t justPressed = arduboy.justPressedButtons();
    uint8_t pressed = arduboy.pressedButtons();

    if ((pressed & LEFT_BUTTON) && (pressed & RIGHT_BUTTON)) {
        
        highScoreVars.resetCounter++;

        if (highScoreVars.resetCounter == 64) {
            initEEPROM(true);
            highScoreVars.resetCounter = 0;
        }

    }
    else if ((justPressed & A_BUTTON) || (justPressed & B_BUTTON)) {

        gameState = GameState::Title_Init;
        highScoreVars.resetCounter = 0;

    }
    else {

        highScoreVars.resetCounter = 0;

    }

    if (arduboy.frameCount >= 126 && gameState == GameState::HighScore_Flash) gameState = GameState::HighScore_NoFlash;
    highScoreVars.decSpinIndex();

}
