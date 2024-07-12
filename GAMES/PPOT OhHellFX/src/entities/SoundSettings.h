
#pragma once

#include <Arduboy2.h>
#include "../utils/Enums.h"
#include "../utils/Enums.h"
#include "../../fxdata/fxdata.h"


struct SoundSettings {

    bool music = true;
    bool sfx = true;
    uint8_t volume = 7;

    bool getMusic()                 { return this->music; }
    bool getSFX()                   { return this->sfx; }
    uint8_t getVolume()             { return this->volume; }

    void setMusic(bool val)         { this->music = val; }
    void setSFX(bool val)           { this->sfx = val; }
    void setVolume(uint8_t val)     { this->volume = val; }

};
