//#include <EEPROM.h>
#include <SPI.h>

#include "AstarokGame.h"
#include "src/images/Images.h"
#include "src/utils/Arduboy2Ext.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "src/utils/Structs.h"
#include "src/font/Font4x6.h"
//#include <ArduboyPlaytune.h>
#include <ESPboyPlaytune.h>

Arduboy2Ext arduboy;
boolean outEn() {return true;}
ESPboyPlaytune tunes(outEn);
//ArduboyPlaytune tunes(arduboy.audio.enabled);
AstarokGame game(&arduboy, &tunes);
GameState gameState = GameState::SplashScreen_Init;

TitleScreenVars titleScreenVars;
HighScoreVars highScoreVars;
IntroTextVars introTextVars;
SeedVars seedVars;

#ifndef DEBUG
 //   ARDUBOY_NO_USB 
#endif

void setup() {

    arduboy.boot();
    arduboy.setFrameRate(30);
	arduboy.flashlight();
	arduboy.systemButtons();

    #ifndef DEBUG
    arduboy.audio.begin();
    tunes.initChannel(PIN_SPEAKER_1);
    tunes.initChannel(PIN_SPEAKER_2);    
    #endif

    initEEPROM(false);

}

void loop() {

    if (!arduboy.nextFrame()) return;
    arduboy.pollButtons();

    switch (gameState) {

        case GameState::SplashScreen_Init:

            splashScreen_Init();
            splashScreen();
            arduboy.display(false);
            break;

        case GameState::SplashScreen:

            splashScreen();
            arduboy.display(false);
            break;

        case GameState::Title_Init:

            tunes.playScore(Sounds::Theme);
            gameState = GameState::Title;
            titleScreen();
            arduboy.display(true);
            break;

        case GameState::Title:

            titleScreen();
            arduboy.display(true);
            break;

        case GameState::IntroText_Init:

            introText_Init();
            introText();
            arduboy.display(true);
            break;

        case GameState::IntroText:

            introText();
            arduboy.display(true);
            break;

        case GameState::Seed_Init:

            seed_Init();
            seed();
            arduboy.display(true);
            break;

        case GameState::Seed:

            seed();
            arduboy.display(true);
            break;

        case GameState::Game_Init:

            game.newGame();
            gameState = GameState::Game_Play;
            game.cycle(gameState);
            game.draw();

            if (game.event == EventType::Death) {
                game.drawScorePanel();
            }

            arduboy.displayWithBackground(game.mapNumber % 2 ? MapLevel::AboveGround : MapLevel::BelowGround);
            break;

        case GameState::Game_Play:

            game.cycle(gameState);
            game.draw();

            if (game.event == EventType::Death) {
                game.drawScorePanel();
            }

            arduboy.displayWithBackground(game.mapNumber % 2 ? MapLevel::AboveGround : MapLevel::BelowGround);
            break;

        case GameState::Game_Mini:

            game.draw();
            game.playMiniGame(gameState);
            arduboy.displayWithBackground(game.mapNumber % 2 ? MapLevel::AboveGround : MapLevel::BelowGround);
            break;

        case GameState::HighScore_Check:

            highScoreVars.reset();
            checkHighScoreSlot(game.score + game.player.x / Constants::TileSize);

            if (highScoreVars.slot != Constants::EEPROM_No_Slot) {
                game.arduboy->resetFrameCount();
                gameState = GameState::HighScore_Flash;
            } 
            else {
                gameState = GameState::HighScore_NoFlash;
            }

            highScores();
            arduboy.display(true);
            break;

        case GameState::HighScore_Flash:
        case GameState::HighScore_NoFlash:

            highScores();
            arduboy.display(true);
            break;

    }

}
