#pragma once

#include "TileTypes.h"
#include "../Utils/PackedConverter.h"

class Map
{
public:
  static const uint8_t mapWidth = 8;
  static const uint8_t mapHeight = 8;
  static const uint8_t tileSize = 8;

  TileType getTile(uint16_t i)
  {
    if (i > mapWidth * mapHeight)
      return TileType::Wall;
    return this->map[i];
  }

  TileType getTile(int8_t x, int8_t y)
  {
    if (NotInBounds(x, y))
      return TileType::Wall;
    return this->map[(y * mapWidth) + x];
  }

  void setTile(int8_t x, int8_t y, TileType tile)
  {
    if (NotInBounds(x, y))
      return;
    this->map[(y * mapWidth) + x] = tile;
  }

  constexpr bool NotInBounds(int8_t x, int8_t y)
  {
    return ((x < 0) || (x >= mapWidth) || (y < 0) || (y >= mapHeight));
  }

  constexpr bool InBounds(int8_t x, int8_t y)
  {
    return !NotInBounds(x, y);
  }

  void reset(TileType tile)
  {
    for (int j = 0; j < mapHeight; j++)
    {
      for (int i = 0; i < mapWidth; i++)
      {
        this->setTile(i, j, tile);
      }
    }
  }

  void replaceTile(TileType find, TileType replace)
  {
    for (int j = 0; j < mapHeight; j++)
    {
      for (int i = 0; i < mapWidth; i++)
      {
        if (getTile(i, j) == find)
          setTile(i, j, replace);
      }
    }
  }

  uint8_t tileCount(TileType find)
  {
    uint8_t count = 0;
    for (int j = 0; j < mapHeight; j++)
    {
      for (int i = 0; i < mapWidth; i++)
      {
        if (this->getTile(i, j) == find)
          count++;
      }
    }
    return count;
  }

  uint8_t getNeighbourTileCount(int8_t x, int8_t y, TileType type)
  {
    uint8_t count = 0;
    if (getTile(x - 1, y) == type)
      ++count;
    if (getTile(x + 1, y) == type)
      ++count;
    if (getTile(x, y - 1) == type)
      ++count;
    if (getTile(x, y + 1) == type)
      ++count;
    return count;
  }

  uint16_t getTilePacked(int8_t x, int8_t y)
  {
    if (NotInBounds(x, y))
      return PackedConverter::toPackedValue(0, Direction::Center, true);
    return toTileIndex(getTile(x, y));
  }

private:
  TileType map[mapWidth * mapHeight];
};
