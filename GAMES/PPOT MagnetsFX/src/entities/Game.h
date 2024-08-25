
#pragma once

#include <ArduboyFX.h>  
#include "../utils/Constants.h"
#include "../utils/Enums.h"

struct Game {

    public:

        uint8_t puzzle[11][12];
        uint8_t solution[11][12];
        uint8_t reset[11][12];
        
    private:

        uint8_t widths[3] = { 6, 8, 10 };
        uint8_t heights[3] = { 5, 7, 9 };
        uint8_t spacings[3] = { 9, 6, 6 };

        uint8_t x = 1;
        uint8_t y = 1;
        bool complete = false;

        GameSize gameSize = GameSize::Small;
        GameLevel gameLevel = GameLevel::Easy;
        RenderSize renderSize = RenderSize::Large;

    public:

        uint8_t getWidth()                              { return this->widths[this->gameSize]; }
        uint8_t getHeight()                             { return this->heights[this->gameSize]; }
        uint8_t getX()                                  { return this->x; }
        uint8_t getY()                                  { return this->y; }
        bool getComplete()                              { return this->complete; }
        GameSize getGameSize()                          { return this->gameSize; }
        GameLevel getGameLevel()                        { return this->gameLevel; }

        void setX(uint8_t val)                          { this->x = val; }
        void setY(uint8_t val)                          { this->y = val; }
        void setComplete(bool val)                      { this->complete = val; }
        void setGameSize(GameSize val)                  { this->gameSize = val; }
        void setGameLevel(GameLevel val)                { this->gameLevel = val; }
        void setRenderSize(RenderSize val)              { this->renderSize = val; }


        RenderSize getRenderSize() { 

            switch (this->renderSize) {

                case RenderSize::Small:
                case RenderSize::Large:
                    return this->renderSize;

                default:

                    switch (this->getGameSize()) {

                        case GameSize::Small:
                        case GameSize::Medium:
                            return RenderSize::Large;

                        case GameSize::Large:
                            return RenderSize::Small;

                    }

                    break;

            }            
            
            return RenderSize::Small;

        }

        uint8_t getSpacing() { 

            switch (this->getRenderSize()) {

                case RenderSize::Small:
                    return 6;

                case RenderSize::Large:
                    return 9;

                default:

                    switch (this->getGameSize()) {

                        case GameSize::Small:
                            return 9;

                        case GameSize::Medium:
                            return 9;

                        case GameSize::Large:
                            return 6;

                    }

                    break;

            }            
            
            return 9;

        }

        bool renderMenu() { 

            switch (this->getGameSize()) {

                case GameSize::Small:
                case GameSize::Medium:
                    return true;

                default:

                    switch (this->getRenderSize()) {

                        case RenderSize::Small:
                        case RenderSize::Auto:
                            return true;

                        case RenderSize::Large:
                            return false;

                    }

                    break;

            }            
            
            return 9;

        }

        void setup() {

            this->x = 1;
            this->y = 1;
            this->complete = false;

            for (uint8_t y = 0; y < 9; y++) {

                for (uint8_t x = 0; x < 10; x++) {
                
                    this->solution[y][x] = 0;
                    this->puzzle[y][x] = 0;
                    this->reset[y][x] = 0;

                }

            }

        }

        void loadPuzzle() {


            // Retrieve template count ..

            FX::seekData(Constants::temp_Count);
            uint8_t levelCount = FX::readPendingUInt8();
            FX::readEnd();

            #ifndef DEBUG_FIXED_PUZZLE

                uint8_t randomLevel = random(levelCount) + (this->getGameSize() * levelCount);

                #ifdef DEBUG
                    DEBUG_PRINT("Level :");
                    DEBUG_PRINT(randomLevel + 1);
                    DEBUG_PRINT(" of ");
                    DEBUG_PRINTLN(levelCount * 2);
                #endif

            #else
                
                uint8_t randomLevel = (this->getGameSize() * levelCount);

                #ifdef DEBUG
                    DEBUG_PRINT("Level :");
                    DEBUG_PRINT(randomLevel + 1);
                    DEBUG_PRINT(" of ");
                    DEBUG_PRINTLN(levelCount * 2);
                #endif

            #endif

            uint24_t startPos = FX::readIndexedUInt24(Constants::temp_Levels, randomLevel);

            for (uint8_t y = 0; y < this->getHeight(); y++) {

                FX::seekData(startPos);
                FX::readBytes((uint8_t*)&puzzle[y + 1][1], this->getWidth());
                FX::readEnd();

                startPos = startPos + this->getWidth();

            }

            this->copy();


            #ifndef DEBUG_FIXED_PUZZLE

                if (random(2) == 0) {
                    #ifdef DEBUG
                    DEBUG_PRINTLN("flipHorizontal()");
                    #endif
                    flipHorizontal();
                }

                if (random(2) == 0) {
                    #ifdef DEBUG
                    DEBUG_PRINTLN("flipVertical()");
                    #endif
                    flipVertical();
                }
            
            #endif

            this->fillPuzzle();
            this->updateNumbers_Puzzle();
            this->updateNumbers_Solution(this->getGameLevel());

            #ifdef DEBUG
                this->printPuzzle();
                this->printSolution();
            #endif

        }


