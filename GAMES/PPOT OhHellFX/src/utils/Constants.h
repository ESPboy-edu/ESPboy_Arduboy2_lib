#pragma once
#include "../../fxdata/fxdata.h"
#include "../../fxdata/images/Images.h"

#define DEBUG_PRINT    Serial.print
#define DEBUG_PRINTLN  Serial.println

#define _DEBUG
#define _DEBUG_CRASH
#define _DEBUG_RENDER_HAND
#define _DEBUG_FIX_TABLE_CARDS
#define _DEBUG_ACTIONS
#define _DEBUG_INCREASE_ROUNDS
#define _DEBUG_AI
#define _DEBUG_SHOW_PAYERS_CARDS
#define _DEBUG_FIXED_RAND
#define _DEBUG_SOUND

#define _TEST_CASE_00
#define _TEST_CASE_01
#define _TEST_CASE_02
#define _TEST_CASE_03

namespace Music {

    constexpr uint24_t Songs[] = { Music::Song_MainTheme };

    constexpr uint24_t SFXs[] =  { Music::SFX_Death, 
                                   Music::SFX_Victory,
                                   Music::SFX_XPGain,
                                   Music::SFX_Three_Inc,
                                   Music::SFX_Three_Dec,
                                   Music::SFX_PlayerBlip,
                                   Music::SFX_EnemyBlip,
                                   Music::SFX_Explosion,
                                   Music::SFX_DoubleBeep,
                                   Music::SFX_DoubleBeepDown,
                                   Music::SFX_Bet1,
                                   Music::SFX_Bet2,
                                   Music::SFX_Bet3,
                                   Music::SFX_Bet4,
                                   Music::SFX_Bet5,
                                   Music::SFX_Bet6,
                                   Music::SFX_Click,
                                   Music::SFX_SmallBlind,
                                   Music::SFX_BigBlind,
                                };

}




namespace Constants {

    const uint16_t DealCardCoords[] PROGMEM = {
      // x,  y,  noOfIncs  StartingRot, numberofRots
        44, 54,  24,       12,          27,  // 01
        48, 48,  24,       12,          27,  // 02
        52, 48,  24,       12,          27,  // 03
        56, 48,  24,       12,          27,  // 04
        60, 48,  24,       12,          27,  // 05
        64, 48,  24,       12,          27,  // 06
        68, 48,  24,       12,          27,  // 07
        72, 48,  24,       12,          27,  // 08
        76, 48,  24,       12,          27,  // 09
        80, 48,  24,       12,          27,  // 10
    };

    const int8_t Bounce[] PROGMEM = { -36, -30, -24, -16, -12, -6, 0, 4, 8, 12, 16, 20, 24, 28, 32, 28, 25, 22, 21, 21, 22, 25, 28, 32, 29, 27, 26, 26, 27, 28, 32, 30, 29, 30, 32, 31, 32 };

    constexpr uint8_t NoPlayer = 255;
    constexpr uint8_t NoCard = 255;
    constexpr uint8_t NoBid = 255;
    constexpr uint8_t NoWinner = 255;
    constexpr uint8_t PlayerCount = 4;
    constexpr uint8_t CardsInDeck = 52;
    constexpr uint8_t HumanPlayer = 1;

    constexpr uint8_t Delay = 12;
    constexpr uint8_t Title_Start_Action = 60;
    
};

struct DealPhysics {

    float x = 0;
    float y = 0;

    float xInc = 0;
    float yInc = 0;
    float x2 = 0;
    float y2 = 0;
    float rot = 0;
    float rotStart = 0;
    float rotInc = 0;
    int8_t noOfIncs = 0;

};