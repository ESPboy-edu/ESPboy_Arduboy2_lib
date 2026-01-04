#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "src/utils/SpritesU.hpp"


void title_Init() {

    gameState = GameState::Title_Main;
    titleCounter = 0;
    game.resetFrameCount();

}

void title_Update() {

    game.incFrameCount();

    uint8_t justPressed = getJustPressedButtons();
    uint8_t aReleased = getJustReleasedButtons(A_BUTTON);
    uint8_t bReleased = getJustReleasedButtons(B_BUTTON);
    uint8_t pressed = getPressedButtons();

    switch (gameState) {

        case GameState::Title_Main:

            if ((game.getFrameCount() > 24) && (justPressed & A_BUTTON)) {

                a.initRandomSeed(); 
                gameState = GameState::Title_Select;
                levelSelect.setACounter(11);
                titleCounter = 0;

            }
            break;

        case GameState::Title_Select:

            if (bReleased && levelSelect.getBCounter() < 10) {
                gameState = GameState::Title_Init;
            }

            if (aReleased && levelSelect.getACounter() < 10) {
                game.setLevel(levelSelect.getSelectedPuzzle());
                gameState = GameState::Play_Init;
                nextGameState = GameState::Play;
            }

            if (pressed & A_BUTTON) {
                levelSelect.incACounter();
            }
            else {
                levelSelect.setACounter(0);
            }

            if (pressed & B_BUTTON) {
                levelSelect.incBCounter();
            }
            else {
                levelSelect.setBCounter(0);
            }

            if (levelSelect.getACounter() > 32 && levelSelect.getBCounter() > 32) {
                gameState = GameState::Title_Clear_Progress;      
            }

            if (justPressed & LEFT_BUTTON && levelSelect.getX() > 0) {
                levelSelect.setX(levelSelect.getX() - 1);
            }

            if (justPressed & RIGHT_BUTTON && levelSelect.getX() < 4 && game.getPuzzle(levelSelect.getSelectedPuzzle() + 1).getStatus() != PuzzleStatus::Locked) {
                levelSelect.setX(levelSelect.getX() + 1);
            }

            if (justPressed & DOWN_BUTTON && levelSelect.getY() < 7 && game.getPuzzle(levelSelect.getSelectedPuzzle() + 4).getStatus() != PuzzleStatus::Locked) {
                levelSelect.setY(levelSelect.getY() + 1);
            }

            if (justPressed & UP_BUTTON && levelSelect.getY() > 0) {
                levelSelect.setY(levelSelect.getY() - 1);
            }

            break;

        case GameState::Title_Clear_Progress:

            if (justPressed & A_BUTTON) {

                cookieReset();
                saveCookie();
                levelSelect.setACounter(11);
                levelSelect.setBCounter(11);   
                gameState = GameState::Title_Select;

            }

            if (justPressed & B_BUTTON) {

                gameState = GameState::Title_Select;

            }

            break;

    }

    if (titleCounter < 254) {
        titleCounter++;
    }

}

