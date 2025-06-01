#include "src/utils/Arduboy2Ext.h"

// ----------------------------------------------------------------------------
//  Initialise state ..
//
void splashScreen_Init() {

    gameStateDetails.setCurrState(GAME_SPLASH_SCREEN);

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void splashScreen() { 

    if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {

//gameStateDetails.sequence = 4;//SJH      
        gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);

    }

    FX::drawBitmap(32, 17, Images::PPOT, arduboy.frameCount % 16 < 8, dbmNormal);

    uint8_t y = 17; // Default pixel position 1 is hidden in the top line of the image
    switch (arduboy.frameCount % 36) {

        case 9 ... 17:
            y = 30; // Move pixel down to position 2
            /*-fallthrough*/
        case 0 ... 8:
            Sprites::drawOverwrite(90, 25, PPOT_Arrow, 0); // Flash 'Play' arrow
            break;

        case 18 ... 26:
            y = 31; // Move pixel down to position 3
            break;

        default: // 36 ... 47:
            y = 32; // Move pixel down to position 4
            break;

    }

    arduboy.drawPixel(52, y, WHITE); // Falling pixel represents the tape spooling

}
