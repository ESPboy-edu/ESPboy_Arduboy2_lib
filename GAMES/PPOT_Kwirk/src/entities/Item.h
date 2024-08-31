#pragma once

#include <Arduboy2.h>
#include "../utils/Enums.h"
#include "../../fxdata/fxdata.h"

#pragma pack(push, 1)

struct RawData {
    uint8_t dataElement1;
    uint8_t dataElement2;
    uint8_t dataElement3;
};

struct Rotator {
    uint8_t direction;
    uint8_t rotationDirection;
    uint8_t rotationDelay;
};

struct Block {
    uint8_t fallen;
    uint8_t startFall;
    uint8_t dataElement3;    
};

class Item {

    private:

        ItemType itemType = ItemType::None;

        uint8_t x = 0;
        uint8_t y = 0;

    public:

        union {
            struct RawData location;
            struct Rotator rotator;
            struct Block block;
        } data; 

    public:

        ItemType getItemType()                      { return this->itemType; }
        uint8_t getX()                              { return this->x; }
        uint8_t getY()                              { return this->y; }

        void setItemType(ItemType val)              { this->itemType = val; }
        void setX(uint8_t val)                      { this->x = val; }
        void setY(uint8_t val)                      { this->y = val; }

        void incItemType() {

            this->itemType = static_cast<ItemType>(static_cast<uint8_t>(this->itemType) + 1);
            
        }

};

#pragma pack(pop)