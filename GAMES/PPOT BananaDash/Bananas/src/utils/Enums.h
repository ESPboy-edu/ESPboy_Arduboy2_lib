#pragma once


struct GameOver {

    uint8_t char1;
    uint8_t char1X;
    int8_t  char1Y;
    uint8_t char2;
    uint8_t char2X;
    int8_t  char2Y;
    uint8_t char3;
    uint8_t char3X;
    int8_t  char3Y;
    uint8_t char4;
    uint8_t char4X;
    int8_t  char4Y;
    uint8_t char5;
    uint8_t char5X;
    int8_t  char5Y;
    uint8_t char6;
    uint8_t char6X;
    int8_t  char6Y;
    uint8_t char7;
    uint8_t char7X;
    int8_t  char7Y;
    uint8_t char8;
    uint8_t char8X;
    int8_t  char8Y;

};

enum class MusicSong : uint8_t {
    Empty,
    MainTheme,
};

enum class MusicSFX : uint8_t {
    SFX_Death,
    SFX_Victory,
    SFX_LandHard,
    SFX_PlayerBlip,
    SFX_EnemyBlip,
    SFX_PickUpItem,
    SFX_XPGain,
    SFX_Glint,
};

struct StanceDetails {

    int8_t stanceY;
    int8_t foreground;        
    int8_t middleground;        
    int8_t background;        
    
};

enum class ItemUpdate : uint8_t {
    None,
    LaunchBanana,
    LaunchFruit,
};

enum class EntityType : uint8_t {
    None,
    Player,
    Barrel,
    Bird,
    Spider,
};

enum class ItemType : uint8_t {
    None,
    Banana,
    Banana_Hidden,
    Heart,
    Heart_Hidden,
    Fire,
    Fire_Hidden,
    Fruit,
    Fruit_Hidden,
    Bananarang_0,
    Bananarang_1,
    Bananarang_2,
    Bananarang_3,
    Puff,
};

enum class PuffType : uint8_t {
    None,
    Banana,
    Heart,
    Fruit,
    Enemy,
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
        Title_Play_Game = Title_Start,
        Title_Highs,
        Title_Sounds,
	Title_End,

    PlayGame_Init,
    PlayGame_Start,
        PlayGame = PlayGame_Start,
        PlayGame_GameOver,
        PlayGame_TimesUp,
    PlayGame_End,

    HighScore_Init,
    HighScore_Start,
        HighScore_Edit = HighScore_Start,
        HighScore_View,
    HighScore_End,
};

inline GameState &operator++(GameState &c) {
  c = static_cast<GameState>( static_cast<uint8_t>(c) + 1 );
  return c;
}

inline GameState operator++(GameState &c, int) {
  GameState result = c;
  ++c;
  return result;
}

inline GameState &operator--( GameState & c ) {
  c = static_cast<GameState>( static_cast<uint8_t>(c) - 1 );
  return c;
};

inline GameState operator--( GameState & c, int ) {
  GameState result = c;
  --c;
  return result;
};

enum Stance : uint16_t {

    Player_Start,

        Player_Start_RH = Player_Start,

            Player_Idle_RH = Player_Start,
            Player_Lying_RH,

            Player_Walk_RH_00,
            Player_Walk_RH_01,
            Player_Walk_RH_02,
            Player_Walk_RH_03,

            Player_Standing_Jump_RH_00,
            Player_Standing_Jump_RH_01,
            Player_Standing_Jump_RH_02,
            Player_Standing_Jump_RH_03,
            Player_Standing_Jump_RH_04,
            Player_Standing_Jump_RH_05,
            Player_Standing_Jump_RH_06,
            Player_Standing_Jump_RH_07,
            Player_Standing_Jump_RH_08,
            Player_Standing_Jump_RH_09,
            Player_Standing_Jump_RH_10,
            Player_Standing_Jump_RH_11,

            Player_Running_Jump_RH_00,
            Player_Running_Jump_RH_01,
            Player_Running_Jump_RH_02,
            Player_Running_Jump_RH_03,
            Player_Running_Jump_RH_04,
            Player_Running_Jump_RH_05,
            Player_Running_Jump_RH_06,
            Player_Running_Jump_RH_07,
            Player_Running_Jump_RH_08,
            Player_Running_Jump_RH_09,
            Player_Running_Jump_RH_10,
            Player_Running_Jump_RH_11,

            Player_Climbing_Vine_Up_RH_00,
            Player_Climbing_Vine_Up_RH_01,
            Player_Climbing_Vine_Up_RH_02,
            Player_Climbing_Vine_Up_RH_03,

