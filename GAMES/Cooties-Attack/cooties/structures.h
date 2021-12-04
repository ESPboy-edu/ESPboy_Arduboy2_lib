#pragma once

#include "general.h"


enum class State : uint8_t
{
	Title, Game, GameOver
};

State state=State::Title;

enum class Direction : uint8_t
{
	Up, Right, Down, Left
};

struct Bullet:Rect
{
	Direction direction;
};



constexpr uint8_t bulletSize = 3; // Size of a square bullet
constexpr uint8_t bullets = 5;    // Maximum number of bullets
constexpr uint8_t bulletWait = 10; // Minimum frames between firing
constexpr int bulletOff = -1;     // "Bullet not in use" value;


constexpr uint8_t maxTargets = 5;    // Maximum number of target
uint8_t targets = 4;
uint8_t maxLife=3;
// For counting the minimum number of frames between bullets
uint8_t waitCount = 0;

Bullet bullet[bullets];


//enemies
// Target definitions
constexpr uint8_t targetWidth = 8;
constexpr uint8_t targetHeight = 8;
 uint8_t targetX = 120;
 uint8_t targetY = 30;



struct Characters:Rect
{   
    /*uint8_t w=8;
    uint8_t h=8;
    int16_t x=64;
    int16_t y=32;*/

    uint8_t iframe=50;

    Direction direction;
    bool enable = false;

    bool collide = false;

    uint8_t speed = 2;
    uint8_t frame = 0;

    int8_t life = 3;

};

Characters hero;
constexpr uint8_t heroWidth = 8;
constexpr uint8_t heroHeight = 8;


Characters cootie[maxTargets];