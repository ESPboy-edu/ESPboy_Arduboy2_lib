#include "GameStateRandomMenu.h"

#include "../Game.h"

void GameStateRandomMenu::update(Game &game)
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
			context.currentSeed = arduboy.generateRandomSeed();
			context.stage = 0;
			this->selection = 0;
			game.setGameMode(GameMode::Random);
			game.setGameState(GameState::LoadLevel);
			break;
		}
		case 1:
		{
			context.currentSeed = 0;
			context.stage = 0;
			this->selection = 0;
			game.setGameMode(GameMode::Random);
			game.setGameState(GameState::SelectSeed);
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

void GameStateRandomMenu::render(Game &game)
{
	auto &arduboy = game.getArduboy();
	arduboy.fillScreen(WHITE);

	const char *line_0 = "Random:";
	const char *line_1 = "  Start";
	const char *line_2 = "  Select Seed";
	const char *strings[] = {line_0, line_1, line_2};
	for (uint8_t i = 0; i < 3; i++)
	{
		CatChars::print(0, (i * 9) + 12, false, strings[i]);
		if (selection == i - 1)
		{
			uint8_t len = CatChars::length(strings[i]);
			CatChars::drawChar((len)*8 + (((sin(arduboy.frameCount / 15.0f) + 1) / 2.0f) * 5) + 1, (this->selection * 9) + 12 + 8, false, 45);
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