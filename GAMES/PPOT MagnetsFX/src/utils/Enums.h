#pragma once


enum class GameState : uint8_t {

    SplashScreen_Start,
        SplashScreen_00 = SplashScreen_Start,
        SplashScreen_01,
        SplashScreen_02,
        SplashScreen_03,
    SplashScreen_End,

    Title_Init,
    Title_Start,
        Title_Play = Title_Start,
        Title_Options,
        Title_Options_Spare,
        Title_Options_Small,
        Title_Options_Large,
        Title_Options_Auto,
        Title_SelectSize,
        Title_SelectHardness,
    Title_End,

    Play_Init,
    Play_Start,
        Play = Play_Start,
        Play_Particles,
        Play_Menu0,
        Play_Menu1,
        Play_Menu2,
        Play_Menu3,
        Play_Menu4,
        Play_Hint0,
        Play_Hint1,
    Play_End,
    
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

inline GameState &operator--(GameState &c) {
  c = static_cast<GameState>( static_cast<uint8_t>(c) - 1 );
  return c;
}

inline GameState operator--(GameState &c, int) {
  GameState result = c;
  --c;
  return result;
}

enum GameSize : uint8_t { 
    Small,
    Medium,
    Large
};

enum GameLevel : uint8_t { 
    Easy,
    Hard
};

enum class RenderSize : uint8_t { 
    Large,
    Small,
    Auto
};

enum Tiles : uint8_t {

    Horizontal_Blank_Start = 51,
    Horizontal_Blank_End = 52,
    Horizontal_MinusPlus_Start = 1,
    Horizontal_MinusPlus_End = 2,
    Horizontal_PlusMinus_Start = 4,
    Horizontal_PlusMinus_End = 3,
    Horizontal_Neutral_Start = 31,
    Horizontal_Neutral_End = 32,
    
    Vertical_Blank_Start = 61,
    Vertical_Blank_End = 62,
    Vertical_MinusPlus_Start = 11,
    Vertical_MinusPlus_End = 12,
    Vertical_PlusMinus_Start = 22,
    Vertical_PlusMinus_End = 21,
    Vertical_Neutral_Start = 41,
    Vertical_Neutral_End = 42,

};

enum ValidMove : uint8_t { 
    Valid,
    NotValid,
    Neutral
};


enum class HintType : uint8_t { 
    InvalidTile,
    RandomTile,
    Non
};

struct Hint {

  uint8_t x;
  uint8_t y; 
  uint8_t tile; 
  uint8_t x2;
  uint8_t y2; 
  uint8_t tile2; 
  uint8_t count; 

  HintType hintType;

};
