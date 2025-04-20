
#pragma once

#include <Arduboy2.h>
#include "../utils/Enums.h"
#include "../utils/Enums.h"
#include "../../fxdata/fxdata.h"

struct SoundSettings {

    bool sounds = true;
    uint8_t volume = 4;

    bool getSounds()                { return this->sounds; }
    uint8_t getVolume()             { return this->volume; }

    void setSounds(bool val)        { this->sounds = val; }
    void setVolume(uint8_t val)     { this->volume = val; }

};
