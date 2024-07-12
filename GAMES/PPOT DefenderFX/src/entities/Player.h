#pragma once

#include <Arduboy2.h>
#include "../utils/Enums.h"
#include "../utils/Enums.h"
#include "../../fxdata/fxdata.h"


class Player {

    private:

        PlayerMovement playerMovement = PlayerMovement::Up_Middle;
        uint8_t health = Constants::HealthMax;
        uint8_t healthBlink = 0;
        uint8_t treasureCount = Constants::TreasureCount;
        uint8_t treasureBlink = 0;

        Direction directionX = Direction::Right;
        Direction directionY = Direction::None;
        uint8_t velocityIdxX = 14;
        uint8_t velocityIdxY = 14;

        SQ15x16 x = 0;
        SQ15x16 y = 0;

        bool deccelerateXFlag = false;
        bool deccelerateYFlag = false;
        bool deathSeq = false;

        uint8_t deathSeqIdx = 0;
        

    public:

        Direction getDirectionX()                       { return this->directionX; }
        Direction getDirectionY()                       { return this->directionY; }
        uint8_t getVelocityIdxX()                       { return this->velocityIdxX; }
        uint8_t getVelocityIdxY()                       { return this->velocityIdxY; }
        SQ15x16 getX()                                  { return this->x; }        
        SQ15x16 getY()                                  { return this->y; }        
        bool isDecceleratingX()                         { return this->deccelerateXFlag; }
        bool getDeccelerateY()                          { return this->deccelerateYFlag; }
        bool getDeathSeq()                              { return this->deathSeq; }
        uint8_t getDeathSeqIdx()                        { return this->deathSeqIdx; }
        uint8_t getHealth()                             { return this->health; }
        uint8_t getHealthBlink()                        { return this->healthBlink; }
        uint8_t getTreasureCount()                      { return this->treasureCount; }
        uint8_t getTreasureBlink()                      { return this->treasureBlink; }

        void setVelocityIdxX(uint8_t val)               { this->velocityIdxX = val; }
        void setDirectionX(Direction val)               { this->directionX = val; }
        void setDirectionY(Direction val)               { this->directionY = val; }
        void setX(SQ15x16 val)                          { this->x = val; }
        void setY(SQ15x16 val)                          { this->y = val; }
        void setDeathSeq(bool val)                      { this->deathSeq = val; deathSeqIdx = 0; }
        void setHealth(uint8_t val)                     { this->health = val; }
        void setHealthBlink(uint8_t val)                { this->healthBlink = val; }
        void setTreasureCount(uint8_t val)              { this->treasureCount = val; }
        void setTreasureBlink(uint8_t val)              { this->treasureBlink = val; }

    public:

        void decHealthBlink() {

            if (this->healthBlink > 0) this->healthBlink--;

        }

        void decTreasureCount() {

            if (this->treasureCount > 0) this->treasureCount--;

        }

        void decTreasureBlink() {

            if (this->treasureBlink > 0) this->treasureBlink--;

        }

        void init() {

            this->setX(0 + (56 - 4));
            this->setY(12);
            this->setDeathSeq(false);
            this->setVelocityIdxX(17);
            this->directionX = Direction::Right;

            this->health = Constants::HealthMax;
            this->healthBlink = 0;            
            this->treasureCount = Constants::TreasureCount;
            this->treasureBlink = 0;
            
        }

        SQ15x16 getVelocityX()                           { return static_cast<SQ15x16>(Constants::Velocity_X[this->velocityIdxX]) / 16; }
        SQ15x16 getVelocityY()                           { return Constants::Velocity_Y[this->velocityIdxY] / 16; }

        void incX(SQ15x16 val)                           { this->x = this->x + val; }

        void incY(SQ15x16 val) { 
            
            this->y = this->y + val; 

            if (this->y < 0) this->y = 0;
            if (this->y > 43) this->y = 43;

        }

        void accelerateX(Direction direction) {

            this->deccelerateXFlag = false;

            switch (direction) {

                case Direction::Left:

                    if (this->velocityIdxX > Constants::Player_Velocity_Right_Min) this->deccelerateXFlag = true;
                    if (this->velocityIdxX > Constants::Player_Velocity_Left_Max) this->velocityIdxX--;
                    break;

                case Direction::Right:

                    if (this->velocityIdxX < Constants::Player_Velocity_Left_Min) this->deccelerateXFlag = true;
                    if (this->velocityIdxX < Constants::Player_Velocity_Right_Max) this->velocityIdxX++;
                    break;

            }

            if (this->velocityIdxX < Constants::Player_Velocity_Stationary) {

                this->directionX = Direction::Left;

            }
            else {

                this->directionX = Direction::Right;

            }

        }

