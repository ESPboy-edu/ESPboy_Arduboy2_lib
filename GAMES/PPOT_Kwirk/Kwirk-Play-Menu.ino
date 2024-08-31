#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "src/entities/Entities.h"
#include "src/utils/SpritesU.hpp"

void menu_Init() {

    #ifndef DEBUG
        
        if (cookie.hasSavedGame) {
            level.setGameState(GameState::Menu_HasSave_Opt0);
        }
        else {
            level.setGameState(GameState::Menu_NoSave_Opt0);
        }

    #else

        level.setGameState(GameState::Menu_NoSave_Opt0);

    #endif

}

void menu_Open_Init() {

    #ifndef DEBUG

        if (cookie.hasSavedGame) {

            level.setGameState(GameState::Play_MenuOpen_HasSave);

        }
        else {

            level.setGameState(GameState::Play_MenuOpen_NoSave);

        }

    #else

        level.setGameState(GameState::Play_MenuOpen_NoSave);

    #endif

}

void menu_Update() {

    bool hasSave = false;
    frameCount++;

    uint8_t justPressed = getJustPressedButtons();

    switch (level.getGameState()) {

        case GameState::Menu_Start ... GameState::Menu_End:

            if (justPressed & A_BUTTON) {

                uint8_t idx = static_cast<uint8_t>(level.getGameState()) - static_cast<uint8_t>(GameState::Menu_HasSave_Opt0);    

                switch (level.getGameState()) {

                    #ifndef DEBUG_REMOVE_UNDO
                        case GameState::Menu_HasSave_Opt0:
                        case GameState::Menu_NoSave_Opt0:

                            if (level.undos.head != level.undos.tail) {
                                
                                Undo &undo = level.undos.undo[level.undos.head];

                                Player &player = level.getPlayer(undo.playerIdx);
                                player.setX_WorldCoords(undo.playerX);
                                player.setY_WorldCoords(undo.playerY);

                                if (undo.itemIdx != Constants::ItemNotFound) {

                                    Item &item = level.getItem(undo.itemIdx);
                                    item.setX(undo.itemX);
                                    item.setY(undo.itemY);
                                    item.data.location.dataElement1 = undo.dataElement;

                                }

                                level.undos.removeHead();
                                level.setActivePlayer(undo.playerIdx);
                                level.loadMap(true);

                                level.setGameState_Close(level.getGameState());
                                level.setGameState(GameState::Play_MenuClose);
                                level.setGameState_Next(GameState::Play);

                            }
                            break;
                        #endif

                    case GameState::Menu_HasSave_Opt1:
                    case GameState::Menu_NoSave_Opt1:
                        level.initMap();
                        level.loadMap(true);
                        level.setGameState_Close(level.getGameState());
                        level.setGameState(GameState::Play_MenuClose);
                        level.setGameState_Next(GameState::Play);
                        break;

                    #ifndef DEBUG
                        
                        case GameState::Menu_HasSave_Opt2:
                        case GameState::Menu_NoSave_Opt2:
                            cookie.hasSavedGame = true;
                            saveCookie(true);
                            level.setGameState_Close(GameState::Menu_HasSave_Opt1);
                            level.setGameState(GameState::Play_MenuClose);
                            level.setGameState_Next(GameState::Play);
                            break;

                        case GameState::Menu_HasSave_Opt3:
                            saveSoundState();                            
                            FX::loadGameState(cookie);
                            level.setGameState_Close(level.getGameState());
                            level.setGameState(GameState::Play_MenuClose);
                            level.setGameState_Next(GameState::Play);
                            level.setMenuLeft(Constants::Menu_Left);
                            restoreSoundState();
                            break;

                        case GameState::Menu_HasSave_Opt4:
                        case GameState::Menu_NoSave_Opt4:
                            level.setGameState_Close(level.getGameState());
                            level.setGameState(GameState::Play_MenuClose);
                            level.setGameState_Next(level.getGameState_Floor());
                            break;

                    #endif

                }

            }

            if (justPressed & B_BUTTON) {

                level.setGameState_Close(level.getGameState());
                level.setGameState_Next(GameState::Play);
                level.setGameState(GameState::Play_MenuClose);

            }

            if (justPressed & RIGHT_BUTTON) {

                level.setGameState_Close(level.getGameState());
                level.setGameState_Next(GameState::Play);
                level.setGameState(GameState::Play_MenuClose);
                
            }

            if (justPressed & UP_BUTTON) {

                if (hasSave && level.getGameState() > GameState::Menu_HasSave_Opt0) {

                    level.decGameState();

                }
                else {

                    switch (level.getGameState()) {

                        case GameState::Menu_NoSave_Opt1:
                        case GameState::Menu_HasSave_Opt1:
                        case GameState::Menu_NoSave_Opt2:
                        case GameState::Menu_HasSave_Opt2:
                        case GameState::Menu_HasSave_Opt3:
                        case GameState::Menu_HasSave_Opt4:
                            level.decGameState();
                            break;

                        case GameState::Menu_NoSave_Opt4:
                            level.setGameState(GameState::Menu_NoSave_Opt2);
                            break;
                        
                    }

                }
                
            }

            if (justPressed & DOWN_BUTTON) {

                if (hasSave && level.getGameState() < GameState::Menu_HasSave_Opt3) {

                    level.incGameState();

                }
                else {

                    switch (level.getGameState()) {

                        case GameState::Menu_NoSave_Opt0:
                        case GameState::Menu_HasSave_Opt0:
                        case GameState::Menu_NoSave_Opt1:
                        case GameState::Menu_HasSave_Opt1:
                        case GameState::Menu_NoSave_Opt3:
                        case GameState::Menu_HasSave_Opt3:
                            level.incGameState();
                            break;

                        case GameState::Menu_HasSave_Opt2:
                            level.setGameState(GameState::Menu_HasSave_Opt3);
                            break;

                        case GameState::Menu_NoSave_Opt2:
                            level.setGameState(GameState::Menu_NoSave_Opt4);
                            break;
                        
                    }
                    
                }

            }

            break;


        case GameState::Play_MenuClose:

            level.incMenuLeft();

            if (level.getMenuLeft() == Constants::Menu_Right) {
                level.setGameState(level.getGameState_Next());
            }

            break;

        case GameState::Play_MenuOpen_NoSave:
        case GameState::Play_MenuOpen_HasSave:

            level.decMenuLeft();

            if (level.getMenuLeft() == Constants::Menu_Left) {
                level.setGameState(GameState::Menu_Init);
            }

            break;

    }

}

