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
        #ifndef MICROCADE
            a.setRGBled(RED_LED, 32);
        #else
            a.setRGBledGreenOff();
            a.setRGBledBlueOff();
        #endif
    }
    #endif

    FX::saveGameState(cookie);

    #ifdef USE_LED
    if (enableLEDs) {
        #ifndef MICROCADE
            a.setRGBled(RED_LED, 0);
            a.setRGBled(GREEN_LED, 32);
        #else
            a.setRGBledRedOff();
            a.setRGBledGreenOn();
        #endif
    }
    #endif

}


void deactivateLEDs() {

    #ifdef USE_LED
        #ifndef MICROCADE
            a.setRGBled(0, 0, 0);
        #else
            a.setRGBledRedOff();
            a.setRGBledGreenOff();
            a.setRGBledBlueOff();
        #endif
    #endif

}


void printPlayerHands() {
    
    #ifdef DEBUG

        DEBUG_PRINT("Player 0: ");
        for (uint8_t i = 0; i < 10; i++) {
            printCard(game.players[0].getCard(i));
            DEBUG_PRINT(" ");
        }
        DEBUG_PRINTLN(" ");

        DEBUG_PRINT("Player 1: ");
        for (uint8_t i = 0; i < 10; i++) {
            printCard(game.players[1].getCard(i));
            DEBUG_PRINT(" ");
        }
        DEBUG_PRINTLN(" ");

        DEBUG_PRINT("Player 2: ");
        for (uint8_t i = 0; i < 10; i++) {
            printCard(game.players[2].getCard(i));
            DEBUG_PRINT(" ");
        }
        DEBUG_PRINTLN(" ");

        DEBUG_PRINT("Player 3: ");
        for (uint8_t i = 0; i < 10; i++) {
            printCard(game.players[3].getCard(i));
            DEBUG_PRINT(" ");
        }
        DEBUG_PRINTLN(" ");

    #endif
    
}


void printPlayedCards() {

    #ifdef DEBUG
        
        DEBUG_PRINTLN("  2  3  4  5  6  7  8  9  10 J  Q  K  A");
        DEBUG_PRINT("S ");

        for (uint8_t i = 0; i < 13; i++) {
            DEBUG_PRINT(game.getPlayedCard(i) < 16 ? "0" : "");
            DEBUG_PRINT(game.getPlayedCard(i), HEX);
            DEBUG_PRINT(" ");

        }

        DEBUG_PRINTLN();
        DEBUG_PRINT("C ");

        for (uint8_t i = 13; i < 26; i++) {
            DEBUG_PRINT(game.getPlayedCard(i) < 16 ? "0" : "");
            DEBUG_PRINT(game.getPlayedCard(i), HEX);
            DEBUG_PRINT(" ");
        }

        DEBUG_PRINTLN();
        DEBUG_PRINT("D ");

        for (uint8_t i = 26; i < 39; i++) {
            DEBUG_PRINT(game.getPlayedCard(i) < 16 ? "0" : "");
            DEBUG_PRINT(game.getPlayedCard(i), HEX);
            DEBUG_PRINT(" ");
        }

        DEBUG_PRINTLN();
        DEBUG_PRINT("H ");

        for (uint8_t i = 39; i < 52; i++) {
            DEBUG_PRINT(game.getPlayedCard(i) < 16 ? "0" : "");
            DEBUG_PRINT(game.getPlayedCard(i), HEX);
            DEBUG_PRINT(" ");
        }

        DEBUG_PRINTLN();

    #endif

}


void printCard_Suit(Suits suit) {
    
    #ifdef DEBUG

        switch (suit) {

            case Suits::Spade:
                DEBUG_PRINT("S");
                break;

            case Suits::Club:
                DEBUG_PRINT("C");
                break;

            case Suits::Diamond:
                DEBUG_PRINT("D");
                break;

            case Suits::Heart:
                DEBUG_PRINT("H");
                break;
                
            case Suits::None:
                DEBUG_PRINT("_");
                break;
        }

   #endif

}


void printCard(Card &card) {
    
    #ifdef DEBUG

        switch (card.getValue()) {

            case 0 ... 10:
                DEBUG_PRINT(card.getValue());
                break;

            case 11:
                DEBUG_PRINT("J");
                break;

            case 12:
                DEBUG_PRINT("Q");
                break;
                
            case 13:
                DEBUG_PRINT("K");
                break;
                
            case 14:
                DEBUG_PRINT("A");
                break;

        }

        switch (card.getSuit()) {

            case Suits::Spade:
                DEBUG_PRINT("S");
                break;

            case Suits::Club:
                DEBUG_PRINT("C");
                break;

            case Suits::Diamond:
                DEBUG_PRINT("D");
                break;

            case Suits::Heart:
                DEBUG_PRINT("H");
                break;
                
            case Suits::None:
                DEBUG_PRINT("_");
                break;
        }

   #endif

}