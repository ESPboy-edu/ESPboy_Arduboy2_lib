#pragma once

#define eeprom_update_byte EEPROM.write
#define eeprom_read_byte EEPROM.read

#include "Arduboy2Ext.h"
#include "Enums.h"


class EEPROM_Utils {

    public: 

        EEPROM_Utils() {};
        
        static void initEEPROM(bool forceClear);

};


/* ----------------------------------------------------------------------------
 *   Is the EEPROM initialised?
 *
 *   Looks for the characters 'B' and 'T' in the first two bytes of the EEPROM
 *   memory range starting from byte EEPROM_STORAGE_SPACE_START.  If not found,
 *   it resets the settings ..
 */
void EEPROM_Utils::initEEPROM(bool forceClear) {
    byte c1 = eeprom_read_byte((Constants::EEPROM_Char1));
    byte c2 = eeprom_read_byte((Constants::EEPROM_Char2));

    if (forceClear || c1 != 'B' || c2 != 'T') { 

        eeprom_update_byte((Constants::EEPROM_Char1), 'B');
        eeprom_update_byte((Constants::EEPROM_Char2), 'T');
        eeprom_update_byte((Constants::EEPROM_Level_Current), 0);
        eeprom_update_byte((Constants::EEPROM_Level_Max), 0);
        eeprom_update_byte((Constants::EEPROM_Tile_Set), 0);
        eeprom_update_byte((Constants::EEPROM_Zoom), 0);

        for (uint8_t x = 0; x < 50; x++) {
    
            eeprom_update_byte((Constants::EEPROM_Level_Rating + x), 0);

        }

    }
  EEPROM.commit();
}