            Player_Climbing_Vine_Down_RH_00,
            Player_Climbing_Vine_Down_RH_01,
            Player_Climbing_Vine_Down_RH_02,
            Player_Climbing_Vine_Down_RH_03,

            Player_Falling_1L_A_RH_00,
            Player_Falling_1L_A_RH_01,
            Player_Falling_1L_A_RH_02,
            Player_Falling_1L_A_RH_03,
            Player_Falling_1L_A_RH_04,
            Player_Falling_1L_A_RH_05,
            Player_Falling_1L_A_RH_06,
            Player_Falling_1L_A_RH_07,

            Player_Falling_1L_B_RH_00,
            Player_Falling_1L_B_RH_01,
            Player_Falling_1L_B_RH_02,
            Player_Falling_1L_B_RH_03,
            Player_Falling_1L_B_RH_04,
            Player_Falling_1L_B_RH_05,
            Player_Falling_1L_B_RH_06,
            Player_Falling_1L_B_RH_07,

            Player_Falling_2L_A_RH_00,
            Player_Falling_2L_A_RH_01,
            Player_Falling_2L_A_RH_02,
            Player_Falling_2L_A_RH_03,
            Player_Falling_2L_A_RH_04,
            Player_Falling_2L_A_RH_05,
            Player_Falling_2L_A_RH_06,
            Player_Falling_2L_A_RH_07,
            Player_Falling_2L_A_RH_08,
            Player_Falling_2L_A_RH_09,
            Player_Falling_2L_A_RH_10,
            Player_Falling_2L_A_RH_11,

            Player_Falling_2L_B_RH_00,
            Player_Falling_2L_B_RH_01,
            Player_Falling_2L_B_RH_02,
            Player_Falling_2L_B_RH_03,
            Player_Falling_2L_B_RH_04,
            Player_Falling_2L_B_RH_05,
            Player_Falling_2L_B_RH_06,
            Player_Falling_2L_B_RH_07,
            Player_Falling_2L_B_RH_08,
            Player_Falling_2L_B_RH_09,
            Player_Falling_2L_B_RH_10,
            Player_Falling_2L_B_RH_11,

            Player_Falling_2L_C_RH_00,
            Player_Falling_2L_C_RH_01,
            Player_Falling_2L_C_RH_02,
            Player_Falling_2L_C_RH_03,
            Player_Falling_2L_C_RH_04,
            Player_Falling_2L_C_RH_05,
            Player_Falling_2L_C_RH_06,
            Player_Falling_2L_C_RH_07,

        Player_End_RH = Player_Falling_2L_C_RH_07,

        Player_Start_LH,

            Player_Idle_LH = Player_Start_LH,
            Player_Lying_LH,

            Player_Walk_LH_00,
            Player_Walk_LH_01,
            Player_Walk_LH_02,
            Player_Walk_LH_03,

            Player_Standing_Jump_LH_00,
            Player_Standing_Jump_LH_01,
            Player_Standing_Jump_LH_02,
            Player_Standing_Jump_LH_03,
            Player_Standing_Jump_LH_04,
            Player_Standing_Jump_LH_05,
            Player_Standing_Jump_LH_06,
            Player_Standing_Jump_LH_07,
            Player_Standing_Jump_LH_08,
            Player_Standing_Jump_LH_09,
            Player_Standing_Jump_LH_10,
            Player_Standing_Jump_LH_11,

            Player_Running_Jump_LH_00,
            Player_Running_Jump_LH_01,
            Player_Running_Jump_LH_02,
            Player_Running_Jump_LH_03,
            Player_Running_Jump_LH_04,
            Player_Running_Jump_LH_05,
            Player_Running_Jump_LH_06,
            Player_Running_Jump_LH_07,
            Player_Running_Jump_LH_08,
            Player_Running_Jump_LH_09,
            Player_Running_Jump_LH_10,
            Player_Running_Jump_LH_11,

            Player_Climbing_Vine_Up_LH_00,
            Player_Climbing_Vine_Up_LH_01,
            Player_Climbing_Vine_Up_LH_02,
            Player_Climbing_Vine_Up_LH_03,

            Player_Climbing_Vine_Down_LH_00,
            Player_Climbing_Vine_Down_LH_01,
            Player_Climbing_Vine_Down_LH_02,
            Player_Climbing_Vine_Down_LH_03,     

            Player_Falling_1L_A_LH_00,
            Player_Falling_1L_A_LH_01,
            Player_Falling_1L_A_LH_02,
            Player_Falling_1L_A_LH_03,
            Player_Falling_1L_A_LH_04,
            Player_Falling_1L_A_LH_05,
            Player_Falling_1L_A_LH_06,
            Player_Falling_1L_A_LH_07,

