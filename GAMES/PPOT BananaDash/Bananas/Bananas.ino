#define ABG_IMPLEMENTATION
#define ABG_TIMER1
#define ABG_SYNC_PARK_ROW
#define SPRITESU_IMPLEMENTATION

#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Enums.h"
#include "fxdta.h"
#include "fxdata/fxdata.h"
#include "fxdata/images/Images.h"
#include "src/images/Images.h"
#include <ArduboyFX.h>  
#include "src/entities/Entities.h"
#include "src/utils/Stack.h"

#define SPRITESU_OVERWRITE
#define SPRITESU_PLUSMASK
#define SPRITESU_RECT
#define SPRITESU_FX

#include "src/utils/SpritesU.hpp"

extern ArduboyGBase_Config<ABG_Mode::L4_Triplane> a;
decltype(a) a;

#define SYNTHU_IMPLEMENTATION
#define SYNTHU_NUM_CHANNELS 4
#define SYNTHU_UPDATE_EVERY_N_FRAMES 3
#define SYNTHU_ENABLE_SFX 1
#define SYNTHU_FX_READDATABYTES_FUNC FX::readDataBytes
#include "src/utils/SynthU.hpp"
#define ABG_TIMER1

Cookie cookie;
SoundSettings &soundSettings = cookie.soundSettings;
World world;
uint8_t currentPlane;
uint8_t titleCounter = 0;
uint16_t directionCounter = 0;
uint8_t directionCounter_Left = 0;
uint8_t directionCounter_Right = 0;
uint16_t healthCounter = 0;
uint16_t healthFlash = 0;
uint16_t timeCounter = 0;
uint16_t frameCount = 0;
int8_t gameOverCount = -1;
int8_t timesUpCount = -1;
int8_t getReadyCount = -1;

// Rect gPlayerRect;
// Rect gEnemyRect;

void setup(void) {

    a.boot();
    a.startGray();
    
    FX::begin(FX_DATA_PAGE, FX_SAVE_PAGE);

    #ifndef DEBUG_SOUND
    audioInit();
    setAudioOn();
    #endif

}

void loop(void) {

    FX::enableOLED();
    a.waitForNextPlane(DARK_GRAY);
    FX::disableOLED();

    currentPlane = a.currentPlane();


	switch (world.getGameState()) {

		case GameState::SplashScreen_Start ... GameState::SplashScreen_End:
			splashScreen(a);
			break;

		case GameState::Title_Init:
			titleScreen_Init();
			[[fallthrough]];

		case GameState::Title_Start ... GameState::Title_End:
			titleScreen(a);
			break;

		case GameState::PlayGame_Init:
			playGame_Init();
			[[fallthrough]];

		case GameState::PlayGame_Start ... GameState::PlayGame_End:
			playGame(a);
			break;

		case GameState::HighScore_Init:
			highScore_Init();
			[[fallthrough]];

		case GameState::HighScore_Start ... GameState::HighScore_End:
			highScore(a);
			break;

		default: break;

	}

    #ifndef DEBUG_SOUND
    audioUpdate();
    #endif

}
