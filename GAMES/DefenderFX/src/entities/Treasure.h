#pragma once

#include <Arduboy2.h>
#include "../utils/Enums.h"
#include "../../fxdata/fxdata.h"

class Treasure {

    private:

        SQ15x16 x = 40;
        SQ15x16 y = 47;
        bool active = false;
        
    public:

        SQ15x16 getX()                                    { return this->x; }        
        SQ15x16 getY()                                    { return this->y; }        
        bool isActive()                                 { return this->active; }        

        void setX(SQ15x16 val)                            { this->x = val; }
        void setY(SQ15x16 val)                            { this->y = val; }
        void setActive(bool val)                        { this->active = val; }

    public:

        void incX(SQ15x16 inc) {

            this->x = this->x + inc;

        }
};