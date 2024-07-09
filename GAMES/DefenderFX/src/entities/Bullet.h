#pragma once

#include <Arduboy2.h>
#include "../utils/Enums.h"
#include "../../fxdata/fxdata.h"

class Bullet {

    private:

        Direction direction = Direction::Right;
        SQ15x16 x = 40;
        SQ15x16 y = 47;
        bool active = false;
        
    public:

        Direction getDirection()                        { return this->direction; }
        SQ15x16 getX()                                    { return this->x; }        
        SQ15x16 getY()                                    { return this->y; }        
        bool isActive()                                 { return this->active; }        

        void setDirection(Direction val)                { this->direction = val; }
        void setX(SQ15x16 val)                            { this->x = val; }
        void setY(SQ15x16 val)                            { this->y = val; }
        void setActive(bool val)                        { this->active = val; }

    public:
        
        void incX(SQ15x16 val) {
        
            this->x = this->x + val;
        
        }

        bool update(int16_t playerX) {

            switch (this->direction) {

                case Direction::Right:
                    this->x = this->x + 4;
                    break;

                case Direction::Left:
                    this->x = this->x - 4;
                    break;

            }

            int16_t xDiff = abs(playerX - this->x.getInteger());

            if (xDiff > 128) { 
                this->active = false; 
            }

            return true;

        }


        Rect getRect() {

            Rect rect;

            rect.x = this->getX().getInteger();
            rect.y = this->getY().getInteger();
            rect.width = 8;
            rect.height = 1;

            return rect;

        }

        #ifdef DEBUG

            void printRect() {

                Rect r = this->getRect();

                DEBUG_PRINT("Player ");
                DEBUG_PRINT(r.x);
                DEBUG_PRINT(",");
                DEBUG_PRINT(r.y);
                DEBUG_PRINT(",");
                DEBUG_PRINT(r.width);
                DEBUG_PRINT(",");
                DEBUG_PRINT(r.height);
            
            }

        #endif        
};