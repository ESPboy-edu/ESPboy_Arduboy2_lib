#include "src/utils/Arduboy2Ext.h"

void drawBackground() {


    Sprites::drawOverwrite(4, 12, Images::Underground_Brick, 0);
    Sprites::drawOverwrite(105, 12, Images::Underground_Brick, 0);

    Sprites::drawOverwrite(-10, 27, Images::Underground_Brick, 0);
    Sprites::drawOverwrite(119, 27, Images::Underground_Brick, 0);

    Sprites::drawOverwrite(4, 41, Images::Underground_Brick, 0);
    Sprites::drawOverwrite(105, 41, Images::Underground_Brick, 0);

    Sprites::drawOverwrite(11, 27, Images::Torch, arduboy.getFrameCount(16) / 4);
    Sprites::drawOverwrite(111, 27, Images::Torch, arduboy.getFrameCount(16) / 4);

}

void titleScreen() {

    uint8_t justPressed = arduboy.justPressedButtons();

    Sprites::drawOverwrite(24, 4, Images::Title, 0);
    Sprites::drawSelfMasked(35, 0, Images::Underground_Chain, 0);
    Sprites::drawSelfMasked(90, 0, Images::Underground_Chain, 0);    
    drawBackground();

    Sprites::drawOverwrite(titleScreenVars.index == TitleScreenMode::Play ? 35 : 65, 59, Images::Title_Highlight, 0);

    if (arduboy.audio.enabled()) {
        Sprites::drawExternalMask(119, 56, Images::Sound_On, Images::Sound_Mask, 0, 0);
    }
    else {
        Sprites::drawExternalMask(119, 56, Images::Sound_Off, Images::Sound_Mask, 0, 0);
    }

    if ((justPressed & A_BUTTON) || (justPressed & B_BUTTON)) {

        switch (titleScreenVars.index) {

            case TitleScreenMode::Play:
                gameState = GameState::IntroText_Init;
                break;

            case TitleScreenMode::HighScore:
                gameState = GameState::HighScore_NoFlash;
                break;

        }

    }

    if ((justPressed & UP_BUTTON) || (justPressed & DOWN_BUTTON)) {
        
        if (arduboy.audio.enabled()) {

            arduboy.audio.off(); 
            arduboy.audio.saveOnOff();

        }
        else {

            arduboy.audio.on(); 
            arduboy.audio.saveOnOff();

        }

    }

    if (justPressed & LEFT_BUTTON) {

        titleScreenVars.index = TitleScreenMode::Play;

    }

    if (justPressed & RIGHT_BUTTON) {

        titleScreenVars.index = TitleScreenMode::HighScore;

    }

}

