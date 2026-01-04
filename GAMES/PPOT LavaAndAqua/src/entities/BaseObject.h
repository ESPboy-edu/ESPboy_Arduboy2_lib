#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"

class BaseObject {

    private:

        uint8_t xy = 0;
        uint8_t prevXY[Constants::Undo_Count];

    public:

        uint8_t getX() const                            { return (this->xy >> 4) & 0x0F; }
        uint8_t getY() const                            { return this->xy & 0x0F; }

        void setX(uint8_t x)                            { this->xy = ((x & 0x0F) << 4) | (this->xy & 0x0F); }
        void setY(uint8_t y)                            { this->xy = (xy & 0xF0) | (y & 0x0F); }

        void incX()                                     { this->setX((this->getX() + 1) & 0x0F); }
        void decX()                                     { this->setX((this->getX() - 1) & 0x0F); }
        void incY()                                     { this->setY((this->getY() + 1) & 0x0F); }
        void decY()                                     { this->setY((this->getY() - 1) & 0x0F); }

        void reset() {

            this->xy = 0;

            for (uint8_t i = 0; i < Constants::Undo_Count; i++) {
                this->prevXY[i] = 0;            
            }

        }

        void captureMove() {

            for (uint8_t i = 0; i < Constants::Undo_Count - 1; i++) {
                this->prevXY[i] = this->prevXY[i + 1];            
            }    

            this->prevXY[Constants::Undo_Count - 1] = this->xy;            

        }

        void revertMove() {

            this->xy = this->prevXY[Constants::Undo_Count - 1];            
     
            for (uint8_t i = Constants::Undo_Count - 1; i > 0; i--) {
                this->prevXY[i] = this->prevXY[i - 1];            
            }    

            this->prevXY[0] = 0;            
   
        }

        bool isActive() {

            return this->getY() > 0 && this->getY() < 11;
        
        }

};