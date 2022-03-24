#pragma once

enum class Direction : uint8_t
{
  Center,
  Left,
  Right,
  Up,
  Down,
};

static bool directionHorizontal(Direction direction)
{
  switch (direction)
  {
  case Direction::Left:
  case Direction::Right:
    return true;
  }
  return false;
}

static bool directionVertical(Direction direction)
{
  switch (direction)
  {
  case Direction::Up:
  case Direction::Down:
    return true;
  }
  return false;
}
