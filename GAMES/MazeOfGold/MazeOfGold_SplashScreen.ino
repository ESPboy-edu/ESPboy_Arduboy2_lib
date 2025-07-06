#include "src/Arduboy2Ext.h"

void splashScreen() { 

    uint8_t justPressed = arduboy.justPressedButtons();

    if (justPressed & (A_BUTTON | B_BUTTON)) {
        
        gameState = GameState::Menu_Init; 
        arduboy.frameCount == 0;

    }


    Sprites::drawOverwrite(32, 17, Images::PPOT, 0);

    uint8_t y = 17; 
    
    switch (arduboy.frameCount % 24) {

        case 6 ... 11:
            y = 30; 
            [[fallthrough]]

        case 0 ... 5:
            Sprites::drawOverwrite(91, 25, Images::PPOT_Arrow, 0); 
            break;

        case 12 ... 17:
            y = 31; 
            break;

        default: // 36 ... 47:
            y = 32; 
            break;

    }

    arduboy.drawPixel(52, y, WHITE); 

    if (y % 2 == 0) { 
        Sprites::drawOverwrite(45, 28, Images::PPOT_Spindle, 0);
    }

}
