#pragma once

#include <stdint.h>
#include <Arduboy2.h>
#include "LevelUtils.h"
#include "../Utils/CatChars.h"

class Game;
namespace LevelRenderer
{
    void render(Game &game);

    void renderRandData(uint8_t xDrawOffset, uint8_t yDrawOffset, Game &game);
    void renderLevelData(uint8_t xDrawOffset, uint8_t yDrawOffset, Game &game);
    void renderLevel(uint8_t xDrawOffset, uint8_t yDrawOffset, Game &game);
    void renderPreview(uint8_t size, uint8_t xDrawOffset, uint8_t yDrawOffset, Game &game);
    void renderPlayer(uint8_t xDrawOffset, uint8_t yDrawOffset, Game &game);
    void renderStuck(uint8_t xDrawOffset, uint8_t yDrawOffset, Game &game);
    void renderWin(uint8_t xDrawOffset, uint8_t yDrawOffset, Game &game);

};