void title(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a) {
       
    if (a.needsUpdate()) title_Update();

    uint8_t currentPlane = a.currentPlane();
    uint8_t yOffset = Constants::levelSelect_Offset[levelSelect.getY()];

    switch (gameState) {

        case GameState::Title_Main:

            if (titleCounter < 42) {
                SpritesU::drawPlusMaskFX(0, -64 + (titleCounter - 10) * 2, Images::Title_Top, currentPlane);
                SpritesU::drawPlusMaskFX(0, 71 - (titleCounter - 10) * 2, Images::Title_Bottom, currentPlane);
            }
            else {
                SpritesU::drawPlusMaskFX(0, -64 + 64, Images::Title_Top, currentPlane);
                SpritesU::drawPlusMaskFX(0, 71 - 64, Images::Title_Bottom, currentPlane);

            }

            if (titleCounter >= 30 && titleCounter < 59) {
                SpritesU::drawPlusMaskFX(Constants::xPos[titleCounter - 30], 1, Images::Title_Lava, currentPlane);
            }
            else if (titleCounter >= 59) {
                SpritesU::drawPlusMaskFX(26, 1, Images::Title_Lava, currentPlane);
            }

            if (titleCounter > 30 && titleCounter < 59) {
                SpritesU::drawPlusMaskFX(-Constants::xPos[titleCounter - 30] + 80, 21, Images::Title_Amp, currentPlane);
            }
            else if (titleCounter >= 59) {
                SpritesU::drawPlusMaskFX(54, 21, Images::Title_Amp, currentPlane);
            }

            if (titleCounter >= 30 && titleCounter < 59) {
                SpritesU::drawPlusMaskFX(Constants::xPos[titleCounter - 30] + 0, 42, Images::Title_Aqua, currentPlane);
            }
            else if (titleCounter >= 59) {
                SpritesU::drawPlusMaskFX(26, 42, Images::Title_Aqua, currentPlane);
            }

            break;

        case GameState::Title_Select:

            if (titleCounter < 29) {

                SpritesU::drawPlusMaskFX(0, - 20 - (titleCounter - 10) * 2, Images::Title_Top, currentPlane);
                SpritesU::drawPlusMaskFX(0, 32 + (titleCounter - 10) * 2, Images::Title_Bottom, currentPlane);

                if (titleCounter < 29) {
                    SpritesU::drawPlusMaskFX(52 - Constants::xPos[28 - (titleCounter)], 1, Images::Title_Lava, currentPlane);
                }
                else {
                    SpritesU::drawPlusMaskFX(26, 1, Images::Title_Lava, currentPlane);
                }

                if (titleCounter < 29) {
                    SpritesU::drawPlusMaskFX(27 + Constants::xPos[28 - (titleCounter)], 21, Images::Title_Amp, currentPlane);
                }
                else {
                    SpritesU::drawPlusMaskFX(54, 21, Images::Title_Amp, currentPlane);
                }

                if (titleCounter < 29) {
                    SpritesU::drawPlusMaskFX(52 - Constants::xPos[28 - (titleCounter)], 42, Images::Title_Aqua, currentPlane);
                }
                else {
                    SpritesU::drawPlusMaskFX(26, 42, Images::Title_Aqua, currentPlane);
                }

            }
            else {                
                
                for (uint8_t y = 0; y < 6; y++) {

                    for (uint8_t x = 0; x < 4; x++) {

                        Puzzle &puzzle = game.getPuzzle((y * 4) + x);

                        if (puzzle.getStatus() == PuzzleStatus::Complete) {

                            SpritesU::drawOverwriteFX(x * 19, (y * 18) - yOffset, Images::Levels_Select, (((y * 4) + x) * 3) + currentPlane);

                        }
                        else if (puzzle.getStatus() == PuzzleStatus::InProgress) {

                            SpritesU::drawOverwriteFX(x * 19, (y * 18) - yOffset, Images::Levels_Select, ((((y * 4) + x) + 28) * 3) + currentPlane);

                        }                        
                        else {

                            SpritesU::drawOverwriteFX(x * 19, (y * 18) - yOffset, Images::Levels_Select, (52 * 3) + currentPlane);

                        }

                        if (((y * 4) + x) == levelSelect.getSelectedPuzzle()) {

                            if (game.getFrameCount(48)) {
                                SpritesU::drawPlusMaskFX(x * 19, (y * 18) - yOffset, Images::Levels_Cursor, currentPlane);
                            }

                        }

                    }
                    
                }

                SpritesU::drawOverwriteFX(128 - 53, 0, Images::Levels_HUD, (levelSelect.getY() * 3) + currentPlane);
                SpritesU::drawOverwriteFX(128 - 53 + 40, 2, Images::Levels_Number, (levelSelect.getSelectedPuzzle() * 3) + currentPlane);
                SpritesU::drawOverwriteFX(128 - 53 + 9, 17, Images::Levels_Status, (static_cast<uint8_t>(game.getPuzzle(levelSelect.getSelectedPuzzle()).getStatus()) * 3) + currentPlane);

                if (game.getPuzzle(levelSelect.getSelectedPuzzle()).getStatus() == PuzzleStatus::Complete) {

                    SpritesU::drawOverwriteFX(128 - 53 + 7, 32, Images::Levels_NumberOfMoves, currentPlane);
                    SpritesU::drawOverwriteFX(113, 41, Images::Levels_NumberOfMoves_Numbers, ((game.getPuzzle(levelSelect.getSelectedPuzzle()).getNumberOfMoves() / 100) * 3) + currentPlane);
                    SpritesU::drawOverwriteFX(118, 41, Images::Levels_NumberOfMoves_Numbers, (((game.getPuzzle(levelSelect.getSelectedPuzzle()).getNumberOfMoves() % 100) / 10) * 3) + currentPlane);
                    SpritesU::drawOverwriteFX(123, 41, Images::Levels_NumberOfMoves_Numbers, (((game.getPuzzle(levelSelect.getSelectedPuzzle()).getNumberOfMoves() % 100) % 10) * 3) + currentPlane);


                }
                
            }

            break;

        case GameState::Title_Clear_Progress:
            SpritesU::drawOverwriteFX(36, 25, Images::ClearProgress, currentPlane);
            break;

    }            

}
