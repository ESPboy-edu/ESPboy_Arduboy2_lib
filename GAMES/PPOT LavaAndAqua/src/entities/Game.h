#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "Block.h"
#include "Player.h"
#include "Portal.h"
#include "GreenDoor.h"
#include "PortalKey.h"
#include "Puzzle.h"

struct Game {

    private:

        uint8_t mapData[9][14];
        uint8_t prevMapData[Constants::Undo_Count][9][14];
        uint8_t level = 0;
        uint8_t portalKeyCount = 0;
        int8_t world_Y_Offset = 0;
        uint16_t frameCount = 0;
        uint16_t moveCount = 0;
        uint8_t undoCount = 0;
        bool preventScrolling = false;

        Player player;
        Portal portal;

        GreenDoor greenDoors[Constants::Green_Door_Count];
        Block blocks[Constants::Block_Count];
        PortalKey portalKeys[Constants::Portal_Key_Count];
        Puzzle puzzles[Constants::Level_Count];

    public:

        uint16_t getFrameCount()                        { return this->frameCount; }
        uint16_t getFrameCount(uint8_t val)             { return this->frameCount % val < val / 2; }
        int8_t getWorld_Y_Offset()                      { return this->world_Y_Offset; }
        uint8_t getLevel()                              { return this->level; }
        uint8_t getPortalKeyCount()                     { return this->portalKeyCount; }
        uint8_t getUndoCount()                          { return this->undoCount; }
        uint16_t getMoveCount()                         { return this->moveCount; }
        bool getPreventScrolling()                      { return this->preventScrolling; }

        Player &getPlayer()                             { return this->player; }
        Portal &getPortal()                             { return this->portal; }
        Block &getBlock(uint8_t idx)                    { return this->blocks[idx]; }
        PortalKey &getPortalKey(uint8_t idx)            { return this->portalKeys[idx]; }
        GreenDoor &getGreenDoor(uint8_t idx)            { return this->greenDoors[idx]; }
        Puzzle &getPuzzle(uint8_t level)                { return this->puzzles[level]; }

        void setMoveCount(uint16_t val)                 { this->moveCount = val; }
        void setFrameCount(uint16_t val)                { this->frameCount = val; }
        void setWorld_Y_Offset(uint8_t val)             { this->world_Y_Offset = val; }
        void setLevel(uint8_t val)                      { this->level = val; }
        void setPortalKeyCount(uint8_t val)             { this->portalKeyCount = val; }

        uint8_t getMapData(uint8_t x, uint8_t y) {
        
            if (x == 0 || x == 15 || y == 0 || y == 10) return 255;
            return this->mapData[y - 1][x - 1];
        }

        void setMapData(uint8_t x, uint8_t y, uint8_t val) {
        
            this->mapData[y - 1][x - 1] = val;
        }

        void resetLevel() {

            this->undoCount= 0;
            this->player.reset();

            for (uint8_t i = 0; i < Constants::Block_Count; i++) {
            
                this->blocks[i].setX(15);
                this->blocks[i].setY(15);

            }

            for (uint8_t i = 0; i < Constants::Portal_Key_Count; i++) {

                this->portalKeys[i].setX(15);
                this->portalKeys[i].setY(15);

            }
            for (uint8_t i = 0; i < Constants::Green_Door_Count; i++) {

                this->greenDoors[i].setX(15);
                this->greenDoors[i].setY(15);

            }


        }

        void incFrameCount() {

            this->frameCount++;

        }
        void decPortalKeyCount() {

            this->portalKeyCount--;

        }

        void resetFrameCount() {

            this->frameCount = 0;
            
        }

        void captureMove() {

            this->player.captureMove();
            this->portal.captureMove();


            for (uint8_t i = 0; i < Constants::Undo_Count - 1; i++) {

                for (uint8_t y = 0; y < Constants::Map_Y_Count - 2; y++) {

                    for (uint8_t x = 0; x < Constants::Map_X_Count - 2; x++) {

                        this->prevMapData[i][y][x] = this->prevMapData[i + 1][y][x];            

                    }
                }

            }    

            for (uint8_t y = 0; y < Constants::Map_Y_Count - 2; y++) {

                for (uint8_t x = 0; x < Constants::Map_X_Count - 2; x++) {

                    this->prevMapData[Constants::Undo_Count - 1][y][x]= this->mapData[y][x];   

                }

            }

            for (uint8_t i = 0; i < Constants::Green_Door_Count; i++) {
            
                GreenDoor &greenDoor = this->greenDoors[i];
                greenDoor.captureMove();

            }
            
            for (uint8_t i = 0; i < Constants::Portal_Key_Count; i++) {
            
                PortalKey &portalKey = this->portalKeys[i];
                portalKey.captureMove();

            }

            for (uint8_t i = 0; i < Constants::Block_Count; i++) {
            
                Block &block = this->blocks[i];
                block.captureMove();

            }

            this->moveCount++;
            if (this->undoCount < Constants::Undo_Count) this->undoCount++;
  
        }

