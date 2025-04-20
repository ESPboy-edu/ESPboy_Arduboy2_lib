#pragma once

#include <ArduboyFX.h>  
#include "../utils/Constants.h"

struct Cookie {

    SoundSettings soundSettings;

    uint8_t highScore_Chars[5][3];
    uint16_t highScore_Values[5];

};
