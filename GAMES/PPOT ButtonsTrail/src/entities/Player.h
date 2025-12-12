#pragma once

#include "../utils/Utils.h"


class Player { 

    private:

        int8_t x;
        int8_t xNew;
        int8_t xOld;

        int8_t y;
        int8_t yNew;
        int8_t yOld;

        bool dying = false;
        bool moving = false;

        int8_t xOffset = 0;
        int8_t yOffset = 0;

        uint8_t yDyingOffset_1 = 0;
        uint8_t yDyingOffset_2 = 0;

    public:

        int8_t getX()                   { return this->x; }
        int8_t getXNew()                { return this->xNew; }
        int8_t getXOld()                { return this->xOld; }
        int8_t getXOffset()             { return this->xOffset; }

        int8_t getY()                   { return this->y; }
        int8_t getYNew()                { return this->yNew; }
        int8_t getYOld()                { return this->yOld; }
        int8_t getYOffset()             { return this->yOffset; }

        uint8_t getYDyingOffset_1()     { return this->yDyingOffset_1; }
        uint8_t getYDyingOffset_2()     { return this->yDyingOffset_2; }
        
        bool isDying()                  { return this->dying; }
        bool isMoving()                 { return this->moving; }

        void setX(int8_t val)           { this->x = val; }
        void setXNew(int8_t val)        { this->xNew = val; }
        void setXOld(int8_t val)        { this->xOld = val; }
        void setXOffset(int8_t val)     { this->xOffset = val; }

        void setY(int8_t val)           { this->y = val; }
        void setYNew(int8_t val)        { this->yNew = val; }
        void setYOld(int8_t val)        { this->yOld = val; }
        void setYOffset(int8_t val)     { this->yOffset = val; }

        void setDying(bool val)         { this->dying = val;}
        void setMoving(bool val)        { this->moving = val;}


    public:

        void init(int8_t x, int8_t y) {
            
            this->x = x;
            this->xOld = x;
            this->xNew = x;

            this->y = y;
            this->yOld = y;
            this->yNew = y;

        }

        void kill() {

            this->dying = true;

        }

        void moveLeft() {

            this->xNew = this->x - 1;
            this->xOld = this->x;
            this->yOld = this->y;
            this->moving = true;

        }

        void moveRight() {

            this->xNew = this->x + 1;
            this->xOld = this->x;
            this->yOld = this->y;
            this->moving = true;

        }

        void moveUp() {

            this->yNew = this->y - 1;
            this->xOld = this->x;
            this->yOld = this->y;
            this->moving = true;

        }

        void moveDown() {

            this->yNew = this->y + 1;
            this->xOld = this->x;
            this->yOld = this->y;
            this->moving = true;

        }

        bool update(bool zoom) {

            uint8_t increment = zoom ? 3 : 2;
            uint8_t xOffsetMax = zoom ? 12 : 8;

            if (this->x != this->xNew) {

                this->xOffset = this->xOffset + (this->x > this->xNew ? -increment : increment);

                if (absT(this->xOffset) == xOffsetMax) {

                    this->xOffset = 0;
                    this->x = this->xNew;
                    this->moving = false;

                }
                
            }

            if (this->y != this->yNew) {

                this->yOffset = this->yOffset + (this->y > this->yNew ? -increment : increment);

                if (absT(this->yOffset) == xOffsetMax) {

                    this->yOffset = 0;
                    this->y = this->yNew;
                    this->moving = false;

                }
                
            }

            if (this->dying) {

                if (!this->moving) this->yDyingOffset_1 = this->yDyingOffset_1 + increment;

                this->yDyingOffset_2 = yDyingOffset_2 + increment;

                if (this->yDyingOffset_1 > 64 && yDyingOffset_2 > 64) {

                    this->yDyingOffset_1 = 0;
                    this->yDyingOffset_2 = 0;
                    this->dying = false;

                    return true;

                }

            }

            return false;

        }

};