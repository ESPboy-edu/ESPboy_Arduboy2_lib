#pragma once
#include "../../fxdata/fxdata.h"
#include "../../fxdata/images/Images.h"

#define DEBUG_PRINT    Serial.print
#define DEBUG_PRINTLN  Serial.println
#define DEBUG_BREAK    asm volatile("break\n");

#define _DEBUG
#define _DEBUG_FALLS


namespace Constants {

	constexpr uint8_t StackSize_Player = 20;
	constexpr uint8_t StackSize_Enemy = 20;
	
	constexpr uint8_t Item_Count = 23;
	constexpr uint8_t Enemy_Count = 8;
	constexpr uint16_t Health_Counter_Max = 640;
	constexpr uint16_t Time_Counter_Max = 320;

	constexpr uint8_t Tile_Count = 94;
	constexpr uint8_t World_Repeat = 2;
	
	
//	constexpr uint8_t Puff_Max = 7 * 16;
	constexpr uint8_t Puff_Max = 7 * 8;


	constexpr uint8_t Level_30 = 14;
	constexpr uint8_t Level_25 = 22;
	constexpr uint8_t Level_20 = 30;
	constexpr uint8_t Level_15 = 38;
	constexpr uint8_t Level_10 = 46;

}

namespace Level {

    uint8_t Level[] = { 

65,38,37,29,22,10,24,37,30,25,39,35,4,65,66,26,38,39,24,43,23,41,50,41,38,37,21,34,23,63,
0,10,20,25,23,36,38,27, 40, 28,39,4,3,35,20,43,23,26,41,47,38,37,39,24,44,23,64,38,40,34,23,26,63,
65,38,37,21,23,38,43,39,10,7,4,65,26,38,34,29,23,51,44,27,39,35,4,60,36,46,38,43,39,35,2,


    };

//     uint8_t Level[Constants::Tile_Count] = { 

// 65,38,37,29,22,10,24,37,30,25,39,35,4,65,66,26,38,39,24,43,23,41,61,41,38,37,21,34,23,63,



//     };
};