        void updateNumbers_Solution(GameLevel gameLevel) {

            // Rows

            for (uint8_t y = 1; y < this->getHeight() + 1; y++) {

                uint8_t minus = 0;
                uint8_t plus = 0;
                
                for (uint8_t x = 1; x < this->getWidth() + 1; x++) {

                    uint8_t i = this->solution[y][x];

                    if (this->isPlus(i)) plus++;
                    if (this->isMinus(i)) minus++;

                }

                this->solution[y][0] = plus;
                this->solution[y][this->getWidth() + 1] = minus;

            }

            // Cols

            for (uint8_t x = 1; x < this->getWidth() + 1; x++) {

                uint8_t minus = 0;
                uint8_t plus = 0;
                
                for (uint8_t y = 1; y < this->getHeight() + 1; y++) {

                    uint8_t i = this->solution[y][x];

                    if (this->isPlus(i)) plus++;
                    if (this->isMinus(i)) minus++;

                }

                this->solution[0][x] = plus;
                this->solution[this->getHeight() + 1][x] = minus;

            }

            if (gameLevel == GameLevel::Hard) {

                for (uint8_t i = 0; i < this->getWidth() / 2; i++) {

                    while (true) {
                        
                        uint8_t x = random(this->getWidth()) + 1;
                        
                        if (this->solution[0][x] < Constants::NoNumber) {
                            this->solution[0][x] = Constants::NoNumber;
                            break;
                        }

                    }

                }

                for (uint8_t i = 0; i < this->getWidth() / 2; i++) {

                    while (true) {
                        
                        uint8_t x = random(this->getWidth()) + 1;
                        if (this->solution[this->getHeight() + 1][x] < Constants::NoNumber) {
                            this->solution[this->getHeight() + 1][x] = Constants::NoNumber;
                            break;
                        }

                    }

                }

                for (uint8_t i = 0; i < this->getHeight() / 2; i++) {

                    while (true) {
                        
                        uint8_t y = random(this->getHeight()) + 1;
                        if (this->solution[y][0] < Constants::NoNumber) {
                            this->solution[y][0] = Constants::NoNumber;
                            break;
                        }

                    }

                }

                for (uint8_t i = 0; i < this->getHeight() / 2; i++) {

                    while (true) {
                        
                        uint8_t y = random(this->getHeight()) + 1;
                        if (this->solution[y][this->getWidth() + 1] < Constants::NoNumber) {
                            this->solution[y][this->getWidth() + 1] = Constants::NoNumber;
                            break;
                        }

                    }

                }

            }

        }


        void updateNumbers_Puzzle() {

            // Rows

            for (uint8_t y = 1; y < this->getHeight() + 1; y++) {

                uint8_t minus = 0;
                uint8_t plus = 0;
                
                for (uint8_t x = 1; x < this->getWidth() + 1; x++) {

                    uint8_t i = this->puzzle[y][x];

                    if (this->isPlus(i)) plus++;
                    if (this->isMinus(i)) minus++;

                }

                this->puzzle[y][0] = plus;
                this->puzzle[y][this->getWidth() + 1] = minus;

            }

            // Cols

            for (uint8_t x = 1; x < this->getWidth() + 1; x++) {

                uint8_t minus = 0;
                uint8_t plus = 0;
                
                for (uint8_t y = 1; y < this->getHeight() + 1; y++) {

                    uint8_t i = this->puzzle[y][x];

                    if (this->isPlus(i)) plus++;
                    if (this->isMinus(i)) minus++;

                }

                this->puzzle[0][x] = plus;
                this->puzzle[this->getHeight() + 1][x] = minus;

            }

        }


