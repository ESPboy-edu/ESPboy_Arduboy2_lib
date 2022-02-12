#pragma once

#include "../utils/Arduboy2Ext.h"
#include "../utils/Constants.h"

class AstarokGame;

class InteractiveObject {

    public:
        int16_t             x, y;
        ObjectTypes         type;
        AstarokGame         * game;
        Arduboy2Ext         * arduboy;

        uint8_t collide(int16_t tX, int16_t tY);
        void deactivate();
        ObjectTypes getType();
};