#pragma once
#include "../../fxdata/fxdata.h"
#include "../../fxdata/images/Tiles.h"

#define DEBUG_PRINT    Serial.print
#define DEBUG_PRINTLN  Serial.println

#define _DEBUG
#define _DEBUG_RENDER_TILES
#define _DEBUG_LEVEL_LOAD
#define _DEBUG_MOVEMENTS_ROTATORS
#define _DEBUG_CAN_ROTATOR
#define _DEBUG_IS_WALKABLE
#define _DEBUG_ITEM_BITMAP

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

    constexpr uint8_t CellWidth = 6;
    constexpr uint8_t CellHeight = 6;
    constexpr uint8_t MapTileWidth = 18;
    constexpr uint8_t MapTileHeight = 10;
    constexpr uint8_t EnemyCount = 10;
    constexpr uint8_t EnemyWidth = 4;
    constexpr int16_t Player_XMin = 0;
    constexpr int16_t Player_XMax = 1592;
    constexpr int16_t Player_YMin = 8;
    constexpr int16_t Player_YMax = 900;
    constexpr int16_t Player_Offset = 144;
    constexpr uint8_t Player_Width_Full = 5;
    constexpr uint8_t Player_Width = 3;
    constexpr uint8_t ParticlesMax = 25;
    constexpr uint8_t ScoreCount = 5;
    constexpr uint8_t ScoreFade = 24;


    constexpr uint8_t PlayerOffset_X[13] = { -11, -9, -6, -4, -1, 0, 0, 0, 0, 0, 0, 0, 0 };

    constexpr int8_t  Player_Movement_X[26] = { -18, -15, -12, -9, -6, -3, 0, 3, 6, 9, 12, 15, 18, -18, -15, -12, -9, -6, -3, 0, 3, 6, 9, 12, 15, 18 };
    constexpr int8_t  Player_Movement_Y[26] = { -18, -21, -24, -27, -30, -33, -36, -33, -30, -27, -24, -21, -18, 18, 21, 24, 27, 30, 33, 36, 33, 30, 27, 24, 21, 18 };
    constexpr uint8_t Player_AltDirection[26] = { 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13 };

    constexpr uint8_t Enemy_Movement_X[13] = { 2, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4, 3, 2 };
    constexpr int8_t  Enemy_Movement_Y[13] = { -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6 };
    constexpr uint8_t Enemy_AltDirection[13] = { 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

    constexpr uint8_t Bubble_Count = 4;
    constexpr uint8_t Bubble_Movement[3] = { 2, 3, 4 };
    constexpr uint8_t Bubble_X_Limit[3] = { 119, 115, 112 };
    constexpr uint8_t Bubble_Y_Limit[3] = { 54, 51, 47 };

};


namespace Images {

   const uint8_t * const Tiles[] = {
       Images::Tile_00_01,
       Images::Tile_00_02,
       Images::Tile_00_03,
       Images::Tile_00_04,
       Images::Tile_00_05,
       Images::Tile_00_06,
       Images::Tile_00_07,
       Images::Tile_00_08,
       Images::Tile_01_01,
       Images::Tile_01_02,
       Images::Tile_01_03,
       Images::Tile_01_04,
       Images::Tile_01_05,
       Images::Tile_01_06,
       Images::Tile_01_07,
       Images::Tile_01_08,
       Images::Tile_02_01,
       Images::Tile_02_02,
       Images::Tile_02_03,
       Images::Tile_02_04,
       Images::Tile_02_05,
       Images::Tile_02_06,
       Images::Tile_02_07,
       Images::Tile_02_08,
   };
    
};
