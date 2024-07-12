#pragma once
#include "../../fxdata/fxdata.h"
#include "../../fxdata/images/Images.h"

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
                                   Music::SFX_Three_Dec,
                                   Music::SFX_PlayerBlip,
                                   Music::SFX_EnemyBlip,
                                   Music::SFX_Explosion,
                                   Music::SFX_DoubleBeep,
                                };

}

struct Point_U8 {
    uint8_t x;
    uint8_t y;
};

struct Point_S8 {
    int8_t x;
    int8_t y;
};


// constexpr SQ15x16 CAMERA_OFFSET_X = 64;
constexpr SQ15x16 CAMERA_OFFSET_X = 58;
constexpr SQ15x16 CAMERA_OFFSET_X_DELTA = 8;
constexpr SQ15x16 CAMERA_OFFSET_X_VEL_DELTA = 100;

constexpr SQ15x16 CAMERA_UPDATE_X_OMEGA = 30;
constexpr SQ15x16 CAMERA_UPDATE_X_ZETA = 1;
constexpr SQ15x16 CAMERA_UPDATE_X_ALPHA = CAMERA_UPDATE_X_OMEGA * CAMERA_UPDATE_X_OMEGA;
constexpr SQ15x16 CAMERA_UPDATE_X_BETA = CAMERA_UPDATE_X_OMEGA * CAMERA_UPDATE_X_ZETA * 2;
constexpr SQ15x16 CAMERA_UPDATE_X_DT = 0.007;

constexpr SQ15x16 PLAYER_MAX_MOVE_VEL_X = 4.0;
constexpr SQ15x16 PLAYER_MAX_VEL_X = 4.0;


namespace Constants {

    constexpr SQ15x16 WorldWidth = 96 * 4 * 4;
    constexpr SQ15x16 DecSpeed = 0.01f;

    constexpr uint8_t HealthMax = 80;
    constexpr uint8_t Health_Plane = 10;
    constexpr uint8_t Health_Zap = 20;
    constexpr uint8_t Health_Bullet = 5;

    constexpr uint8_t EnemyCount = 10;
    constexpr uint8_t EnemyCount_Heart = 1;
    constexpr uint8_t EnemyCount_Mine = 3;
    constexpr uint8_t EnemyCount_Zap = 2;

    constexpr uint8_t Player_Width = 14;
    constexpr uint8_t ParticlesMax = 30;
    constexpr uint8_t BulletCount_Player = 5;
    constexpr uint8_t BulletCount_Enemy = 5;
    constexpr uint8_t TreasureCount = 5;
    constexpr uint8_t EnemyPickup = 192;

    constexpr uint8_t HUD_Left = 40;
    constexpr uint8_t DeathSeq_ExplodePlane = 10;
    constexpr uint8_t DeathSeq_Final = 56;

    constexpr int16_t Player_Velocity_Left_Max = 0;
    constexpr int16_t Player_Velocity_Left_Min = 13;
    constexpr int16_t Player_Velocity_Up_Max = 0;
    constexpr int16_t Player_Velocity_Up_Min = 13;
    constexpr int16_t Player_Velocity_Stationary = 14;
    constexpr int16_t Player_Velocity_Right_Min = 15;
    constexpr int16_t Player_Velocity_Right_Max = 28;
    constexpr int16_t Player_Velocity_Down_Min = 15;
    constexpr int16_t Player_Velocity_Down_Max = 28;

    constexpr int8_t Velocity_X[29] = { -32, -32, -32, -32, -32, -30, -28, -26, -23, -20, -16, -12, -8, -4, 0, 4, 8, 12, 16, 20, 23, 26, 28, 30, 32, 32, 32, 32, 32 };
    constexpr int8_t Velocity_Y[29] = { -32, -32, -32, -32, -32, -30, -28, -26, -23, -20, -16, -12, -8, -4, 0, 4, 8, 12, 16, 20, 23, 26, 28, 30, 32, 32, 32, 32, 32 };
    constexpr int8_t Camera_Offset_X[29] = { 26, 26, 18, 18, 12, 12, 8, 8, 6, 6, 4, 4, 2, 2, 0, 2, 2, 4, 4, 6, 6, 8, 8, 12, 12, 18, 18, 26, 26 };
    constexpr int8_t Thrust_Img[29] = { 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4,4, 5, 5, 5, 5 };

    constexpr int8_t Title_YPos[37] = { -48, -45, -42, -39, -36, -33, -30, -27, -24, -21, -18, -15, -12, -9, -6, -3, 0, 3, 6, 9, 6, 3, 1, 0, 0, 1, 3, 6, 9, 7, 6, 6, 7, 9, 8, 8, 9 };
    
};
