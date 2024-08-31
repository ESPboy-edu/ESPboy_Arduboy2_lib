#pragma once

#include <Arduboy2.h>
#include "Entities.h"
#include "Player.h"
#include "../utils/Enums.h"
#include "../../fxdata/fxdata.h"


struct SoundSettings {

    bool music = true;
    bool sfx = true;
    uint8_t volume = 7;

    bool getMusic()                 { return this->music; }
    bool getSFX()                   { return this->sfx; }
    uint8_t getVolume()             { return this->volume; }

    void setMusic(bool val)         { this->music = val; }
    void setSFX(bool val)           { this->sfx = val; }
    void setVolume(uint8_t val)     { this->volume = val; }

};

class Level {

    private:

        #ifndef DEBUG
        
            GameState gameState = GameState::SplashScreen_Start;
            GameState gameState_Skill = GameState::SplashScreen_Start;
            GameState gameState_Floor = GameState::SplashScreen_Start;
            GameState gameState_Next = GameState::SplashScreen_Start;
            GameState gameState_Close = GameState::SplashScreen_Start;

        #else
        
            GameState gameState = GameState::Title_Init;
            GameState gameState_Skill = GameState::Title_Init;
            GameState gameState_Floor = GameState::Title_Init;
            GameState gameState_Next = GameState::Title_Init;
            GameState gameState_Close = GameState::Title_Init;

        #endif

        #ifdef SOUNDS_SYNTHU

            SoundSettings soundSettings;

        #endif

        uint8_t itemBitset[Constants::LevelMapHeight][Constants::LevelMapWidth];
        uint8_t levelComplete[30];

        Item items[Constants::ItemCount];
        MapElement mapElements[Constants::LevelMapHeight][Constants::LevelMapWidth];
        Player players[Constants::PlayerCount];
        uint8_t activePlayer = 0;

        uint8_t levelNumber = START_LEVEL;
        uint8_t width = 0;
        uint8_t height = 0;
        uint8_t menuLeft = Constants::Menu_Right;
        SkillLevels skillLevel = SkillLevels::Easy;

        Point_S8 map = { 0, 0 };
        Point_U8 view = { 0, 0 };
        Point_S8 moving = { 0, 0 };
        Point_U8 stair = { 0, 0 };
        
        bool allowScrollX = true;
        bool allowScrollY = true;

    public:

        #ifndef DEBUG_REMOVE_UNDO
            Undos undos;
        #endif

    public:

        GameState getGameState()                                        { return this->gameState; }
        GameState getGameState_Skill()                                  { return this->gameState_Skill; }
        GameState getGameState_Floor()                                  { return this->gameState_Floor; }
        GameState getGameState_Next()                                   { return this->gameState_Next; }
        GameState getGameState_Close()                                  { return this->gameState_Close; }
        SkillLevels getSkillLevel()                                     { return this->skillLevel; }

        uint8_t getLevelNumber()                                        { return this->levelNumber; }
        uint8_t getWidth()                                              { return this->width; }
        uint8_t getHeight()                                             { return this->height; }
        uint8_t getMapX()                                               { return this->map.x; }
        uint8_t getMapY()                                               { return this->map.y; }
        uint8_t getStairX()                                             { return this->stair.x; }
        uint8_t getStairY()                                             { return this->stair.y; }
        uint8_t getMenuLeft()                                           { return this->menuLeft; }
        uint8_t getLevelComplete(uint8_t idx)                           { return this->levelComplete[idx]; }

        int8_t getMoving_X()                                            { return this->moving.x; }
        int8_t getMoving_Y()                                            { return this->moving.y; }

        bool getAllowScrollX()                                          { return this->allowScrollX; }
        bool getAllowScrollY()                                          { return this->allowScrollY; }

        #ifdef SOUNDS_SYNTHU

            SoundSettings& getSoundSettings()                               { return this->soundSettings; };

        #endif

        void setGameState(GameState val)                                { this->gameState = val; }
        void setGameState_Skill(GameState val)                          { this->gameState_Skill = val; }
        void setGameState_Floor(GameState val)                          { this->gameState_Floor = val; }
        void setGameState_Next(GameState val)                           { this->gameState_Next = val; }
        void setGameState_Close(GameState val)                          { this->gameState_Close = val; }

