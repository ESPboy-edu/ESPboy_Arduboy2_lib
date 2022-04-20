#include "GameStateCampaignMenu.h"

#include "../Game.h"

void GameStateCampaignMenu::update(Game &game)
{
	auto &arduboy = game.getArduboy();
	if (arduboy.justPressed(UP_BUTTON))
	{
		if (this->selection > 0)
			this->selection--;
	}

	else if (arduboy.justPressed(DOWN_BUTTON))
	{
		if (this->selection < maxSelection)
			this->selection++;
	}

	else if (arduboy.justPressed(A_BUTTON))
	{
		auto &context = game.getGameContext();
		switch (selection)
		{
		case 0:
		{
			context.currentSeed = 0;
			Save saveValue = SaveUtil::get();
			context.stage = saveValue.lastStage;
			this->selection = 0;
			game.setGameMode(GameMode::Linear);
			game.setGameState(GameState::LoadLevel);
			break;
		}
		case 1:
		{
			Save saveValue = SaveUtil::get();
			saveValue.lastStage = 0;
			SaveUtil::update(saveValue);

			context.currentSeed = 0;
			context.stage = 0;
			this->selection = 0;
			game.setGameMode(GameMode::Linear);
			game.setGameState(GameState::LoadLevel);
			break;
		}
		case 2:
		{
			context.currentSeed = 0;
			context.stage = 0;
			this->selection = 0;
			LevelUtils::copyStaticLevel(game);
			game.setGameMode(GameMode::Fixed);
			game.setGameState(GameState::SelectLevel);
			break;
		}
		}
	}

	else if (arduboy.justPressed(B_BUTTON))
	{
		this->selection = 0;
		game.setGameState(GameState::MainMenu);
	}
}

void GameStateCampaignMenu::render(Game &game)
{
	auto &arduboy = game.getArduboy();
	arduboy.fillScreen(WHITE);

	const char *line_0 = "Campaign:";
	const char *line_1 = "  Continue";
	const char *line_2 = "  Start";
	const char *line_3 = "  Select Level";
	const char *strings[] = {line_0, line_1, line_2, line_3};
	for (uint8_t i = 0; i < 4; i++)
	{
		CatChars::print(0, (i * 9) + 12, false, strings[i]);
		if (selection == i - 1)
		{
			uint8_t len = CatChars::length(strings[i]);
			CatChars::drawChar((len)*8 + (((sin(arduboy.frameCount / 15.0f) + 1) / 2.0f) * 5) + 1, (this->selection * 9) + 12 + 8, false, 45);
		}
	}

	CatChars::menuDecoration();
}