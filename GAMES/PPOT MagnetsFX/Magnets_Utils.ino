#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "src/entities/Entities.h"
#include "src/utils/SpritesU.hpp"



uint8_t getJustPressedButtons() {

    a.pollButtons();

    return a.justPressedButtons();

}

uint8_t getPressedButtons() {

    return a.pressedButtons();

}

void saveCookie(bool enableLEDs) {

    #ifdef USE_LED
    if (enableLEDs) {
        a.digitalWriteRGB(BLUE_LED, RGB_ON);
    }
    #endif

    #ifdef USE_LED_NOT_BRIGHT
    if (enableLEDs) {
        a.setRGBled(BLUE_LED, 16);
    }
    #endif
    
    FX::saveGameState(cookie);

    #ifdef USE_LED
    if (enableLEDs) {
        LED_Counter = 32;
    }
    #endif

    #ifdef USE_LED_NOT_BRIGHT
    if (enableLEDs) {
        LED_Counter = 32;
    }
    #endif
}

void deactivateLEDs() {

    #ifdef USE_LED
        a.digitalWriteRGB(RED_LED, RGB_OFF);
        a.digitalWriteRGB(GREEN_LED, RGB_OFF);
        a.digitalWriteRGB(BLUE_LED, RGB_OFF);
    #endif

    #ifdef USE_LED_NOT_BRIGHT
        a.setRGBled(RED_LED, 0);
        a.setRGBled(GREEN_LED, 0);
        a.setRGBled(BLUE_LED, 0);
    #endif
}

void printPuzzle() {

    #ifdef DEBUG

    DEBUG_PRINTLN(F("Puzzle ----------------------"));

    for (uint8_t y = 1; y < game.getHeight() + 1; y++) {

        for (uint8_t x = 1; x < game.getWidth() + 1; x++) {
        
            DEBUG_PRINT(game.puzzle[y][x]);
            DEBUG_PRINT(" ");

        }

        DEBUG_PRINTLN();
    }
    #endif

}

void printSolution() {

    #ifdef DEBUG

    DEBUG_PRINTLN(F("Solution ----------------------"));

    for (uint8_t y = 1; y < game.getHeight() + 1; y++) {

        for (uint8_t x = 1; x < game.getWidth() + 1; x++) {
        
            DEBUG_PRINT(game.solution[y][x]);
            DEBUG_PRINT(" ");

        }

        DEBUG_PRINTLN();
    }
    #endif

}