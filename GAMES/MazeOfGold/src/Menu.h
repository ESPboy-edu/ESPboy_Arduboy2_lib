#pragma once

#include "Constants.h"
#include "Enums.h"

class Menu {
    
    public:
    
        MenuDirection direction = MenuDirection::None;
        uint8_t x = 128;
        uint8_t y = 0;
        uint8_t top = 0;

        void reset() {

            this->x = 128;
            this->y = 0;
            this->top = 0;
            this->direction = MenuDirection::None;

        }

};