        void flipHorizontal() {

            for (uint8_t y = 1; y < this->getHeight() + 1; y++) {

                for (uint8_t x = 1; x < (this->getWidth() / 2) + 1; x++) {
                    
                    uint8_t x2 = this->getWidth() - x + 1;

                    uint8_t l = this->solution[y][x];
                    uint8_t r = this->solution[y][x2];

                    switch (l) {

                        case Tiles::Horizontal_Blank_Start:
                            l = Tiles::Horizontal_Blank_End;
                            break;

                        case Tiles::Horizontal_Blank_End:
                            l = Tiles::Horizontal_Blank_Start;
                            break;

                        default:
                            break;
                            
                    }

                    switch (r) {

                        case Tiles::Horizontal_Blank_Start:
                            r = Tiles::Horizontal_Blank_End;
                            break;

                        case Tiles::Horizontal_Blank_End:
                            r = Tiles::Horizontal_Blank_Start;
                            break;

                        default:
                            break;
                            
                    }

                    this->solution[y][x] = r;
                    this->solution[y][x2] = l;

                }

            }

        }


        void flipVertical() {

            for (uint8_t x = 1; x < this->getWidth() + 1; x++) {

                for (uint8_t y = 1; y < (this->getHeight() / 2) + 1; y++) {
                    
                    uint8_t y2 = this->getHeight() - y + 1;

                    uint8_t u = this->solution[y][x];
                    uint8_t d = this->solution[y2][x];

                    switch (u) {

                        case Tiles::Vertical_Blank_Start:
                            u = Tiles::Vertical_Blank_End;
                            break;

                        case Tiles::Vertical_Blank_End:
                            u = Tiles::Vertical_Blank_Start;
                            break;

                        default:
                            break;
                            
                    }

                    switch (d) {

                        case Tiles::Vertical_Blank_Start:
                            d = Tiles::Vertical_Blank_End;
                            break;

                        case Tiles::Vertical_Blank_End:
                            d = Tiles::Vertical_Blank_Start;
                            break;

                        default:
                            break;
                            
                    }

                    this->solution[y][x] = d;
                    this->solution[y2][x] = u;

                }

            }


            // Fix centre line ..

            for (uint8_t x = 1; x < this->getWidth() + 1; x++) {

                uint8_t y = (this->getHeight() / 2) + 1;
                uint8_t u = this->solution[y][x];

                switch (u) {

                    case Tiles::Vertical_Blank_Start:
                        u = Tiles::Vertical_Blank_End;
                        break;

                    case Tiles::Vertical_Blank_End:
                        u = Tiles::Vertical_Blank_Start;
                        break;

                    default:
                        break;
                        
                }

                this->solution[y][x] = u;


            }

        }