            Player_Falling_1L_B_LH_00,
            Player_Falling_1L_B_LH_01,
            Player_Falling_1L_B_LH_02,
            Player_Falling_1L_B_LH_03,
            Player_Falling_1L_B_LH_04,
            Player_Falling_1L_B_LH_05,
            Player_Falling_1L_B_LH_06,
            Player_Falling_1L_B_LH_07,
            
            Player_Falling_2L_A_LH_00,
            Player_Falling_2L_A_LH_01,
            Player_Falling_2L_A_LH_02,
            Player_Falling_2L_A_LH_03,
            Player_Falling_2L_A_LH_04,
            Player_Falling_2L_A_LH_05,
            Player_Falling_2L_A_LH_06,
            Player_Falling_2L_A_LH_07,
            Player_Falling_2L_A_LH_08,
            Player_Falling_2L_A_LH_09,
            Player_Falling_2L_A_LH_10,
            Player_Falling_2L_A_LH_11,

            Player_Falling_2L_B_LH_00,
            Player_Falling_2L_B_LH_01,
            Player_Falling_2L_B_LH_02,
            Player_Falling_2L_B_LH_03,
            Player_Falling_2L_B_LH_04,
            Player_Falling_2L_B_LH_05,
            Player_Falling_2L_B_LH_06,
            Player_Falling_2L_B_LH_07,
            Player_Falling_2L_B_LH_08,
            Player_Falling_2L_B_LH_09,
            Player_Falling_2L_B_LH_10,
            Player_Falling_2L_B_LH_11,

            Player_Falling_2L_C_LH_00,
            Player_Falling_2L_C_LH_01,
            Player_Falling_2L_C_LH_02,
            Player_Falling_2L_C_LH_03,
            Player_Falling_2L_C_LH_04,
            Player_Falling_2L_C_LH_05,
            Player_Falling_2L_C_LH_06,
            Player_Falling_2L_C_LH_07,

        Player_End_LH = Player_Falling_2L_C_LH_07,

    Player_End = Player_End_LH,

    Enemy_Start,

        Enemy_Start_RH = Enemy_Start,

            Enemy_Idle_RH = Enemy_Start_RH,

            Enemy_Rolling_RH_00,
            Enemy_Rolling_RH_01,
            Enemy_Rolling_RH_02,
            Enemy_Rolling_RH_03,

            Enemy_Falling_1L_RH_00,
            Enemy_Falling_1L_RH_01,
            Enemy_Falling_1L_RH_02,
            Enemy_Falling_1L_RH_03,
            Enemy_Falling_1L_RH_04,
            Enemy_Falling_1L_RH_05,
            Enemy_Falling_1L_RH_06,
            Enemy_Falling_1L_RH_07,

            Enemy_Falling_2L_RH_00,
            Enemy_Falling_2L_RH_01,
            Enemy_Falling_2L_RH_02,
            Enemy_Falling_2L_RH_03,
            Enemy_Falling_2L_RH_04,
            Enemy_Falling_2L_RH_05,
            Enemy_Falling_2L_RH_06,
            Enemy_Falling_2L_RH_07,
            Enemy_Falling_2L_RH_08,
            Enemy_Falling_2L_RH_09,
            Enemy_Falling_2L_RH_10,
            Enemy_Falling_2L_RH_11,

            Enemy_Flying_RH_00,
            Enemy_Flying_RH_01,
            Enemy_Flying_RH_02,
            Enemy_Flying_RH_03,
            Enemy_Flying_RH_04,
            Enemy_Flying_RH_05,
            Enemy_Flying_RH_06,
            Enemy_Flying_RH_07,
            Enemy_Flying_RH_08,
            Enemy_Flying_RH_09,
            Enemy_Flying_RH_10,
            Enemy_Flying_RH_11,
            Enemy_Flying_RH_12,
            Enemy_Flying_RH_13,
            Enemy_Flying_RH_14,
            Enemy_Flying_RH_15,
            Enemy_Flying_RH_16,
            Enemy_Flying_RH_17,
            Enemy_Flying_RH_18,
            Enemy_Flying_RH_19,

        Enemy_End_RH = Enemy_Flying_RH_19,

        Enemy_Start_LH,

            Enemy_Idle_LH = Enemy_Start_LH,

            Enemy_Rolling_LH_00,
            Enemy_Rolling_LH_01,
            Enemy_Rolling_LH_02,
            Enemy_Rolling_LH_03,

