#pragma once

#include "../utils/Enums.h"

#define FLAG_FOLLOW_AI  0b00000000
#define FLAG_IGNORE_AI  0b00000001

#define AI_NONE         0b00000000
#define AI_SEEK         0b00000100
#define AI_DETECT_JUMP  0b00001000
#define AI_DETECT_WALL  0b00000001
#define AI_DETECT_GAP   0b00000010

namespace Data {

    PROGMEM const uint8_t Astarok[] {
         ObjectTypes::Player, 0, 0, 
         AI_NONE,
         AI_NONE,
         AI_NONE,
    };

    PROGMEM const uint8_t Spider[] {
        ObjectTypes::Spider, 
        FLAG_FOLLOW_AI,       // Flags
        1, 
        AI_DETECT_WALL | AI_DETECT_GAP,
        AI_SEEK | AI_DETECT_WALL | AI_DETECT_GAP,
        AI_SEEK | AI_DETECT_WALL | AI_DETECT_GAP,
    };

    PROGMEM const uint8_t Starman[] {
        ObjectTypes::Starman, 
        FLAG_FOLLOW_AI,       // Flags
        2, 
        AI_SEEK | AI_DETECT_JUMP | AI_DETECT_WALL | AI_DETECT_GAP,
        AI_SEEK | AI_DETECT_JUMP | AI_DETECT_WALL | AI_DETECT_GAP,
        AI_SEEK | AI_DETECT_JUMP | AI_DETECT_WALL | AI_DETECT_GAP,
    };

    PROGMEM const uint8_t Skull[] {
        ObjectTypes::Skull, 
        FLAG_FOLLOW_AI,       // Flags
        2, 
        AI_DETECT_WALL,   
        AI_SEEK | AI_DETECT_WALL,
        AI_SEEK | AI_DETECT_JUMP | AI_DETECT_WALL,
    };

    PROGMEM const uint8_t Health[] {
        ObjectTypes::Health, 
        FLAG_FOLLOW_AI,
        1, 
        AI_NONE,
        AI_NONE,
        AI_NONE,
    };

    PROGMEM const uint8_t Bolt[] {
        ObjectTypes::Bolt, 
        FLAG_IGNORE_AI,
        2, 
        AI_NONE,
        AI_NONE,
        AI_NONE,
    };

    PROGMEM const uint8_t Fireball[] {
        ObjectTypes::Fireball, 
        FLAG_FOLLOW_AI,
        0, 
        AI_NONE,
        AI_NONE,
        AI_NONE,
    };

    PROGMEM const uint8_t Coin[] {
        ObjectTypes::Coin, 
        FLAG_FOLLOW_AI,
        0, 
        AI_NONE,
        AI_NONE,
        AI_NONE,
    };

};