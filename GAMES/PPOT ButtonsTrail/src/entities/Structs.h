#pragma once

#define eeprom_update_byte EEPROM.write
#define eeprom_read_byte EEPROM.read

#include "../utils/Utils.h"

struct GameStats {

    public: 

        GameStats() { };

        uint8_t level = 0;
        uint8_t maxLevel = 0;
        
        uint8_t titleSel = 0;
        int16_t xOffset = 0;
        uint8_t yOffset = 0;
        uint16_t moves = 0;
        uint8_t minimumMoves = 0;
        uint8_t instruction = 0;
        uint8_t start = 0;
        uint8_t exit = 0;
        uint8_t tileSet = 0;
        bool endOfGame = false;
        bool zoom = false;
        uint8_t zoomCount = 0;

        void reset() {

            uint8_t level = eeprom_read_byte((Constants::EEPROM_Level_Current));

            this->titleSel = level > 0 ? 1 : 0;

        }

};


struct LevelSelectVars {

    public: 

        LevelSelectVars() { };

        uint8_t cursor = 0;
        uint8_t cursorFlashTop = 0;
        uint8_t cursorFlashBottom = 0;
        uint8_t topRow = 0;

};