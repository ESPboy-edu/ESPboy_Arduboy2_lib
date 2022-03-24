#pragma once

#include <stdint.h>
#include "../Direction/Direction.h"
#include "../Map/TileTypes.h"
#include "../Map/Map.h"
#include "../Map/maps.h"

class PackedConverter
{
private:
    PackedConverter(){};
    // Random mapObject generation
    static constexpr uint16_t depthMask = 0xff;
    static constexpr uint8_t depthShift = 0;

    static constexpr uint16_t directionMask = 0x7;
    static constexpr uint16_t directionShift = 8;

    static constexpr uint16_t usedMask = 0x1;
    static constexpr uint16_t usedShift = 11;

public:
    static const uint16_t toPackedValue(uint8_t Depth, Direction direction, bool used)
    {
        uint16_t value = ((static_cast<uint16_t>(Depth & depthMask) << depthShift));
        value |= (((static_cast<uint16_t>(direction) & directionMask) << directionShift));
        value |= (((static_cast<uint16_t>(used) & usedMask) << usedShift));
        return value;
    }

    static const uint8_t depthFromPacked(uint16_t packed)
    {
        return (static_cast<uint8_t>(packed) >> depthShift) & depthMask;
    }

    static const Direction directionFromPacked(uint16_t packed)
    {
        return static_cast<Direction>((packed >> directionShift) & directionMask);
    }

    static const bool usedFromPacked(uint16_t packed)
    {
        return static_cast<bool>((packed >> usedShift) & usedMask);
    }
};