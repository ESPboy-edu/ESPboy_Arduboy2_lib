#include "LevelUtils.h"
#include "../Game.h"
using namespace LevelUtils;

void LevelUtils::copyStaticLevel(Game &game)
{
    auto &context = game.getGameContext();
    uint8_t stage = context.stage;
    for (uint8_t j = 0; j < 8; j++)
    {
        byte mapObjectValues = pgm_read_byte_near(levels[stage] + j);
        for (uint8_t i = 0; i < 8; i++)
        {
            if (bitRead(mapObjectValues, 7 - i))
            {
                context.mapObject.setTile(i, j, TileType::Wall);
            }
            else
            {
                context.mapObject.setTile(i, j, TileType::Empty);
            }
        }
    }
    context.hero.x = pgm_read_byte_near(levels[stage] + 8);
    context.hero.y = pgm_read_byte_near(levels[stage] + 9);
}

void LevelUtils::initPlayer(Game &game)
{
    auto &context = game.getGameContext();
    context.hero.dx = 0;
    context.hero.dy = 0;
    context.hero.x = 0;
    context.hero.y = 0;
    context.hero.direction = Direction::Center;
    context.hero.lastDirection = Direction::Center;
}

bool LevelUtils::isStuck(Game &game)
{
    auto &context = game.getGameContext();
    if (context.mapObject.getNeighbourTileCount(context.hero.x, context.hero.y, TileType::Empty) == 0)
        return true;
    return false;
}

bool LevelUtils::checkWin(Game &game)
{
    auto &context = game.getGameContext();
    for (uint8_t i = 0; i < (Map::mapWidth * Map::mapHeight); i++)
    {
        if (context.mapObject.getTile(i) == TileType::Empty)
        {
            return false;
        }
    }
    return true;
}