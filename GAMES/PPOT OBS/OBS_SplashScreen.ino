#include "src/utils/Arduboy2Ext.h"

// ----------------------------------------------------------------------------
//  Initialise state ..
//
void splashScreen_Init() {

    gameState = GameState::SplashScreen;

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void splashScreen() { 

    auto justPressed = arduboy.justPressedButtons();

    if (splashScreenVars.delay < 32) {
        splashScreenVars.delay++;
    
    }

    if (justPressed > 0 && splashScreenVars.delay >= 32) {
        
        gameState = GameState::Title_Init; 

    }


    Sprites::drawOverwrite(32, 17, Images::PPOT, 0);

    uint8_t y = 17; // Default pixel position 1 is hidden in the top line of the image
    switch (arduboy.getFrameCount(48)) {

        case 12 ... 23:
            y = 30; // Move pixel down to position 2
            /*-fallthrough*/
        case 0 ... 11:
            Sprites::drawOverwrite(91, 25, Images::PPOT_Arrow, 0); // Flash 'Play' arrow
            break;

        case 24 ... 35:
            y = 31; // Move pixel down to position 3
            break;

        default: // 36 ... 47:
            y = 32; // Move pixel down to position 4
            break;

    }

    arduboy.drawPixel(52, y, WHITE); // Falling pixel represents the tape spooling
    if (y % 2 == 0) { // On even steps of pixel movement, update the spindle image
        Sprites::drawOverwrite(45, 28, Images::PPOT_Spindle, 0);
    }

}
