#pragma once

#include <stdint.h>
#include "../Map/TileTypes.h"
#include "../Map/Map.h"
#include "../Map/maps.h"
#include "../Hero/Hero.h"
#include "../GameContext/GameContext.h"


class Game;
namespace LevelUtils
{
    void copyStaticLevel(Game &game);
    void initPlayer(Game &game);
    bool isStuck(Game &game);
    bool checkWin(Game &game);
};