#pragma once

enum class MusicSong : uint8_t {
    MainTheme
};

enum class MusicSFX : uint8_t {
    SFX_Death,
    SFX_Victory,
    SFX_XPGain,
    SFX_Three_Inc,
    SFX_Three_Dec,
    SFX_PlayerBlip,
    SFX_EnemyBlip,
    SFX_Explosion,
    SFX_DoubleBeep,
};

enum class GameState : uint8_t {

    SplashScreen_Start,
        SplashScreen_00 = SplashScreen_Start,
        SplashScreen_01,
        SplashScreen_02,
        SplashScreen_03,
    SplashScreen_End,

    Title_Init,
    Title_Start,
        Title_OptPlay = Title_Start,
        Title_OptSound,
        Title_OptCredits,
        Title_ShowCredits,
        Title_OptSound_Music,
        Title_OptSound_SFX,
        Title_OptSound_Volume,
    Title_End,

    Play_Init,
    Play_Start,
        Play = Play_Start,
        Play_EndOfGame,
        Play_Quit,
    Play_End,

};

enum class Direction : uint8_t {
    None,
    Up,
    Right,
    Down,
    Left
};

enum class Size : uint8_t {
    Small,
    Medium,
    Large,
};

enum class Movement : uint8_t {
    Up_02_06,    // right, up
    Up_03_05,
    Up_04_04, 
    Up_05_03,
    Up_06_02,
    Up_07_01,
    Middle,
    Down_07_01,
    Down_06_02,
    Down_05_03,
    Down_04_04,
    Down_03_05,
    Down_02_06,
};

enum class PlayerMovement : uint8_t {
    Up_Left_12_12,
    Up_Left_10_14,
    Up_Left_08_16,
    Up_Left_06_18, 
    Up_Left_04_20,
    Up_Left_02_22,    // right, up
    Up_Middle,
    Up_Right_02_22,    // right, up
    Up_Right_04_20,
    Up_Right_06_18, 
    Up_Right_08_16,
    Up_Right_10_14,
    Up_Right_12_12,
    Down_Left_12_12,
    Down_Left_10_14,
    Down_Left_08_16,
    Down_Left_06_18, 
    Down_Left_04_20,
    Down_Left_02_22,    // right, up
    Down_Middle,
    Down_Right_02_22,    // right, up
    Down_Right_04_20,
    Down_Right_06_18, 
    Down_Right_08_16,
    Down_Right_10_14,
    Down_Right_12_12,

};

enum class Speed : uint8_t {
    Slow,
    Medium,
    Fast,
};

enum class CellOwner : uint8_t {
    None,
    Player,
    Enemy,
    Explode,
};

enum class EnemyType : uint8_t {
    Heart,
    Mine,
    Plane_Start,
        Plane = Plane_Start,
        Plane_Decelerate,
        Plane_SetHeight,
        Plane_Pickup,
        Plane_Accelerate,
    Plane_End,
    Zap
};

enum class EnemyUpdate : uint8_t {
    Normal,
    Inactive,
    Treasure_StartPickup,
    Treasure_PickedUp,
};

enum class PlayerUpdate : uint8_t {
    Normal,
    Dead,
};
