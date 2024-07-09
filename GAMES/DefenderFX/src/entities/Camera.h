#pragma once

#include <Arduboy2.h>
#include "../utils/Enums.h"
#include "../../fxdata/fxdata.h"

class Camera {

    private:

        SQ15x16 x = 0;
        SQ15x16 vx = 0;
        
    public:

        SQ15x16 getX()                                  { return this->x; }        
        SQ15x16 getVelocityX()                          { return this->vx; }        

        void setX(SQ15x16 val)                          { this->x = val; }
        void setVelocityX(SQ15x16 val)                  { this->vx = val; }

    public:

        void incX(SQ15x16 val) {
        
            this->x = this->x + val;
        
        }

        void incVelocityX(SQ15x16 val) {
        
            this->vx = this->vx + val;
        
        }

        bool update(SQ15x16 playerVelocityIdxX) {
          return 1;   
        }
};