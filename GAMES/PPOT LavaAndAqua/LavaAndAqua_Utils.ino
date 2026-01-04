#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "src/utils/SpritesU.hpp"

void updateBlocks() {

    for (uint8_t i = 0; i < Constants::Block_Count; i++) {

        Block &block = game.getBlock(i);
        block.updateNext();

    }

}

void clearBlocks() {

    for (uint8_t i = 0; i < Constants::Block_Count; i++) {

        Block &block = game.getBlock(i);
        block.setX_Next(15);
        block.setY_Next(15);

    }

}

uint8_t getJustPressedButtons() {

    a.pollButtons();

    return a.justPressedButtons();

}

uint8_t getPressedButtons() {

    return a.pressedButtons();

}

uint8_t getJustReleasedButtons(uint8_t button) {

    return a.justReleasedButtons(button);

}

bool isPortal(uint8_t x, uint8_t y) {

    return game.getPortal().getX() == x && game.getPortal().getY() == y;

}

bool isWalkable(ObjectType objectType, int8_t xOffset, int8_t yOffset) {

    return isWalkable(objectType, 0, 0, xOffset, yOffset);

}

bool isWalkable(ObjectType objectType, uint8_t x, uint8_t y, int8_t xOffset, int8_t yOffset) {

    switch (objectType) {

        case ObjectType::Player:

            switch (game.getMapData(game.getPlayer().getX() + xOffset, game.getPlayer().getY() + yOffset)) {
            
                case Constants::Tile_Border:
                    return false;

            }

            switch (game.getMapData(game.getPlayer().getX() + xOffset, game.getPlayer().getY() + yOffset)) {
            
                case Constants::Tile_Basalt:
                    return false;
            
                case Constants::Tile_Counter_00 ... Constants::Tile_Counter_65:
                    return false;

            }


            // Push block?

            for (uint8_t i = 0; i < Constants::Block_Count; i++) {

                Block &block = game.getBlock(i);

                if (!block.isActive()) break;

                if (block.getX() == game.getPlayer().getX() + xOffset && block.getY() == game.getPlayer().getY() + yOffset) {
                
                    if (isWalkable(ObjectType::Block, block.getX(), block.getY(), xOffset, yOffset)) {

                        block.setX_Next(block.getX() + xOffset);
                        block.setY_Next(block.getY() + yOffset);

                        if (game.getMapData(block.getX() + xOffset, block.getY() + yOffset) == Constants::Tile_Lava) {
                            game.setMapData(block.getX() + xOffset, block.getY() + yOffset, 0);
                        }

                        return true;
                    
                    }
                    else {
                        return false;
                    }

                }

            }
            

            return true;

        case ObjectType::Lava:

            switch (game.getMapData(x + xOffset, y + yOffset)) {
            
                case Constants::Tile_Border:
                case Constants::Tile_Basalt:
                case Constants::Tile_Counter_00 ... Constants::Tile_Counter_65:
                case Constants::Tile_Waters_Edge:
                case Constants::Temp_Lava_And_Partial_Wall:
                case Constants::Temp_Lava:
                    return false;

            }

            // Is lava blocked by a block?

            for (uint8_t i = 0; i < Constants::Block_Count; i++) {

                Block &block = game.getBlock(i);
                
                if (!block.isActive()) break;

                if (block.getX() == x + xOffset && block.getY() == y + yOffset) {

                    return false;

                }

            }

            // Is lava blocked by a block?

            for (uint8_t i = 0; i < Constants::Green_Door_Count; i++) {

                GreenDoor &greenDoor = game.getGreenDoor(i);
                
                if (!greenDoor.isActive()) break;

                if (!greenDoor.isOpen() && greenDoor.getX() == x + xOffset && greenDoor.getY() == y + yOffset) {

                    return false;

                }

            }

            return true;

        case ObjectType::Water:

            switch (game.getMapData(x + xOffset, y + yOffset)) {
            
                case Constants::Tile_Border:
                case Constants::Tile_Basalt:
                case Constants::Tile_Counter_00 ... Constants::Tile_Counter_65:
                case Constants::Tile_Waters_Edge:
                case Constants::Temp_Water_And_Partial_Wall:
                case Constants::Temp_Water:
                    return false;

            }

            // Is the water blocked by a block?

            for (uint8_t i = 0; i < Constants::Block_Count; i++) {

                Block &block = game.getBlock(i);
                
                if (!block.isActive()) break;

                if (block.getX() == x + xOffset && block.getY() == y + yOffset) {

                    return false;

                }

            }


            // Is lava blocked by a green door?

            for (uint8_t i = 0; i < Constants::Green_Door_Count; i++) {

                GreenDoor &greenDoor = game.getGreenDoor(i);
                
                if (!greenDoor.isActive()) break;

                if (!greenDoor.isOpen() && greenDoor.getX() == x + xOffset && greenDoor.getY() == y + yOffset) {

                    return false;

                }

            }
            
            return true;

        case ObjectType::Block:

            switch (game.getMapData(x + xOffset, y + yOffset)) {
            
                case Constants::Tile_Border:
                case Constants::Tile_Counter_00 ... Constants::Tile_Counter_65:
                case Constants::Tile_Partial_Wall:
                case Constants::Tile_Lava_And_Partial_Wall:
                    return false;

            }


            // Push into a second block?

            for (uint8_t i = 0; i < Constants::Block_Count; i++) {

                Block &block = game.getBlock(i);

                if (!block.isActive()) break;
                
                if (block.getX() == game.getPlayer().getX() + (2 * xOffset) && block.getY() == game.getPlayer().getY() + (2 * yOffset)) {

                    return false;

                }


                // Is the block blocked by a green door?

                for (uint8_t i = 0; i < Constants::Green_Door_Count; i++) {

                    GreenDoor &greenDoor = game.getGreenDoor(i);
                    
                    if (!greenDoor.isActive()) break;

                    if (!greenDoor.isOpen() && greenDoor.getX() == x + xOffset && greenDoor.getY() == y + yOffset) {

                        return false;

                    }

                }
                
            }

            return true;

    }

    return true;

}

