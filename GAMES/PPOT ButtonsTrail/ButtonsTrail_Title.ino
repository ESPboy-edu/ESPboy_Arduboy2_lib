#include <Arduboy2.h>

#define eeprom_update_byte EEPROM.write
#define eeprom_read_byte EEPROM.read


// ----------------------------------------------------------------------------
//  Initialise state ..
//
void title_Init() {

    uint8_t maxLevel = eeprom_read_byte((Constants::EEPROM_Level_Max));
    uint8_t curlevel = eeprom_read_byte((Constants::EEPROM_Level_Current));
    gameStats.tileSet = eeprom_read_byte((Constants::EEPROM_Tile_Set)) == 1 ? 0 : 1;

    gameStats.titleSel = maxLevel > 0 ? 1 : 0;
    gameStats.maxLevel = maxLevel;
    gameStats.level = curlevel;
    
    gameState = GameState::Title;
    
}   


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void title() {


    // Handle player actions ..

    if (arduboy.justPressed(LEFT_BUTTON)) { 

        gameStats.titleSel = 0;

    }         

    if (arduboy.justPressed(RIGHT_BUTTON) && gameStats.maxLevel > 0) { 

        gameStats.titleSel = 1;

    }         

    if (arduboy.justPressed(A_BUTTON)) { 

        if (gameStats.titleSel == 0) { 
            gameStats.level = 0;
            gameStats.instruction = 0;
            gameState = GameState::Instructions_Init;
            eeprom_update_byte((Constants::EEPROM_Level_Current), gameStats.level);
            EEPROM.commit();
        }
        else {
            gameState = GameState::LevelSelect_Init;
        }

    }         


    // Change tilest ..
    
    if (arduboy.pressed(B_BUTTON)) { 

        gameStats.exit++;

        if (gameStats.exit == 32) {

            gameStats.tileSet = gameStats.tileSet == 1 ? 0 : 1;
            eeprom_update_byte((Constants::EEPROM_Tile_Set), gameStats.tileSet);
            gameStats.exit = 0;
            EEPROM.commit();
        }
    }
    else {

        gameStats.exit = 0;

    }


    // Render page ..

    Sprites::drawOverwrite(9, 9, Images::Title, gameStats.tileSet);
    Sprites::drawOverwrite(25, 49, Images::Title_New, 0);

    if (gameStats.maxLevel > 0) {
     
        Sprites::drawOverwrite(68, 49, Images::Title_Continue, 0);

    }

    uint8_t frame = arduboy.getFrameCount(32) / 8;
    Sprites::drawSelfMasked(14 + (gameStats.titleSel * 44), 50, Images::Player, frame);

}
