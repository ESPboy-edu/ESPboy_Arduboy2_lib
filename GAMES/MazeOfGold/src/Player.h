#pragma once

#include "Constants.h"
#include "Enums.h"

class Player {

    public:
        
        int16_t x = 1 * Constants::TileSize;
        int16_t y = 1 * Constants::TileSize;
        int8_t vx = 0;
        int8_t vy = 0;

        uint8_t dir;
        uint8_t inventoryCount;

        ItemType inventory[Constants::InventoryCount];
        bool dead = false;
        bool holdingGun = false;
        uint8_t bulletCount = 0;
        uint8_t usingCandle = 0;

        Player() { }

        bool addItem(ItemType item) {
        
            if (this->inventoryCount == Constants::InventoryCount) return false;

            this->inventory[this->inventoryCount] = item;
            this->inventoryCount++;
            return true;

        }

        bool hasItem(ItemType item) {
        
            if (this->inventoryCount == Constants::InventoryCount) return false;

            for (uint8_t i = 0; i < Constants::InventoryCount; i++) {
            
                if (this->inventory[i] == item) {

                    return true;

                }

            }

            return false;

        }

        bool removeItem(uint8_t idx) {
        
            for (uint8_t i = idx + 1; i < Constants::InventoryCount; i++) {
            
                this->inventory[i - 1] = this->inventory[i];

            }

            this->inventory[9] = ItemType::None;
            this->inventoryCount--;

            return 0;
        }

        uint8_t getItemIdx(ItemType itemType) {
         
            for (uint8_t i = 0; i < Constants::InventoryCount; i++) {
            
                if (this->inventory[i] == itemType) {

                    return i;

                }

            }

            return Constants::NoItem;

        }

        void reset(bool clearInventory, bool addMap) {

            this->x = 1 * Constants::TileSize;
            this->y = 1 * Constants::TileSize;
            this->vx = 0;
            this->vy = 0;
            this->dir = 2;

            if (clearInventory) {
                this->inventoryCount = 0;
                this->bulletCount = 0;
            }

            if (addMap) {
                if (this->inventoryCount == 0) {
                    this->addItem(ItemType::Map);
                }
            }

            this->dead = false;
            this->holdingGun = false;

            #ifdef DEBUG
                this->addItem(ItemType::Gun);
                this->addItem(ItemType::Bullets);
                this->bulletCount = 2;
            #endif

        }

        uint8_t getInventoryCount()                     { return this->inventoryCount; }
        ItemType &getInventoryItem(uint8_t idx)         { return this->inventory[idx]; }
        bool isDead()                                   { return this->dead; }
        bool isHoldingGun()                             { return this->holdingGun; }
        bool isUsingCandle()                            { return this->usingCandle > 0; }
        uint8_t getBulletCount()                        { return this->bulletCount; }
        uint8_t getCandleCount()                        { return this->usingCandle; }

        void setDead(bool val)                          { this->dead = val; }
        void setHoldingGun(bool val)                    { this->holdingGun = val; }
        void setBulletCount(uint8_t val)                { this->bulletCount = val; }
        void setCandleCount(uint8_t val)                { this->usingCandle = val; }

};