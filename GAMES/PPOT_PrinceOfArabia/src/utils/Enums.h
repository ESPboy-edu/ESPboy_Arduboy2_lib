
#pragma once

enum class ItemType : uint8_t {
    /* 00 */ InteractiveItemType_Start,
    /* 00 */ AppearingFloor = 0,
    /* 01 */ CollapsingFloor,
    /* 02 */ FloorButton1,
    /* 03 */ FloorButton2,
    /* 04 */ Spikes,
    /* 05 */ ExitDoor_Button,
    /* 06 */ ExitDoor_Button_Cropped,
    /* 07 */ FloorButton3_UpDown,   // << needed?      
    /* 08 */ FloorButton3_UpOnly,   // << needed?      
    /* 09 */ FloorButton3_DownOnly,   // << needed?      
    /* 10 */ Mirror_Button,
    /* 11 */ FloorButton4,
    /* __ */ InteractiveItemType_End = FloorButton4,
    /* 15 */ Skeleton = 15,
    /* 16 */ ExitDoor_SelfOpen,
    /* 17 */ Gate,               
    /* 18 */ Gate_StayOpen,          
    /* 19 */ CollpasedFloor,
    /* 20 */ Potion_Small,
    /* 21 */ Potion_Large,
    /* 22 */ Potion_Poison,
    /* 23 */ Potion_Float,
    /* 24 */ EntryDoor_Cropped,
    /* 25 */ Blade,
    /* 26 */ ExitDoor_ButtonOpen,
    /* 27 */ Sword,
    /* 28 */ EntryDoor_HalfTileLeft,
    /* 29 */ Mirror,
    /* 30 */ Gate_StayClosed,
    /* 31 */ EntryDoor,
    /* 32 */ DecorativeDoor,
    /* 50 */ General = 50,
    /* 51 */ Invader_1,
    /* 52 */ Invader_2,
    /* 53 */ Invader_3,
    /* 54 */ Player,
    /* 55 */ Barrier,
    /* 56 */ Bullet,
    /* 96 */ None = 96,
    /* 97 */ LoveHeart = 97,
    /* 98 */ Sign = 98,
    /* 99 */ Flash = 99,
    /* 99 */ AllItemTypes_End = Flash,
};

enum GameState : uint8_t {
    SplashScreen_Init,
    SplashScreen,
    Title_Init,
    Title,
    Game_Init,
    Game,
    Game_StartLevel,
    #ifndef SAVE_MEMORY_OTHER
    Menu,
    Menu_Confirm,
    #endif
};

enum class Direction : uint8_t {
    None,
    Left,
    Right,
    Up,
    Down,
    Forward,
    Back,
};

enum class Layer : uint8_t {
    Background,
    Foreground,
};

enum class Action : uint8_t {
    Step                    = 0,
    SmallStep               = 1,
    RunStart                = 2,
    RunRepeat               = 3,
    SwordStep               = 4,        // Do not move from 4
    RunJump_Normal          = 5,
    RunJump_Level6Exit      = 6,
    StandingJump            = 7,
    SwordStep2              = 8,        // Do not move from 8
    CrouchHop               = 9,
    RunningTurn             = 10,
};

enum class CanJumpUpResult : uint8_t {
    None,
    Jump,
    StepThenJump,
    JumpThenFall,
    JumpThenFall_HideHands,
    TurnThenJump,
    JumpDist10,
    JumpThenFall_CollapseFloor,             
    StepThenJumpThenFall_CollapseFloor,     
    JumpThenFall_CollapseFloorAbove,        
};

enum class CanClimbDownResult : uint8_t {
    None,
    ClimbDown,
    StepThenClimbDown,
    TurnThenClimbDown,
    StepThenTurnThenClimbDown,
};

enum class CanClimbDownPart2Result : uint8_t {
    None,
    Level_1,
    Falling,
    Level_1_Under,
};

enum class StandingJumpResult : uint8_t {
    None,
    Normal_20,
    Normal_24,
    Normal_28,
    Normal_32,
    Normal_36,
    DropLevel_36,
    DropLevel_40,
    GrabLedge_28, 
    GrabLedge_32, 
    GrabLedge_36,
    GrabLedge_40,
};


enum class RunningJumpResult : uint8_t {
    None,
    Normal_Pos2,
    Normal_Pos6,
    Jump4_GrabLedge_Pos2,
    Jump4_GrabLedge_Pos6,
    Jump4_GrabLedge_Pos10,
    Jump4_DropLevel,
    Jump4_DropLevel_Pos10,
    Jump3_Pos2,
    Jump3_Pos6,
    Jump3_Pos10,
    Jump3_DropLevel,
    Jump3_DropLevel_Pos10,
    Jump2_Pos2,
    Jump2_Pos6,
    Jump2_DropLevel,
    Jump2_DropLevel_Pos10,
    Jump2_Pos10,
    Jump1_Pos2,
    Jump1_Pos6,
    Jump1_Pos10,
};