        void fillPuzzle() {

            // Fill one random spot ..

            bool placed = false;

            while (!placed) {

                uint8_t x = random(1, this->getWidth() + 1);
                uint8_t y = random(1, this->getHeight() + 1);
                uint8_t space = this->solution[y][x];

                switch (space) {
                    
                    case Tiles::Horizontal_Blank_Start: 
                        {
                            uint8_t r = random(2);

                            switch (r) {

                                case 0:
                                    this->solution[y][x] = Tiles::Horizontal_MinusPlus_Start;
                                    this->solution[y][x + 1] = Tiles::Horizontal_MinusPlus_End;
                                    break;

                                case 1:
                                    this->solution[y][x] = Tiles::Horizontal_PlusMinus_Start;
                                    this->solution[y][x + 1] = Tiles::Horizontal_PlusMinus_End;
                                    break;

                            }

                            placed = true;

                        }
                                    
                        break;
                        
                    case Tiles::Vertical_Blank_Start: 
                        {
                            uint8_t r = random(2);

                            switch (r) {


                                case 0:
                                    this->solution[y][x] = Tiles::Vertical_MinusPlus_Start;
                                    this->solution[y + 1][x] = Tiles::Vertical_MinusPlus_End;
                                    break;

                                case 1:
                                    this->solution[y][x] = Tiles::Vertical_PlusMinus_Start;
                                    this->solution[y + 1][x] = Tiles::Vertical_PlusMinus_End;
                                    break;

                            }

                            placed = true;

                        }

                        break;

                }

            }



            // Fill the rest ..

            for (uint8_t y = 1; y < this->getHeight() + 1; y++) {

                for (uint8_t x = 1; x < this->getWidth() + 1; x++) {
                
                    uint8_t space = this->puzzle[y][x];

                    switch (space) {
                        
                        case Tiles::Horizontal_Blank_Start: 
                            {
                                uint8_t r = random(Constants::Fill_MaxRand);

                                uint8_t surroundingPlus_L = 0;
                                uint8_t surroundingMinus_L = 0;
                                uint8_t surroundingPlus_R = 0;
                                uint8_t surroundingMinus_R = 0;

                                if (this->isPlus(x, y - 1))         surroundingPlus_L++;
                                if (this->isPlus(x, y + 1))         surroundingPlus_L++;
                                if (this->isPlus(x - 1, y))         surroundingPlus_L++;
                                if (this->isPlus(x + 1, y))         surroundingPlus_L++;

                                if (this->isMinus(x, y - 1))        surroundingMinus_L++;
                                if (this->isMinus(x, y + 1))        surroundingMinus_L++;
                                if (this->isMinus(x - 1, y))        surroundingMinus_L++;
                                if (this->isMinus(x + 1, y))        surroundingMinus_L++;

                                if (this->isPlus(x + 1, y - 1))     surroundingPlus_R++;
                                if (this->isPlus(x + 1, y + 1))     surroundingPlus_R++;

                                if (this->isMinus(x + 1, y - 1))    surroundingMinus_R++;
                                if (this->isMinus(x + 1, y + 1))    surroundingMinus_R++;

                                if (x + 2 < this->getWidth() + 2) {
    
                                    if (this->isPlus(x + 2, y))         surroundingPlus_R++;
                                    if (this->isMinus(x + 2, y))        surroundingMinus_R++;

                                }

                                #ifdef DEBUG_POPULATE_PUZZLE
                                    DEBUG_PRINT("H ");
                                    DEBUG_PRINT(x);
                                    DEBUG_PRINT(",");
                                    DEBUG_PRINT(y);
                                    DEBUG_PRINT("=");
                                    DEBUG_PRINT(r);
                                    DEBUG_PRINT(" (+");
                                    DEBUG_PRINT(surroundingPlus_L);
                                    DEBUG_PRINT(" -");
                                    DEBUG_PRINT(surroundingMinus_L);
                                    DEBUG_PRINT(") ");
                                    DEBUG_PRINT(" (+");
                                    DEBUG_PRINT(surroundingPlus_R);
                                    DEBUG_PRINT(" -");
                                    DEBUG_PRINT(surroundingMinus_R);
                                    DEBUG_PRINTLN(") ");
                                #endif

                                switch (r) {

                                    case 0 ... Constants::Fill_Orient_2:
                                        {
                                            
                                            //Must be neutral ..

                                            if (surroundingPlus_L > 0 && surroundingMinus_L > 0) {

                                                this->solution[y][x] = Tiles::Horizontal_Neutral_Start;
                                                this->solution[y][x + 1] = Tiles::Horizontal_Neutral_End;

                                            }
                                            else if (surroundingPlus_L > 0 && surroundingPlus_R > 0) {

                                                this->solution[y][x] = Tiles::Horizontal_Neutral_Start;
                                                this->solution[y][x + 1] = Tiles::Horizontal_Neutral_End;

                                            }
                                            else if (surroundingMinus_L > 0 && surroundingMinus_R > 0) {

                                                this->solution[y][x] = Tiles::Horizontal_Neutral_Start;
                                                this->solution[y][x + 1] = Tiles::Horizontal_Neutral_End;

                                            }

                                            // OK to place a tile ..                                            
                                            else if (surroundingPlus_L > 0 && surroundingPlus_R == 0) {

                                                this->solution[y][x] = Tiles::Horizontal_MinusPlus_Start;
                                                this->solution[y][x + 1] = Tiles::Horizontal_MinusPlus_End;

                                            }
                                            else if (surroundingPlus_L == 0 && surroundingPlus_R > 0) {

                                                this->solution[y][x] = Tiles::Horizontal_PlusMinus_Start;
                                                this->solution[y][x + 1] = Tiles::Horizontal_PlusMinus_End;

                                            }
                                            else if (surroundingMinus_L > 0 && surroundingMinus_R == 0) {

                                                this->solution[y][x] = Tiles::Horizontal_PlusMinus_Start;
                                                this->solution[y][x + 1] = Tiles::Horizontal_PlusMinus_End;

                                            }
                                            else if (surroundingMinus_L == 0 && surroundingMinus_R > 0) {

                                                this->solution[y][x] = Tiles::Horizontal_MinusPlus_Start;
                                                this->solution[y][x + 1] = Tiles::Horizontal_PlusMinus_End;

                                            }
                                            else {

                                                switch (r) {

                                                    case 0 ... Constants::Fill_Orient_1:
                                                        this->solution[y][x] = Tiles::Horizontal_MinusPlus_Start;
                                                        this->solution[y][x + 1] = Tiles::Horizontal_MinusPlus_End;
                                                        break;

                                                    case Constants::Fill_Orient_1 + 1 ... Constants::Fill_Orient_2:
                                                        this->solution[y][x] = Tiles::Horizontal_PlusMinus_Start;
                                                        this->solution[y][x + 1] = Tiles::Horizontal_PlusMinus_End;
                                                        break;

                                                    default:
                                                        this->solution[y][x] = Tiles::Horizontal_Neutral_Start;
                                                        this->solution[y][x + 1] = Tiles::Horizontal_Neutral_End;
                                                        break;

                                                }

                                            }

                                        }
                                        break;

                                    default:

                                        this->solution[y][x] = Tiles::Horizontal_Neutral_Start;
                                        this->solution[y][x + 1] = Tiles::Horizontal_Neutral_End;
                                        break;
                                        

                                }

                                x = x+ 1;

                            }

                            break;
                            
                        case Tiles::Vertical_Blank_Start: 
                            {
                                uint8_t r = random(Constants::Fill_MaxRand);

                                uint8_t surroundingPlus_U = 0;
                                uint8_t surroundingMinus_U = 0;
                                uint8_t surroundingPlus_D = 0;
                                uint8_t surroundingMinus_D = 0;

                                if (this->isPlus(x, y - 1))         surroundingPlus_U++;
                                if (this->isPlus(x, y + 1))         surroundingPlus_U++;
                                if (this->isPlus(x - 1, y))         surroundingPlus_U++;
                                if (this->isPlus(x + 1, y))         surroundingPlus_U++;

                                if (this->isMinus(x, y - 1))        surroundingMinus_U++;
                                if (this->isMinus(x, y + 1))        surroundingMinus_U++;
                                if (this->isMinus(x - 1, y))        surroundingMinus_U++;
                                if (this->isMinus(x + 1, y))        surroundingMinus_U++;

                                if (this->isPlus(x - 1, y + 1))     surroundingPlus_D++;
                                if (this->isPlus(x + 1, y + 1))     surroundingPlus_D++;

                                if (this->isMinus(x - 1, y + 1))    surroundingMinus_D++;
                                if (this->isMinus(x + 1, y + 1))    surroundingMinus_D++;

                                if (x + 2 < this->getHeight() + 2) {

                                    if (this->isPlus(x, y + 2))         surroundingPlus_D++;
                                    if (this->isMinus(x, y + 2))        surroundingMinus_D++;

                                }

                                #ifdef DEBUG_POPULATE_PUZZLE
                                    DEBUG_PRINT("V ");
                                    DEBUG_PRINT(x);
                                    DEBUG_PRINT(",");
                                    DEBUG_PRINT(y);
                                    DEBUG_PRINT("=");
                                    DEBUG_PRINT(r);
                                    DEBUG_PRINT(" (+");
                                    DEBUG_PRINT(surroundingPlus_U);
                                    DEBUG_PRINT(" -");
                                    DEBUG_PRINT(surroundingMinus_U);
                                    DEBUG_PRINT(")");
                                    DEBUG_PRINT(" (+");
                                    DEBUG_PRINT(surroundingPlus_D);
                                    DEBUG_PRINT(" -");
                                    DEBUG_PRINT(surroundingMinus_D);
                                    DEBUG_PRINTLN(")");
                                #endif

                                switch (r) {

                                    case 0 ... Constants::Fill_Orient_2:

                                        //Must be neutral ..
                                        
                                        if (surroundingPlus_U > 0 && surroundingMinus_D > 0) {

                                            this->solution[y][x] = Tiles::Vertical_Neutral_Start;
                                            this->solution[y + 1][x] = Tiles::Vertical_Neutral_End;

                                        }
                                        else if (surroundingPlus_U > 0 && surroundingPlus_D > 0) {

                                            this->solution[y][x] = Tiles::Vertical_Neutral_Start;
                                            this->solution[y + 1][x] = Tiles::Vertical_Neutral_End;

                                        }
                                        else if (surroundingMinus_U > 0 && surroundingMinus_D > 0) {

                                            this->solution[y][x] = Tiles::Vertical_Neutral_Start;
                                            this->solution[y + 1][x] = Tiles::Vertical_Neutral_End;

                                        }

                                        // OK to place a tile ..             

                                        else if (surroundingPlus_U > 0 && surroundingPlus_D == 0) {

                                            this->solution[y][x] = Tiles::Vertical_MinusPlus_Start;
                                            this->solution[y + 1][x] = Tiles::Vertical_MinusPlus_End;

                                        }
                                        else if (surroundingPlus_U == 0 && surroundingPlus_D > 0) {

                                            this->solution[y][x] = Tiles::Vertical_PlusMinus_Start;
                                            this->solution[y + 1][x] = Tiles::Vertical_PlusMinus_End;

                                        }
                                        else if (surroundingMinus_U > 0 && surroundingMinus_D == 0) {

                                            this->solution[y][x] = Tiles::Vertical_PlusMinus_Start;
                                            this->solution[y + 1][x] = Tiles::Vertical_PlusMinus_End;

                                        }
                                        else if (surroundingMinus_U == 0 && surroundingMinus_D > 0) {

                                            this->solution[y][x] = Tiles::Vertical_MinusPlus_Start;
                                            this->solution[y + 1][x] = Tiles::Vertical_PlusMinus_End;

                                        }
                                        else {

                                            switch (r) {

                                                case 0 ... Constants::Fill_Orient_1:
                                                    this->solution[y][x] = Tiles::Vertical_MinusPlus_Start;
                                                    this->solution[y + 1][x] = Tiles::Vertical_MinusPlus_End;
                                                    break;

                                                case Constants::Fill_Orient_1 + 1 ... Constants::Fill_Orient_2:
                                                    this->solution[y][x] = Tiles::Vertical_PlusMinus_Start;
                                                    this->solution[y + 1][x] = Tiles::Vertical_PlusMinus_End;
                                                    break;

                                                default:
                                                    this->solution[y][x] = Tiles::Vertical_Neutral_Start;
                                                    this->solution[y + 1][x] = Tiles::Vertical_Neutral_End;
                                                    break;

                                            }

                                        }

                                        break;

                                    default:

                                        this->solution[y][x] = Tiles::Vertical_Neutral_Start;
                                        this->solution[y + 1][x] = Tiles::Vertical_Neutral_End;
                                        break;
                                        

                                }

                            }

                            break;

                    }
                            
                }

            }

        }

