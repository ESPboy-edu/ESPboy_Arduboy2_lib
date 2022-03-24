#include "GameStateLoadLevel.h"

#include "../Game.h"

void GameStateLoadLevel::update(Game &game)
{
  switch (game.getGameMode())
  {
  case GameMode::Random:
    loadRandomLevel(game);
    break;
  default:
    loadStaticLevel(game);
    break;
  }
  game.setGameState(GameState::GamePlay);
}

void GameStateLoadLevel::render(Game &game)
{
}

void GameStateLoadLevel::loadStaticLevel(Game &game)
{
  LevelUtils::initPlayer(game);
  LevelUtils::copyStaticLevel(game);
}

bool GameStateLoadLevel::canMoveRandom(int8_t x, int8_t y, Direction currentDirection, uint8_t currentDepth, Game &game)
{
  auto &context = game.getGameContext();
  int8_t dx = 0;
  int8_t dy = 0;
  switch (currentDirection)
  {
  case Direction::Left:
    dx = -1;
    dy = 0;
    break;
  case Direction::Right:
    dx = 1;
    dy = 0;
    break;
  case Direction::Up:
    dx = 0;
    dy = -1;
    break;
  case Direction::Down:
    dx = 0;
    dy = 1;
    break;
  }

  bool inBounds = context.mapObject.InBounds(x + dx, y + dy);
  bool inBoundsNext = context.mapObject.InBounds(x + (dx * 2), y + (dy * 2));
  uint16_t currentTilePacked = context.mapObject.getTilePacked(x + dx, y + dy);
  uint16_t nextTilePacked = context.mapObject.getTilePacked(x + (dx * 2), y + (dy * 2));

  bool isUsed = PackedConverter::usedFromPacked(currentTilePacked);
  bool isNextUsed = PackedConverter::usedFromPacked(nextTilePacked);
  uint8_t nextDepth = PackedConverter::depthFromPacked(nextTilePacked);

  bool canMove = true;

  if (inBoundsNext&&isNextUsed&&!(nextDepth < currentDepth))
  {
    canMove = false;
  }

  return inBounds && !isUsed && canMove;
}

uint8_t GameStateLoadLevel::getNeighbourUsed(int8_t x, int8_t y, Game &game)
{
  auto &context = game.getGameContext();
  uint8_t count = 0;
  if (!PackedConverter::usedFromPacked(context.mapObject.getTilePacked(x - 1, y)))
    ++count;
  if (!PackedConverter::usedFromPacked(context.mapObject.getTilePacked(x + 1, y)))
    ++count;
  if (!PackedConverter::usedFromPacked(context.mapObject.getTilePacked(x, y - 1)))
    ++count;
  if (!PackedConverter::usedFromPacked(context.mapObject.getTilePacked(x, y + 1)))
    ++count;
  return count;
}

uint8_t GameStateLoadLevel::checkSteps(int8_t x, int8_t y, Direction direction, uint8_t depth, Game &game)
{
  uint8_t steps = 0;
  int8_t dx = 0;
  int8_t dy = 0;
  switch (direction)
  {
  case Direction::Left:
    dx = -1;
    dy = 0;
    break;
  case Direction::Right:
    dx = 1;
    dy = 0;
    break;
  case Direction::Up:
    dx = 0;
    dy = -1;
    break;
  case Direction::Down:
    dx = 0;
    dy = 1;
    break;
  }

  while (canMoveRandom(x, y, direction, depth + steps, game))
  {
    x += dx;
    y += dy;
    steps++;
  }
  return steps;
}

void GameStateLoadLevel::generateRandomLevel(Game &game)
{
  auto &context = game.getGameContext();
  randomSeed(context.currentSeed + 1);
  randomSeed(context.currentSeed);

  context.hero.x = random(8);
  context.hero.y = random(8);

  context.mapObject.reset(fromTileIndex(PackedConverter::toPackedValue(0, Direction::Center, false)));

  uint8_t Depth = 0;
  int8_t x = context.hero.x;
  int8_t y = context.hero.y;
  uint8_t direction = random(4) + 1;
  uint8_t previousDirection = direction;
  int8_t dx = 0;
  int8_t dy = 0;

  int8_t pdx = 0;
  int8_t pdy = 0;
  context.mapObject.setTile(x, y, fromTileIndex(PackedConverter::toPackedValue(Depth, static_cast<Direction>(direction), true)));
  while (true)
  {
    if (getNeighbourUsed(x, y, game) == 0)
    {
      break;
    }

    previousDirection = direction;
    pdx = dx;
    pdy = dy;

    do
    {
      direction = random(4) + 1;
      switch (direction)
      {
      case 1:
        dx = -1;
        dy = 0;
        break;
      case 2:
        dx = 1;
        dy = 0;
        break;
      case 3:
        dx = 0;
        dy = -1;
        break;
      case 4:
        dx = 0;
        dy = 1;
        break;
      }
    } while (canMoveRandom(x, y, static_cast<Direction>(direction), Depth, game) == false);

    int8_t magnitude = random(checkSteps(x, y, static_cast<Direction>(direction), Depth, game)) + 1;

    if (direction != previousDirection)
    {
      if (!PackedConverter::usedFromPacked(context.mapObject.getTilePacked(x + pdx, y + pdy)))
        context.mapObject.setTile(x + pdx, y + pdy, fromTileIndex(PackedConverter::toPackedValue(Depth, Direction::Center, true)));
    }

    for (; magnitude > 0; --magnitude)
    {
      if (canMoveRandom(x, y, static_cast<Direction>(direction), Depth, game))
      {
        ++Depth;
        x += dx;
        y += dy;
        context.mapObject.setTile(x, y, fromTileIndex(PackedConverter::toPackedValue(Depth, static_cast<Direction>(direction), true)));
      }
      else
      {
        break;
      }
    }
  }

  for (int j = 0; j < 8; j++)
  {
    for (int i = 0; i < 8; i++)
    {
      if (PackedConverter::usedFromPacked(toTileIndex(context.mapObject.getTile(i, j))) && PackedConverter::directionFromPacked(toTileIndex(context.mapObject.getTile(i, j))) != Direction::Center)
      {
        context.mapObject.setTile(i, j, TileType::Empty);
      }
      else
      {
        context.mapObject.setTile(i, j, TileType::Wall);
      }
    }
  }
}

void GameStateLoadLevel::loadRandomLevel(Game &game)
{
  LevelUtils::initPlayer(game);
  generateRandomLevel(game);
}