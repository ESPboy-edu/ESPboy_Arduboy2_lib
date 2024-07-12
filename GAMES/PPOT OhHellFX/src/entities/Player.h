#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "Card.h"

struct Player {

    private:
    
        uint8_t playerNumber;
        uint8_t lastScore;
        uint16_t scores[6];
        uint8_t cardCount;
        uint8_t bid;
        uint8_t tricks;
        Card cards[10];

    public:

        uint8_t getPlayerNumber()                       { return this->playerNumber; }
        uint8_t getLastScore()                          { return this->lastScore; }
        uint16_t getScores(uint8_t idx)                 { return this->scores[idx]; }
        uint8_t getCardCount()                          { return this->cardCount; }
        uint8_t getBid()                                { return this->bid; }
        uint8_t getTricks()                             { return this->tricks; }
        Card &getCard(uint8_t idx)                      { return this->cards[idx]; }

        void setPlayerNumber(uint8_t val)               { this->playerNumber = val; }
        void setLastScore(uint8_t val)                  { this->lastScore = val; }
        void setScores(uint8_t idx, uint16_t val)       { this->scores[idx] = val; }
        void setBid(uint8_t val)                        { this->bid = val; }
        void setTricks(uint8_t val)                     { this->tricks = val; }
        void setCardCount(uint8_t val)                  { this->cardCount = val; }
        void setCard(uint8_t idx, Card &card)           { this->cards[idx] = card; }

        void addScore(uint8_t score) {

            for (int8_t i = 4; i >= 0; i--) {

                this->scores[i + 1] = this->scores[i];

            }

            this->scores[0] = this->scores[0] + score;
            this->lastScore = score;

        }

        void resetEOR() {

            this->bid = Constants::NoBid;
            this->tricks = 0;
            this->cardCount = 0;

            for (uint8_t i = 0; i < 10; i++) {

                this->cards[i].setSuit(Suits::None);

            }

        }

        void sort() {

            for (uint8_t i = 0; i < this->cardCount; i++) {

                for (uint8_t j = i + 1; j < this->cardCount; j++) {

                    if (this->cards[j].getSortValue() < cards[i].getSortValue()) {
                        Card card;
                        card.setSuit(cards[i].getSuit());
                        card.setValue(cards[i].getValue());

                        this->cards[i] = this->cards[j];
                        this->cards[j].setSuit(card.getSuit());
                        this->cards[j].setValue(card.getValue());
                    }
                    
                }

            }

            this->cardCount = 0;
            
            for (uint8_t i = 0; i < 10; i++) {

                if (this->cards[i].getSuit() != Suits::None) {
                    
                    this->cardCount++;

                }
                else {
                    
                    break;

                }

            }

        }


        void addCard(Card &card)   { 
            
            this->cards[cardCount] = card; 
            cardCount++;

        }

        bool hasSuit(Suits suit) {

            for (uint8_t i = 0; i < cardCount; i++) {

                if (cards[i].getSuit() == suit) return true;

            }

            return false;

        }

        uint8_t getInitialBid(Suits trumps) {

            uint8_t bid = 0;

            for (uint8_t i = 0; i < cardCount; i++) {

                if (cards[i].getSuit() == trumps) bid++;
                if (cards[i].getSuit() != trumps && cards[i].getValue() == 14) bid++;

            }

            return bid;

        }

        uint8_t getHighCard(Suits suit) {

            uint8_t highValue = 0;
            uint8_t returnCard = Constants::NoCard;

            for (uint8_t i = 0; i < cardCount; i++) {

                if (this->cards[i].getSuit() == suit && cards[i].getValue() > highValue) {
                    
                    highValue = cards[i].getValue();
                    returnCard = i;

                }

            }

            return returnCard;

        }

        uint8_t getLowCard() {

            uint8_t lowValue = 255;
            uint8_t returnCard = Constants::NoCard;

            for (uint8_t i = 0; i < this->cardCount; i++) {

                if (this->cards[i].getSortValue() < lowValue) {
                    
                    lowValue = this->cards[i].getSortValue();
                    returnCard = i;

                }

            }

            return returnCard;

        }

        uint8_t getLowCard(Suits suit) {

            uint8_t lowValue = 255;
            uint8_t returnCard = Constants::NoCard;

            for (uint8_t i = 0; i < cardCount; i++) {

                if (this->cards[i].getSuit() == suit && this->cards[i].getValue() < lowValue) {
                    
                    lowValue = this->cards[i].getValue();
                    returnCard = i;

                }

            }

            return returnCard;

        }

        uint8_t getCard(Suits suit, uint8_t value) {

            for (uint8_t i = 0; i < cardCount; i++) {

                if (suit != Suits::None) {

                    if (this->cards[i].getSuit() == suit && cards[i].getValue() == value) {
                        
                        return i;

                    }

                }
                else {

                    if (cards[i].getValue() == value) {
                        
                        return i;

                    }

                }

            }

            return Constants::NoCard;

        }


        uint8_t getBestCard(uint8_t *playedCards) {

            uint8_t bestIdx = Constants::NoCard;
            int8_t bestScore = -120;

            for (uint8_t i = 0; i < this->getCardCount(); i++) {


                // Skip the current card if the next in seq is the same suit ..

                if (i < this->getCardCount() - 1 && this->cards[i].getSuit() == this->cards[i + 1].getSuit()) {
                    continue;
                }


                // Check to see if all cards above this one are played ..

                uint8_t startCard = this->cards[i].getSortValue() + 1;
                uint8_t endCard = ((static_cast<uint8_t>(this->cards[i].getSuit())) * 13);
                uint8_t notPlayed = 0;
                uint8_t played = 0;

                for (uint8_t j = startCard; j < endCard; j++) {

                    if (playedCards[j] & 0x0f > 0) {

                        played++;
                        
                    }
                    else {

                        notPlayed++;
                    }

                }


                // New best card?

                if (this->cards[i].getValue() + played - notPlayed > bestScore) {

                    bestScore = this->cards[i].getValue() + played - notPlayed;
                    bestIdx = i;

                }

            }

            return bestIdx;

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

};
