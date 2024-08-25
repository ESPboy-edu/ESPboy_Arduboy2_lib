#include <Arduboy2.h>


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void splashScreen_Update() { 

    frameCount++; 
    if (hudCounter < 32) hudCounter++;   

    uint8_t justPressed = getJustPressedButtons();

    if (justPressed > 0 && hudCounter == 32) {
        
        gameState = GameState::Title_Init; 

    }

}


void splashScreen(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a) {
    
    if (a.needsUpdate()) splashScreen_Update();

    uint8_t currentPlane = a.currentPlane();
    uint8_t idx = static_cast<uint8_t>(gameState) - static_cast<uint8_t>(GameState::SplashScreen_Start);

    SpritesU::drawOverwriteFX(0, 0, Images::PPOT, (3 * idx) + currentPlane);

    if (frameCount == 12 && gameState != GameState::Title_Init) {

        idx = (idx + 1) % 4;
        frameCount = 0;
        gameState = static_cast<GameState>(static_cast<uint8_t>(GameState::SplashScreen_Start) + idx);

    }

}