        void revertMove() {

            if (this->undoCount == 0) return;

            this->moveCount--;
            this->player.revertMove();
            this->portal.revertMove();
       

            for (uint8_t y = 0; y < Constants::Map_Y_Count - 2; y++) {

                for (uint8_t x = 0; x < Constants::Map_X_Count - 2; x++) {

                    this->mapData[y][x] = this->prevMapData[Constants::Undo_Count - 1][y][x];   

                }

            }

            for (uint8_t i = Constants::Undo_Count - 1; i > 0; i--) {

                for (uint8_t y = 0; y < Constants::Map_Y_Count - 2; y++) {

                    for (uint8_t x = 0; x < Constants::Map_X_Count - 2; x++) {

                        this->prevMapData[i][y][x] = this->prevMapData[i - 1][y][x];            

                    }
                }

            }    

            for (uint8_t y = 0; y < Constants::Map_Y_Count - 2; y++) {

                for (uint8_t x = 0; x < Constants::Map_X_Count - 2; x++) {

                    this->prevMapData[0][y][x] = 0;   

                }

            }


            for (uint8_t i = 0; i < Constants::Block_Count; i++) {
            
                Block &block = this->blocks[i];
                block.revertMove();

            }

            for (uint8_t i = 0; i < Constants::Green_Door_Count; i++) {
            
                GreenDoor &greenDoor = this->greenDoors[i];
                greenDoor.revertMove();

            }

            uint8_t keyCount = 0;

            for (uint8_t i = 0; i < Constants::Portal_Key_Count; i++) {
            
                PortalKey &portalKey = this->portalKeys[i];
                portalKey.revertMove();

                if (portalKey.isActive()) keyCount++;

            }

            this->portalKeyCount = keyCount;

            this->undoCount--;

        }

        void loadMap(uint8_t level) {

            uint8_t blockIdx = 0;
            uint8_t portalKeyIdx = 0;
            uint8_t greenDoorIdx = 0;

            this->resetLevel();

            // Load Map Data ..
            {
                uint24_t levelStart = FX::readIndexedUInt24(Levels::Levels, level);

                for (uint8_t y = 0; y < Constants::Map_Y_Count - 2; y++) {
                        
                    FX::seekDataArray(levelStart, y, 0, Constants::Map_X_Count - 2);            
                    FX::readObject(mapData[y]);
                    FX::readEnd();

                }

                for (uint8_t y = 1; y < Constants::Map_Y_Count - 1; y++) {

                    for (uint8_t x = 1; x < Constants::Map_X_Count - 1; x++) {
                            
                        switch (this->getMapData(x, y)) {
                        
                            case Constants::Tile_Green_Closed:

                                this->getGreenDoor(greenDoorIdx).setX(x);
                                this->getGreenDoor(greenDoorIdx).setY(y);
                                this->setMapData(x, y, 0);
                                greenDoorIdx++;
                                break;

                            case Constants::Tile_Block:
                            
                                this->getBlock(blockIdx).setX(x);
                                this->getBlock(blockIdx).setY(y);
                                this->setMapData(x, y, 0);
                                blockIdx++;
                                break;

                            case Constants::Tile_Portal:

                                this->getPortal().setX(x);
                                this->getPortal().setY(y);
                                this->getPortal().setOpen(true);
                                this->setMapData(x, y, 0);
                                break;

                            case Constants::Tile_Portal_Inactive:

                                this->getPortal().setX(x);
                                this->getPortal().setY(y);
                                this->getPortal().setOpen(false);
                                this->setMapData(x, y, 0);
                                break;

                            case Constants::Tile_Portal_Key:
                        
                                this->getPortalKey(portalKeyIdx).setX(x);
                                this->getPortalKey(portalKeyIdx).setY(y);
                                this->setMapData(x, y, 0);
                                portalKeyIdx++;
                                break;

                            case Constants::Tile_Player:
                        
                                this->getPlayer().setX(x);
                                this->getPlayer().setY(y);
                                this->setMapData(x, y, 0);
                                break;

                        }

                    }
                        
                }

            }

            this->setPortalKeyCount(portalKeyIdx);

        }

};