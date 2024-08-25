#pragma once

#include <Arduboy2.h>
#include "../utils/Enums.h"
#include "../../fxdata/fxdata.h"

class Score {

    private:

        uint8_t score = 1;
        PlayerMovement playerMovement = PlayerMovement::Up_Middle;
        int16_t x = 40;
        int16_t y = 47;
        bool active = false;
        uint8_t counter = 0;
        
    public:

        PlayerMovement getMovement()                    { return this->playerMovement; }
        uint8_t getScore()                              { return this->score; }
        uint8_t getCounter()                            { return this->counter; }
        int8_t getX_Screen()                            { return (this->x / 16) - 8; }        
        int8_t getY_Screen()                            { return (this->y / 16) - 2; }        
        int16_t getX()                                  { return this->x; }        
        int16_t getY()                                  { return this->y; }        
        bool isActive()                                 { return this->active; }        

        void setMovement(PlayerMovement val)            { this->playerMovement = val; }
        void setScore(uint8_t val)                      { this->score = val; }
        void setCounter(uint8_t val)                    { this->counter = val; }
        void setX(int16_t val)                          { this->x = val; }
        void setY(int16_t val)                          { this->y = val; }
        void setActive(bool val)                        { this->active = val; }

    public:

        bool update(){

            counter++;

            if (counter == 4 * Constants::ScoreFade) {
                
                active = false;
                return false;

            }

            return true;

        }
};