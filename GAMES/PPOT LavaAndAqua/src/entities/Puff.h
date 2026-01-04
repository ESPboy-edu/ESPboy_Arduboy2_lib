#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"

struct Puff : public BaseObject {

    private:

        uint8_t counter;

    public:

        uint8_t getCounter()                { return this->counter; }

        void setCounter(uint8_t val)        { this->counter = val; }

};