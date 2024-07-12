
#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "Card.h"
#include "Player.h"

struct Game {

        
    private:

        uint16_t frameCount = 0;
        uint8_t firstPlayer;
        uint8_t dealer;
        uint8_t winner;
        uint8_t pickUpCardIdx = 0;
        uint8_t cardCount = 10;

        uint8_t selecteCard = 255;
        uint8_t validCards[10];
        uint8_t playedCards[52];

        Card turnedCard;
        Card tableCards[4];
        Direction direction = Direction::Down;
        GameState prevGameState = GameState::SplashScreen_Start;

    public:

        Player players[4];
        Card cardsPile[52];

        uint8_t getCardCount()                          { return this->cardCount; }
        uint8_t getDealer()                             { return this->dealer; }
        uint8_t getWinner()                             { return this->winner; }
        uint8_t getFirstPlayer()                        { return this->firstPlayer; }
        uint8_t getSelectedCard()                       { return this->selecteCard; }
        uint8_t getPlayedCard(uint8_t idx)              { return this->playedCards[idx]; }
        uint8_t* getPlayedCards()                       { return this->playedCards; }
        bool getValidCard(uint8_t idx)                  { return this->validCards[idx]; }
        uint16_t getFrameCount()                        { return this->frameCount; }

        Card &getTurnedCard()                           { return this->turnedCard; }
        Card &getTableCard(uint8_t idx)                 { return this->tableCards[idx]; }
        Direction getDirection()                        { return this->direction; }
        GameState getPrevGameState()                    { return this->prevGameState; }

        void setCardCount(uint8_t val)                  { this->cardCount = val; }
        void setDealer(uint8_t val)                     { this->dealer = val; }
        void setWinner(uint8_t val)                     { this->winner = val; }
        void setFirstPlayer(uint8_t val)                { this->firstPlayer = val; }
        void setSelectedCard(uint8_t val)               { this->selecteCard = val; }
        void setPlayedCard(uint8_t idx,uint8_t val)     { this->playedCards[idx] = val; }
        void setValidCard(uint8_t idx, bool val)        { this->validCards[idx] = val; }
        void setDirection(Direction val)                { this->direction = val; }
        void setPrevGameState(GameState val)            { this->prevGameState = val; }
        void setFrameCount(uint16_t val)                { this->frameCount = val; }


        void incFrameCount() {

            this->frameCount++;

        }

        void resetFrameCount() {

            this->frameCount = 0;
            
        }

        void fillCardsPile() {

            uint8_t cardCounter = 0;

            pickUpCardIdx = 0;

            for (uint8_t i = 2; i < 15; ++i) {

                for (uint8_t j = 1; j < 5; ++j) {

                    this->cardsPile[cardCounter].setValue(static_cast<Values>(i));
                    this->cardsPile[cardCounter].setSuit(static_cast<Suits>(j));
                    cardCounter++;

                }

            }

        }

        void shuffleCardsPile() {

            uint8_t n = Constants::CardsInDeck;

            for (uint8_t i = 0; i < n - 1; i++) {

                uint8_t j = random(Constants::CardsInDeck);
                Card t = this->cardsPile[j];
                this->cardsPile[j] = this->cardsPile[i];
                this->cardsPile[i] = t;

            }

        }


        Card pickUpCard() {

            Card chosenCard;

            chosenCard.setValue(this->cardsPile[pickUpCardIdx].getValue());
            chosenCard.setSuit(this->cardsPile[pickUpCardIdx].getSuit());

            pickUpCardIdx++;

            return chosenCard;

        }


        void setup() {

            this->cardCount = 10;
            this->direction = Direction::Down;
            this->turnedCard.setSuit(Suits::None);

            for (uint8_t i = 0; i < Constants::PlayerCount; i++) {

                this->players[i].setPlayerNumber(i);

                for (uint8_t j = 0; j < 6; j++) {

                    this->players[i].setScores(j, 0);

                }

            }

        }

        uint8_t getTableCardCount() {

            uint8_t count = 0;

            if (this->tableCards[0].getSuit() != Suits::None) count++;
            if (this->tableCards[1].getSuit() != Suits::None) count++;
            if (this->tableCards[2].getSuit() != Suits::None) count++;
            if (this->tableCards[3].getSuit() != Suits::None) count++;
            
            return count;

        }

        uint8_t checkWinner(bool updateWinner) {    // Which player has won  the hand?
          
            uint8_t winner = this->firstPlayer;
            bool trumped = false;

            Suits playedSuit = this->tableCards[this->firstPlayer].getSuit();
            uint8_t highestValue = this->tableCards[this->firstPlayer].getValue();

            for (uint8_t i = 0; i < Constants::PlayerCount; i++) {

                if (i != this->firstPlayer) {
                    
                    Card &card = this->tableCards[i];

                    if (card.getSuit() != Suits::None) {

                        if (playedSuit == card.getSuit()) {

                            if (!trumped && (card.getValue() > highestValue)) {

                                highestValue = card.getValue();
                                winner = i;

                            }

                        }
                        else if (card.getSuit() == this->getTurnedCard().getSuit()) {

                            if (trumped) {

                                if (card.getValue() > highestValue) {
                                    highestValue = card.getValue();
                                    winner = i;
                                }
                            }
                            else {

                                highestValue = card.getValue();
                                winner = i;
                                trumped = true;
                                
                            }

                        }

                    }
                    else {

                        #ifdef DEBUG

                            if (updateWinner) {
                                DEBUG_PRINTLN("Error: Shouldn't get here");
                            }

                        #endif

                    }

                }

            }

            if (updateWinner) {

                this->players[winner].setTricks(this->players[winner].getTricks() + 1);
                this->winner = winner;

            }

            return winner;

        }

        bool cardIsHighestOnTable(Card &selectedCard) {     // Is the card nominated the highest on the table?

            uint8_t winner = checkWinner(false);

            if (winner == Constants::NoPlayer) return true;

            Card &card = this->tableCards[winner];

            if (card.getSuit() == selectedCard.getSuit()) {

                return selectedCard.getValue() > card.getValue();

            }
            else if (card.getSuit() == this->getTurnedCard().getSuit()) {
                
                return false;

            }
            else {
                
                return selectedCard.getSuit() == this->getTurnedCard().getSuit();

            }

        }

        void resetEOH() {

            this->firstPlayer = this->winner;
            this->winner = 0;

            for(uint8_t i = 0; i < Constants::PlayerCount; i++) {

                this->tableCards[i].setSuit(Suits::None);

            }

        }

        void resetEOR() {

            for(uint8_t i = 0; i < Constants::CardsInDeck; i++) {

                this->playedCards[i] = 0;

            }

        }

};
