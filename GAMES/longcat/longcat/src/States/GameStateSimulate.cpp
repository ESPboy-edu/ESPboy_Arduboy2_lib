#include "GameStateSimulate.h"

#include "../Game.h"

void GameStateSimulate::update(Game &game)
{
  auto &arduboy = game.getArduboy();
  auto &context = game.getGameContext();
  if (arduboy.everyXFrames(5))
  {
    if (movePlayer(game) == false)
    {
      game.setGameState(GameState::GamePlay);
    }
  }

  if (LevelUtils::checkWin(game))
  {
    switch (game.getGameMode())
    {
    case GameMode::Linear:
      if (context.stage + 1 >= maxLevel)
      {
        game.setGameState(GameState::EndScreen);
      }
      else
      {
        nextStage(game);
        ++context.stage;
        Save saved = SaveUtil::get();
        if (context.stage > saved.lastStage)
        {
          saved.lastStage = context.stage;
          SaveUtil::update(saved);
        }
        game.setGameState(GameState::LoadLevel);
      }
      break;
    case GameMode::Random:
    {
      nextStage(game);
      ++context.stage;
      context.currentSeed = arduboy.generateRandomSeed();
      game.setGameState(GameState::LoadLevel);
    }
    break;
    case GameMode::Fixed:
      game.setGameState(GameState::EndScreen);
    }
  }
}

void GameStateSimulate::render(Game &game)
{
  LevelRenderer::render(game);
}

void GameStateSimulate::nextStage(Game &game)
{
  auto &arduboy = game.getArduboy();
  //arduboy.drawBitmap(63,0,thumbsup,64,64,WHITE);
  //arduboy.display();
  //arduboy.delayShort(1000);
}

void GameStateSimulate::placeCatPiece(int8_t x, int8_t y, Game &game)
{
  auto &context = game.getGameContext();
  switch (context.hero.direction)
  {
  case Direction::Left:
    switch (context.hero.lastDirection)
    {
    case Direction::Center:
      context.mapObject.setTile(x, y, TileType::Cat_Tail_Left);
      break;
    case Direction::Left:
      context.mapObject.setTile(x, y, TileType::Cat_Body_Horizontal);
      break;
    case Direction::Right:
      break;
    case Direction::Up:
      context.mapObject.setTile(x, y, TileType::Cat_Body_Down_Left);
      break;
    case Direction::Down:
      context.mapObject.setTile(x, y, TileType::Cat_Body_Up_Left);
      break;
    }
    break;
  case Direction::Right:
    switch (context.hero.lastDirection)
    {
    case Direction::Center:
      context.mapObject.setTile(x, y, TileType::Cat_Tail_Right);
      break;
    case Direction::Left:
      break;
    case Direction::Right:
      context.mapObject.setTile(x, y, TileType::Cat_Body_Horizontal);
      break;
    case Direction::Up:
      context.mapObject.setTile(x, y, TileType::Cat_Body_Down_Right);
      break;
    case Direction::Down:
      context.mapObject.setTile(x, y, TileType::Cat_Body_Up_Right);
      break;
    }
    break;
  case Direction::Up:
    switch (context.hero.lastDirection)
    {
    case Direction::Center:
      context.mapObject.setTile(x, y, TileType::Cat_Tail_Up);
      break;
    case Direction::Left:
      context.mapObject.setTile(x, y, TileType::Cat_Body_Up_Right);
      break;
    case Direction::Right:
      context.mapObject.setTile(x, y, TileType::Cat_Body_Up_Left);
      break;
    case Direction::Up:
      context.mapObject.setTile(x, y, TileType::Cat_Body_Vertical);
      break;
    case Direction::Down:
      break;
    }
    break;
  case Direction::Down:
    switch (context.hero.lastDirection)
    {
    case Direction::Center:
      context.mapObject.setTile(x, y, TileType::Cat_Tail_Down);
      break;
    case Direction::Left:
      context.mapObject.setTile(x, y, TileType::Cat_Body_Down_Right);
      break;
    case Direction::Right:
      context.mapObject.setTile(x, y, TileType::Cat_Body_Down_Left);
      break;
    case Direction::Up:
      break;
    case Direction::Down:
      context.mapObject.setTile(x, y, TileType::Cat_Body_Vertical);
      break;
    }
    break;
  }
}

bool GameStateSimulate::movePlayer(Game &game)
{
  auto &context = game.getGameContext();
  placeCatPiece(context.hero.x, context.hero.y, game);
  context.hero.lastDirection = context.hero.direction;
  int nextX = constrain(context.hero.x + context.hero.dx, 0, 7);
  int nextY = constrain(context.hero.y + context.hero.dy, 0, 7);
  if (context.mapObject.getTile(nextX, nextY) == TileType::Empty)
  {
    context.hero.x = nextX;
    context.hero.y = nextY;
    return true;
  }
  else
  {
    context.hero.dx = 0;
    context.hero.dy = 0;
	game.clickSound();
    return false;
  }
}
