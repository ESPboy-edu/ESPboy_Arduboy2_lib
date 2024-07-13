#define ABG_IMPLEMENTATION
#define ABG_TIMER1
#define ABG_SYNC_PARK_ROW
#define SPRITESU_IMPLEMENTATION

#include "fxdta.h"
#include <ArduboyFX.h>  
#include <FixedPointsCommon.h>
#include "src/utils/ArduboyG.h"
#include "src/utils/Enums.h"
#include "src/utils/Constants.h"
#include "fxdata/fxdata.h"
#include "fxdata/images/Images.h"
#include "src/entities/Entities.h"
#include "src/utils/Random.h"

#define SPRITESU_OVERWRITE
#define SPRITESU_PLUSMASK
#define SPRITESU_RECT
#define SPRITESU_FX

#include "src/utils/SpritesU.hpp"

extern ArduboyGBase_Config<ABG_Mode::L4_Triplane> a;
decltype(a) a;

#define SOUNDS_SYNTHU
#define SYNTHU_IMPLEMENTATION
#define SYNTHU_NUM_CHANNELS 6
#define SYNTHU_UPDATE_EVERY_N_FRAMES 3
#define SYNTHU_ENABLE_SFX 1
#define SYNTHU_FX_READDATABYTES_FUNC FX::readDataBytes
#include "src/utils/SynthU.hpp"
#define ABG_TIMER1

#include "src/fonts/Font3x5.h"

#include <stdio.h>
#include <stdlib.h>
#include "time.h"

Cookie cookie;
Game &game = cookie.game;
SoundSettings &soundSettings = cookie.soundSettings;
GameState gameState = GameState::SplashScreen_Start;

uint8_t titleCounter = 0;
int16_t instructions_Y = 0;

Font3x5 font3x5 = Font3x5();
DealPhysics dealPhysics;

#ifdef DEBUG_FIXED_RAND
uint16_t seed = 72;
#endif


void setup() {
    Serial.begin(74880);
    a.boot();
    //abg_detail::send_cmds_prog<0xDB, 0x20>();
    a.startGray();
    a.setFrameRate(350);
    FX::begin(FX_DATA_PAGE, FX_SAVE_PAGE);
    //FX::loadGameState((uint8_t*)&cookie, sizeof(cookie));
    FX::loadGameState(cookie);
    #ifndef DEBUG_SOUND
    audioInit();
    setAudioOn();
    #endif

    game.setFrameCount(0);

}

void loop() {
   static uint32_t soundtmr;
    if (millis() - soundtmr > 1000/250 ) { audioUpdate(); soundtmr=millis();}
    if(!a.nextFrame()) return;

    //FX::enableOLED();
    a.waitForNextPlane(BLACK);
    //FX::disableOLED();

    #ifdef DEBUG
        font3x5.setCursor(0, 0);
        font3x5.print((uint8_t)gameState);
    #endif


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
        case GameState::Pause_01:
        case GameState::Pause_02:
            play(a);
            break;


    }

    #ifndef DEBUG_SOUND
    //audioUpdate();
    #endif

}
