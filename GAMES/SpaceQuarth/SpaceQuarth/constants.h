/*
 * The MIT License
 *
 * Copyright 2017 Schuemi.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* 
 * File:   constants.h
 * Author: Schuemi
 *
 * Created on 10. Dezember 2017, 21:02
 */


#ifndef CONSTANTS_H
#define CONSTANTS_H

enum GAME_MODE {
    GAME_MODE_EASY = 0,
    GAME_MODE_NORMAL,
    GAME_MODE_HARD,
    GAME_MODE_NONE
    
};
#define VERSION_MAJOR "1"
#define VERSION_MINOR "2"

#define GAMEFIELD_X 35
#define GAMEFIELD_BRICKS_WIDTH 16
#define GAMEFIELD_BRICKS_HEIGHT 26 // you can't see the first 5 Bricks
#define MAX_SHOOTS_AT_ONCE 4
#define MAX_STARS 10

#define EEPROM_MAGIC_BYTE 0 // the byte, where the magic starts (after EEPROM_STORAGE_SPACE_START)
#define EEPROM_HIGHSCORE_BYTE 20 // the byte, where the highscore starts (after EEPROM_STORAGE_SPACE_START)
#define EEPROM_HIGHSCORE_SIZE 48 // highscore SIze in bytes
#define EEPROM_AUTOSAVE_BYTE 19
#define EEPROM_GAME_CLEARED_BYTE 18 // here we save, if the user managed to clear all levels. After this he can choose the level in the dashboard
#define EEPROM_GAME_MODE_BYTE 17 // the Game Mode Easy / Middle / Hard

#define EEPROM_GAMEDATA_BYTE EEPROM_HIGHSCORE_BYTE + EEPROM_HIGHSCORE_SIZE// the byte where the gamedata starts (gamedata size: 450 Bytes)
#define EEPROM_MAGIC 0xAAEE // If I find this magic in the EEPROM, I load the data from it




#endif /* CONSTANTS_H */
