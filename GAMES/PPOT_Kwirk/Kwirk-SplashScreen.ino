#include <Arduboy2.h>


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void splashScreen_Update() { 

    frameCount++;    

    uint8_t justPressed = getJustPressedButtons();

    if (justPressed > 0) {
        
        level.setGameState(GameState::Title_Init); 

    }

}


void splashScreen(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a) {
    
    if (a.needsUpdate()) splashScreen_Update();

    uint8_t currentPlane = a.currentPlane();
    uint8_t idx = static_cast<uint8_t>(level.getGameState()) - static_cast<uint8_t>(GameState::SplashScreen_Start);

    SpritesU::drawOverwriteFX(0, 0, Images::PPOT, (3 * idx) + currentPlane);

    if (frameCount == 12 && level.getGameState() != GameState::Title_Init) {

        idx = (idx + 1) % 4;
        frameCount = 0;
        level.setGameState(static_cast<GameState>(static_cast<uint8_t>(GameState::SplashScreen_Start) + idx));

    }

}
