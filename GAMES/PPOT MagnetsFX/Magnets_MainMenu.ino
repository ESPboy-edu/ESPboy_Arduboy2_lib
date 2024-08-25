#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "src/entities/Entities.h"
#include "src/utils/SpritesU.hpp"

int16_t xTop = 0;
int16_t xBot = 0;
uint16_t titleAnimations = 0;

void title_Init() {

    gameState = GameState::Title_Play;
    frameCount = 0;
    titleAnimations = 0;

}

void title_Update() {

    frameCount++;
    titleAnimations = titleAnimations + 1;

    uint8_t justPressed = getJustPressedButtons();
    uint8_t pressed = getPressedButtons();

    switch (gameState) {
        
        case GameState::Title_Play:

            if (justPressed & DOWN_BUTTON) {
                titleAnimations = 300;
                gameState = GameState::Title_Options;
            }

            if (justPressed & A_BUTTON) {                
                titleAnimations = 300;
                gameState = GameState::Title_SelectSize;
                game.setRenderSize(soundSettings.getRenderSize());
            }

            break;
        
        case GameState::Title_Options:

            if (justPressed & UP_BUTTON) {
                titleAnimations = 300;
                gameState = GameState::Title_Play;
            }

            if (justPressed & A_BUTTON) {
                gameState = GameState::Title_Options_Small;
            }

            break;
        
        case GameState::Title_SelectSize:

            if (justPressed & UP_BUTTON) {
                if (game.getGameSize() > GameSize::Small) {
                    game.setGameSize((GameSize)(game.getGameSize() - 1));
                }
            }

            if (justPressed & DOWN_BUTTON) {
                if (game.getGameSize() < GameSize::Large) {
                    game.setGameSize((GameSize)(game.getGameSize() + 1));
                }
            }

            if (justPressed & A_BUTTON) {
                gameState = GameState::Title_SelectHardness;
            }

            if (justPressed & B_BUTTON) {
                gameState = GameState::Title_Play;
            }

            break;

        case GameState::Title_SelectHardness:

            if (justPressed & UP_BUTTON) {
                game.setGameLevel(GameLevel::Easy);
            }

            if (justPressed & DOWN_BUTTON) {
                game.setGameLevel(GameLevel::Hard);
            }

            if (justPressed & A_BUTTON) {
                gameState = GameState::Play_Init;
            }

            if (justPressed & B_BUTTON) {
                gameState = GameState::Title_SelectSize;
            }

            break;

        case GameState::Title_Options_Small:
            
            if (justPressed & A_BUTTON) {

                soundSettings.setRenderSize(RenderSize::Small);

            }
            
            if (justPressed & B_BUTTON) {

                gameState = GameState::Title_Options;
                saveCookie(true);
                
            }
            
            if (justPressed & DOWN_BUTTON) {

                gameState++;

            }

            break;

        case GameState::Title_Options_Large:
            
            if (justPressed & A_BUTTON) {

                soundSettings.setRenderSize(RenderSize::Large);

            }
            
            if (justPressed & B_BUTTON) {

                gameState = GameState::Title_Options;
                saveCookie(true);
                
            }
            
            if (justPressed & DOWN_BUTTON) {

                gameState++;

            }

            if (justPressed & UP_BUTTON) {

                gameState--;

            }

            break;

        case GameState::Title_Options_Auto:
            
            if (justPressed & A_BUTTON) {

                soundSettings.setRenderSize(RenderSize::Auto);

            }
            
            if (justPressed & B_BUTTON) {

                gameState = GameState::Title_Options;
                saveCookie(true);
                
            }
            
            if (justPressed & UP_BUTTON) {

                gameState--;

            }

            break;

        
    }

    if (titleAnimations < 300) titleAnimations++;

}

void title(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a) {

    if (frameCount % 4 == 0) {

        xBot = xBot - 1;        if (xBot == -204) xBot = 0;

    }

    if (frameCount % 5 == 0) {

        xTop = xTop + 1;        if (xTop == 204) xTop = 0;

    }

    if (a.needsUpdate()) title_Update();

    uint8_t currentPlane = a.currentPlane();

    switch (gameState) {
        
        case GameState::Title_Options_Small ... GameState::Title_Options_Auto:
            {
                renderCommon(currentPlane);

                if (soundSettings.getRenderSize() == RenderSize::Small)   SpritesU::drawPlusMaskFX(88, 38, Images::Sound_Checkbox, currentPlane);
                if (soundSettings.getRenderSize() == RenderSize::Large)   SpritesU::drawPlusMaskFX(88, 48, Images::Sound_Checkbox, currentPlane);
                if (soundSettings.getRenderSize() == RenderSize::Auto)    SpritesU::drawPlusMaskFX(88, 58, Images::Sound_Checkbox, currentPlane);

            }

            break;     
        
        case GameState::Title_SelectSize: 
            renderWaves(currentPlane);
            SpritesU::drawPlusMaskFX(80, 21, Images::Select, (game.getGameSize() * 3) + currentPlane);
            break;

        case GameState::Title_SelectHardness: 
            renderWaves(currentPlane);
            SpritesU::drawPlusMaskFX(80, 21, Images::Select, 9 + (game.getGameLevel() * 3) + currentPlane);
            break;

        default:
            renderCommon(currentPlane);
            break;    

    }

}

void renderWaves(uint8_t currentPlane) {
    
    SpritesU::drawOverwriteFX(xTop, -3, Images::Wave_Top, currentPlane);
    SpritesU::drawOverwriteFX(xTop - 204, -3, Images::Wave_Top, currentPlane);
    SpritesU::drawOverwriteFX(xBot, 43, Images::Wave_Bot, currentPlane);
    SpritesU::drawOverwriteFX(xBot + 204, 43, Images::Wave_Bot, currentPlane);

}

void renderCommon(uint8_t currentPlane) {

    renderWaves(currentPlane);

    switch (gameState) {

        case GameState::Title_Start ... GameState::Title_Options:

            switch (titleAnimations) {

                case 0 ... Constants::Title_Start_Action - 1:

                    SpritesU::drawPlusMaskFX(4, 0, Images::Magnets, currentPlane);
                    break;

                case Constants::Title_Start_Action ... Constants::Title_Start_Action + 150:

                    SpritesU::drawPlusMaskFX(4, static_cast<int16_t>(-( static_cast<int16_t>(titleAnimations) - Constants::Title_Start_Action) / 4), Images::Magnets, currentPlane);
                    SpritesU::drawPlusMaskFX(80, 64 - static_cast<int16_t>((static_cast<int16_t>(titleAnimations) - Constants::Title_Start_Action) / 4), Images::Title, (3 * (static_cast<uint8_t>(gameState) - 6)) + currentPlane);
                    break;

                default:

                    SpritesU::drawPlusMaskFX(80, 27, Images::Title, (3 * (static_cast<uint8_t>(gameState) - 6)) + currentPlane);
                    break;

            }

            break;

        case  GameState::Title_Options_Small ... GameState::Title_Options_Auto:

            SpritesU::drawPlusMaskFX(80, 15, Images::Title, (3 * (static_cast<uint8_t>(gameState) - 6)) + currentPlane);
            break;

    }

}