        void printPuzzle() {

            #ifdef DEBUG

            DEBUG_PRINTLN(F("Puzzle ----------------------"));

            for (uint8_t y = 0; y < this->getHeight() + 2; y++) {

                for (uint8_t x = 0; x < this->getWidth() + 2; x++) {
                
                    DEBUG_PRINT(this->puzzle[y][x]);
                    DEBUG_PRINT(" ");

                }

                DEBUG_PRINTLN();
            }

            #endif

        }

        void printSolution() {

            #ifdef DEBUG

            DEBUG_PRINTLN(F("Solution ----------------------"));

            for (uint8_t y = 0; y < this->getHeight() + 2; y++) {

                for (uint8_t x = 0; x < this->getWidth() + 2; x++) {
                
                    DEBUG_PRINT(this->solution[y][x]);
                    DEBUG_PRINT(" ");

                }

                DEBUG_PRINTLN();
            }

            #endif

        }

        bool isPlus(uint8_t x, uint8_t y) {

            uint8_t i = this->solution[y][x];
            return isPlus(i);

        }


        bool isPlus(uint8_t i) {

            switch (i) {

                case Tiles::Horizontal_MinusPlus_End:
                case Tiles::Horizontal_PlusMinus_Start:
                case Tiles::Vertical_MinusPlus_End:
                case Tiles::Vertical_PlusMinus_Start:
                    return true;

                default:
                    return false;
                                
            }

        } 


