#pragma once

#include "Arduboy2Ext.h"

#define EEPROM_START                  EEPROM_STORAGE_SPACE_START + 160
#define EEPROM_START_C1               EEPROM_START
#define EEPROM_START_C2               EEPROM_START + 1
#define EEPROM_TOP_START              EEPROM_START + 2

void EEPROM_update(uint16_t addr, uint8_t dat){
  EEPROM.write (addr, dat);
};


class EEPROM_Utils {

  public: 

    EEPROM_Utils(){};
        
    static void initEEPROM(bool forceClear);
    static uint16_t getScore();
    static void saveScore(uint16_t score);

};


/* ----------------------------------------------------------------------------
 *   Is the EEPROM initialised?
 *
 *   Looks for the characters 'M' and 'G' in the first two bytes of the EEPROM
 *   memory range starting from byte EEPROM_STORAGE_SPACE_START.  If not found,
 *   it resets the settings ..
 */

const uint8_t letter1 = 'M'; 
const uint8_t letter2 = 'G'; 

void EEPROM_Utils::initEEPROM(bool forceClear) {

	byte c1 = EEPROM.read(EEPROM_START_C1);
	byte c2 = EEPROM.read(EEPROM_START_C2);

	if (forceClear || c1 != letter1 || c2 != letter2) { 

		uint16_t score = 0;

		EEPROM_update(EEPROM_START_C1, letter1);
		EEPROM_update(EEPROM_START_C2, letter2);
		EEPROM.put(EEPROM_TOP_START, score);

	}
	
	EEPROM.commit();

}


/* -----------------------------------------------------------------------------
 *   Save score 
 */
void EEPROM_Utils::saveScore(uint16_t score) {

    EEPROM_update(EEPROM_TOP_START, score);
    EEPROM.commit();

}

/* -----------------------------------------------------------------------------
 *   Get score 
 */
uint16_t EEPROM_Utils::getScore() {

    uint16_t score = 0;
	EEPROM.get(EEPROM_TOP_START, score);

	return score;

}

