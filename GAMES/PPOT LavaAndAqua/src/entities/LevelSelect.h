
#pragma once

#include <Arduboy2.h>

struct LevelSelect {

    private:

        uint8_t selectedPuzzle = 0;
        uint8_t x = 0;
        uint8_t y = 0;
        uint8_t aCounter = 0;
        uint8_t bCounter = 0;

    public:

        uint8_t getSelectedPuzzleX() const                          { return this->selectedPuzzle; }
        uint8_t getX() const                                        { return this->x; }
        uint8_t getY() const                                        { return this->y; }
        uint8_t getACounter() const                                 { return this->aCounter; }
        uint8_t getBCounter() const                                 { return this->bCounter; }

        void setSelectedPuzzleX(uint8_t val)                        { this->selectedPuzzle = val; }
        void setX(uint8_t val)                                      { this->x = val; }
        void setY(uint8_t val)                                      { this->y = val; }
        void setACounter(uint8_t val)                               { this->aCounter = val; }
        void setBCounter(uint8_t val)                               { this->bCounter = val; }

        uint8_t getSelectedPuzzle() {

            return (this->y * 4) + this->x;
            
        }

        void increaseGame() {

            this->x++;

            if (this->x == 5) {
                this->x = 0;
                this->y++;
            }

        }

        void incACounter() {
            this->aCounter++;
        }

        void incBCounter() {
            this->bCounter++;
        }

};
