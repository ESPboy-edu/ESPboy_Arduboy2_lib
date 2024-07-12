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
    SFX_DoubleBeepDown,
    SFX_Bet1,
    SFX_Bet2,
    SFX_Bet3,
    SFX_Bet4,
    SFX_Bet5,
    SFX_Bet6,
    SFX_Click,
    SFX_SmallBlind,
    SFX_BigBlind,
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
        Title_OptResume,
        Title_OptPlay2,
        Title_OptSound2,
        Title_OptSound_Music,
        Title_OptSound_SFX,
        Title_OptSound_Volume,
        Title_OptSound_Music2,
        Title_OptSound_SFX2,
        Title_OptSound_Volume2,
        Title_ShowInstructions,
    Title_End,

    Play_Init,
    Play_Start,
        Play_Deal_1 = Play_Start,
        Play_Deal_2,
        Play_Deal_3,
        Play_Deal_4,
        Play_Deal_5,
        Play_Deal_6,
        Play_Deal_7,
        Play_Deal_8,
        Play_Deal_9,
        Play_Deal_10,
        Play_Flip,
        Play_Bid_00,
        Play_Bid_01,
        Play_Bid_02,
        Play_Bid_03,
        Play_Bid_10,
        Play_Bid_11,
        Play_Bid_12,
        Play_Player_00,
        Play_Player_01,
        Play_Player_02,
        Play_Player_03,
        Play_Player_10,
        Play_Player_11,
        Play_Player_12,
        Play_EOH,
        Play_EOR,
        Play_EOR_ShowScore,
        Play_EOG,
    Play_End,

    Pause_01,
    Pause_02,
    
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

enum class Suits : uint8_t { 
    Spade = 1, 
    Club = 2, 
    Diamond = 3, 
    Heart = 4, 
    None = 0 
};

enum Values : uint8_t { two = 2, three = 3, four = 4, five = 5, six = 6, seven = 7, eight = 8, nine = 9, ten = 10, jack = 11, queen = 12, king = 13, ace = 14 };
enum class Action : uint8_t { Fold = 0, Call = 1, Raise = 2, Check = 3, AllIn = 4, AllIn_NoBanner = 5, None = 255 };

enum HandValues : uint8_t { 
    Nothing = 0, 
    OnePair = 2, 
    TwoPiars,
    ThreeOfAKind, 
    Straight, 
    Flush, 
    FullHouse,
    FourOfAKind, 
    StraightFlush, 
    RoyalFlush 
};

enum Direction : uint8_t { 
    Down,
    Up
};