        bool isMinus(uint8_t x, uint8_t y) {

            uint8_t i = this->solution[y][x];
            return isMinus(i);

        }


        bool isMinus(uint8_t i) {

            switch (i) {


                case Tiles::Horizontal_MinusPlus_Start:
                case Tiles::Horizontal_PlusMinus_End:
                case Tiles::Vertical_MinusPlus_Start:
                case Tiles::Vertical_PlusMinus_End:
                    return true;

                default:
                    return false;
                    
            }

        }

        void copy() {
            
            for (uint8_t y = 0; y < this->getHeight() + 1; y++) {

                for (uint8_t x = 0; x < this->getWidth() + 1; x++) {
                
                    this->solution[y][x] = this->puzzle[y][x];
                    this->reset[y][x] = this->puzzle[y][x];

                }

            }

        }

        void copy_SolutionToPuzzle() {
            
            for (uint8_t y = 0; y < this->getHeight() + 1; y++) {

                for (uint8_t x = 0; x < this->getWidth() + 1; x++) {
                
                    this->puzzle[y][x] = this->solution[y][x];

                }

            }

        }

        void clearPuzzle() {
            
            for (uint8_t y = 1; y < this->getHeight() + 1; y++) {

                for (uint8_t x = 1; x < this->getWidth() + 1; x++) {
                
                    this->puzzle[y][x] = this->reset[y][x];
                    this->updateNumbers_Puzzle();

                }

            }

        }

