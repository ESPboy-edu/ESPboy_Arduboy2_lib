#pragma once

#include <Arduboy2.h>
#include "Constants.h"

#define eeprom_read_byte EEPROM.read
#define eeprom_update_byte EEPROM.write

class EEPROM_Utils {

    public: 

        EEPROM_Utils() {};
        
        static void initEEPROM(bool forceClear);
        static uint16_t getScore(uint8_t index);
        static uint8_t saveScore(uint16_t score);

};


/* ----------------------------------------------------------------------------
 *   Is the EEPROM initialised?
 */
void EEPROM_Utils::initEEPROM(bool forceClear) {

    byte c1 = eeprom_read_byte(/*reinterpret_cast<uint8_t *>*/(Constants::EEPROM_Char1));
    byte c2 = eeprom_read_byte(/*reinterpret_cast<uint8_t *>*/(Constants::EEPROM_Char2));

    if (forceClear || c1 != 'O' || c2 != 'B') { 

        eeprom_update_byte(/*reinterpret_cast<uint8_t *>*/(Constants::EEPROM_Char1), 'O');
        eeprom_update_byte(/*reinterpret_cast<uint8_t *>*/(Constants::EEPROM_Char2), 'B');

        for (uint8_t i = 0; i < 3; i++) {

            EEPROM.put(Constants::EEPROM_Scores + (i * 2), 10 - (i * 2));

        }
     EEPROM.commit();
    }

}

uint16_t EEPROM_Utils::getScore(uint8_t index) {

    uint16_t score = 0;
    EEPROM.get(Constants::EEPROM_Scores + (index * 2), score);

    return score;

}

uint8_t EEPROM_Utils::saveScore(uint16_t score) {

    if (score >= getScore(0)) {

        EEPROM.put(Constants::EEPROM_Scores + 4, getScore(1));
        EEPROM.put(Constants::EEPROM_Scores + 2, getScore(0));
        EEPROM.put(Constants::EEPROM_Scores, score);
        EEPROM.commit();
        return 0;

    }

    if (score >= getScore(1)) {

        EEPROM.put(Constants::EEPROM_Scores + 4, getScore(1));
        EEPROM.put(Constants::EEPROM_Scores + 2, score);
        EEPROM.commit();
        return 1;

    }

    if (score >= getScore(2)) {

        EEPROM.put(Constants::EEPROM_Scores + 4, score);
        EEPROM.commit();
        return 2;

    }

    return 255;

}
