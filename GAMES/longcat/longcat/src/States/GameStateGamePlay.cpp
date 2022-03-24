#include "GameStateGamePlay.h"

#include "../Game.h"

void GameStateGamePlay::update(Game &game)
{
	readPlayerInput(game);
}

void GameStateGamePlay::render(Game &game)
{
	LevelRenderer::render(game);
}

void GameStateGamePlay::readPlayerInput(Game &game)
{
	auto &arduboy = game.getArduboy();
	auto &context = game.getGameContext();
	if (arduboy.pressed(UP_BUTTON))
	{
		if (context.mapObject.getTile(context.hero.x, context.hero.y - 1) == TileType::Empty)
		{
			context.hero.dx = 0;
			context.hero.dy = -1;
			context.hero.lastDirection = context.hero.direction;
			context.hero.direction = Direction::Up;
			game.setGameState(GameState::Simulate);
		}
	}
	else if (arduboy.pressed(RIGHT_BUTTON))
	{
		if (context.mapObject.getTile(context.hero.x + 1, context.hero.y) == TileType::Empty)
		{
			context.hero.dx = 1;
			context.hero.dy = 0;
			context.hero.lastDirection = context.hero.direction;
			context.hero.direction = Direction::Right;
			game.setGameState(GameState::Simulate);
		}
	}
	else if (arduboy.pressed(DOWN_BUTTON))
	{
		if (context.mapObject.getTile(context.hero.x, context.hero.y + 1) == TileType::Empty)
		{
			context.hero.dx = 0;
			context.hero.dy = 1;
			context.hero.lastDirection = context.hero.direction;
			context.hero.direction = Direction::Down;
			game.setGameState(GameState::Simulate);
		}
	}
	else if (arduboy.pressed(LEFT_BUTTON))
	{
		if (context.mapObject.getTile(context.hero.x - 1, context.hero.y) == TileType::Empty)
		{
			context.hero.dx = -1;
			context.hero.dy = 0;
			context.hero.lastDirection = context.hero.direction;
			context.hero.direction = Direction::Left;
			game.setGameState(GameState::Simulate);
		}
	}
	else if (arduboy.pressed(B_BUTTON))
	{
		switch (game.getGameMode())
		{
		case GameMode::Fixed: 
		case GameMode::Linear: game.setGameState(GameState::CampaignMenu); break;
		case GameMode::Random: game.setGameState(GameState::RandomMenu); break;
		default:
			game.setGameState(GameState::MainMenu); break;
		}
		
	}
	else if (arduboy.justPressed(A_BUTTON))
	{
		game.setGameState(GameState::LoadLevel);
	}
}