#pragma once

#include "Constants.h"

#define DEBUG_BREAK    asm volatile("break\n");

enum CellTypes : uint8_t {
    Empty,
    Wall,
    Stairs,
    GateClosed,
    GateOpen,
};

enum ExitType : uint8_t {
    None,
    BottomLeft,
    TopRight,
    BottomRight,
};

enum class MenuDirection : uint8_t {
    None,
    Openning,
    Closing,
};

enum class GameState : uint8_t {
    PPOT,
    Menu_Init,
    Menu,
    Menu_Select,
    GamePlay,
    ShowMenu,
    Map,
    GameOver,
    LevelUp,
    HighScore,
};

enum class ItemType : uint8_t {
    None,
    Map,
    Bomb,
    Gun,
    Bullets,
    Wine,
    Key,
    Candle,
    //-------------------------
    Bomb_Active,
    Puff_Bomb,
    Puff_Stairs,
    Death_Player,
    Death_Enemy,
    Bullet_Normal,
    Bullet_None,

};
