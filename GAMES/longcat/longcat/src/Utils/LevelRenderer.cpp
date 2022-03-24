#include "LevelRenderer.h"
#include "../Game.h"
using namespace LevelRenderer;

void LevelRenderer::renderStuck(uint8_t xDrawOffset, uint8_t yDrawOffset, Game &game){
    auto &arduboy = game.getArduboy();
    if (LevelUtils::isStuck(game))
    {
		arduboy.fillRect(xDrawOffset-3,yDrawOffset+1,58,35,WHITE);
		arduboy.fillRect(xDrawOffset+4,yDrawOffset+8,44,21,BLACK);
		CatChars::print(xDrawOffset+6, yDrawOffset+14, true, "STUCK");	
    }
}

void LevelRenderer::renderRandData(uint8_t xDrawOffset, uint8_t yDrawOffset, Game &game)
{
    auto &arduboy = game.getArduboy();
    auto &context = game.getGameContext();
    CatKey r = RandKey::toCatKey(context.currentSeed);
    for (uint8_t i = 0; i < CatKeyLength; i++)
    {
        CatChars::drawChar(xDrawOffset + (i * 8), yDrawOffset, true, r.key[i] + 2);
    }
}

void LevelRenderer::renderLevelData(uint8_t xDrawOffset, uint8_t yDrawOffset, Game &game)
{
    auto &arduboy = game.getArduboy();
    auto &context = game.getGameContext();
	
	CatChars::print(xDrawOffset, yDrawOffset, true, "Level");	
	CatChars::printBCD(xDrawOffset + 42, yDrawOffset, true, context.stage + 1);
	CatChars::print(xDrawOffset, yDrawOffset+20, true, "A-Retry");
	CatChars::print(xDrawOffset, yDrawOffset+30, true, "b-menu");
}

void LevelRenderer::renderLevel(uint8_t xDrawOffset, uint8_t yDrawOffset, Game &game)
{
    auto &context = game.getGameContext();
    for (int j = 0; j < Map::mapHeight; j++)
    {
        for (int i = 0; i < Map::mapWidth; i++)
        {
            uint8_t tileIndex = toTileIndex(context.mapObject.getTile(i, j));
            Sprites::drawOverwrite((i * Map::tileSize) + xDrawOffset,(j * Map::tileSize) + yDrawOffset, Cats, tileIndex);
        }
    }
}

void LevelRenderer::renderPlayer(uint8_t xDrawOffset, uint8_t yDrawOffset, Game &game)
{
    auto &context = game.getGameContext();
    TileType type;
    switch (context.hero.lastDirection)
    {
    case Direction::Center:
        type = TileType::Cat_Head_Down;
        break;
    case Direction::Down:
        type = TileType::Cat_Head_Down;
        break;
    case Direction::Up:
        type = TileType::Cat_Head_Up;
        break;
    case Direction::Left:
        type = TileType::Cat_Head_Left;
        break;
    case Direction::Right:
        type = TileType::Cat_Head_Right;
        break;
    }
    Sprites::drawOverwrite((context.hero.x * Map::tileSize) + xDrawOffset, (context.hero.y * Map::tileSize) + yDrawOffset, Cats, toTileIndex(type));
}

//=============================================


void LevelRenderer::render(Game &game)
{
    renderLevel(0,0,game);
    renderPlayer(0,0,game);
    renderLevelData(70,20,game);
    renderStuck(70,0,game);
    if (game.getGameMode() == GameMode::Random)
    {
        renderRandData(64,0,game);
    }
}

