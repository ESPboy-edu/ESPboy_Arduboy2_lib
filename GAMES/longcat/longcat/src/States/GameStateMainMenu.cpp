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
	const char *strings[] = {linear_s, random_s, options_s};
	for (uint8_t i = 0; i < 3; i++)
	{
		CatChars::print(0, (i * 9) + 12, false, strings[i]);
		if (selection == i)
		{
			uint8_t len = CatChars::length(strings[i]);
			CatChars::drawChar((len)*8 + (((sin(arduboy.frameCount / 15.0f) + 1) / 2.0f) * 5) + 1, (this->selection * 9) + 12, false, 45);
		}
	}

	const int loopLength = 80;
	if (arduboy.frameCount % (2 * loopLength) <= loopLength)
	{
		CatChars::print((arduboy.frameCount / 2 % loopLength), 2, false, "<  <  <  <");
	}
	else
	{
		CatChars::print(loopLength - (arduboy.frameCount / 2 % loopLength), 2, false, "<  <  <  <");
	}

	if (arduboy.frameCount % (2 * loopLength) <= loopLength)
	{
		CatChars::print(loopLength / 2 - (arduboy.frameCount / 2 % loopLength), 52, false, "<  <  <  <");
	}
	else
	{
		CatChars::print((arduboy.frameCount / 2 % loopLength) - loopLength / 2, 52, false, "<  <  <  <");
	}
}
