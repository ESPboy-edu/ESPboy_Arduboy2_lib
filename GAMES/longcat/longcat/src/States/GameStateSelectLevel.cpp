#include "GameStateSelectLevel.h"

#include "../Game.h"

void GameStateSelectLevel::update(Game &game)
{
	auto &arduboy = game.getArduboy();
	auto &context = game.getGameContext();
	if (arduboy.justPressed(DOWN_BUTTON))
	{
		if (this->selection > 0)
			this->selection--;
		context.stage = this->selection;
		LevelUtils::copyStaticLevel(game);
	}

	else if (arduboy.justPressed(UP_BUTTON))
	{
		if (this->selection < maxLevel-1)
			this->selection++;
		context.stage = this->selection;
		LevelUtils::copyStaticLevel(game);
	}

	else if (arduboy.justPressed(A_BUTTON))
	{
		
		context.currentSeed = 0;
		context.stage = this->selection;
		this->selection = 0;
		game.setGameMode(GameMode::Linear);
		game.setGameState(GameState::LoadLevel);
	}

	else if (arduboy.justPressed(B_BUTTON))
	{
		this->selection = 0;
		game.setGameState(GameState::CampaignMenu);
	}
}

void GameStateSelectLevel::render(Game &game)
{
	const char *selectStage_s = "Stage:";
	uint8_t length = CatChars::length(selectStage_s);
	CatChars::print(0, 0, false, selectStage_s);
	CatChars::printBCD(length * 8, 0, false,this->selection + 1);
	LevelRenderer::renderLevel(32,8,game);
	LevelRenderer::renderPlayer(32,8,game);
}