        bool isGameComplete() {

            this->complete = false;

            for (uint8_t y = 1; y < this->getHeight() + 1; y++) {

                if (this->solution[y][0] != Constants::NoNumber && this->solution[y][0] != this->puzzle[y][0]) {

                    return false;
                    
                }

                if (this->solution[y][this->getWidth() + 1] != Constants::NoNumber && this->solution[y][this->getWidth() + 1] != this->puzzle[y][this->getWidth() + 1]) {

                    return false;
                    
                }

            }

            for (uint8_t x = 1; x < this->getWidth() + 1; x++) {
            
                if (this->solution[0][x] != Constants::NoNumber && this->solution[0][x] != this->puzzle[0][x]) {

                    return false;
                    
                }

                if (this->solution[this->getHeight() + 1][x] != Constants::NoNumber && this->solution[this->getHeight() + 1][0] != this->puzzle[this->getHeight() + 1][0]) {

                    return false;
                    
                }

            }



            // If all the numbers match then look for +ve touching +ve, -ve touching -ve ..


            for (uint8_t y = 1; y < this->getHeight() + 1; y++) {

                for (uint8_t x = 1; x < this->getWidth() + 1; x++) {
                
                    switch (this->puzzle[y][x]) {

                        case Tiles::Vertical_Blank_Start:
                        case Tiles::Horizontal_Blank_Start:
                            return false;

                        case Tiles::Vertical_MinusPlus_Start:
                        case Tiles::Horizontal_MinusPlus_Start:

                            if (this->hasSurroundingMinus(x, y))   {

                                #ifdef DEBUG_IS_COMPLETE
                                    DEBUG_PRINT("Adj minus, tile ");
                                    DEBUG_PRINT(this->puzzle[y][x]);
                                    DEBUG_PRINT(" @ ");
                                    DEBUG_PRINT(x);
                                    DEBUG_PRINT(",");
                                    DEBUG_PRINTLN(y);
                                    printPuzzle();
                                #endif

                                return false;

                            }
                            break;

                        case Tiles::Vertical_PlusMinus_Start:
                        case Tiles::Horizontal_PlusMinus_Start:

                            if (this->hasSurroundingPlus(x, y))   {

                                #ifdef DEBUG_IS_COMPLETE
                                    DEBUG_PRINT("Adj plus, tile ");
                                    DEBUG_PRINT(this->puzzle[y][x]);
                                    DEBUG_PRINT(" @ ");
                                    DEBUG_PRINT(x);
                                    DEBUG_PRINT(",");
                                    DEBUG_PRINTLN(y);
                                    printPuzzle();
                                #endif
                                
                                return false;

                            }
                            break;

                    }

                }

            }


            this->complete = true;
            return true;

        }


