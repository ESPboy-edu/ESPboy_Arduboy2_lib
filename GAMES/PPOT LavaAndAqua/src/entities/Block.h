#pragma once

#include <Arduboy2.h>
#include "BaseObject.h"

class Block : public BaseObject {

    private:

        uint8_t xy_Next = 255;

    public:

        uint8_t getX_Next() const                       { return (this->xy_Next >> 4) & 0x0F; }
        uint8_t getY_Next() const                       { return this->xy_Next & 0x0F; }

        void setX_Next(uint8_t x)                       { this->xy_Next = ((x & 0x0F) << 4) | (this->xy_Next & 0x0F); }
        void setY_Next(uint8_t y)                       { this->xy_Next = (this->xy_Next & 0xF0) | (y & 0x0F); }

        void updateNext() {

            if (xy_Next != 255) {
                this->setX(this->getX_Next());
                this->setY(this->getY_Next());
                this->setX_Next(15);
                this->setY_Next(15);
            }
        
        }

};