enum class MenuOption : uint8_t {
    Resume = 0,
    MainMenu = 1,
    Save = 2,
    Load_PrinceDead = 2,
    Load = 3,
    Clear_PrinceDead = 3,
    Clear = 4,
};

enum class TitleScreenOptions : uint8_t {
    Play,
    Resume,
    Credits, 
    High,
};

enum class TitleScreenMode : uint8_t {
    Intro,                          // 0
    Main,                           // 1
    Credits,                        // 2
    High,                           // 3
    IntroGame_1A,                   // 4
    CutScene_1,                     // 5
    IntroGame_1B,                   // 6
    CutScene_2,                     // 7
    CutScene_3,                     // 8
    CutScene_4,                     // 9
    CutScene_5,                     // 10
    CutScene_6,                     // 11
    CutScene_7_Hint,                // 12
    CutScene_7_Transition,          // 13
    CutScene_7_PlayGame,            // 14
    CutScene_End,                   // 15
    IntroGame_End,                  // 16
    TimeOut,                        // 17
    MaxUniqueScenes                 = CutScene_End - 1
};

enum class WallTileResults : uint8_t {
    None,
    SolidWall,
    GateClosed, 
};

enum class SignType : uint8_t {
    PressA,
    GameOver,
};

enum class FlashType : uint8_t {
    None,
    SwordFight,
    MirrorLevel12,
};

enum class LevelUpdate : uint8_t {
    NoAction,
    FloorCollapsedOnPrince,
};

enum class Status : uint8_t {
    Active,
    Safe,
    Dormant,
    Dormant_ActionReady,
    Dormant_ActionDone,
    Exploding1,
    Exploding2,
    Exploding3,
    Exploding4,
    Dead,
    EnemiesAppearing,
};

enum class EnemyType : uint8_t {
    Guard,
    Skeleton,
    Mirror,
    MirrorAttackingL12,
    MirrorAfterChallengeL12,
    None = 255
};

enum class DeathType : uint8_t {
    Falling,
    Blade,
    SwordFight,
    Spikes,
};

enum class CanFallResult : uint8_t {
    CanFall,
    CannotFall,
    CanFallToHangingPosition,
};

enum class GateType : uint8_t {
    Normal,
    Level6Exit,
};






enum class TitleFrameIndex : uint8_t {
    Credits_PoP,
    High_PoP_Frame,
    Main_PoP_Frame_NC,
    Main_PoP_Frame_NCH,
    Main_PoP_Frame_NRC,
    Main_PoP_Frame_NRCH,
    Intro_Last_PoP_Frame_NC,
    Intro_Last_PoP_Frame_NCH,
    Intro_Last_PoP_Frame_NRC,
    Intro_Last_PoP_Frame_NRCH,
    Main_Game_PoP_Frame_NC,
    Main_Game_PoP_Frame_NCH,
    Main_Game_PoP_Frame_NRC,
    Main_Game_PoP_Frame_NRCH,
    IntroGame_End_PoP_Frame,
    Intro_PoP_Frame_NC,
    Intro_PoP_Frame_NCH,
    Intro_PoP_Frame_NRC,
    Intro_PoP_Frame_NRCH,
    TimeOut_PoP_Frame,
    IntroGame_1A_Frame,
    CutScene_1_Frame,
    IntroGame_1B_Frame,
    CutScene_2_Frame,
    CutScene_3_Frame,
    CutScene_4_Frame,
    CutScene_5_Frame,
    CutScene_6_Frame,
    CutScene_2B_Frame,
    CutScene_7_Frame,
    CutScene_End_Frame
};

enum class SoundIndex : uint8_t {
    Dead,
    Grab1,
    Grab2,
    Grab3,
    Grab4,
    Seque,
    Tada,
    Theme,
    Triumph,
    Victory,
    Ending,
    OutOfTime,
    GateGoingDown,
    GateGoingUp,
    ChopChop,
    Thump,
    Strike,
    Invader_Wave_Start,
    Invader_Wave_Success,
    Invader_End_of_Game,
    Invader_Player_Fires_Bullet,
    Invader_Enemy_Fires_Bullet,
    Invader_Player_Explosion,
    Invader_Enemy_Explosion,
    Invader_Hit_Barrier,
};