        ValidMove isValidMove(uint8_t x, uint8_t y) {

            switch (this->puzzle[y][x]) {

                case Tiles::Vertical_MinusPlus_Start:
                case Tiles::Horizontal_MinusPlus_Start:
                    if (this->hasSurroundingMinus(x, y))   return ValidMove::NotValid;
                    break;

                case Tiles::Vertical_PlusMinus_Start:
                case Tiles::Horizontal_PlusMinus_Start:
                    if (this->hasSurroundingPlus(x, y))   return ValidMove::NotValid;
                    break;

            }

            switch (this->puzzle[y][x]) {

                case Tiles::Vertical_MinusPlus_Start:
                    if (this->hasSurroundingPlus(x, y + 1))   return ValidMove::NotValid;
                    break;

                case Tiles::Vertical_PlusMinus_Start:
                    if (this->hasSurroundingMinus(x, y + 1))   return ValidMove::NotValid;
                    break;

                case Tiles::Horizontal_PlusMinus_Start:
                    if (this->hasSurroundingMinus(x + 1, y))   return ValidMove::NotValid;
                    break;

                case Tiles::Horizontal_MinusPlus_Start:
                    if (this->hasSurroundingPlus(x + 1, y))   return ValidMove::NotValid;
                    break;

            }

            return ValidMove::Valid;

        }

        bool hasSurroundingPlus(uint8_t x, uint8_t y) {

            uint8_t tile = 0;

            if (x > 1) {

                tile = this->puzzle[y][x - 1];
                if (this->isPlus(tile)) return true;

            }

            if (x < this->getWidth()) {

                tile = this->puzzle[y][x + 1];
                if (this->isPlus(tile)) return true;

            }

            if (y > 1) {

                tile = this->puzzle[y - 1][x];
                if (this->isPlus(tile)) return true;

            }

            if (y < this->getHeight()) {

                tile = this->puzzle[y + 1][x];
                if (this->isPlus(tile)) return true;

            }

            return false;

        }

        bool hasSurroundingMinus(uint8_t x, uint8_t y) {

            uint8_t tile = 0;
            
            if (x > 1) {

                tile = this->puzzle[y][x - 1];

                if (this->isMinus(tile)) {
                    return true;
                }

            }

            if (x < this->getWidth()) {

                tile = this->puzzle[y][x + 1];

                if (this->isMinus(tile)) {
                    return true;
                }

            }

            if (y > 1) {

                tile = this->puzzle[y - 1][x];

                if (this->isMinus(tile)) {
                    return true;
                }

            }

            if (y < this->getHeight()) {

                tile = this->puzzle[y + 1][x];

                if (this->isMinus(tile)) {
                    return true;
                }

            }

            return false;

        }


        Hint getHint() {

            Hint hint;

            // Look for incorrect value ..

            for (uint8_t y = 1; y < this->getHeight() + 1; y++) {

                for (uint8_t x = 1; x < this->getWidth() + 1; x++) {
                
                    uint8_t tile = this->puzzle[y][x];

                    switch (tile) {

                        case Tiles::Horizontal_Blank_Start:
                        case Tiles::Horizontal_Blank_End:
                        case Tiles::Vertical_Blank_Start:
                        case Tiles::Vertical_Blank_End:
                            break;

                        default:
      
                            if (this->solution[y][x] != this->puzzle[y][x]) {

                                hint.x = x;
                                hint.y = y;
                                hint.hintType = HintType::InvalidTile;
                                hint.count = 240;
                                return hint;

                            }

                            break;
                        
                    }

                }

            }

            for (uint8_t y = 1; y < this->getHeight() + 1; y++) {

                for (uint8_t x = 1; x < this->getWidth() + 1; x++) {
                
                    uint8_t tile = this->puzzle[y][x];

                    switch (tile) {

                        case Tiles::Horizontal_Blank_Start:
                        case Tiles::Horizontal_Blank_End:

                            hint.x = x;
                            hint.y = y;
                            hint.tile = this->solution[y][x];
                            hint.x2 = x + 1;
                            hint.y2 = y;
                            hint.tile2 = this->solution[y][x + 1];
                            hint.hintType = HintType::RandomTile;
                            hint.count = 255;
                            return hint;

                        case Tiles::Vertical_Blank_Start:
                        case Tiles::Vertical_Blank_End:

                            hint.x = x;
                            hint.y = y;
                            hint.tile = this->solution[y][x];
                            hint.x2 = x;
                            hint.y2 = y + 1;
                            hint.tile2 = this->solution[y + 1][x];
                            hint.hintType = HintType::RandomTile;
                            hint.count = 255;
                            return hint;

                        default:
                            break;
                        
                    }

                }

            }

            return hint;

        }

};