            Enemy_Falling_1L_LH_00,
            Enemy_Falling_1L_LH_01,
            Enemy_Falling_1L_LH_02,
            Enemy_Falling_1L_LH_03,
            Enemy_Falling_1L_LH_04,
            Enemy_Falling_1L_LH_05,
            Enemy_Falling_1L_LH_06,
            Enemy_Falling_1L_LH_07,

            Enemy_Falling_2L_LH_00,
            Enemy_Falling_2L_LH_01,
            Enemy_Falling_2L_LH_02,
            Enemy_Falling_2L_LH_03,
            Enemy_Falling_2L_LH_04,
            Enemy_Falling_2L_LH_05,
            Enemy_Falling_2L_LH_06,
            Enemy_Falling_2L_LH_07,
            Enemy_Falling_2L_LH_08,
            Enemy_Falling_2L_LH_09,
            Enemy_Falling_2L_LH_10,
            Enemy_Falling_2L_LH_11,

            Enemy_Flying_LH_00,
            Enemy_Flying_LH_01,
            Enemy_Flying_LH_02,
            Enemy_Flying_LH_03,
            Enemy_Flying_LH_04,
            Enemy_Flying_LH_05,
            Enemy_Flying_LH_06,
            Enemy_Flying_LH_07,
            Enemy_Flying_LH_08,
            Enemy_Flying_LH_09,
            Enemy_Flying_LH_10,
            Enemy_Flying_LH_11,
            Enemy_Flying_LH_12,
            Enemy_Flying_LH_13,
            Enemy_Flying_LH_14,
            Enemy_Flying_LH_15,
            Enemy_Flying_LH_16,
            Enemy_Flying_LH_17,
            Enemy_Flying_LH_18,
            Enemy_Flying_LH_19,

        Enemy_End_LH = Enemy_Flying_LH_19,

        Enemy_Fall_1L_00,
        Enemy_Fall_1L_01,
        Enemy_Fall_1L_02,
        Enemy_Fall_1L_03,
        Enemy_Fall_1L_04,
        Enemy_Fall_1L_05,
        Enemy_Fall_1L_06,

        Enemy_Spider_R1_00,
        Enemy_Spider_R1_01,
        Enemy_Spider_R1_02,
        Enemy_Spider_R1_03,
        Enemy_Spider_R1_04,
        Enemy_Spider_R1_05,
        Enemy_Spider_R1_06,
        Enemy_Spider_R1_07,
        Enemy_Spider_R1_08,
        Enemy_Spider_R1_09,
        Enemy_Spider_R1_10,
        Enemy_Spider_R1_11,
        Enemy_Spider_R1_12,
        Enemy_Spider_R1_13,
        Enemy_Spider_R1_14,
        Enemy_Spider_R1_15,

        Enemy_Spider_L1_00,
        Enemy_Spider_L1_01,
        Enemy_Spider_L1_02,
        Enemy_Spider_L1_03,
        Enemy_Spider_L1_04,
        Enemy_Spider_L1_05,
        Enemy_Spider_L1_06,
        Enemy_Spider_L1_07,
        Enemy_Spider_L1_08,
        Enemy_Spider_L1_09,
        Enemy_Spider_L1_10,
        Enemy_Spider_L1_11,
        Enemy_Spider_L1_12,
        Enemy_Spider_L1_13,
        Enemy_Spider_L1_14,
        Enemy_Spider_L1_15,

        Enemy_Spider_Idle_00,
        Enemy_Spider_Idle_01,
        Enemy_Spider_Idle_02,
        Enemy_Spider_Idle_03,
        Enemy_Spider_Idle_04,
        Enemy_Spider_Idle_05,
        Enemy_Spider_Idle_06,
        Enemy_Spider_Idle_07,
        Enemy_Spider_Idle_08,
        Enemy_Spider_Idle_09,
        Enemy_Spider_Idle_10,
        Enemy_Spider_Idle_11,
        Enemy_Spider_Idle_12,
        Enemy_Spider_Idle_13,
        Enemy_Spider_Idle_14,
        Enemy_Spider_Idle_15,

        Enemy_Spider_Die_00,
        Enemy_Spider_Die_01,
        Enemy_Spider_Die_02,
        Enemy_Spider_Die_03,
        Enemy_Spider_Die_04,
        Enemy_Spider_Die_05,

    None,

};


enum class Movements : uint8_t {
    None,
    Up = 1,
    Down = 2, 
    Left = 4,
    Right = 8,
    Jump = 16,
    Reverse = 32,
    JumpUp = 128,
    ThrowKey = 128,  
};

enum class Direction : uint8_t {
    Left = 1,
    Right = 2,
    None = 4
};