        void setLevelNumber(uint8_t val)                                { this->levelNumber = val; }
        void setWidth(uint8_t val)                                      { this->width = val; }
        void setHeight(uint8_t val)                                     { this->height = val; }
        void setMapX(int8_t val)                                        { this->map.x = val; }
        void setMapY(int8_t val)                                        { this->map.y = val; }
        void setViewX(uint8_t val)                                      { this->view.x = val; }
        void setViewY(uint8_t val)                                      { this->view.y = val; }
        void setMoving_X(int8_t val)                                    { this->moving.x = val; }
        void setMoving_Y(int8_t val)                                    { this->moving.y = val; }

        void setAllowScrollX(bool val)                                  { this->allowScrollX = val; }
        void setAllowScrollY(bool val)                                  { this->allowScrollY = val; }
        void setMenuLeft(uint8_t val)                                   { this->menuLeft = val; }
        void setLevelComplete(uint8_t idx)                              { this->levelComplete[idx] = 1; }
        void setSkillLevel(SkillLevels val)                             { this->skillLevel = val; }
        void setSkillLevel(uint8_t val)                                 { this->skillLevel = static_cast<SkillLevels>(val); }

        Player& getPlayer()                                             { return this->players[this->activePlayer]; }
        Player& getPlayer(uint8_t idx)                                  { return this->players[idx]; }
        uint8_t getActivePlayer()                                       { return this->activePlayer; }
       
        void setActivePlayer(uint8_t val)                               { this->activePlayer = val; }

        void swapPlayer()                                               { 

            while (true) {
    
                this->activePlayer++;
                
                if (this->activePlayer == Constants::PlayerCount) {
                    this->activePlayer = 0;
                }

                if (this->players[this->activePlayer].getX_WorldCoords() != 0) {

                    return;

                }

            }

        }

        uint8_t getPlayerCount()                                        { 

            uint8_t playerCount = 0;

            for (uint8_t playerIdx = 0; playerIdx < Constants::PlayerCount; playerIdx++) {

                if (this->players[playerIdx].getX_WorldCoords() != 0) {
                    playerCount++;
                }
            
            }

            return playerCount;
        }

        void reset() {

            for (uint8_t i = 0; i < 30; i++) {
                levelComplete[i] = 0;
            }

        }

        #include "Level_Load.hpp"
        #include "Level_Utils.hpp"
        #include "Level_Rotate.hpp"
        #include "Level_Items.hpp"

        #ifndef DEBUG_REMOVE_UNDO

            void addUndo(Item &item, uint8_t itemIdx) {

                Undo &undo = this->undos.addEntry();
                undo.playerIdx = this->getActivePlayer();
                undo.playerX = this->getPlayer().getX_WorldCoords();
                undo.playerY = this->getPlayer().getY_WorldCoords();
                undo.itemIdx = itemIdx;
                undo.itemX = item.getX();
                undo.itemY = item.getY();
                undo.dataElement = item.data.location.dataElement1;

                #if defined(DEBUG) && defined(DEBUG_UNDOS)
                    printUndos();
                #endif

            }

            void addUndoMoveOnly() {

                Undo &undo = this->undos.addEntry();
                undo.playerIdx = this->getActivePlayer();
                undo.playerX = this->getPlayer().getX_WorldCoords();
                undo.playerY = this->getPlayer().getY_WorldCoords();
                undo.itemIdx = Constants::ItemNotFound;

                #if defined(DEBUG) && defined(DEBUG_UNDOS)
                    printUndos();
                #endif

            }

        #else
            
            void addUndo(Item &item, uint8_t itemIdx) { }
            void addUndoMoveOnly() { }

        #endif

