#pragma once

enum class TileType : uint16_t
{
  Empty,
  Wall,
  Cat_Head_Down,
  Cat_Head_Right,
  Cat_Head_Left,
  Cat_Head_Up,
  Cat_Tail_Up,
  Cat_Tail_Down,
  Cat_Tail_Left,
  Cat_Tail_Right,
  Cat_Body_Horizontal,
  Cat_Body_Vertical,
  Cat_Body_Up_Left,
  Cat_Body_Up_Right,
  Cat_Body_Down_Left,
  Cat_Body_Down_Right,
};

constexpr uint16_t toTileIndex(TileType tile)
{
  return static_cast<uint16_t>(tile);
}

constexpr TileType fromTileIndex(uint16_t tileIndex)
{
  return static_cast<TileType>(tileIndex);
}
