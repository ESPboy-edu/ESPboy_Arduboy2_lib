#include "src/utils/Arduboy2Ext.h"
#include <Sprites.h>

#include "src/images/Images.h"
#include "src/puzzles/Puzzles.h"
#include "src/utils/Enums.h"
#include "src/utils/Utils.h"
#include "src/utils/EEPROM_Utils.h"
#include "src/entities/Entities.h"
#include "src/fonts/Font3x5.h"


#ifdef SOUNDS
#include <ArduboyTones.h>
#include "src/sounds/Sounds.h"
#endif


Arduboy2Ext arduboy;
Font3x5 font3x5 = Font3x5();

#ifdef SOUNDS
ArduboyTones sound(arduboy.audio.enabled);
#endif

GameState gameState = GameState::Splash_Init;
GameStats gameStats;
LevelSelectVars levelSelectVars;
Player player;
FallingTile fallingTiles[Constants::FallingTilesCount];
Arrow arrows[3];

uint8_t board[Constants::BoardHeight][Constants::BoardWidth];
Particle particles[Constants::ParticlesMax];

void setup() {

    arduboy.begin();
    arduboy.setFrameRate(30);
	//arduboy.flashlight();
	//arduboy.systemButtons();
    EEPROM_Utils::initEEPROM(false);

    #ifdef SOUNDS
    arduboy.audio.begin();
    #endif

}

void loop() {

    if (!arduboy.nextFrame()) return;    
	arduboy.pollButtons();

    switch (gameState) {

        case GameState::Splash_Init:
            splashScreen_Init();
            splashScreen();
            break;

        case GameState::Splash:
            splashScreen();
            break;

        case GameState::Title_Init:
            title_Init();
            title();
            break;

        case GameState::Title:
            title();
            break;

        case GameState::LevelSelect_Init:
            levelSelect_Init();
            levelSelect();
            break;

        case GameState::LevelSelect:
            levelSelect();
            break;

        case GameState::Instructions_Init:
            instructions_Init();
            instructions();
            break;

        case GameState::Instructions:
            instructions();
            break;

        case GameState::Game_Init:
            game_Init();
            game();
            arduboy.display(true);
            break;

        case GameState::Game:
            game();
            break;

    }

    arduboy.display(true);

}