        PlayerCanMove isWalkable_RelativeToPlayer(Direction direction, int8_t x, int8_t y) {


            // Make sure there are no other players in the way ..

            for (uint8_t i = 0; i < Constants::PlayerCount; i++) {

                if (i == this->getActivePlayer()) continue;

                if (this->getPlayer(i).getX_WorldCoords() == this->getPlayer().getX_WorldCoords() + x &&
                    this->getPlayer(i).getY_WorldCoords() == this->getPlayer().getY_WorldCoords() + y) {

                    return PlayerCanMove::False; 
             
                }

            }
            

            MapElement mapElement = this->getMapElement_RelativeToPlayer(x, y);

            #if defined(DEBUG) && defined(DEBUG_IS_WALKABLE)
            DEBUG_PRINT(F("isWalkable_RelativeToPlayer() Offset: "));
            DEBUG_PRINT(x);
            DEBUG_PRINT(",");
            DEBUG_PRINT(y);
            DEBUG_PRINT(", map ");
            DEBUG_PRINT((uint8_t)mapElement);
            DEBUG_PRINT(F(", loc: "));
            DEBUG_PRINT(this->getPlayer().getX_WorldCoords() + x);
            DEBUG_PRINT(",");
            DEBUG_PRINTLN(this->getPlayer().getY_WorldCoords() +y);
            #endif

            switch (mapElement) {

                case MapElement::Tile_Solid:
                case MapElement::Tile_Hole:
                case MapElement::Tile_UpperEdge:
                case MapElement::Tile_BottonEdge_Left:
                case MapElement::Tile_BottonEdge_Right:
                case MapElement::Tile_BottonEdge_Middle:
                case MapElement::Tile_BottonEdge_FullWidth:
                case MapElement::Tile_Line_Left:
                case MapElement::Tile_Line_Middle:
                case MapElement::Tile_Line_Right:                
                case MapElement::Tile_Line_FullWidth:
                    {        

                        // Immediately ahead ..

                        uint8_t itemIdx = this->getItemIdx_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y);//, ItemType::FirstItemType, ItemType::LastItemType);

                        if (itemIdx != Constants::ItemNotFound) {

                            Item &item = this->getItem(itemIdx);

                            switch (item.getItemType()) {
                               
                                case ItemType::Block_Start ... ItemType::Block_End:

                                    if (item.data.block.fallen == Constants::FallenCountMax) {
                                        this->addUndoMoveOnly();
                                        return PlayerCanMove::True;
                                    }

                                    break;
                               
                                case ItemType::Rotator_Start ... ItemType::Rotator_End:
                                    return PlayerCanMove::False;


                            }

                        }

                    }

                    break;

            }

            switch (mapElement) {

                case MapElement::Tile_Solid:
                case MapElement::Tile_UpperEdge:
                    {        

                        uint8_t itemIdx = this->getItemIdx_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y);

                        switch (direction) {

                            case Direction::North:
                                {

                                    // Handle blocks ..

                                    itemIdx = this->isItemAtWorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y);

                                    if (itemIdx != Constants::ItemNotFound) {

                                        Item &item = this->getItem(itemIdx);

                                        if (item.getItemType() >= ItemType::Block_Start && item.getItemType() <= ItemType::Block_End && item.data.block.fallen < Constants::FallenCountMax) {

                                            if (this->canBlockMove(itemIdx, Direction::North)) {

                                                this->addUndo(item, itemIdx);
                                                item.setY(item.getY() - 1);

                                                if (this->canBlockFall(item)) {
                                                    
                                                    item.data.block.startFall = true;

                                                }

                                                this->loadItemBitset();
                                                return PlayerCanMove::True;

                                            }
                                            else {

                                                return PlayerCanMove::False;

                                            }

                                        }

                                    }


                                    // Two spaces ahead ..

                                    itemIdx = this->getItemIdx_WorldCoords(this->getPlayer().getX_WorldCoords() + (x * 2), this->getPlayer().getY_WorldCoords() + (y * 2));

                                    if (itemIdx != Constants::ItemNotFound) {

                                        Item &item = this->getItem(itemIdx);

                                        switch (item.getItemType()) {

                                            case ItemType::Rotator_One:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {
                                                
                                                    case Direction::East:
                                                    case Direction::North:
                                                    case Direction::West:

                                                        if (this->isEmpty_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y) == Constants::ItemNotFound) {
  
                                                            this->addUndoMoveOnly();
                                                            return PlayerCanMove::True;

                                                        }
                                                        
                                                        break;

                                                    default: 
                                                        return PlayerCanMove::False;

                                                }

                                                break;
                                            
                                            case ItemType::Rotator_Two:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::North:
                                                    case Direction::West:

                                                        if (this->isEmpty_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y) == Constants::ItemNotFound) {
  
                                                            this->addUndoMoveOnly();
                                                            return PlayerCanMove::True;

                                                        }
                                                        
                                                        break;

                                                    default: 
                                                        return PlayerCanMove::False;

                                                }

                                                break;

                                            case ItemType::Rotator_Three:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::North:

                                                        if (this->isEmpty_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y) == Constants::ItemNotFound) {
  
                                                            this->addUndoMoveOnly();
                                                            return PlayerCanMove::True;

                                                        }
                                                        
                                                        break;

