#include "GameStateSelectSeed.h"

#include "../Game.h"

void GameStateSelectSeed::update(Game &game)
{
    auto &arduboy = game.getArduboy();
	if (arduboy.justPressed(LEFT_BUTTON))
	{
		if (this->selection > 0)
			this->selection--;
	}

	else if (arduboy.justPressed(RIGHT_BUTTON))
	{
		if (this->selection < CatKeyLength)
			this->selection++;
	}

    else if (arduboy.justPressed(DOWN_BUTTON))
	{
		if (this->key.key[this->selection] > 0)
			this->key.key[this->selection]--;
	}

	else if (arduboy.justPressed(UP_BUTTON))
	{
		if (this->key.key[this->selection] < 15)
			this->key.key[this->selection]++;
	}

	else if (arduboy.justPressed(A_BUTTON))
	{
		auto &context = game.getGameContext();
		context.currentSeed = RandKey::fromCatKey(this->key);
        this->key = RandKey::toCatKey(0);
		this->selection = 0;
		game.setGameMode(GameMode::Random);
		game.setGameState(GameState::LoadLevel);
	}

	else if (arduboy.justPressed(B_BUTTON))
	{
		this->selection = 0;
        this->key = RandKey::toCatKey(0);
		game.setGameState(GameState::RandomMenu);
	}
}

void GameStateSelectSeed::render(Game &game)
{
    uint8_t cx = 32;
    uint8_t cy = 16;

    for(uint8_t i = 0; i < CatKeyLength; i++){
        if(this->selection == i){
            CatChars::drawChar(cx + (i * 8),cy,true,43);
            CatChars::drawChar(cx + (i * 8),cy+8,true,this->key.key[i]+2);
            CatChars::drawChar(cx + (i * 8),cy+16,true,44);
        }
        else{
            CatChars::drawChar(cx + (i * 8),cy,true,47);
            CatChars::drawChar(cx + (i * 8),cy+8,true,this->key.key[i]+2);
            CatChars::drawChar(cx + (i * 8),cy+16,true,48);
        }
    }
}