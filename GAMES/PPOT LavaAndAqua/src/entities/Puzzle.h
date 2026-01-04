#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"

struct Puzzle {

    private:

        PuzzleStatus status = PuzzleStatus::Locked;
        uint16_t numberOfMoves;

    public:

        uint16_t getNumberOfMoves() const               { return this->numberOfMoves; }
        PuzzleStatus getStatus() const                  { return this->status; }

        void setNumberOfMoves(uint16_t val)             { this->numberOfMoves = val; }
        void setStatus(PuzzleStatus val)                { this->status = val; }

};