                                                    default: 
                                                        return PlayerCanMove::False;

                                                }

                                                break;

                                            case ItemType::Rotator_Five:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::East:
                                                    case Direction::West:

                                                        if (this->isEmpty_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y) == Constants::ItemNotFound) {
  
                                                            this->addUndoMoveOnly();
                                                            return PlayerCanMove::True;

                                                        }
                                                        
                                                        break;

                                                    default: 
                                                        return PlayerCanMove::False;

                                                }

                                                break;

                                            default: break;

                                        }

                                    }


                                    // Upper Left

                                    uint8_t itemIdx = this->getItemIdx_WorldCoords(this->getPlayer().getX_WorldCoords() + x - 1, this->getPlayer().getY_WorldCoords() + y);

                                    if (itemIdx != Constants::ItemNotFound) {

                                        Item &item = this->getItem(itemIdx);

                                        #if defined(DEBUG) && defined(DEBUG_MOVEMENTS_ROTATORS)
                                        DEBUG_PRINTLN(F("Rotator, North UL "));
                                        DEBUG_PRINT(F("Orig Direction "));
                                        DEBUG_PRINTLN(item.data.rotator.direction);
                                        #endif

                                        switch (item.getItemType()) {

                                            case ItemType::Rotator_One:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::East:
                                                       
                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }
        
                                                break;

                                            case ItemType::Rotator_Two:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::North:
                                                       
                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }
        
                                                        break;

                                                    case Direction::East:
                                                       
                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {
                                                    
                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True_TwoSpaces;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }
        
                                                        break;

                                                }
        
                                                break;

                                            case ItemType::Rotator_Three:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::North:
                                                       
                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }
        
                                                        break;

                                                    case Direction::East:
                                                    case Direction::South:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True_TwoSpaces;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }
        
                                                        break;

                                                }
    
                                                break;

                                            case ItemType::Rotator_Four:

                                                if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                    this->addUndo(item, itemIdx);
                                                    this->rotateAntiClockwise(item);
                                                    return PlayerCanMove::True_TwoSpaces;

                                                }
                                                else {

                                                    return PlayerCanMove::False;

                                                }

                                                break;

                                            case ItemType::Rotator_Five:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::East:
                                                    case Direction::West:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }
        
                                                        break;

                                                }

                                                break;

                                            default: break;

                                        }

                                    }

                                    // Upper Right

                                    itemIdx = this->getItemIdx_WorldCoords(this->getPlayer().getX_WorldCoords() + x + 1, this->getPlayer().getY_WorldCoords() + y);

                                    if (itemIdx != Constants::ItemNotFound) {

                                        Item &item = this->getItem(itemIdx);

                                        #if defined(DEBUG) && defined(DEBUG_MOVEMENTS_ROTATORS)
                                        DEBUG_PRINTLN(F("Rotator, North UR "));
                                        DEBUG_PRINT(F("Orig Direction "));
                                        DEBUG_PRINTLN(item.data.rotator.direction);
                                        #endif

                                        switch (item.getItemType()) {

                                            case ItemType::Rotator_One:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::West:
                                                     
                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True;
                                                            
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Two:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::West:
                                                     
                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {
                                                       
                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True;
                                                            
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                    case Direction::South:
                                                     
                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {
                                                       
                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True_TwoSpaces;
                                                            
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Three:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::North:
                                                     
                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {
                                                                                                                    
                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True;
                                                        
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                    case Direction::South:
                                                    case Direction::West:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True_TwoSpaces;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Four:

                                                if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                    this->addUndo(item, itemIdx);
                                                    this->rotateClockwise(item);
                                                    return PlayerCanMove::True_TwoSpaces;

                                                }
                                                else {

                                                    return PlayerCanMove::False;

                                                }

                                                break;

                                            case ItemType::Rotator_Five:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::East:
                                                    case Direction::West:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }

                                                break;

                                            default: break;

                                        }

                                    }

                                }

                                this->addUndoMoveOnly();
                                return PlayerCanMove::True;

                            case Direction::East:
                                {

                                    // Handle blocks ..

                                    itemIdx = this->isItemAtWorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y);

                                    if (itemIdx != Constants::ItemNotFound) {
                                        
                                        Item &item = this->getItem(itemIdx);

                                        if (item.getItemType() >= ItemType::Block_Start && item.getItemType() <= ItemType::Block_End && item.data.block.fallen < Constants::FallenCountMax) {

                                            if (this->canBlockMove(itemIdx, Direction::East)) {

                                                this->addUndo(item, itemIdx);
                                                item.setX(item.getX() + 1);

                                                if (this->canBlockFall(item)) {
                                                    
                                                    item.data.block.startFall = true;

                                                }

                                                this->loadItemBitset();
                                                return PlayerCanMove::True;

                                            }
                                            else {

                                                return PlayerCanMove::False;

                                            }

                                        }

                                    }


                                    // Two spaces ahead ..

                                    itemIdx = this->getItemIdx_WorldCoords(this->getPlayer().getX_WorldCoords() + (x * 2), this->getPlayer().getY_WorldCoords() + (y * 2));

                                    if (itemIdx != Constants::ItemNotFound) {

                                        Item &item = this->getItem(itemIdx);

                                        switch (item.getItemType()) {

                                            case ItemType::Rotator_One:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::North:
                                                    case Direction::East:
                                                    case Direction::South:
                                                        
                                                        if (this->isEmpty_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y) == Constants::ItemNotFound) {

                                                            this->addUndoMoveOnly();
                                                            return PlayerCanMove::True;

                                                        }
                                                        
                                                        break;                                                    

                                                    default: 
                                                        return PlayerCanMove::False;

                                                }

                                                break;

                                            case ItemType::Rotator_Two:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::North:
                                                    case Direction::East:
                                                        
                                                        if (this->isEmpty_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y) == Constants::ItemNotFound) {
  
                                                            this->addUndoMoveOnly();
                                                            return PlayerCanMove::True;

                                                        }
                                                        
                                                        break;

                                                    default: 
                                                        return PlayerCanMove::False;

                                                }

                                                break;

                                            case ItemType::Rotator_Three:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    // case Direction::North:
                                                    case Direction::East:
                                                    // case Direction::South:
                                                        
                                                        if (this->isEmpty_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y) == Constants::ItemNotFound) {
  
                                                            this->addUndoMoveOnly();
                                                            return PlayerCanMove::True;

                                                        }
                                                        
                                                        break;

                                                    default: 
                                                        return PlayerCanMove::False;

                                                }

                                                break;

                                            case ItemType::Rotator_Five:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::North:
                                                    case Direction::South:

                                                        if (this->isEmpty_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y) == Constants::ItemNotFound) {
  
                                                            this->addUndoMoveOnly();
                                                            return PlayerCanMove::True;

                                                        }
                                                        
                                                        break;

                                                    default: 
                                                        return PlayerCanMove::False;

                                                }

                                                break;

                                            default: break;

                                        }

                                    }


                                    // Upper Right

                                    uint8_t itemIdx = this->getItemIdx_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y - 1);

                                    if (itemIdx != Constants::ItemNotFound) {

                                        Item &item = this->getItem(itemIdx);

                                        #if defined(DEBUG) && defined(DEBUG_MOVEMENTS_ROTATORS)
                                        DEBUG_PRINTLN(F("Rotator, East UR "));
                                        DEBUG_PRINT(F("Orig Direction "));
                                        DEBUG_PRINTLN(item.data.rotator.direction);
                                        #endif

                                        switch (item.getItemType()) {

                                            case ItemType::Rotator_One:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::South:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Two:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::East:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                    case Direction::South:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True_TwoSpaces;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Three:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::East:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True;
                                                            
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                    case Direction::South:
                                                    case Direction::West:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True_TwoSpaces;
                                                            
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Four:

                                                if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                    this->addUndo(item, itemIdx);
                                                    this->rotateAntiClockwise(item);
                                                    return PlayerCanMove::True_TwoSpaces;

                                                }
                                                else {

                                                    return PlayerCanMove::False;

                                                }

                                                break;

                                            case ItemType::Rotator_Five:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::North:
                                                    case Direction::South:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True;
                                                            
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }

                                                break;

                                            default: break;

                                        }

                                    }

                                    // Lower Right

                                    itemIdx = this->getItemIdx_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y + 1);

                                    if (itemIdx != Constants::ItemNotFound) {

                                        Item &item = this->getItem(itemIdx);

                                        #if defined(DEBUG) && defined(DEBUG_MOVEMENTS_ROTATORS)
                                        DEBUG_PRINTLN(F("Rotator, East LR "));
                                        DEBUG_PRINT(F("Orig Direction "));
                                        DEBUG_PRINTLN(item.data.rotator.direction);
                                        #endif

                                        switch (item.getItemType()) {

                                            case ItemType::Rotator_One:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::North:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True;
                                                                
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }
        
                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Two:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::North:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True;
                                                                
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                    case Direction::West:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True_TwoSpaces;
                                                                
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Three:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::East:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True;
                                                                
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                    case Direction::North:
                                                    case Direction::West:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True_TwoSpaces;
                                                                
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Four:

                                                if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                    this->addUndo(item, itemIdx);
                                                    this->rotateClockwise(item);
                                                    return PlayerCanMove::True_TwoSpaces;

                                                }
                                                else {

                                                    return PlayerCanMove::False;

                                                }

                                                break;

                                            case ItemType::Rotator_Five:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::North:
                                                    case Direction::South:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True;
                                                            
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }

                                                break;

                                            default: break;

                                        }

                                    }

                                }

                                this->addUndoMoveOnly();
                                return PlayerCanMove::True;

                            case Direction::South:
                                {

                                    // Handle blocks ..

                                    itemIdx = this->isItemAtWorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y);

                                    if (itemIdx != Constants::ItemNotFound) {
                                        
                                        Item &item = this->getItem(itemIdx);

                                        if (item.getItemType() >= ItemType::Block_Start && item.getItemType() <= ItemType::Block_End && item.data.block.fallen < Constants::FallenCountMax) {

                                            if (this->canBlockMove(itemIdx, Direction::South)) {

                                                this->addUndo(item, itemIdx);
                                                item.setY(item.getY() + 1);

                                                if (this->canBlockFall(item)) {
                                                    
                                                    item.data.block.startFall = true;

                                                }

                                                this->loadItemBitset();
                                                return PlayerCanMove::True;

                                            }
                                            else {

                                                return PlayerCanMove::False;

                                            }

                                        }

                                    }


                                    // Two spaces ahead ..

                                    itemIdx = this->getItemIdx_WorldCoords(this->getPlayer().getX_WorldCoords() + (x * 2), this->getPlayer().getY_WorldCoords() + (y * 2));

                                    if (itemIdx != Constants::ItemNotFound) {

                                        Item &item = this->getItem(itemIdx);

                                        switch (item.getItemType()) {

                                            case ItemType::Rotator_One:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::East:
                                                    case Direction::South:
                                                    case Direction::West:

                                                        if (this->isEmpty_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y) == Constants::ItemNotFound) {

                                                            this->addUndoMoveOnly();
                                                            return PlayerCanMove::True;

                                                        }
                                                        
                                                        break;

                                                    default: 
                                                        return PlayerCanMove::False;

                                                }

                                                break;

                                            case ItemType::Rotator_Two:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::East:
                                                    case Direction::South:

                                                        if (this->isEmpty_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y) == Constants::ItemNotFound) {
  
                                                            this->addUndoMoveOnly();
                                                            return PlayerCanMove::True;

                                                        }
                                                        
                                                        break;

                                                    default: 
                                                        return PlayerCanMove::False;

                                                }

                                                break;

                                            case ItemType::Rotator_Three:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::South:

                                                        if (this->isEmpty_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y) == Constants::ItemNotFound) {
  
                                                            this->addUndoMoveOnly();
                                                            return PlayerCanMove::True;

                                                        }
                                                        
                                                        break;

                                                    default: 
                                                        return PlayerCanMove::False;

                                                }                                                

                                                break;

                                            case ItemType::Rotator_Five:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::East:
                                                    case Direction::West:
                                                        
                                                        if (this->isEmpty_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y) == Constants::ItemNotFound) {
  
                                                            this->addUndoMoveOnly();
                                                            return PlayerCanMove::True;

                                                        }
                                                        
                                                        break;

                                                    default: 
                                                        return PlayerCanMove::False;

                                                }                                                

                                                break;

                                            default: break;

                                        }

                                    }

                                    // Lower Left

                                    uint8_t itemIdx = this->getItemIdx_WorldCoords(this->getPlayer().getX_WorldCoords() + x - 1, this->getPlayer().getY_WorldCoords() + y);

                                    if (itemIdx != Constants::ItemNotFound) {

                                        Item &item = this->getItem(itemIdx);

                                        #if defined(DEBUG) && defined(DEBUG_MOVEMENTS_ROTATORS)
                                        DEBUG_PRINTLN(F("Rotator, South LL "));
                                        DEBUG_PRINT(F("Orig Direction "));
                                        DEBUG_PRINTLN(item.data.rotator.direction);
                                        #endif

                                        switch (item.getItemType()) {

                                            case ItemType::Rotator_One:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::East:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {
                                                       
                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True;
                                                                    
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Two:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::East:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True;
                                                                    
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                    case Direction::North:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True_TwoSpaces;
                                                                    
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Three:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::South:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True;
                                                                    
                                                        }
                                                        else {


                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                    case Direction::East:
                                                    case Direction::North:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True_TwoSpaces;
                                                                    
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Four:

                                                if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {
                                                    
                                                    this->addUndo(item, itemIdx);
                                                    this->rotateClockwise(item);
                                                    return PlayerCanMove::True_TwoSpaces;

                                                }
                                                else {

                                                    return PlayerCanMove::False;

                                                }

                                                break;

                                            case ItemType::Rotator_Five:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::East:
                                                    case Direction::West:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True;
                                                            
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }

                                                break;

                                            default: break;

                                        }

                                    }

                                    // Lower Right

                                    itemIdx = this->getItemIdx_WorldCoords(this->getPlayer().getX_WorldCoords() + x + 1, this->getPlayer().getY_WorldCoords() + y);

                                    if (itemIdx != Constants::ItemNotFound) {

                                        Item &item = this->getItem(itemIdx);

                                        #if defined(DEBUG) && defined(DEBUG_MOVEMENTS_ROTATORS)
                                        DEBUG_PRINTLN(F("Rotator, South LR"));
                                        DEBUG_PRINT(F("Orig Direction "));
                                        DEBUG_PRINTLN(item.data.rotator.direction);
                                        #endif

                                        switch (item.getItemType()) {

                                            case ItemType::Rotator_One:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::West:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True;
                                                                        
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;
                                                        
                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Two:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::South:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;
                                                        
                                                        }

                                                        break;

                                                    case Direction::West:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True_TwoSpaces;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;
                                                        
                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Three:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::South:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;
                                                        
                                                        }

                                                        break;

                                                    case Direction::North:
                                                    case Direction::West:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {
                                                            
                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True_TwoSpaces;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;
                                                        
                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Four:

                                                if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                    this->addUndo(item, itemIdx);
                                                    this->rotateAntiClockwise(item);
                                                    return PlayerCanMove::True_TwoSpaces;

                                                }
                                                else {

                                                    return PlayerCanMove::False;

                                                }

                                                break;

                                            case ItemType::Rotator_Five:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::East:
                                                    case Direction::West:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True;
                                                            
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }

                                                break;

                                            default: break;

                                        }

                                    }

                                }

                                this->addUndoMoveOnly();
                                return PlayerCanMove::True;

                            case Direction::West:
                                {

                                    // Handle blocks ..

                                    itemIdx = this->isItemAtWorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y);

                                    if (itemIdx != Constants::ItemNotFound) {

                                        Item &item = this->getItem(itemIdx);

                                        if (item.getItemType() >= ItemType::Block_Start && item.getItemType() <= ItemType::Block_End && item.data.block.fallen < Constants::FallenCountMax) {

                                            if (item.data.block.fallen < Constants::FallenCountMax) {

                                                if (this->canBlockMove(itemIdx, Direction::West)) {

                                                    this->addUndo(item, itemIdx);
                                                    item.setX(item.getX() - 1);

                                                    if (this->canBlockFall(item)) {
                                                        item.data.block.startFall = true;
                                                    }

                                                    this->loadItemBitset();
                                                    return PlayerCanMove::True;

                                                }
                                                else {

                                                    return PlayerCanMove::False;

                                                }

                                            }

                                        }

                                    }


                                    // Two spaces ahead ..

                                    itemIdx = this->getItemIdx_WorldCoords(this->getPlayer().getX_WorldCoords() + (x * 2), this->getPlayer().getY_WorldCoords() + (y * 2));

                                    if (itemIdx != Constants::ItemNotFound) {

                                        Item &item = this->getItem(itemIdx);

                                        switch (item.getItemType()) {

                                            case ItemType::Rotator_One:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::North:
                                                    case Direction::South:
                                                    case Direction::West:
                                                        
                                                        if (this->isEmpty_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y) == Constants::ItemNotFound) {
  
                                                            this->addUndoMoveOnly();
                                                            return PlayerCanMove::True;

                                                        }
                                                        
                                                        break;

                                                    default: 
                                                        return PlayerCanMove::False;

                                                }

                                                break;

                                            case ItemType::Rotator_Two:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::South:
                                                    case Direction::West:
                                                        
                                                        if (this->isEmpty_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y) == Constants::ItemNotFound) {
  
                                                            this->addUndoMoveOnly();
                                                            return PlayerCanMove::True;

                                                        }
                                                        
                                                        break;

                                                    default: 
                                                        return PlayerCanMove::False;

                                                }

                                                break;

                                            case ItemType::Rotator_Three:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::West:
                                                        
                                                        if (this->isEmpty_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y) == Constants::ItemNotFound) {
  
                                                            this->addUndoMoveOnly();
                                                            return PlayerCanMove::True;

                                                        }
                                                        
                                                        break;

                                                    default: 
                                                        return PlayerCanMove::False;

                                                }

                                                break;

                                            case ItemType::Rotator_Five:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {
                                                    case Direction::North:
                                                    case Direction::South:
                                                        
                                                        if (this->isEmpty_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y) == Constants::ItemNotFound) {
  
                                                            this->addUndoMoveOnly();
                                                            return PlayerCanMove::True;

                                                        }
                                                        
                                                        break;

                                                    default: 
                                                        return PlayerCanMove::False;
                                                
                                                }

                                                break;

                                            default: break;

                                        }

                                    }


                                    // Upper Left

                                    uint8_t itemIdx = this->getItemIdx_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y - 1);

                                    if (itemIdx != Constants::ItemNotFound) {

                                        Item &item = this->getItem(itemIdx);

                                        #if defined(DEBUG) && defined(DEBUG_MOVEMENTS_ROTATORS)
                                        DEBUG_PRINTLN(F("Rotator, West UL"));
                                        DEBUG_PRINT(F("Orig Direction "));
                                        DEBUG_PRINTLN(item.data.rotator.direction);
                                        #endif

                                        switch (item.getItemType()) {

                                            case ItemType::Rotator_One:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::South:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;
                                                        
                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Two:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::South:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;
                                                        
                                                        }

                                                        break;

                                                    case Direction::East:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True_TwoSpaces;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;
                                                        
                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Three:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::West:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;
                                                        
                                                        }

                                                        break;

                                                    case Direction::East:
                                                    case Direction::South:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True_TwoSpaces;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;
                                                        
                                                        }
        
                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Four:

                                                if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                    this->addUndo(item, itemIdx);
                                                    this->rotateClockwise(item);
                                                    return PlayerCanMove::True_TwoSpaces;

                                                }
                                                else {

                                                    return PlayerCanMove::False;

                                                }

                                                break;

                                            case ItemType::Rotator_Five:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::North:
                                                    case Direction::South:

                                                        if (this->canRotate(item, itemIdx, Rotation::Clockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateClockwise(item);
                                                            return PlayerCanMove::True;
                                                            
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }

                                                break;

                                            default: break;

                                        }

                                    }


                                    // Lower Left

                                    itemIdx = this->getItemIdx_WorldCoords(this->getPlayer().getX_WorldCoords() + x, this->getPlayer().getY_WorldCoords() + y + 1);

                                    if (itemIdx != Constants::ItemNotFound) {

                                        Item &item = this->getItem(itemIdx);

                                        #if defined(DEBUG) && defined(DEBUG_MOVEMENTS_ROTATORS)
                                        DEBUG_PRINTLN(F("Rotator, West LL"));
                                        DEBUG_PRINT(F("Orig Direction "));
                                        DEBUG_PRINTLN(item.data.rotator.direction);
                                        #endif

                                        switch (item.getItemType()) {

                                            case ItemType::Rotator_One:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::North:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;
                                                        
                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Two:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::West:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;
                                                        
                                                        }

                                                        break;

                                                    case Direction::North:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True_TwoSpaces;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;
                                                        
                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Three:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::West:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;
                                                        
                                                        }

                                                        break;

                                                    case Direction::East:
                                                    case Direction::North:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True_TwoSpaces;

                                                        }
                                                        else {

                                                            return PlayerCanMove::False;
                                                        
                                                        }

                                                        break;

                                                }

                                                break;

                                            case ItemType::Rotator_Four:

                                                if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                    this->addUndo(item, itemIdx);
                                                    this->rotateAntiClockwise(item);
                                                    return PlayerCanMove::True_TwoSpaces;

                                                }
                                                else {

                                                    return PlayerCanMove::False;

                                                }

                                                break;

                                            case ItemType::Rotator_Five:

                                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                                    case Direction::North:
                                                    case Direction::South:

                                                        if (this->canRotate(item, itemIdx, Rotation::AntiClockwise)) {

                                                            this->addUndo(item, itemIdx);
                                                            this->rotateAntiClockwise(item);
                                                            return PlayerCanMove::True;
                                                            
                                                        }
                                                        else {

                                                            return PlayerCanMove::False;

                                                        }

                                                        break;

                                                }

                                                break;

                                            default: break;

                                        }

                                    }

                                }

                                break;

                        }

                        this->addUndoMoveOnly();
                        return PlayerCanMove::True;

                    }
                    break;

            }

            return PlayerCanMove::False;

        }

};