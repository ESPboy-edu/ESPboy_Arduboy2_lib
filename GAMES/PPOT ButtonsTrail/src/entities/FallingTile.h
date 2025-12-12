#pragma once

#include "../utils/Utils.h"


class FallingTile { 

    private:

        uint8_t x;
        uint8_t y;
        bool active = false;
        uint8_t yOffset = 0;

    public:

        uint8_t getX()                          { return this->x; }
        uint8_t getY()                          { return this->y; }
        uint8_t getYOffset()                    { return this->yOffset; }
        bool isActive()                         { return this->active; }

        void setX(uint8_t val)                  { this->x = val; }
        void setY(uint8_t val)                  { this->y = val; }
        void getYOffset(uint8_t val)            { this->yOffset = val; }
        void setActive(bool val)                { this->active = val; }

    public:

        void init(uint8_t x, uint8_t y) {
            
            this->x = x;
            this->y = y;
            this->yOffset = 0;
            this->active = true;

        }

        void update() {

            if (this->active) {

                this->yOffset = yOffset + 2;

                if (this->yOffset > 64) {

                    this->active = false;

                }

            }

        }

};