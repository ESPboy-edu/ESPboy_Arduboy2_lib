#include <Arduboy2.h>
#include "SaveUtil.h"

uint16_t SaveUtil::hash(Save value)
{
    uint16_t runner = static_cast<uint16_t>(value.lastStage);
    runner ^= static_cast<uint16_t>(value.randomDifficulty);
    return (runner);
}

Save SaveUtil::get()
{
    Save returnValue;
    EEPROM.get(MemorySaveLocation, returnValue);
    return returnValue;
}

bool SaveUtil::tryGet(Save &value)
{
    EEPROM.get(MemorySaveLocation, value);
    if (value.hash != hash(value))
        return false;
    return true;
}

void SaveUtil::update(Save value)
{
    value.hash = hash(value);
    EEPROM.put(MemorySaveLocation, value);
    EEPROM.commit();
}
