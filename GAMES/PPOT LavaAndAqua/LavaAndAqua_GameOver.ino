#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "src/utils/SpritesU.hpp"


void gameOver_Init() {

    gameState = GameState::GameOver_Main;
    titleCounter = 0;
    game.resetFrameCount();

}

void gameOver_Update() {

    game.incFrameCount();

    uint8_t justPressed = getJustPressedButtons();
    uint8_t aReleased = getJustReleasedButtons(A_BUTTON);
    uint8_t bReleased = getJustReleasedButtons(B_BUTTON);
    uint8_t pressed = getPressedButtons();

    switch (gameState) {

        case GameState::GameOver_Main:

            if ((game.getFrameCount() > 24) && (justPressed & A_BUTTON)) {

                a.initRandomSeed(); 
                gameState = GameState::GameOver_Leave;
                levelSelect.setACounter(11);
                titleCounter = 0;

            }
            break;

        case GameState::GameOver_Leave:

            if (aReleased && levelSelect.getACounter() < 10) {
                game.setLevel(levelSelect.getSelectedPuzzle());
                gameState = GameState::Play_Init;
            }

            break;


    }

    if (titleCounter < 254) {
        titleCounter++;
    }

}

void gameOver(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a) {
       
    if (a.needsUpdate()) gameOver_Update();

    uint8_t currentPlane = a.currentPlane();
    uint8_t yOffset = Constants::levelSelect_Offset[levelSelect.getY()];

    switch (gameState) {

        case GameState::GameOver_Main:

            SpritesU::drawPlusMaskFX(0, -64 + 64, Images::Title_Top, currentPlane);
            SpritesU::drawPlusMaskFX(0, 71 - 64, Images::Title_Bottom, currentPlane);

            if (titleCounter >= 30 && titleCounter < 59) {
                SpritesU::drawPlusMaskFX(Constants::xPos[titleCounter - 30], 11, Images::Title_Good, currentPlane);
            }
            else if (titleCounter >= 59) {
                SpritesU::drawPlusMaskFX(26, 11, Images::Title_Good, currentPlane);
            }

            if (titleCounter > 30 && titleCounter < 59) {
                SpritesU::drawPlusMaskFX(-Constants::xPos[titleCounter - 30] + 44, 32, Images::Title_Work, currentPlane);
            }
            else if (titleCounter >= 59) {
                SpritesU::drawPlusMaskFX(18, 32, Images::Title_Work, currentPlane);
            }

            break;

        case GameState::GameOver_Leave:

            if (titleCounter < 29) {

                SpritesU::drawPlusMaskFX(0, - 20 - (titleCounter - 10) * 2, Images::Title_Top, currentPlane);
                SpritesU::drawPlusMaskFX(0, 32 + (titleCounter - 10) * 2, Images::Title_Bottom, currentPlane);

                if (titleCounter < 29) {
                    SpritesU::drawPlusMaskFX(52 - Constants::xPos[28 - (titleCounter)], 11, Images::Title_Good, currentPlane);
                }
                else {
                    SpritesU::drawPlusMaskFX(26, 11, Images::Title_Good, currentPlane);
                }

                if (titleCounter < 29) {
                    SpritesU::drawPlusMaskFX(- 9 + Constants::xPos[28 - (titleCounter)], 32, Images::Title_Work, currentPlane);
                }
                else {
                    SpritesU::drawPlusMaskFX(44, 32, Images::Title_Work, currentPlane);
                }

            }
            else {                
                
                gameState = GameState::Title_Select;
              
            }

            break;

    }            

}
