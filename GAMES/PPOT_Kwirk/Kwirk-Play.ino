#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "src/entities/Entities.h"
#include "src/utils/SpritesU.hpp"


void play_Init() {

    level.initMap();
    level.setGameState(GameState::Play_BouncingHead);
    level.getPlayer().setHeadBounce(12);

}

bool endOfLevel() {

    uint8_t playerPos = level.getPlayer().getX_WorldCoords();
    uint8_t stairPos = level.getStairX();

    if (playerPos == stairPos) {

        playerPos = level.getPlayer().getY_WorldCoords();
        stairPos = level.getStairY();

        if (playerPos == stairPos) {
            
            switch (level.getPlayerCount()) {

                case 1:

                    completeLevel();
                    return true;

                default:    // Are any other players active?
                    {
                        uint8_t thisPlayerIdx = level.getActivePlayer();
                        bool swappedPlayer = false;

                        for (uint8_t i = 0; i < Constants::PlayerCount; i++) {

                            if (i != thisPlayerIdx) {

                                if (level.getPlayer(i).getX_WorldCoords() != 0) {

                                    level.getPlayer(thisPlayerIdx).setX_WorldCoords(0);
                                    level.setActivePlayer(i);
                                    level.loadMap();
                                    level.setGameState(GameState::Play_BouncingHead);
                                    level.getPlayer().setHeadBounce(12);
                                    swappedPlayer = true;

                                }

                            }

                        }

                        if (!swappedPlayer) {
                            completeLevel();
                            return true;
                        }

                    }

                    break;

            }

        }

    }

    return false;

}


void completeLevel() {

    level.setLevelComplete(level.getLevelNumber());
    level.setActivePlayer(0);

    if (level.allFloorsComplete()) {

        playSFX(MusicSFX::SFX_Victory);
        level.setGameState(GameState::Title_FloorsComplete);

    }
    else {

        if (level.getLevelNumber() % 10 == 9) {
                
            playSFX(MusicSFX::SFX_Victory);
            level.setGameState(level.getGameState_Floor());

        }
        else {

            level.setGameState_Floor(static_cast<GameState>(static_cast<uint8_t>(level.getGameState_Floor()) + 1));
            level.setLevelNumber(level.getLevelNumber() + 1);
            level.initMap();
            level.setGameState(GameState::Play);

            cookie.hasSavedGame = true;
            saveCookie(true);
            playSFX(MusicSFX::SFX_XPGain);

        }

    }
    
}

bool updatePlayer(Player &player) {

    if (player.getMoving_X() != 0) {

        switch (player.getMoving_X()) {

            case 1 ... 8:

                player.setMoving_X(player.getMoving_X() - level.getPlayer().getSpeed());

                if (player.getMoving_X() == 0) {

                    player.incX(1);
                    player.setSpeed(2);
                    return true;

                }

                break;

            case -8 ... -1:

                player.setMoving_X(player.getMoving_X() + level.getPlayer().getSpeed());

                if (player.getMoving_X() == 0) {

                    player.incX(-1);
                    player.setSpeed(2);
                    return true;

                }

                break;

        }

    }

    if (player.getMoving_Y() != 0) {

        switch (player.getMoving_Y()) {

            case 1 ... 8:

                player.setMoving_Y(player.getMoving_Y() - level.getPlayer().getSpeed());

                if (player.getMoving_Y() == 0) {

                    player.incY(1);
                    player.setSpeed(2);
                    return true;

                }

                break;

            case -8 ... -1:

                player.setMoving_Y(player.getMoving_Y() + level.getPlayer().getSpeed());

                if (player.getMoving_Y() == 0) {

                    player.incY(-1);
                    player.setSpeed(2);
                    return true;

                }

                break;

        }

    }   

    return false;
    
}


