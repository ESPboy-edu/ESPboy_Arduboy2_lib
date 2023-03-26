#pragma once

#include "../utils/Arduboy2Ext.h"
#include "../utils/Constants.h"

struct GamePlay {

    GameState gameState = GameState::SplashScreen_Init;
    
    uint16_t frameCount = 0;
    uint8_t grab = 3;
    uint8_t level = 1;
    uint8_t timer_Sec = 0;
    uint8_t timer_Min = 60;
    uint8_t saves = 0;

    uint8_t crouchTimer = 0;
    uint8_t timeRemaining = 0;

    uint8_t startOfLevelHealth = 3;
    uint8_t startOfLevelHealthMax = 3;


    void init(uint8_t level) {

        this->frameCount = 0;

        this->level = level;
        this->timer_Sec = 59;
        this->timer_Min = 59;
        this->saves = 0;

        this->timeRemaining = 144;
        this->gameState = GameState::Game;

    }

    void restartLevel() {

        this->frameCount = 0;

        this->timeRemaining = 144;
        this->gameState = GameState::Game;

    }

    bool update(Arduboy2Ext & arduboy) {

        #ifndef SAVE_MEMORY_OTHER
        if (gameState != GameState::Menu && (arduboy.getFrameCount() - this->frameCount) % Constants::FrameRate == 0) {
        #else
        if ((arduboy.getFrameCount() - this->frameCount) % Constants::FrameRate == 0) {
        #endif

            if (this->timer_Sec == 0) {
                if (this->timer_Min > 0) {
                    this->timer_Min--;
                    this->timer_Sec = 59;
                }
            }
            else {
                this->timer_Sec--;
            }

        }

        if (this->timeRemaining > 0) this->timeRemaining--;

        return (this->timer_Min == 0 && this->timer_Sec == 0);

    }

    void incLevel() {

        level++;

    }

    bool isGameOver() {

        if (this->timer_Min > 0) return false;
        if (this->timer_Sec > 0) return false;

        return true;
        
    }

    uint8_t getGrab() {

        this->grab++;

        if (this->grab == 4) this->grab = 0;

        return this->grab;
        
    }

};