bool isLava(uint8_t x, uint8_t y) {

    uint8_t tile = game.getMapData(x, y);

    return tile == Constants::Tile_Lava;
    
}

void incWater() {

    
    // Water first ..

    for (uint8_t y = 1; y < Constants::Map_Y_Count - 1; y++) {

        for (uint8_t x = 1; x < Constants::Map_X_Count - 1; x++) {

            uint8_t mp00 = game.getMapData(x, y);

            if (mp00 == Constants::Tile_Water || mp00 == Constants::Tile_Water_And_Partial_Wall) {

                uint8_t mpN0 = game.getMapData(x - 1, y);
                uint8_t mpP0 = game.getMapData(x + 1, y);
                uint8_t mp0N = game.getMapData(x, y - 1);
                uint8_t mp0P = game.getMapData(x, y + 1);

                if (mpN0 == Constants::Tile_Lava)      { game.setMapData(x - 1, y, Constants::Tile_Basalt);     mpN0 = Constants::Tile_Basalt; }
                if (mpP0 == Constants::Tile_Lava)      { game.setMapData(x + 1, y, Constants::Tile_Basalt);     mpP0 = Constants::Tile_Basalt; }
                if (mp0N == Constants::Tile_Lava)      { game.setMapData(x, y - 1, Constants::Tile_Basalt);     mp0N = Constants::Tile_Basalt; }
                if (mp0P == Constants::Tile_Lava)      { game.setMapData(x, y + 1, Constants::Tile_Basalt);     mp0P = Constants::Tile_Basalt; }

                if (mpN0 != Constants::Tile_Border && mpN0 != Constants::Tile_Basalt && mpN0 != Constants::Tile_Water && mpN0 != Constants::Tile_Water_And_Partial_Wall && isWalkable(ObjectType::Water, x, y, -1, 0) && !isPortal(x - 1, y)) { 
                    if (mpN0 == Constants::Tile_Partial_Wall) {
                        game.setMapData(x - 1, y, Constants::Temp_Water_And_Partial_Wall);
                    }
                    else {
                        game.setMapData(x - 1, y, Constants::Temp_Water);
                    }
                }

                if (mpP0 != Constants::Tile_Border && mpP0 != Constants::Tile_Basalt && mpP0 != Constants::Tile_Water && mpP0 != Constants::Tile_Water_And_Partial_Wall && isWalkable(ObjectType::Water, x, y, 1, 0) && !isPortal(x + 1, y)) { 
                    if (mpP0 == Constants::Tile_Partial_Wall) {
                        game.setMapData(x + 1, y, Constants::Temp_Water_And_Partial_Wall);
                    }
                    else {
                        game.setMapData(x + 1, y, Constants::Temp_Water);
                    }
                }

                if (mp0N != Constants::Tile_Border && mp0N != Constants::Tile_Basalt && mp0N != Constants::Tile_Water && mp0N != Constants::Tile_Water_And_Partial_Wall && isWalkable(ObjectType::Water, x, y, 0, -1) && !isPortal(x, y - 1)) { 
                    if (mp0N == Constants::Tile_Partial_Wall) {
                        game.setMapData(x, y - 1, Constants::Temp_Water_And_Partial_Wall);
                    }
                    else {
                        game.setMapData(x, y - 1, Constants::Temp_Water);
                    }
                }

                if (mp0P != Constants::Tile_Border && mp0P != Constants::Tile_Basalt && mp0P != Constants::Tile_Water && mp0P != Constants::Tile_Water_And_Partial_Wall && isWalkable(ObjectType::Water, x, y, 0, 1) && !isPortal(x, y + 1)) { 
                    if (mp0P == Constants::Tile_Partial_Wall) {
                        game.setMapData(x, y + 1, Constants::Temp_Water_And_Partial_Wall);
                    }
                    else {
                        game.setMapData(x, y + 1, Constants::Temp_Water);
                    }
                }

            }

        }
    
    }

    for (uint8_t y = 0; y < Constants::Map_Y_Count; y++) {

        for (uint8_t x = 0; x < Constants::Map_X_Count; x++) {

            uint8_t mp00 = game.getMapData(x, y);

            if (mp00 == Constants::Temp_Water)                     { game.setMapData(x, y, Constants::Tile_Water); }
            if (mp00 == Constants::Temp_Water_And_Partial_Wall)    { game.setMapData(x, y, Constants::Tile_Water_And_Partial_Wall); }
       
        }
    
    }

}

