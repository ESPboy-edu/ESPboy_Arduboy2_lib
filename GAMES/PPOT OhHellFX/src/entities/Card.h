#pragma once

#include <Arduboy2.h>
#include "../utils/Enums.h"

struct Card {

    private:

        Suits suit;
        Values value;
        bool playerCard;
        bool winningCard;

    public:

        Suits getSuit()                         { return this->suit; }
        Values getValue()                       { return this->value; }
        bool isPlayerCard()                     { return this->playerCard; }
        bool isWinningCard()                    { return this->winningCard; }

        void setSuit(Suits val)                 { this->suit = val; }
        void setValue(Values val)               { this->value = val; }
        void setIsPlayerCard(bool val)          { this->playerCard = val; }
        void setIsWinningCard(bool val)         { this->winningCard = val; }

        uint8_t getCardIndex() {

            return (3 * (((static_cast<uint8_t>(this->getSuit()) - 1) * 13) + (this->getValue() - 2)));
            
        }

        uint8_t getSortValue() {
            
            if (this->getSuit() == Suits::None) { return 255;}

            return ((static_cast<uint8_t>(this->getSuit()) - 1) * 13) + (this->getValue() - 2);

        }

} card;
