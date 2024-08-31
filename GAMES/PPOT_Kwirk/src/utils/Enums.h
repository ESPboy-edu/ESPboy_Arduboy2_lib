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
};

enum class GameState : uint8_t {

    Title_Init,
    Title_Start,
        Title_SelectFloor_1 = Title_Start,
        Title_SelectFloor_2,
        Title_SelectFloor_3,
        Title_SelectFloor_4,
        Title_SelectFloor_5,
        Title_SelectFloor_6,
        Title_SelectFloor_7,
        Title_SelectFloor_8,
        Title_SelectFloor_9,
        Title_SelectFloor_10,
        Title_SelectSkill_3_1,
        Title_SelectSkill_3_2,
        Title_SelectSkill_3_3,
        Title_SelectSkill_1_1,
        Title_SelectSkill_2_1,
        Title_SelectSkill_2_2,
        Title_SelectSkill_Blank,
        Title_ShowCredits,
        Title_FloorsComplete,
        Title_FloorsComplete2,
        Title_OptPlay,
        Title_OptSound1,
        Title_OptCredits,
        Title_OptResume,
        Title_OptSound2,
        Title_OptRestart,
        Title_OptSound_Music,
        Title_OptSound_SFX,
        Title_OptSound_Volume,
    Title_End,

    Play_Init,
    Play_Start,
        Play,
        Play_BouncingHead,
        Play_ZoomIn,
        Play_ZoomOut,
    Play_End,

    Menu_Init,
    Menu_Start,
        Menu_HasSave_Opt0,
        Menu_HasSave_Opt1,
        Menu_HasSave_Opt2,
        Menu_HasSave_Opt3,
        Menu_HasSave_Opt4,
        Menu_NoSave_Opt0,
        Menu_NoSave_Opt1,
        Menu_NoSave_Opt2,
        Menu_NoSave_Opt3,
        Menu_NoSave_Opt4,
    Menu_End,

    Play_MenuOpen_Init,
    Play_MenuOpen_NoSave,
    Play_MenuOpen_HasSave,
    Play_MenuClose,

    SplashScreen_Start,
        SplashScreen_00 = SplashScreen_Start,
        SplashScreen_01,
        SplashScreen_02,
        SplashScreen_03,
    SplashScreen_End,

};

enum class ItemType : uint8_t {
    None,                               // 00
    FirstItemType,                      // 01
    Rotator_Start = FirstItemType,      // 01
        Rotator_One = Rotator_Start,    // 01
        Rotator_Two,                    // 02
        Rotator_Three,                  // 03
        Rotator_Four,                   // 04
        Rotator_Five,                   // 05
    Rotator_End = Rotator_Five,         // 05                    
    Stairs,                             // 06
    Block_Start,                        // 07
        Block_01_01 = Block_Start,      // 07
        Block_01_02,                    // 08
        Block_01_03,                    // 09
        Block_01_04,                    // 10
        Block_01_05,                    // 11
        Block_01_06,                    // 12
        Block_02_01,                    // 13
        Block_02_02,                    // 14
        Block_02_03,                    // 15
        Block_03_01,                    // 16
        Block_04_01,                    // 17
        Block_05_01,                    // 18
        Block_03_02,                    // 19
        Block_03_03,                    // 20
        Block_01_11,                    // 21
    Block_End = Block_01_11,
    LastItemType = 255
};

enum class MapElement : uint8_t {
    Tile_Start,                                     // 00
        Tile_Overwrite_Start = Tile_Start,          // 00
            Tile_Solid = Tile_Start,                // 00
            Tile_StandAlone,                        // 01
            Tile_StandAlone_Half,                   // 02
            Tile_RightSide,                         // 03
            Tile_FullWidth_Top,                     // 04
            Tile_FullWidth,                         // 05
            Tile_LeftSide,                          // 06
            Tile_RightSide_Top,                     // 07
            Tile_LeftSide_Top,                      // 08
            Tile_Hole,                              // 09
            Tile_UpperEdge,                         // 10
        Tile_Overwrite_End = Tile_UpperEdge,
        Tile_Masked_Start,                          // 11
            Tile_BottonEdge_Left = Tile_Masked_Start,
            Tile_BottonEdge_Right,                  // 12
            Tile_Line_FullWidth,                    // 13
            Tile_Line_Right,                        // 14
            Tile_Line_Left,                         // 15
            Tile_BottonEdge_Middle,                 // 16
            Tile_BottonEdge_FullWidth,              // 17
            Tile_Line_Middle,                       // 18
        Tile_Masked_End = Tile_Line_Middle,
        Tile_FullRow                                = 50,
        Tile_HalfRow                                = 51,
    Tile_End,
    None,
};

enum class Direction : uint8_t {
    North,
    NorthEast,
    East,
    SouthEast,
    South,
    SouthWest,
    West,
    NorthWest,
};

enum class Rotation : uint8_t {
    None,
    Clockwise,
    AntiClockwise,
};

enum class Movement : uint8_t {
    Forward,
    Backward,
};

enum class PlayerCanMove : uint8_t {
    False,
    True,
    True_TwoSpaces,
};

enum class SkillLevels : uint8_t {
    Easy,
    Advanced,
    Hard,
};

enum class RotorTurning : uint8_t {
    FinishedTurning,
    StillTurning,
    NoneTurning,
};
