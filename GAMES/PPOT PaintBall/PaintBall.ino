#define ABG_IMPLEMENTATION
#define ABG_TIMER1
#define ABG_SYNC_PARK_ROW
#define SPRITESU_IMPLEMENTATION

#include "fxdta.h"
#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "fxdata/images/Tiles.h"
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
#define SYNTHU_NUM_CHANNELS 4
#define SYNTHU_UPDATE_EVERY_N_FRAMES 6
#define SYNTHU_ENABLE_SFX 1
#define SYNTHU_FX_READDATABYTES_FUNC FX::readDataBytes
#include "src/utils/SynthU.hpp"
#define ABG_TIMER1

Cookie cookie;
SoundSettings &soundSettings = cookie.soundSettings;
Player player;
Enemy enemies[Constants::EnemyCount];
Score scores[Constants::ScoreCount];
Particle particles[Constants::ParticlesMax];
GameState gameState = GameState::SplashScreen_Start;

uint8_t frameCount = 0;
uint8_t gridPosition = 0;
uint8_t grid[Constants::MapTileHeight][Constants::MapTileWidth];
Bubble bubbles[Constants::Bubble_Count];

bool particlesNeedRendering = false;
bool scoresNeedRendering = false;

uint8_t scorePerPass = 0;
uint8_t launchEnemyCounter_Max = 255;
uint8_t launchEnemyCounter = 255;
uint8_t hudCounter = 0;
uint8_t gameOverCounter = 0;
uint8_t scoreMax = 0;


void setup() {

    a.boot();
    //abg_detail::send_cmds_prog<0xDB, 0x20>();
    a.startGray();
    
    FX::begin(FX_DATA_PAGE, FX_SAVE_PAGE);
    FX::loadGameState(cookie/*(uint8_t*)&cookie, sizeof(cookie)*/);

    gameState = GameState::SplashScreen_Start;
   
    audioInit();
    setAudioOn();

    bubbles[0].setSize(Size::Large);
    bubbles[0].setX(12 * 16);
    bubbles[0].setY(12 * 16);
    bubbles[0].setDirection(Direction::NorthEast);
    bubbles[0].setSpeed(Speed::Slow);

    bubbles[1].setSize(Size::Small);
    bubbles[1].setX(24 * 16);
    bubbles[1].setY(17 * 16);
    bubbles[1].setDirection(Direction::NorthWest);
    bubbles[1].setSpeed(Speed::Medium);

    bubbles[2].setSize(Size::Small);
    bubbles[2].setX(89 * 16);
    bubbles[2].setY(37 * 16);
    bubbles[2].setDirection(Direction::SouthEast);
    bubbles[2].setSpeed(Speed::Fast);

    bubbles[3].setSize(Size::Medium);
    bubbles[3].setX(12 * 16);
    bubbles[3].setY(37 * 16);
    bubbles[3].setDirection(Direction::SouthWest);
    bubbles[3].setSpeed(Speed::Fast);

}


void loop() {

    //FX::enableOLED();
    a.waitForNextPlane();
    //FX::disableOLED();

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

    if (cookie.score > cookie.highScore) {

        cookie.highScore = cookie.score;

    }

    audioUpdate();

}