        void deccelerateX() {

            this->deccelerateXFlag = false;

            switch (this->directionX) {

                case Direction::Left:

                    if (this->velocityIdxX < Constants::Player_Velocity_Left_Min - 1) {
                        this->velocityIdxX++;
                        this->deccelerateXFlag = true;
                    }

                    break;

                case Direction::Right:

                    if (this->velocityIdxX > Constants::Player_Velocity_Right_Min + 1) {
                        this->velocityIdxX--;
                        this->deccelerateXFlag = true;
                    }

                    break;

            }

        }


        void accelerateY(Direction direction) {

            this->deccelerateYFlag = false;

            switch (direction) {

                case Direction::Up:

                    if (this->velocityIdxY > Constants::Player_Velocity_Down_Min) {
                        this->velocityIdxY = this->velocityIdxY - 2;
                        this->deccelerateYFlag = true;
                    }
                    if (this->velocityIdxY > Constants::Player_Velocity_Up_Max) this->velocityIdxY--;
                    break;

                case Direction::Down:

                    if (this->velocityIdxY < Constants::Player_Velocity_Up_Min) {
                        this->velocityIdxY = this->velocityIdxY + 2;
                        this->deccelerateYFlag = true;
                    }
                    if (this->velocityIdxY < Constants::Player_Velocity_Down_Max) this->velocityIdxY++;
                    break;

            }

            if (this->velocityIdxY < Constants::Player_Velocity_Stationary) {

                this->directionY = Direction::Up;

            }
            else {

                this->directionY = Direction::Down;

            }

        }

        void deccelerateY() {

            this->deccelerateYFlag = false;

            switch (this->directionY) {

                case Direction::Up:

                    if (this->velocityIdxY < Constants::Player_Velocity_Up_Min) {
                        this->velocityIdxY = this->velocityIdxY + 2;
                        this->deccelerateYFlag = true;
                    }

                    break;

                case Direction::Down:

                    if (this->velocityIdxY > Constants::Player_Velocity_Down_Min) {
                        this->velocityIdxY = this->velocityIdxY - 2;
                        this->deccelerateYFlag = true;
                    }

                    break;
            

            }

        }     

        PlayerUpdate update(uint8_t frameCount) {

            this->incX(this->getVelocityX());
            this->incY(this->getVelocityY());

            if (this->deathSeq == true) {

                if (this->deathSeqIdx < Constants::DeathSeq_Final && frameCount % 2 == 0) {

                    this->deathSeqIdx++;

                    if (this->deathSeqIdx == Constants::DeathSeq_ExplodePlane) {

                        return PlayerUpdate::Dead;
                        
                    }

                }

            }

            return PlayerUpdate::Normal;

        }   

        bool decHealth(uint8_t val) {   // returns true when dead

            if (this->health > 0 && this->health > val) {
                this->health = this->health - val;
            }
            else {

                this->health = 0;
                this->setDeathSeq(true);
                return true;

            }

            if (this->healthBlink == 0) {
                this->healthBlink = this->healthBlink + 96;
            }
            else if (this->healthBlink < 84) {
                this->healthBlink = this->healthBlink + 32;
            }

            return false;
            
        }

        void incHealth(uint8_t val) {

            if (this->health + val < Constants::HealthMax) {
                this->health =this-> health + val;
            }
            else {
                this->health = Constants::HealthMax;
            }

            if (this->healthBlink == 0) {
                this->healthBlink = this->healthBlink + 96;
            }
            else if (healthBlink < 84) {
                this->healthBlink = this->healthBlink + 32;
            }
            
        }

        Rect getRect() {

            Rect rect;

            rect.x = this->getX().getInteger() + 1;
            rect.y = this->getY().getInteger() + 1;
            rect.width = 14;
            rect.height = 12;

            return rect;

        }

        #ifdef DEBUG

            void printRect() {

                Rect r = this->getRect();

                DEBUG_PRINT("Player ");
                DEBUG_PRINT(r.x);
                DEBUG_PRINT(",");
                DEBUG_PRINT(r.y);
                DEBUG_PRINT(",");
                DEBUG_PRINT(r.width);
                DEBUG_PRINT(",");
                DEBUG_PRINT(r.height);
            
            }

        #endif
        
};