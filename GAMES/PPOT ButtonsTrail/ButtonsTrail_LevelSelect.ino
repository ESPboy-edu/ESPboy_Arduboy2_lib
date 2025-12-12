#include "src/utils/Arduboy2Ext.h"

#define eeprom_update_byte EEPROM.write
#define eeprom_read_byte EEPROM.read

// ----------------------------------------------------------------------------
//  Initialise state ..
//
void levelSelect_Init() {

    gameState = GameState::LevelSelect;

    levelSelectVars.cursor = eeprom_read_byte((Constants::EEPROM_Level_Current));
    gameStats.maxLevel = eeprom_read_byte((Constants::EEPROM_Level_Max));

    if (gameStats.maxLevel <= 3)                                    { levelSelectVars.topRow = 0; }
    else if (levelSelectVars.cursor <= 1)                           { levelSelectVars.topRow = 0; }
    else if (levelSelectVars.cursor == gameStats.maxLevel)          { levelSelectVars.topRow = levelSelectVars.cursor - 3; }
    else if (levelSelectVars.cursor == gameStats.maxLevel - 1)      { levelSelectVars.topRow = levelSelectVars.cursor - 2; }
    else                                                            { levelSelectVars.topRow = levelSelectVars.cursor - 1; }
    
}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void levelSelect() { 

    if (levelSelectVars.cursorFlashTop > 0)     levelSelectVars.cursorFlashTop--;
    if (levelSelectVars.cursorFlashBottom > 0)  levelSelectVars.cursorFlashBottom--;

    if (arduboy.justPressed(A_BUTTON)) {

        gameStats.level = levelSelectVars.cursor;
        gameStats.moves = 0;
        eeprom_update_byte((Constants::EEPROM_Level_Current), gameStats.level);
        gameState = GameState::Game_Init; 

    }

    if (arduboy.justPressed(B_BUTTON)) {

        gameState = GameState::Title_Init; 

    }

    if (arduboy.justPressed(UP_BUTTON) && levelSelectVars.cursor > 0) {

        levelSelectVars.cursor--;
        levelSelectVars.cursorFlashTop = 4;

        if (gameStats.maxLevel <= 3)                                    { levelSelectVars.topRow = 0; }
        else if (levelSelectVars.cursor <= 1)                           { levelSelectVars.topRow = 0; }
        else if (levelSelectVars.cursor == gameStats.maxLevel)          { levelSelectVars.topRow = levelSelectVars.cursor - 3; }
        else if (levelSelectVars.cursor == gameStats.maxLevel - 1)      { levelSelectVars.topRow = levelSelectVars.cursor - 2; }
        else                                                            { levelSelectVars.topRow = levelSelectVars.cursor - 1; }

    }

    if (arduboy.justPressed(DOWN_BUTTON) && levelSelectVars.cursor < gameStats.maxLevel) {

        levelSelectVars.cursor++;
        levelSelectVars.cursorFlashBottom = 4;

        if (gameStats.maxLevel <= 3)                                    { levelSelectVars.topRow = 0; }
        else if (levelSelectVars.cursor <= 1)                           { levelSelectVars.topRow = 0; }
        else if (levelSelectVars.cursor == gameStats.maxLevel)          { levelSelectVars.topRow = levelSelectVars.cursor - 3; }
        else if (levelSelectVars.cursor == gameStats.maxLevel - 1)      { levelSelectVars.topRow = levelSelectVars.cursor - 2; }
        else if (levelSelectVars.cursor == gameStats.maxLevel - 2)      { levelSelectVars.topRow = levelSelectVars.cursor - 1; }
        else                                                            { levelSelectVars.topRow = levelSelectVars.cursor - 1; }

    }



    if (levelSelectVars.topRow > 0) { 
        
        Sprites::drawOverwrite(61, levelSelectVars.cursorFlashTop > 0 ? 2 : 0, Images::Arrow_Up, 0); 
        arduboy.drawFastHLine(0, 0, 54);
        arduboy.drawFastHLine(75, 0, 54);        

    }
    else {

        arduboy.drawFastHLine(0, 0, 128);

    }


    uint8_t y = 10;

    for (uint8_t i = levelSelectVars.topRow; i <= (gameStats.maxLevel <= 3 ? gameStats.maxLevel : levelSelectVars.topRow + 3); i++) {

        Sprites::drawOverwrite(20, y, Images::Level, 0);

        {
            uint8_t digits[2] = {};
            uint8_t x = 61;

            extractDigits(digits, i);

            for (uint8_t i = 0; i < 2; ++i) {

                if (digits[i] == 1) x = x - 3;

            }

            for (uint8_t i = 0; i < 2; ++i) {
                Sprites::drawOverwrite(x, y, Images::Numbers, digits[i]);
                x = x - (digits[i] == 1 ? 5 : 8);
            }

        }

        uint8_t stars = eeprom_read_byte((Constants::EEPROM_Level_Rating) + i);
        Sprites::drawExternalMask(74, y - 1, (stars > 0 ? Images::Star_Filled : Images::Star_Hollow), Images::Star_Mask, 0, 0);
        Sprites::drawExternalMask(86, y - 1, (stars > 1 ? Images::Star_Filled : Images::Star_Hollow), Images::Star_Mask, 0, 0);
        Sprites::drawExternalMask(98, y - 1, (stars > 2 ? Images::Star_Filled : Images::Star_Hollow), Images::Star_Mask, 0, 0);

        if (i == levelSelectVars.cursor) {
            uint8_t frame = arduboy.getFrameCount(32) / 8;
            Sprites::drawSelfMasked(7, y, Images::Player, frame);
        }

        y = y + 12;

    }

    if (levelSelectVars.topRow + 3 < gameStats.maxLevel) { 
        
        Sprites::drawOverwrite(61, 58 - (levelSelectVars.cursorFlashBottom > 0 ? 2 : 0), Images::Arrow_Down, 0); 
        arduboy.drawFastHLine(0, 63, 54);
        arduboy.drawFastHLine(75, 63, 54);
        
    }
    else {

        arduboy.drawFastHLine(0, 63, 128);

    }

}