void incLava() {

    for (uint8_t y = 1; y < Constants::Map_Y_Count - 1; y++) {

        for (uint8_t x = 1; x < Constants::Map_X_Count - 1; x++) {

            uint8_t mp00 = game.getMapData(x, y);

            if (mp00 == Constants::Tile_Lava || mp00 == Constants::Tile_Lava_And_Partial_Wall) {

                uint8_t mpN0 = game.getMapData(x - 1, y);
                uint8_t mpP0 = game.getMapData(x + 1, y);
                uint8_t mp0N = game.getMapData(x, y - 1);
                uint8_t mp0P = game.getMapData(x, y + 1);
                
                if (mpN0 == Constants::Tile_Water)      { game.setMapData(x - 1, y, Constants::Tile_Basalt);     mpN0 = Constants::Tile_Basalt; }
                if (mpP0 == Constants::Tile_Water)      { game.setMapData(x + 1, y, Constants::Tile_Basalt);     mpP0 = Constants::Tile_Basalt; }
                if (mp0N == Constants::Tile_Water)      { game.setMapData(x, y - 1, Constants::Tile_Basalt);     mp0N = Constants::Tile_Basalt; }
                if (mp0P == Constants::Tile_Water)      { game.setMapData(x, y + 1, Constants::Tile_Basalt);     mp0P = Constants::Tile_Basalt; }

                if (mpN0 != Constants::Tile_Border && mpN0 != Constants::Tile_Basalt && mpN0 != Constants::Tile_Lava && mpN0 != Constants::Tile_Lava_And_Partial_Wall && isWalkable(ObjectType::Lava, x, y, -1, 0) && !isPortal(x - 1, y)) { 
                    if (mpN0 == Constants::Tile_Partial_Wall) {
                        game.setMapData(x - 1, y, Constants::Temp_Lava_And_Partial_Wall);
                    }
                    else {
                        game.setMapData(x - 1, y, Constants::Temp_Lava);
                    }
                }
                
                if (mpP0 != Constants::Tile_Border && mpP0 != Constants::Tile_Basalt && mpP0 != Constants::Tile_Lava && mpP0 != Constants::Tile_Lava_And_Partial_Wall && isWalkable(ObjectType::Lava, x, y, 1, 0) && !isPortal(x + 1, y)) { 
                    if (mpP0 == Constants::Tile_Partial_Wall) {
                        game.setMapData(x + 1, y, Constants::Temp_Lava_And_Partial_Wall);
                    }
                    else {
                        game.setMapData(x + 1, y, Constants::Temp_Lava);
                    }
                }

                if (mp0N != Constants::Tile_Border && mp0N != Constants::Tile_Basalt && mp0N != Constants::Tile_Lava && mp0N != Constants::Tile_Lava_And_Partial_Wall && isWalkable(ObjectType::Lava, x, y, 0, -1) && !isPortal(x, y - 1)) { 
                    if (mp0N == Constants::Tile_Partial_Wall) {
                        game.setMapData(x, y - 1, Constants::Temp_Lava_And_Partial_Wall);
                    }
                    else {
                        game.setMapData(x, y - 1, Constants::Temp_Lava);
                    }
                }

                if (mp0P != Constants::Tile_Border && mp0P != Constants::Tile_Basalt && mp0P != Constants::Tile_Lava && mp0P != Constants::Tile_Lava_And_Partial_Wall && isWalkable(ObjectType::Lava, x, y, 0, 1) && !isPortal(x, y + 1)) { 
                    if (mp0P == Constants::Tile_Partial_Wall) {
                        game.setMapData(x, y + 1, Constants::Temp_Lava_And_Partial_Wall);
                    }
                    else {
                        game.setMapData(x, y + 1, Constants::Temp_Lava);
                    }
                }

            }

        }
    
    }

    for (uint8_t y = 0; y < Constants::Map_Y_Count; y++) {

        for (uint8_t x = 0; x < Constants::Map_X_Count; x++) {

            uint8_t mp00 = game.getMapData(x, y);

            if (mp00 == Constants::Temp_Lava)                      { game.setMapData(x, y, Constants::Tile_Lava); }
            if (mp00 == Constants::Temp_Lava_And_Partial_Wall)     { game.setMapData(x, y, Constants::Tile_Lava_And_Partial_Wall); }
       
            if (mp00 >= Constants::Tile_Counter_00 && mp00 <= Constants::Tile_Counter_65) {
            
                game.setMapData(x, y, mp00 - 1);

                if (game.getMapData(x, y) == Constants::Tile_Counter_00) {

                    game.setMapData(x, y, 0);

                }

            }

        }
    
    }

}

