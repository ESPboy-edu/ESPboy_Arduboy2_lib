#include "src/utils/Arduboy2Ext.h"
#include "src/utils/Constants.h"
#include "src/images/Images.h"

void seed_Init() {

    seedVars.reset();
    gameState = GameState::Seed;

}

void seed() {

    drawBackground();
    
    Sprites::drawSelfMasked(34, 8, Images::EnterRunes_Text, 0);
    Sprites::drawSelfMasked(24, 21, Images::EnterRunes_00, 0);
    Sprites::drawSelfMasked(24, 47, Images::EnterRunes_02, 0);
    Sprites::drawSelfMasked(96, 21, Images::EnterRunes_01, 0);
    Sprites::drawSelfMasked(96, 47, Images::EnterRunes_03, 0);

    arduboy.drawFastHLine(24, 21, 72);
    arduboy.drawFastHLine(24, 54, 72);
    arduboy.drawFastHLine(26, 56, 75);

    for (uint8_t i = 0; i < 5; i++) {

        if (i == seedVars.index && seedVars.spinIndex > 0) {
            Sprites::drawOverwrite(30 + (i* 14), 31, Images::Rune_Frame, 16 - seedVars.spinIndex);
        }
        else {
            Sprites::drawOverwrite(30 + (i* 14), 31, Images::Rune_Frame, 0);
            Sprites::drawOverwrite(33 + (i* 14), 35, Images::Runes, seedVars.seed[i]);
        }

    }

    Sprites::drawSelfMasked(33 + (seedVars.index * 14), 26, Images::ArrowUp, 0);
    Sprites::drawSelfMasked(33 + (seedVars.index* 14), 47, Images::ArrowDown, 0);

    uint8_t justPressed = arduboy.justPressedButtons();

    if (seedVars.spinIndex == 0) {

        if (justPressed & UP_BUTTON) {
            seedVars.incValue();
            tunes.playScore(Sounds::ButtonPress);
        }

        if (justPressed & DOWN_BUTTON) { 
            seedVars.decValue();
            tunes.playScore(Sounds::ButtonPress);
        }

        if (justPressed & LEFT_BUTTON) { 
            if (seedVars.decIndex()) tunes.playScore(Sounds::ButtonPress);
        }

        if (justPressed & RIGHT_BUTTON) { 
            if (seedVars.incIndex()) tunes.playScore(Sounds::ButtonPress);
        }

        if ((justPressed & A_BUTTON) || (justPressed & B_BUTTON)) {

            game.seed = seedVars.getSeedValue();
            for (uint8_t a = 0; a < Constants::GameSeeds; a++ ) {
                game.seeds[a] = hash(game.seed);
            }

            tunes.playScore(Sounds::ButtonPress);
            gameState = GameState::Game_Init;

        }

    }

    seedVars.decSpinIndex();

}

