#pragma once

#include <Arduboy2.h>
#include "../utils/Enums.h"
#include "../utils/Enums.h"
#include "../../fxdata/fxdata.h"

class Player {

    private:

        PlayerMode playerMode = PlayerMode::Rotate;
        PlayerMovement playerMovement = PlayerMovement::Up_Middle;

        uint8_t direction = 6;
        int8_t directionMovement = 1;
        int16_t x = 40 * 16;
        int16_t y = Constants::Player_YMax - Constants::Player_Offset;
        

    public:

        PlayerMovement getMovement()                    { return this->playerMovement; }
        PlayerMode getPlayerMode()                      { return this->playerMode; }
        uint8_t getDirection()                          { return this->direction; }
        int8_t getDirectionMovement()                   { return this->directionMovement; }
        int8_t getX_Screen()                            { return this->x / 16; }        
        int8_t getY_Screen()                            { return this->y / 16; }        
        int16_t getX()                                  { return this->x; }        
        int16_t getY()                                  { return this->y; }        

        void setMovement(PlayerMovement val)            { this->playerMovement = val; }
        void setDirectionMovement(int8_t val)           { this->directionMovement = val; }
        void setPlayerMode(PlayerMode val)              { this->playerMode = val; }
        void setDirection(uint8_t val)                  { this->direction = val; }
        void setX(int16_t val)                          { this->x = val; }
        void setY(int16_t val)                          { this->y = val; }

    public:

        void shoot() {

            this->playerMode = PlayerMode::Moving;

            this->playerMovement = static_cast<PlayerMovement>(this->direction + (this->y <= 8 ? 13 : 0));

            if (static_cast<int16_t>(this->y) >= static_cast<int16_t>(Constants::Player_YMax - Constants::Player_Offset)) {
                this->y = this->y + Constants::Player_Offset;
            }

        }

};