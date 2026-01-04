#pragma once

#include <Arduboy2.h>
#include "BaseObject.h"

class BaseOpen : public BaseObject {

    private:

        bool open = false;
        uint8_t prevOpen[Constants::Undo_Count];

    public:

        bool isOpen()                                  { return this->open; }
    
        void setOpen(bool val)                         { this->open = val; }
   
        void captureMove() {

            BaseObject::captureMove();

            for (uint8_t i = 0; i < Constants::Undo_Count - 1; i++) {
                this->prevOpen[i] = this->prevOpen[i + 1];            
            }    

            this->prevOpen[Constants::Undo_Count - 1] = this->open;            

        }

        void revertMove() {

            BaseObject::revertMove();

            this->open = this->prevOpen[Constants::Undo_Count - 1];            
     
            for (uint8_t i = Constants::Undo_Count - 1; i > 0; i--) {
                this->prevOpen[i] = this->prevOpen[i - 1];            
            }    

            this->prevOpen[0] = 0;            
   
        }
};