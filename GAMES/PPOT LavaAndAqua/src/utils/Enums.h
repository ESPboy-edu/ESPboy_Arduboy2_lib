#pragma once

enum PuzzleStatus : uint8_t { 
    Locked,
    Complete,
    InProgress,
};

enum class ObjectType : uint8_t { 
    Player,
    Lava, 
    Block,
    Water, 
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
        Title_Main = Title_Start,
        Title_Select,
        Title_Clear_Progress,
    Title_End,

    Play_Init,
    Play_Start,
        Play = Play_Start,
        Play_Dead, 
        Play_FadeOut,
        Play_FadeIn,
    Play_End,
    
    GameOver_Init,
    GameOver_Main,
    GameOver_Leave,

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

enum Direction : uint8_t { 
    None,
    Left,
    Right,
};
