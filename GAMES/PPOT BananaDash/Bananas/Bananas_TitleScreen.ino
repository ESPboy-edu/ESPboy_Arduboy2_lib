#include <Arduboy2.h>
#include "src/utils/SynthU.hpp"


void titleScreen_Init() {

    world.setGameState(GameState::Title_Start);

    world.getItem(0).setX(20);
    world.getItem(0).setY(-50);
    world.getItem(0).setCounter(0);

    world.getItem(1).setX(40);
    world.getItem(1).setY(-70);
    world.getItem(1).setCounter(0);

    world.getItem(2).setX(70);
    world.getItem(2).setY(-20);
    world.getItem(2).setCounter(0);

    world.getItem(3).setX(95);
    world.getItem(3).setY(-40);
    world.getItem(3).setCounter(0);

}

void title_SaveSoundSettings() {
    
    soundSettings.setSounds(!soundSettings.getSounds());
    
    if (soundSettings.getSounds()) {
        playMusic();
    }
    else {
        SynthU::stop();
    }

    saveCookie();

}


void titleScreen_Update() {

    const uint8_t justPressed = getJustPressedButtons();
    const uint8_t pressed = getPressedButtons();

    // frameCount = frameCount + 1;

    // if ((frameCount % 3) == 0) {
    
    //     for (uint8_t i = 0; i < 4; i++) {
        
    //         world.getItem(i).setY(world.getItem(i).getY() + 2);
    //         world.getItem(i).setCounter((world.getItem(i).getCounter() + 1) % 8);
            
    //     }

    // }


    // Handle other input ..

    if (justPressed & UP_BUTTON && world.getGameState() != GameState::Title_Play_Game) {
    
        world.decGameState();

    }

    if (justPressed & DOWN_BUTTON  && world.getGameState() != GameState::Title_Sounds) {
    
        world.incGameState();

    }
   

    if (justPressed & A_BUTTON || justPressed & B_BUTTON) {

        world.setPrevGameState(world.getGameState()); 

        switch (world.getGameState()) {
        
            case GameState::Title_Play_Game:
                world.setGameState(GameState::PlayGame_Init); 
                break;
        
            case GameState::Title_Highs:
                highScore_Entry = 5;
                world.setGameState(GameState::HighScore_View); 
                break;
        
            case GameState::Title_Sounds:
                title_SaveSoundSettings();
                break;
            
        }

    }


}


void titleScreen(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a) {

    if (a.needsUpdate()) { titleScreen_Update(); }

    titleScreen_Render();

}


void titleScreen_Render() {

    uint8_t idx = static_cast<uint8_t>(world.getGameState()) - static_cast<uint8_t>(GameState::Title_Play_Game);

    if (!soundSettings.getSounds()) idx = idx + 3;

    SpritesU::drawOverwriteFX(0, 0, Images::Title, (idx * 3) + currentPlane);

    // for (uint8_t i = 0; i < 4; i++) {
    
    //     Item &item = world.getItem(i);
    //     SpritesU::drawPlusMaskFX(item.getX(), item.getY(), Images::SpinningBananas, (item.getCounter() * 3) + currentPlane);
        
    // }

}
