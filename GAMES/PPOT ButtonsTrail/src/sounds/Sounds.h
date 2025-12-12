#pragma once

#include "../utils/Enums.h"

namespace Sounds {

    #ifdef SOUNDS
    const uint16_t Positive[] PROGMEM = { 220,50, 440,50, 880,50, TONES_END };
    const uint16_t Death[] PROGMEM = { 660,50, 440,50, 180,50, TONES_END };
    const uint16_t Tone_03[] PROGMEM = { 480, 50, TONES_END };
    const uint16_t Tone_02[] PROGMEM = { 380, 50, TONES_END };
    const uint16_t Tone_01[] PROGMEM = { 280, 50, TONES_END };
    #endif

}
