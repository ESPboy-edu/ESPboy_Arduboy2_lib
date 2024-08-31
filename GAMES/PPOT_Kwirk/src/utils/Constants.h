#pragma once
#include "../../fxdata/fxdata.h"
#include "../../fxdata/images/Tiles.h"

#define SOUNDS_SYNTHU
#define START_LEVEL 0

#define DEBUG_PRINT    Serial.print
#define DEBUG_PRINTLN  Serial.println


// Free memory
#define _DEBUG
#define _DEBUG_REMOVE_MENU
#define _DEBUG_REMOVE_UNDO
#define _DEBUG_RENDER_TILES
#define _DEBUG_NO_ROTATION

// Print commands
#define _DEBUG_LEVEL_LOAD
#define _DEBUG_MOVEMENTS_ROTATORS
#define _DEBUG_CAN_ROTATOR
#define _DEBUG_IS_WALKABLE
#define _DEBUG_ITEM_BITSET

namespace Music {

    constexpr uint24_t Songs[] = { Music::Song_MainTheme };

    constexpr uint24_t SFXs[] =  { Music::SFX_Death, 
                                   Music::SFX_Victory,
                                   Music::SFX_XPGain,
                                   Music::SFX_Three_Inc,
                                   Music::SFX_Three_Dec  };

}

struct Point_U8 {
    uint8_t x;
    uint8_t y;
};

struct Point_S8 {
    int8_t x;
    int8_t y;
};

namespace Constants {

    constexpr uint8_t LevelLeadingMemOffset = 18;
    constexpr uint8_t LevelMapWidth = 18;
    constexpr uint8_t LevelMapHeight = 11;

    constexpr uint8_t MapTileWidth = 8;
    constexpr uint8_t MapTileHeight = 8;
    constexpr uint8_t MapXPosition = 56;
    constexpr uint8_t MapYPosition = 24;

    constexpr uint8_t ItemCount = 40;
    constexpr uint8_t ItemNotFound = 255;
    constexpr uint8_t ItemFound = 128;

    constexpr uint8_t RotationDelay = 4;

    constexpr uint8_t Block_Widths[15] PROGMEM =  { 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 4, 5, 3, 3, 1 };
    constexpr uint8_t Block_Heights[15] PROGMEM = { 1, 2, 3, 4, 5, 6, 1, 2, 3, 1, 1, 1, 2, 3, 11, };

    constexpr uint8_t Menu_Left = 128 - 40;
    constexpr uint8_t Menu_Right = 128;

    constexpr uint8_t FallenCountMax = 3;
    constexpr uint8_t PlayerCount = 4;
    constexpr uint8_t UndoCount = 30;

    constexpr uint8_t Level_Tick_Start[10] = { 0, 0, 1, 2, 3, 4, 5, 6, 6, 6 };


    constexpr int8_t Rotator_1_X[8] PROGMEM = { 0, 0, 0, 0, 0, -8, -8, -8 };
    constexpr int8_t Rotator_1_Y[8] PROGMEM = { -8, -8, 0, 0, 0, 0, 0, -8 };

    constexpr int8_t Rotator_2_X[8] PROGMEM = { 0, 0, 0, -8, -8, -8, -8, -8 };
    constexpr int8_t Rotator_2_Y[8] PROGMEM = { -8, -8, 0, 0, 0, -8, -8, -8 };

    constexpr int8_t Rotator_3_X[8] PROGMEM = { -8, -8, 0, -8, -8, -8, -8, -8 };
    constexpr int8_t Rotator_3_Y[8] PROGMEM = { -8, -8, -8, -8, 0, -8, -8, -8 };

    constexpr int8_t Rotator_5_X[8] PROGMEM = { 0, -8, -8, -8, 0, -8, -8, -8 };
    constexpr int8_t Rotator_5_Y[8] PROGMEM = { -8, -8, 0, -8, -8, -8, 0, -8 };

};

