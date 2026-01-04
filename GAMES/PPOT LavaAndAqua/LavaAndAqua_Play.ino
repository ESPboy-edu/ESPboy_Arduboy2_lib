#include <ArduboyFX.h>  
#include "fxdata/fxdata.h"

bool debug = false;

void play_Init() { 

    game.setMoveCount(0);
    gameState = nextGameState; //GameState::Play;
    titleCounter = 0;

    game.loadMap(game.getLevel());
    popoutMenu.setSelect(0);
    popoutMenu.setX(128);
    popoutMenu.setDirection(Direction::None);
    fix_World_Y_Offset();
    clearBlocks();
    doIncLava = false;

}

void handleMenu(uint8_t justPressed) {

    if (justPressed & UP_BUTTON) {

        if (popoutMenu.getSelect() >= (game.getUndoCount() == 0 ? 2 : 1)) {
            popoutMenu.setSelect(popoutMenu.getSelect() - 1);
        }

    }

    else if (justPressed & DOWN_BUTTON) {

        if (popoutMenu.getSelect() < 2) {
            popoutMenu.setSelect(popoutMenu.getSelect() + 1);
        }

    }

    else if (justPressed & A_BUTTON) {

        puff.setCounter(0);

        switch (popoutMenu.getSelect()) {
        
            case 0:
                game.revertMove();
                popoutMenu.setAllowClose(true);
                if (game.getUndoCount() == 0) {
                    popoutMenu.setDirection(Direction::Right);
                }
                break;
        
            case 1:
                popoutMenu.setAllowClose(true);
                gameState = GameState::Play_Init;
                nextGameState = GameState::Play;
                break;
        
            case 2:
                popoutMenu.setAllowClose(true);
                gameState = GameState::Title_Init;
                break;
                
        }

    }

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//

void play_Update() { 

    uint8_t justPressed = getJustPressedButtons();
    uint8_t pressed = getPressedButtons();

    game.incFrameCount();

    if (gameState == GameState::Play || gameState == GameState::Play_Dead) {
        
        if (puff.getCounter() == 0) {

            if (popoutMenu.getX() == 128) {
                
                if (doIncLava) {
                    incLava();
                    doIncLava = false;
                }
                else {

                    if (justPressed & LEFT_BUTTON && isWalkable(ObjectType::Player, -1, 0)) {
                        game.captureMove();
                        game.getPlayer().decX();
                        updateBlocks();
                        updateGreenDoors();
                        incWater();
                        doIncLava = true;
                    }

                    else if (justPressed & RIGHT_BUTTON && isWalkable(ObjectType::Player, 1, 0)) {
                        game.captureMove();
                        game.getPlayer().incX();
                        updateBlocks();
                        updateGreenDoors();
                        incWater();
                        doIncLava = true;
                    }

                    else if (justPressed & UP_BUTTON && isWalkable(ObjectType::Player, 0, -1)) {
                        game.captureMove();
                        game.getPlayer().decY();
                        updateBlocks();
                        updateGreenDoors();
                        incWater();
                        fix_World_Y_Offset();
                        doIncLava = true;
                    }

                    else if (justPressed & DOWN_BUTTON && isWalkable(ObjectType::Player, 0, 1)) {
                        game.captureMove();
                        game.getPlayer().incY();
                        updateBlocks();
                        updateGreenDoors();
                        incWater();
                        fix_World_Y_Offset();
                        doIncLava = true;
                    }

                }


                // Have we won?

                if (game.getPortalKeyCount() == 0 && game.getPlayer().getX() == game.getPortal().getX() && game.getPlayer().getY() == game.getPortal().getY()) {

                    titleCounter = 0;
                
                    game.getPuzzle(game.getLevel()).setStatus(PuzzleStatus::Complete);
                    game.getPuzzle(game.getLevel()).setNumberOfMoves(game.getMoveCount());
                    
                    uint8_t completed = 0;

                    for (uint8_t i = 0; i < Constants::Level_Count; i++) {

                        if (game.getPuzzle(i).getStatus() == PuzzleStatus::Complete) {
                            completed++;
                        }

                    }

                    if (completed == 24) {

                        gameState = GameState::Play_FadeOut;
                        nextGameState = GameState::GameOver_Init;

                    }
                    else {

                        if (game.getLevel() < Constants::Level_Count - 1 && game.getPuzzle(game.getLevel() + 1).getStatus() != PuzzleStatus::Complete) {

                            gameState = GameState::Play_FadeOut;
                            nextGameState = GameState::Play_FadeIn;

                        }
                        else {

                            gameState = GameState::Play_FadeOut;
                            nextGameState = GameState::Title_Select;

                        }

                    }

                    saveCookie();

                }


                // Have we lost?

                if (isLava(game.getPlayer().getX() , game.getPlayer().getY())) {
                
                    puff.setCounter(1);
                    puff.setX(game.getPlayer().getX());
                    puff.setY(game.getPlayer().getY());

                }


                // Portal Key

                for (uint8_t i = 0; i < Constants::Portal_Key_Count; i++) {
                    
                    if (game.getPlayer().getX() == game.getPortalKey(i).getX() && game.getPlayer().getY() == game.getPortalKey(i).getY()) {
                    
                        game.decPortalKeyCount();
                        game.getPortalKey(i).setY(15);
                        game.getPortalKey(i).setY(15);

                        if (game.getPortalKeyCount() == 0) {
                        
                            game.getPortal().setOpen(true);
                            
                        }

                    }
                
                }

            }
            else {

                handleMenu(justPressed);

            }

        }
        else if (puff.getCounter() == 10) {
            
            handleMenu(justPressed);

        }

            
        // Open menu ..

        if (justPressed & B_BUTTON) {
                    
            if (popoutMenu.getX() == 128) {
                popoutMenu.setDirection(Direction::Left);
                popoutMenu.setSelect(game.getUndoCount() == 0 ? 1 : 0); 
            }
            else if (popoutMenu.getX() == 128 - 32 && popoutMenu.getAllowClose()) {
                popoutMenu.setDirection(Direction::Right);
            }
            
        }

        switch (popoutMenu.getDirection()) {
        
            case Direction::Left:

                popoutMenu.setX(popoutMenu.getX() - 2);

                if (popoutMenu.getX() == 128 - 32) {
                    popoutMenu.setDirection(Direction::None);
                }

                break;
        
            case Direction::Right:

                popoutMenu.setX(popoutMenu.getX() + 2);

                if (popoutMenu.getX() == 128) {
                    popoutMenu.setDirection(Direction::None);
                }

                break;
            
        }


        if (puff.getCounter() > 0 && game.getFrameCount() % 4 == 0) {

            if (puff.getCounter() <10) {

                puff.setCounter(puff.getCounter() + 1);
            
            }

            switch (puff.getCounter()) {
            
                case 4:
                    
                    break;

                case 9:

                    popoutMenu.setDirection(Direction::Left);
                    popoutMenu.setAllowClose(false);
                    break;

            }

        }

    }
    else {
    
        titleCounter++;

        // Fade Out / In
        if (titleCounter == 43) {

            titleCounter = 0;

            switch (nextGameState) {
            
                case GameState::Play_FadeIn:

                    if (gameState == GameState::Play_FadeOut) {
                        gameState = GameState::Play_Init;
                        nextGameState = GameState::Play_FadeIn;
                        game.getPuzzle(game.getLevel() + 1).setStatus(PuzzleStatus::InProgress);
                        game.setLevel(game.getLevel() + 1);
                        levelSelect.increaseGame();
                        debug = true;
                    }
                    else {
                    
                        gameState = GameState::Play;

                    }

                    break;

                default:

                    gameState = nextGameState;
                    break;

            }

        }
    
    }

}


void play(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a) {

    uint8_t currentPlane = a.currentPlane();
    if (a.needsUpdate()) { play_Update(); }

    uint24_t levelIdx = FX::readIndexedUInt24(Images::Level_Images, game.getLevel());
    SpritesU::drawOverwriteFX(0, - game.getWorld_Y_Offset(), levelIdx, currentPlane);

    SpritesU::drawOverwriteFX(120, 0, Images::Mini_HUD, currentPlane);
    SpritesU::drawOverwriteFX(121, 17, Images::Numbers_HUD, ((game.getLevel() + 1) * 3) + currentPlane);
    SpritesU::drawOverwriteFX(121, 50, Images::Numbers_HUD, ((game.getMoveCount() / 10) * 3) + currentPlane);
    SpritesU::drawOverwriteFX(121, 54, Images::Numbers_HUD, ((game.getMoveCount() % 100) * 3) + currentPlane);
   Serial.println("------"); 
    for (uint8_t y = 0; y < Constants::Map_Y_Count; y++) {

        int8_t yPos = (y * 8) - game.getWorld_Y_Offset() - Constants::YOffset_Pixels;

        if (yPos > 63) continue;
        if (yPos < -7) continue;

        for (uint8_t x = 0; x < Constants::Map_X_Count; x++) {

            int8_t xPos = (x * 8) - Constants::XOffset_Pixels;

            if (xPos > popoutMenu.getX() - 8) continue;

            if (game.getMapData(x, y) >= Constants::Tile_Counter_00 && game.getMapData(x, y) <= Constants::Tile_Counter_65) {

                SpritesU::drawOverwriteFX(xPos, yPos, Images::Numbers_5x3_2D_WB, ((game.getMapData(x, y) - Constants::Tile_Counter_00) * 3) + currentPlane);

            }
            else if (game.getMapData(x, y) == Constants::Tile_Waters_Edge) {

                SpritesU::drawPlusMaskFX(xPos, yPos, Images::Tiles, (Constants::Image_Waters_Edge * 3) + currentPlane);

            }
            else if (game.getMapData(x, y) == Constants::Tile_Green_Switch) {

                SpritesU::drawPlusMaskFX(xPos, yPos, Images::Tiles, (Constants::Image_Green_Switch * 3) + currentPlane);

            }
            else if (game.getMapData(x, y) == Constants::Tile_Partial_Wall) {

                SpritesU::drawPlusMaskFX(xPos, yPos, Images::Tiles, (Constants::Image_Partial_Wall * 3) + currentPlane);

            }
            else if (game.getMapData(x, y) == Constants::Tile_Lava_And_Partial_Wall) {

                SpritesU::drawPlusMaskFX(xPos, yPos, Images::Tiles, ((Constants::Image_Lava_And_Partial_Wall + ((game.getFrameCount() % 32) / 8)) * 3) + currentPlane);

            }
            else if (game.getMapData(x, y) == Constants::Tile_Water_And_Partial_Wall) {

                SpritesU::drawPlusMaskFX(xPos, yPos, Images::Tiles, ((Constants::Image_Water_And_Partial_Wall + ((game.getFrameCount() % 32) / 8)) * 3) + currentPlane);

            }
            else if (game.getMapData(x, y) == Constants::Tile_Lava) {

                uint8_t len = 1;

                for (uint8_t i = 1; i < 14; i++) {

                    if (game.getMapData(x + i, y) == Constants::Tile_Lava) {
                        len = i + 1;
                    }
                    else {
                    
                        break;
                    }

                }

                uint24_t imgIdx = FX::readIndexedUInt24(Images::Lavas, len - 1);
                SpritesU::drawOverwriteFX(xPos, yPos, imgIdx, (((game.getFrameCount() % 32) / 8) * 3) + currentPlane);

                x = x + len - 1;

            }

            else if (game.getMapData(x, y) == Constants::Tile_Water) {

                uint8_t len = 1;

                for (uint8_t i = 1; i < 14; i++) {

                    if (game.getMapData(x + i, y) == Constants::Tile_Water) {
                        len = i + 1;
                    }
                    else {
                    
                        break;
                    }

                }
Serial.println(len);
                uint24_t imgIdx = FX::readIndexedUInt24(Images::Waters, len - 1);
                SpritesU::drawOverwriteFX(xPos, yPos, imgIdx, (((game.getFrameCount() % 32) / 8) * 3) + currentPlane);

                x = x + len - 1;

            }

            if (game.getMapData(x, y) == Constants::Tile_Basalt) {

                SpritesU::drawPlusMaskFX(xPos, yPos, Images::Tiles, (Constants::Image_Basalt * 3) + currentPlane);

            }
        
        }
    
    }


    // Blocks ..

    for (uint8_t i = 0; i < Constants::Block_Count; i++) {

        Block &block = game.getBlock(i);

        if (block.isActive()) {
            SpritesU::drawPlusMaskFX((block.getX() * 8) - Constants::XOffset_Pixels, (block.getY() * 8) - game.getWorld_Y_Offset() - Constants::YOffset_Pixels, Images::Tiles, (Constants::Image_Block * 3) + currentPlane);
        }
        else {
            break;
        }

    }


    // Portal Keys ..

    for (uint8_t i = 0; i < Constants::Portal_Key_Count; i++) {

        PortalKey &key = game.getPortalKey(i);

        if (key.isActive()) {
            SpritesU::drawPlusMaskFX((key.getX() * 8) - Constants::XOffset_Pixels, ((key.getY() * 8) - game.getWorld_Y_Offset()) - Constants::YOffset_Pixels, Images::Tiles, ((5 + ((game.getFrameCount() % 36) / 12)) * 3) + currentPlane);
        }

    }



    // Green Doors ..

    for (uint8_t i = 0; i < Constants::Green_Door_Count; i++) {

        GreenDoor &greenDoor = game.getGreenDoor(i);

        if (game.getMapData(greenDoor.getX(), greenDoor.getY()) != Constants::Tile_Basalt) {

            if (greenDoor.isActive()) {

                if (greenDoor.isOpen()) {
                    SpritesU::drawPlusMaskFX((greenDoor.getX() * 8) - Constants::XOffset_Pixels, ((greenDoor.getY() * 8) - game.getWorld_Y_Offset()) - Constants::YOffset_Pixels, Images::Tiles, (Constants::Image_Green_Open * 3) + currentPlane);
                }
                else {
                    SpritesU::drawPlusMaskFX((greenDoor.getX() * 8) - Constants::XOffset_Pixels, ((greenDoor.getY() * 8) - game.getWorld_Y_Offset()) - Constants::YOffset_Pixels, Images::Tiles, (Constants::Image_Green_Closed * 3) + currentPlane);
                }

            }

        }

    }


    // Portal ..

    if (game.getPortal().isOpen()) {

        SpritesU::drawPlusMaskFX((game.getPortal().getX() * 8) - Constants::XOffset_Pixels, ((game.getPortal().getY() * 8) - game.getWorld_Y_Offset()) - Constants::YOffset_Pixels, Images::Tiles, ((Constants::Image_Portal + ((game.getFrameCount() % 40) / 8)) * 3) + currentPlane);
    }
    else {

        SpritesU::drawPlusMaskFX((game.getPortal().getX() * 8) - Constants::XOffset_Pixels, ((game.getPortal().getY() * 8) - game.getWorld_Y_Offset()) - Constants::YOffset_Pixels, Images::Tiles, (Constants::Image_Portal_Inactive * 3) + currentPlane);

    }


    // Player ..


    if (puff.getCounter() > 5) {
        SpritesU::drawPlusMaskFX((game.getPlayer().getX() * 8) - Constants::XOffset_Pixels, ((game.getPlayer().getY() * 8) - game.getWorld_Y_Offset()) - Constants::YOffset_Pixels, Images::Tiles, (Constants::Image_Player_Dead * 3) + currentPlane);
    }
    else {
        SpritesU::drawPlusMaskFX((game.getPlayer().getX() * 8) - Constants::XOffset_Pixels, ((game.getPlayer().getY() * 8) - game.getWorld_Y_Offset()) - Constants::YOffset_Pixels, Images::Tiles, (Constants::Image_Player * 3) + currentPlane);
    }

    if (puff.getCounter() > 0 && puff.getCounter() < 9) {
        SpritesU::drawPlusMaskFX((puff.getX() * 8) - Constants::XOffset_Pixels - 13, (puff.getY() * 8) - game.getWorld_Y_Offset() - Constants::YOffset_Pixels - 13, Images::Puff, ((puff.getCounter() - 1) * 3) + currentPlane);
    }

    if (popoutMenu.getX() < 128) {
        SpritesU::drawOverwriteFX(popoutMenu.getX(), 0, Images::Menu, ((popoutMenu.getSelect() + (game.getUndoCount() == 0 ? 2 : 0)) * 3) + ((popoutMenu.getAllowClose() ? 0 : 5) * 3) + currentPlane);
    }


    // Fade Out

    if (gameState == GameState::Play_FadeOut) {

        if (titleCounter < 42) {
            SpritesU::drawPlusMaskFX(0, -64 + (titleCounter - 10) * 2, Images::Title_Top, currentPlane);
            SpritesU::drawPlusMaskFX(0, 71 - (titleCounter - 10) * 2, Images::Title_Bottom, currentPlane);
        }
        else {
            SpritesU::drawPlusMaskFX(0, -64 + 64, Images::Title_Top, currentPlane);
            SpritesU::drawPlusMaskFX(0, 71 - 64, Images::Title_Bottom, currentPlane);

        }

    }

    // Fade In

    else if (gameState == GameState::Play_FadeIn) {

        if (titleCounter < 42) {

            SpritesU::drawPlusMaskFX(0, - 20 - (titleCounter - 10) * 2, Images::Title_Top, currentPlane);
            SpritesU::drawPlusMaskFX(0, 32 + (titleCounter - 10) * 2, Images::Title_Bottom, currentPlane);

        }

    }

}
