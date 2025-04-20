#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../Utils/Enums.h"
#include "../utils/Stack.h"

class Player {

    public:

        Stack <Stance, Constants::StackSize_Player> stack;
        Stance stance = Stance::Player_Idle_RH;
        uint8_t y;
        uint8_t health;
        uint8_t imageIdx;
        int8_t velocityX;

    public:

        Stance getStance()                                  { return this->stance; }
        uint8_t getImageIdx()                               { return this->imageIdx; }
        uint8_t getY()                                      { return this->y; }
        uint8_t getHealth()                                 { return this->health; }
        int8_t getVelocityX()                               { return this->velocityX; }
      
        void setStance(Stance val)                          { this->stance = val; }
        void setImageIdx(uint8_t val)                       { this->imageIdx = val; }
        void setY(uint8_t val)                              { this->y = val; }
        void setHealth(uint8_t val)                         { this->health = val; }
        void setVelocityX(int8_t val)                       { this->velocityX = val; }

        void reset() {

            this->y = 46;
            this->velocityX = 0;
            this->health = 11;
            this->clear();

        }

        void update(){
        }

        void decHealth() {
        
            if (this->health > 1) {
                this->health = this->health - 2;
            }
            else {
                this->health = 0;
           
            }

        }

        void incHealth(uint8_t val) {
        
            if (this->health + val < 11) {
                this->health = this->health + val;
            }
            else {
                this->health = 11;
           
            }

        }

        Direction getDirection() {
        
            switch (this->stance) {
            
                case Stance::Player_Start_RH ... Stance::Player_End_RH:
                    return Direction::Right;
            
                case Stance::Player_Start_LH ... Stance::Player_End_LH:
                    return Direction::Left;

                default: 
                    return Direction::Right;
                    
            }

        }

        Rect getRect(int8_t worldOffset) {

            Rect rect;

            switch (this->getStance()) {

                case Stance::Player_Idle_RH:
                    rect.x = 58 +worldOffset;
                    rect.y = this->getY() + 2;
                    rect.width = 8;
                    rect.height = 9;
                    break;

                case Stance::Player_Lying_RH:
                    rect.x = 57 +worldOffset;
                    rect.y = this->getY() + 7;
                    rect.width = 11;
                    rect.height = 4;
                    break;

                case Stance::Player_Standing_Jump_RH_00 ... Stance::Player_Standing_Jump_RH_11:
                case Stance::Player_Running_Jump_RH_00 ... Stance::Player_Running_Jump_RH_11:
                case Stance::Player_Walk_RH_00 ... Stance::Player_Walk_RH_03:
                case Stance::Player_Falling_1L_A_RH_00 ... Stance::Player_Falling_1L_B_RH_07:
                    rect.x = 55 +worldOffset;
                    rect.y = this->getY() + 2;
                    rect.width = 14;
                    rect.height = 9;
                    break;

                case Stance::Player_Climbing_Vine_Up_RH_00 ... Stance::Player_Climbing_Vine_Down_RH_03:
                    rect.x = 56 + worldOffset;
                    rect.y = this->getY() + 2;
                    rect.width = 12;
                    rect.height = 10;
                    break;

                case Stance::Player_Idle_LH:
                    rect.x = 57 + worldOffset;
                    rect.y = this->getY() + 2;
                    rect.width = 8;
                    rect.height = 9;
                    break;

                case Stance::Player_Lying_LH:
                    rect.x = 57 + worldOffset;
                    rect.y = this->getY() + 7;
                    rect.width = 11;
                    rect.height = 4;
                    break;

                case Stance::Player_Standing_Jump_LH_00 ... Stance::Player_Standing_Jump_LH_11:
                case Stance::Player_Running_Jump_LH_00 ... Stance::Player_Running_Jump_LH_11:
                case Stance::Player_Walk_LH_00 ... Stance::Player_Walk_LH_03:
                case Stance::Player_Falling_1L_A_LH_00 ... Stance::Player_Falling_1L_B_LH_07:
                    rect.x = 55 + worldOffset;
                    rect.y = this->getY() + 2;
                    rect.width = 14;
                    rect.height = 9;
                    break;

                case Stance::Player_Climbing_Vine_Up_LH_00 ... Stance::Player_Climbing_Vine_Down_LH_03:
                    rect.x = 56 + worldOffset;
                    rect.y = this->getY() + 2;
                    rect.width = 12;
                    rect.height = 10;
                    break;

                default:
                    rect.x = 55 + worldOffset;
                    rect.y = this->getY() + 1;
                    rect.width = 14;
                    rect.height = 9;
                    break;

            }    

            return rect;

        }

        // Stack Methods ---------------------------------------
        
        bool isEmpty(void) {
            return this->stack.isEmpty();
        }

        Stance pop(void) {
            return this->stack.pop();
        }

      	uint8_t getCount(void) {
            return this->stack.getCount();
        }

        Stance & peek(void) {
            return this->stack.peek();
        }

        const Stance & peek(void) const {
            return this->stack.peek();
        }

        bool insert(const Stance & item) {
            return this->stack.insert(item);
        }

        void clear(void) {
            this->stack.clear();
        }

        bool push(Stance item) {
            return this->stack.push(static_cast<Stance>(item));
        }

        void insertSequence(Stance fromStance, Stance toStance) {

            for (uint16_t x = toStance; x >= fromStance; x--) {

                this->stack.insert(static_cast<Stance>(x));

            }

        }

        void pushSequence(Stance fromStance, Stance toStance) {

            this->pushSequence(fromStance, toStance, false);

        }

        void pushSequence(Stance fromStance, Stance toStance, bool clear) {

            if (clear) {

                this->stack.clear();
            }

            for (uint16_t x = static_cast<uint16_t>(toStance); x >= static_cast<uint16_t>(fromStance); x--) {

                this->stack.push(static_cast<Stance>(x));

            }

        }

};
