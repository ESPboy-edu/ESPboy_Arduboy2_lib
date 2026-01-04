#define ABG_IMPLEMENTATION
#define ABG_TIMER1
#define ABG_SYNC_PARK_ROW
#define SPRITESU_IMPLEMENTATION

#include <ArduboyFX.h>  
#include <FixedPointsCommon.h>
#include "fxdta.h"
#include "src/utils/ArduboyG.h"
#include "src/utils/Enums.h"
#include "src/utils/Constants.h"
#include "src/entities/Game.h"
#include "fxdata/fxdata.h"
#include "fxdata/images/Images.h"
#include "src/utils/Random.h"
#include "src/entities/Cookie.h"
#include "src/entities/Puff.h"
#include "src/entities/LevelSelect.h"
#include "src/entities/PopoutMenu.h"

#define SPRITESU_OVERWRITE
#define SPRITESU_PLUSMASK
#define SPRITESU_RECT
#define SPRITESU_FX

#include "src/utils/SpritesU.hpp"

extern ArduboyGBase_Config<ABG_Mode::L4_Triplane> a;
decltype(a) a;

#include <stdio.h>
#include <stdlib.h>
#include "time.h"

Cookie cookie;
Game &game = cookie.game;
GameState gameState = GameState::SplashScreen_Start;
GameState nextGameState = GameState::SplashScreen_Start;
bool doIncLava = false;
uint8_t titleCounter = 0;
LevelSelect levelSelect;
PopoutMenu popoutMenu;
Puff puff;

void setup() {

    a.boot();
    //abg_detail::send_cmds_prog<0xDB, 0x20>();
    a.startGray();
    
    FX::begin(FX_DATA_PAGE, FX_SAVE_PAGE);
    game.setFrameCount(0);
    a.initRandomSeed();

    if (!cookie.isInitialised) {

        cookieReset();
        cookie.isInitialised = true;
        saveCookie();
        
    }
    
}

void loop() {

    FX::enableOLED();
    a.waitForNextPlane(BLACK);
    FX::disableOLED();

    switch (gameState) {

        case GameState::SplashScreen_Start ... GameState::SplashScreen_End:
            splashScreen(a);
            break;

        case GameState::Title_Init:
            title_Init();
            [[fallthrough]];

        case GameState::Title_Start ... GameState::Title_End:
            title(a);
            break;

        case GameState::Play_Init:
            play_Init();
            [[fallthrough]];

        case GameState::Play_Start ... GameState::Play_End:
            play(a);
            break;

        case GameState::GameOver_Init:
            gameOver_Init();
            [[fallthrough]];

        case GameState::GameOver_Main ... GameState::GameOver_Leave:
            gameOver(a);
            break;

    }

}
