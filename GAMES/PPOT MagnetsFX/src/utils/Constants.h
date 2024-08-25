#pragma once
#include "../../fxdata/fxdata.h"
#include "../../fxdata/images/Images.h"

#define _PARTICLES
#define _USE_LED
#define USE_LED_NOT_BRIGHT
#define DEBUG_PRINT    Serial.print
#define DEBUG_PRINTLN  Serial.println

#define _DEBUG
#define _DEBUG_FIXED_PUZZLE
#define _DEBUG_FIXED_RAND
#define _DEBUG_SOUND
#define _DEBUG_FORCE_EOG


namespace Constants {

    const int8_t Bounce[] PROGMEM = { 
        -36, -36, -36, -36, 
        -36, -36, -36, -36, 
        -36, -36, -36, -36, 
        -36, -36, 
        -36, -30, -24, -16, -12, -6, 0, 4, 8, 12, 16, 20, 24, 28, 32, 28, 25, 22, 21, 21, 22, 25, 28, 32, 29, 27, 26, 26, 27, 28, 32, 30, 29, 30, 32, 31, 32 };

    constexpr uint8_t Title_Start_Action = 40;
    constexpr uint8_t Fill_MaxRand = 22;
    constexpr uint8_t Fill_Orient_1 = 10;
    constexpr uint8_t Fill_Orient_2 = 20;
    constexpr uint8_t ParticlesMax = 35;
    constexpr uint8_t NoNumber = 255;
    constexpr uint8_t LED_Period = 4 * 8;
    
};
