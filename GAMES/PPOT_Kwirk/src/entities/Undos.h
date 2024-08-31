#pragma once

#include <Arduboy2.h>
#include "Undo.h"
#include "../utils/Enums.h"
#include "../../fxdata/fxdata.h"


class Undos {

    public:

        Undo undo[Constants::UndoCount];

        uint8_t head = 0;
        uint8_t tail = 0;

        Undo &addEntry() {

            this->head++;

            if (this->head == Constants::UndoCount) {

                this->head = 0;

            }

            if (this->head == this->tail) {

                this->tail++;

                if (this->tail == Constants::UndoCount) {

                    this->tail = 0;

                }

            }

            return undo[this->head];

        }

        void removeHead() {
                            
            this->head--;
            
            if (this->head == 255) {
                this->head = Constants::UndoCount;
            }

        }
};
