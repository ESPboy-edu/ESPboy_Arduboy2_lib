#pragma once

#include "Constants.h"

struct TitleScreenVars {

    TitleScreenMode index = TitleScreenMode::Play;

    void reset() {

        this->index = TitleScreenMode::Play;

    }

};

struct IntroTextVars {

    uint8_t panel = 0;
    uint8_t index = 0;
    bool skipTypewriter = false;

    void reset() {

        this->panel = 0;
        this->index = 0;
        this->skipTypewriter = false;

    }

};

struct HighScoreVars {

    char seeds[5];
    uint16_t score;
    uint8_t index = 0;
    uint8_t slot = Constants::EEPROM_No_Slot;
    uint8_t resetCounter = 0;
    uint8_t spinIndex;
    uint8_t spinCountdown;

    void reset() {

        this->index = 0;
        this->resetCounter = 0;
        this->slot = Constants::EEPROM_No_Slot;
        this->spinCountdown = 100;

    }

    void decSpinIndex() {

        if (this->spinIndex > 0) {

            this->spinIndex--;
            
        }

        this->spinCountdown--;
        
    }


};

struct SeedVars {

    uint8_t seed[5];
    uint8_t index;
    uint8_t spinIndex;
    uint8_t arrowUp;
    uint8_t arrowDown;

    void incValue() {

        if (this->seed[this->index] == 5) {

            this->seed[this->index] = 0;
            
        }
        else {

            this->seed[this->index]++;

        }

        EEPROM.write(Constants::EEPROM_Seeds + this->index, this->seed[this->index]);
        this->spinIndex = 17;
        
    }

    void decValue() {

        if (this->seed[this->index] == 0) {

            this->seed[this->index] = 5;
            
        }
        else {

            this->seed[this->index]--;

        }

        EEPROM.write(Constants::EEPROM_Seeds + this->index, this->seed[this->index]);
        this->spinIndex = 17;
        
    }

    bool incIndex() {

        if (this->index < 4) {

            this->index++;
            return true;

        }

        return false;
        
    }

    bool decIndex() {

        if (this->index > 0) {

            this->index--;
            return true;
            
        }

        return false;
        
    }

    void decSpinIndex() {

        if (this->spinIndex > 0) {

            this->spinIndex--;
            
        }
        
    }

    void reset() {

        for (uint8_t i = 0; i < 5; i++) {
            this->seed[i] = static_cast<uint8_t>(EEPROM.read(Constants::EEPROM_Seeds + i));
        }

        this->arrowDown = 0;
        this->arrowUp = 0;
        this->index = 0;
        this->spinIndex = 0;
        
    }

    uint16_t getSeedValue() {

        return (this->seed[0] * 1296 + 
                this->seed[1] * 216 +
                this->seed[2] * 36 +
                this->seed[3] * 6 +
                this->seed[4]) + 1;
               
    }

};