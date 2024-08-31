#pragma once

#include <Arduboy2.h>
#include "../utils/Enums.h"
#include "../../fxdata/fxdata.h"


class Undo {

    public:

        uint8_t playerIdx = 0;
        uint8_t playerX = 0;
        uint8_t playerY = 0;
       
        uint8_t itemIdx = 0;
        uint8_t itemX = 0;
        uint8_t itemY = 0;
        uint8_t dataElement;       // Rotator - direction, Block - fallen 
        
};
