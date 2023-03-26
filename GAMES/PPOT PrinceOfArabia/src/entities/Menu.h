#pragma once

#include <Arduboy2.h>   
#include "../utils/Constants.h"

struct MenuItem {

    Direction direction = Direction::None;
    uint8_t x;
    uint8_t cursor;

    void init() {

        this->x = 130;
        this->direction = Direction::None;

    }

    bool update() {
            
        switch (this->direction) {

            case Direction::Left:
                
                this->x = this->x - 4;
              
                if (this->x == 86) {
                    this->direction = Direction::None;
                }
                
                return false;

            case Direction::Right:

                this->x = this->x + 4;

                if (this->x == 130) {

                    this->direction = Direction::None;
                    return true;
                }

                return false;

            default: return false;

        }

    }
 
};
