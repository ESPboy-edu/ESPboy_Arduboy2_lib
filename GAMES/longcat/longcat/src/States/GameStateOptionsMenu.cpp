#include "GameStateCredits.h"

#include "../Game.h"
#include "../Utils/CatChars.h"

void GameStateOptionsMenu::update(Game &game)
{
  auto &arduboy = game.getArduboy();
  if (arduboy.justPressed(B_BUTTON))
  {
    game.setGameState(GameState::MainMenu);
  }

	if (arduboy.justPressed(UP_BUTTON))
	{
		if (this->selection > 0)
			this->selection--;
	}

	else if (arduboy.justPressed(DOWN_BUTTON))
	{
		if (this->selection < maxSelection)
			this->selection++;
	}else if (arduboy.justPressed(A_BUTTON))
	{
		auto &context = game.getGameContext();
		switch (selection)
		{
			case 0:
			{

				if (arduboy.audio.enabled())
				{
					arduboy.audio.off();
				}
				else
				{
					arduboy.audio.on();
				}
				arduboy.audio.saveOnOff();
				ArduboyTones::tone(NOTE_CS6, 30);

				this->selection = 0;
				break;
			}
		}
	}

}

void GameStateOptionsMenu::render(Game &game)
{
  auto &arduboy = game.getArduboy();
  auto &context = game.getGameContext();
  arduboy.fillScreen(WHITE);

	const char *line_0 = "Options:";
	const char *line_1 = "  Sound ON";
	const char *line_2 = "  Sound OFF";
	const char *strings[] = {line_0, line_1};
	if(!arduboy.audio.enabled()){
		strings[1] = line_2;
	}
	for (uint8_t i = 0; i < 2; i++)
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
