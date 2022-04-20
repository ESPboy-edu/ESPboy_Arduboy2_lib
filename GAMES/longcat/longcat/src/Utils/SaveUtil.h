#pragma once

#include <stdint.h>
//#include <EEPROM.h>

struct Save
{
    uint16_t hash;
    uint8_t lastStage;
    uint8_t randomDifficulty;
};

namespace SaveUtil
{
    const int MemorySaveLocation = 100;
    uint16_t hash(Save value);
    Save get();
    bool tryGet(Save &value);
    void update(Save value);
};
