#include "GameStateMainMenu.h"

#include "../Game.h"

void GameStateMainMenu::update(Game &game)
{
	auto &arduboy = game.getArduboy();
	if (arduboy.justPressed(UP_BUTTON))
	{
		if (this->selection > 0)
			this->selection--;
	}

	if (arduboy.justPressed(DOWN_BUTTON))
	{
		if (this->selection < maxSelection)
			this->selection++;
	}

	if (arduboy.justPressed(A_BUTTON))
	{
		auto &context = game.getGameContext();
		switch (selection)
		{
		case 0:
			this->selection = 0;
			game.setGameState(GameState::CampaignMenu);
			break;
		case 1:
			this->selection = 0;
			game.setGameState(GameState::RandomMenu);
			break;
		case 2:
			this->selection = 0;
			game.setGameState(GameState::OptionsMenu);
			break;
		case 3:
			this->selection = 0;
			game.setGameState(GameState::CreditsMenu);
			break;
		}
	}

	if (arduboy.justPressed(B_BUTTON))
	{
		this->selection = 0;
		game.setGameState(GameState::SplashScreen);
	}
}

void GameStateMainMenu::render(Game &game)
{
	auto &arduboy = game.getArduboy();
	arduboy.fillScreen(WHITE);
	const char *linear_s = "  Campaign";
	const char *random_s = "  Random";
	const char *options_s = "  Options";
	const char *credits_s = "  Credits";
	const char *strings[] = {linear_s, random_s, options_s, credits_s};
	for (uint8_t i = 0; i < 4; i++)
	{
		CatChars::print(0, (i * 9) + 12, false, strings[i]);
		if (selection == i)
		{
			uint8_t len = CatChars::length(strings[i]);
			CatChars::drawChar((len)*8 + (((sin(arduboy.frameCount / 15.0f) + 1) / 2.0f) * 5) + 1, (this->selection * 9) + 12, false, 45);
		}
	}

	CatChars::menuDecoration();
}
