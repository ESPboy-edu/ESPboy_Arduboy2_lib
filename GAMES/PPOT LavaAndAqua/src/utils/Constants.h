#pragma once
#include "../../fxdata/fxdata.h"
#include "../../fxdata/images/Images.h"

#define _DEBUG_PRINT    Serial.print
#define _DEBUG_PRINTLN  Serial.println
#define DEBUG_BREAK    asm volatile("break\n");

namespace Constants {

    constexpr int8_t xPos[] = { -125, -112, -101, -90, -80, -70, -61, -52, -44, -36, -29, -22, -16, -10, -5, 0, 4, 8, 12, 15, 18, 20, 22, 23, 24, 24, 25, 25, 25, };

    constexpr uint8_t levelSelect_Offset[] = { 0, 0, 14, 31, 43, 43 };

    constexpr uint8_t XOffset_Pixels = 6;
    constexpr uint8_t YOffset_Pixels = 6;
    constexpr uint8_t Undo_Count = 5;

    constexpr uint8_t Map_X_Count = 16;
    constexpr uint8_t Map_Y_Count = 11;
    constexpr uint8_t Level_Count = 24;

    constexpr uint8_t Tile_Player = 234;
    constexpr uint8_t Tile_Lava = 250;
    constexpr uint8_t Tile_Water = 239;
    constexpr uint8_t Tile_Basalt = 240;
    constexpr uint8_t Tile_Portal = 241;
    constexpr uint8_t Tile_Portal_Inactive = 242;
    constexpr uint8_t Tile_Portal_Key = 243;
    constexpr uint8_t Tile_Block = 251;
    constexpr uint8_t Tile_Counter_00 = 100;
    constexpr uint8_t Tile_Counter_65 = 165;
    constexpr uint8_t Tile_Border = 255;
    constexpr uint8_t Tile_Partial_Wall = 252;
    constexpr uint8_t Tile_Lava_And_Partial_Wall = 253;
    constexpr uint8_t Tile_Water_And_Partial_Wall = 254;
    constexpr uint8_t Tile_Waters_Edge = 238;
    constexpr uint8_t Tile_Green_Switch = 235;
    constexpr uint8_t Tile_Green_Closed = 236;
    constexpr uint8_t Tile_Green_Open = 237;

    constexpr uint8_t Block_Count = 18;
    constexpr uint8_t Portal_Key_Count = 6;
    constexpr uint8_t Green_Door_Count = 4;

    constexpr uint8_t Image_Portal = 0;
    constexpr uint8_t Image_Portal_Inactive = 8;
    constexpr uint8_t Image_Partial_Wall = 9;
    constexpr uint8_t Image_Lava_And_Partial_Wall = 10;
    constexpr uint8_t Image_Water_And_Partial_Wall = 16;
    constexpr uint8_t Image_Block = 15;
    constexpr uint8_t Image_Player = 14;
    constexpr uint8_t Image_Player_Dead = 25;
    constexpr uint8_t Image_Basalt = 20;
    constexpr uint8_t Image_Waters_Edge = 21;
    constexpr uint8_t Image_Green_Switch = 22;
    constexpr uint8_t Image_Green_Closed = 23;
    constexpr uint8_t Image_Green_Open = 24;

    constexpr uint8_t Temp_Water = 99;
    constexpr uint8_t Temp_Water_And_Partial_Wall = 98;
    constexpr uint8_t Temp_Lava = 97;
    constexpr uint8_t Temp_Lava_And_Partial_Wall = 96;
    
};