void menu(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a) {
    
    if (a.needsUpdate()) menu_Update();

    uint8_t currentPlane = a.currentPlane();

    render(currentPlane, (Constants::Menu_Right - level.getMenuLeft()) / 8);

    switch (level.getGameState()) {

        case GameState::Menu_Start ... GameState::Menu_End:
            {
                uint8_t idx = static_cast<uint8_t>(level.getGameState()) - static_cast<uint8_t>(GameState::Menu_HasSave_Opt0);   
                uint8_t skill = static_cast<uint8_t>(level.getSkillLevel()) + 1;

                #ifndef DEBUG_REMOVE_UNDO

                    if (level.undos.head == level.undos.tail) {

                        idx = idx + 10;

                    }

                #endif

                SpritesU::drawOverwriteFX(128 - 40, 0, Images::Menu, (3 * idx) + currentPlane);
                SpritesU::drawPlusMaskFX(117, 7, Images::Numbers_1D_WB, (3 * skill) + currentPlane);
                SpritesU::drawPlusMaskFX(117, 14, Images::Numbers_2D_WB, (3 * ((level.getLevelNumber() + 1) % 10)) + currentPlane);
            }

            break;

        case GameState::Play_MenuOpen_NoSave:
            SpritesU::drawOverwriteFX(level.getMenuLeft(), 0, Images::Menu, (3 * 15) + currentPlane);
            break;

        #ifndef DEBUG

            case GameState::Play_MenuOpen_HasSave:
                
                SpritesU::drawOverwriteFX(level.getMenuLeft(), 0, Images::Menu, (3 * 10) + currentPlane);
                break;

        #endif

        case GameState::Play_MenuClose:
            {
                uint8_t idx = static_cast<uint8_t>(level.getGameState_Close()) - static_cast<uint8_t>(GameState::Menu_HasSave_Opt0) + 10;    
                SpritesU::drawOverwriteFX(level.getMenuLeft(), 0, Images::Menu, (3 * idx) + currentPlane);
            }

            break;

    }


}