void fix_World_Y_Offset() {

    if (game.getPreventScrolling()) return;

    switch (game.getPlayer().getY()) {
    
        case 1 ... 3:
            game.setWorld_Y_Offset(0);
            break;

        case 4:
            game.setWorld_Y_Offset(4);
            break;

        case 5:
            game.setWorld_Y_Offset(8);
            break;

        case 6 ... 9:
            game.setWorld_Y_Offset(12);
            break;


    }
    
}

void updateGreenDoors() {

    for (uint8_t i = 0; i < Constants::Green_Door_Count; i++) {
        
        GreenDoor &greenDoor = game.getGreenDoor(i);
        greenDoor.setOpen(false);

        if (game.getMapData(game.getPlayer().getX(), game.getPlayer().getY()) == Constants::Tile_Green_Switch) {
            greenDoor.setOpen(true);
        }
        else {

            for (uint8_t j = 0; j < Constants::Block_Count; j++) {
                
                Block &block = game.getBlock(j);
                if (!block.isActive()) break;

                if (game.getMapData(block.getX(), block.getY()) == Constants::Tile_Green_Switch) {
    
                    greenDoor.setOpen(true);

                }
            
            }

        }

    }
    
}

void saveCookie() {

    FX::saveGameState(cookie);

}

void cookieReset() {

    for (uint8_t i = 0; i < 24; i++) {

        // if (i == 0) { //SJH
        if (i < 23) { //SJH
            game.getPuzzle(i).setStatus(PuzzleStatus::InProgress);
            game.getPuzzle(i).setNumberOfMoves(0);
        }
        else {
            game.getPuzzle(i).setStatus(PuzzleStatus::Locked);
            game.getPuzzle(i).setNumberOfMoves(0);
        }

    }
    
    levelSelect.setX(0);
    levelSelect.setY(0);

}