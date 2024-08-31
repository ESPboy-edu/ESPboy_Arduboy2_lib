#pragma once

#include <Arduboy2.h>
#include "../utils/Enums.h"
#include "../../fxdata/fxdata.h"

class Player {

    private:

        Direction direction = Direction::East;
        PlayerCanMove lastMove = PlayerCanMove::False;

        uint8_t x = 2;
        uint8_t y = 8;

        uint8_t headBounce = 8;
        int8_t moving_X = 0;
        int8_t moving_Y = 0;
        uint8_t speed = 1;

    public:

        Direction getDirection()                        { return this->direction; }
        PlayerCanMove getLastMove()                     { return this->lastMove; }
        uint8_t getX_WorldCoords()                      { return this->x; }
        uint8_t getY_WorldCoords()                      { return this->y; }
        uint8_t getHeadBounce()                         { return this->headBounce; }
        uint8_t getSpeed()                              { return this->speed; }

        int8_t getMoving_X()                            { return this->moving_X; }
        int8_t getMoving_Y()                            { return this->moving_Y; }

        void setDirection(Direction val)                { this->direction = val; }
        void setLastMove(PlayerCanMove val)             { this->lastMove = val; }
        void setX_WorldCoords(uint8_t val)              { this->x = val; }
        void setY_WorldCoords(uint8_t val)              { this->y = val; }

        void setMoving_X(uint8_t val)                   { this->moving_X = val; }
        void setMoving_Y(uint8_t val)                   { this->moving_Y = val; }
        void setHeadBounce(uint8_t val)                 { this->headBounce = val; }
        void setSpeed(uint8_t val)                      { this->speed = val; }

        void incX(int8_t val) { 
        
            this->x = this->x + val; 
        
        }

        void incY(int8_t val) { 
        
            this->y = this->y + val; 
        
        }

};