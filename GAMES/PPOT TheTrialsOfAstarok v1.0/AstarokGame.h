#pragma once

//#include <ArduboyPlaytune.h>
#include <ESPboyPlaytune.h>
#include "src/utils/Arduboy2Ext.h"
#include "src/data/SpriteData.h"
#include "src/images/Images.h"
#include "src/sounds/Sounds.h"
#include "src/utils/Enums.h"
#include "src/utils/Constants.h"
#include "src/utils/Utils.h"

#include "src/entities/Sprite.h"
#include "src/entities/AISprite.h"
#include "src/entities/InteractiveObject.h"
#include "src/entities/Room.h"
#include "src/entities/Map.h"

#include "src/entities/Particle.h"

class AstarokGame {

    public:  // Constructor
//        AstarokGame(Arduboy2Ext *arduboy, ArduboyPlaytune *tunes);
        AstarokGame(Arduboy2Ext *arduboy, ESPboyPlaytune *tunes);
        
    public:  // Variables
        EventType             event;
        uint8_t               eventCounter;
        byte                  seeds[Constants::GameSeeds];
        uint8_t               lives;
        int16_t               score;
        uint16_t              mapNumber;
        Point                 camera;

        InteractiveObject     * chestObj;
        Direction             ballDirection;
        uint8_t               ballX;
        uint8_t               ballIdx;
        uint8_t               ballDelay;

        Sprite                player;
        AISprite              mobs[Constants::SpriteCap];
        Map                   level;
        Arduboy2Ext           * arduboy;
        //ArduboyPlaytune       * tunes;
        ESPboyPlaytune       * tunes;
        uint32_t              seed;

    private:

        #ifdef PAUSE
        uint8_t               pauseCount = 0;  
        bool                  pause = false;
        #endif

        bool                  collideWithChest = false;
        Particle particles[Constants::ParticlesMax];


    public:  // Methods

        void newGame();
        void cycle(GameState &gameState);
        void draw();
        uint8_t addMob(const uint8_t *data, const uint8_t *sprite, const uint8_t *mask, int8_t x, uint8_t y, uint8_t maxHeight = 255);
        uint8_t getSpareMobCount();
        void drawScorePanel();
        void playMiniGame(GameState &gameState);

    private:  // Methods

        void startLevel();
        bool testCollision(Sprite * player, AISprite * sprite);
        void die(GameState &gameState);
        void drawMap_Background();
        void drawMap_Background_2();
        void drawMap_Foreground();
        void drawHUD();
        void drawPlayer();
        void drawMobs();
        void adjustCamera();
        void processButtons();
        uint8_t spawnY();

        #ifdef PAUSE
        void renderPause();
        #endif

};
