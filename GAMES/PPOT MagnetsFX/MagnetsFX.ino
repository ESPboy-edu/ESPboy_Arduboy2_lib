#include "fxdta.h"

#define ABG_IMPLEMENTATION
#define ABG_TIMER3
#define ABG_SYNC_PARK_ROW
#define SPRITESU_IMPLEMENTATION

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
#define ABG_PLANE_CONTRAST

#include "src/utils/SpritesU.hpp"

extern ArduboyGBase_Config<ABG_Mode::L4_Triplane> a;
decltype(a) a;

#include "src/fonts/Font3x5.h"

#include <stdio.h>
#include <stdlib.h>
#include "time.h"

Cookie cookie;
Game &game = cookie.game;
SoundSettings &soundSettings = cookie.soundSettings;
GameState gameState = GameState::SplashScreen_Start;

uint16_t frameCount = 0;
Hint hint;
Font3x5 font3x5 = Font3x5();
bool particlesNeedRendering = false;

#ifdef PARTICLES
Particle particles[Constants::ParticlesMax];
#endif

#ifdef DEBUG_FIXED_RAND
uint16_t seed = 72;
#endif

#ifdef USE_LED
uint8_t LED_Counter = 0;
#endif

#ifdef USE_LED_NOT_BRIGHT
uint8_t LED_Counter = 0;
#endif


void setup() {

    a.boot();
    //abg_detail::send_cmds_prog<0xDB, 0x20>();
    a.startGray();
    
    FX::begin(FX_DATA_PAGE, FX_SAVE_PAGE);
    FX::loadGameState((uint8_t*)&cookie, sizeof(cookie));
        
    #ifdef USE_LED
        deactivateLEDs();
    #endif
      
    #ifdef USE_LED_NOT_BRIGHT
        a.setRGBled(0, 0, 0);
        deactivateLEDs();
    #endif

    frameCount = 0;
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


    }

    #ifdef USE_LED

        if (LED_Counter > 0) {

            LED_Counter--;

            if (LED_Counter == 0) {
                deactivateLEDs();             
            }

        }
        
    #endif

    #ifdef USE_LED_NOT_BRIGHT

        if (LED_Counter > 0) {

            LED_Counter--;

            if (LED_Counter == 0) {
                deactivateLEDs();             
            }

        }
        
    #endif

    if (hint.count > 0) {

        hint.count--;
    
        if (hint.count == 0) {

            gameState = GameState::Play;

        }

    }

}