void play_Update() {

    frameCount++;

    uint8_t justPressed = getJustPressedButtons();
    uint8_t pressed = getPressedButtons();
    uint8_t activePlayer = level.getActivePlayer();
    
    if (frameCount & 0x01) {

        bool playerHasMoved = false;

        if (level.getMoving_X() != 0) {

            switch (level.getMoving_X()) {

                case 1 ... 8:

                    level.setMoving_X(level.getMoving_X() - level.getPlayer().getSpeed());

                    if (level.getMoving_X() == 0) {

                        level.getPlayer().incX(-1);
                        level.getPlayer().setSpeed(2);
                        playerHasMoved = true;

                    }

                    break;

                case -8 ... -1:

                    level.setMoving_X(level.getMoving_X() + level.getPlayer().getSpeed());

                    if (level.getMoving_X() == 0) {

                        level.getPlayer().incX(1);
                        level.getPlayer().setSpeed(2);
                        playerHasMoved = true;

                    }

                    break;
                
            }

        }

        if (level.getMoving_Y() != 0) {

            switch (level.getMoving_Y()) {

                case 1 ... 8:

                    level.setMoving_Y(level.getMoving_Y() - level.getPlayer().getSpeed());

                    if (level.getMoving_Y() == 0) {

                        level.getPlayer().incY(-1);
                        level.getPlayer().setSpeed(2);
                        playerHasMoved = true;

                    }

                    break;

                case -8 ... -1:

                    level.setMoving_Y(level.getMoving_Y() + level.getPlayer().getSpeed());

                    if (level.getMoving_Y() == 0) {

                        level.getPlayer().incY(1);
                        level.getPlayer().setSpeed(2);
                        playerHasMoved = true;

                    }

                    break;

            }

        }


        // Move player?

        Player &player = level.getPlayer(activePlayer);
        playerHasMoved = updatePlayer(player);
        

        // Are we at the end of the level?

        level.loadMap();

        if (endOfLevel()) {

            for (uint8_t i = 0; i < 4; i++) {
                
                Player &player = level.getPlayer(i);
                player.setMoving_X(0);
                player.setMoving_Y(0);
                return;

            }

        }


        // If the player was moving two spaces then handle that movement..

        if (level.getMoving_X() == 0 && player.getMoving_X() == 0 && level.getMoving_Y() == 0 && player.getMoving_Y() == 0 && player.getLastMove() == PlayerCanMove::True_TwoSpaces) {

            player.setLastMove(PlayerCanMove::True);

            switch (player.getDirection()) {

                case Direction::East:
                    handleEastScroll(PlayerCanMove::True_TwoSpaces);
                    break;

                case Direction::West:
                    handleWestScroll(PlayerCanMove::True_TwoSpaces);
                    break;
                                
                case Direction::North:
                    handleNorthScroll(PlayerCanMove::True_TwoSpaces);
                    break;

                case Direction::South:
                    handleSouthScroll(PlayerCanMove::True_TwoSpaces);
                    break;
                    
            }

        }

    }


    // Handle button presses ..

    if (!rotorsStillTurning && level.getMoving_X() == 0 && level.getMoving_Y() == 0 && level.getPlayer(activePlayer).getMoving_X() == 0 && level.getPlayer(activePlayer).getMoving_Y() == 0) {

        if (justPressed & A_BUTTON) {

            level.setGameState(GameState::Play);

            if (level.getPlayerCount() > 1) {

                level.swapPlayer();
                level.loadMap();
                level.setGameState(GameState::Play_BouncingHead);
                level.getPlayer().setHeadBounce(12);
                
            }

        }

        else if (justPressed & B_BUTTON) {

            level.setGameState(GameState::Play_MenuOpen_Init);
        
        }

        else if (pressed & LEFT_BUTTON) {

            PlayerCanMove playerCanMove = level.isWalkable_RelativeToPlayer(Direction::West, -1, 0);
            level.setGameState(GameState::Play);

            switch (playerCanMove) {

                case PlayerCanMove::True:
                case PlayerCanMove::True_TwoSpaces:

                    level.getPlayer().setLastMove(playerCanMove);
                    handleWestScroll(playerCanMove);

                    break;

                default: break;

            }
            
        }

        else if (pressed & RIGHT_BUTTON) {

            PlayerCanMove playerCanMove = level.isWalkable_RelativeToPlayer(Direction::East, 1, 0);
            level.setGameState(GameState::Play);

            switch (playerCanMove) {

                case PlayerCanMove::True:
                case PlayerCanMove::True_TwoSpaces:
                    
                    level.getPlayer().setLastMove(playerCanMove);
                    handleEastScroll(playerCanMove);

                    break;

                default: break;

            }
            
        }

        else if (pressed & UP_BUTTON) {

            PlayerCanMove playerCanMove = level.isWalkable_RelativeToPlayer(Direction::North, 0, -1);
            level.setGameState(GameState::Play);

            switch (playerCanMove) {

                case PlayerCanMove::True:
                case PlayerCanMove::True_TwoSpaces:

                    level.getPlayer().setLastMove(playerCanMove);
                    handleNorthScroll(playerCanMove);

                    break;

                default: break;

            }
            
        }

        else if (pressed & DOWN_BUTTON) {

            PlayerCanMove playerCanMove = level.isWalkable_RelativeToPlayer(Direction::South, 0, 1);
            level.setGameState(GameState::Play);

            switch (playerCanMove) {

                case PlayerCanMove::True:
                case PlayerCanMove::True_TwoSpaces:

                    level.getPlayer().setLastMove(playerCanMove);
                    handleSouthScroll(playerCanMove);

            }
            
        }

    }


    RotorTurning rotorTurning = level.updateItems();

    if (frameCount % 4 == 0) {
        
        if (level.updateFallingBlocks()) {

            level.loadMap(true);

        }
        else if (rotorTurning == RotorTurning::FinishedTurning) {
            
            level.loadItemBitset();

        }

        if (level.getPlayer().getHeadBounce() > 0) {

            level.getPlayer().setHeadBounce(level.getPlayer().getHeadBounce() - 1);

            if (level.getPlayer().getHeadBounce() == 0) {

                level.setGameState(GameState::Play);

            }

        }
        
    }
    else if (rotorTurning == RotorTurning::FinishedTurning) {
        
        level.loadItemBitset();
        
    }

}            


void play(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a) {
    
    if (a.needsUpdate()) play_Update();

    uint8_t currentPlane = a.currentPlane();
    
    switch (level.getGameState()) {

        case GameState::Play:
        case GameState::Play_BouncingHead:

            render(currentPlane, 0);
            break;

        default:
            break;

    }

}

