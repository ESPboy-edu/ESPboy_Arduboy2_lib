#pragma once

#include <Arduboy2.h>
#include "../utils/Enums.h"
#include "../../fxdata/fxdata.h"

class Bubble {

    private:

        Direction direction = Direction::East;
        Speed speed = Speed::Slow;
        Size size = Size::Large;
        int16_t x = 40;
        int16_t y = 47;
        
    public:

        Direction getDirection()                        { return this->direction; }
        Speed getSpeed()                                { return this->speed; }
        Size getSize()                                  { return this->size; }
        int16_t getX()                                  { return this->x; }        
        int16_t getY()                                  { return this->y; }        
        int8_t getX_Screen()                            { return this->x / 16; }        
        int8_t getY_Screen()                            { return this->y / 16; }        

        void setDirection(Direction val)                { this->direction = val; }
        void setSpeed(Speed val)                        { this->speed = val; }
        void setSize(Size val)                          { this->size = val; }
        void setX(int16_t val)                          { this->x = val; }
        void setY(int16_t val)                          { this->y = val; }

    public:

        void update() {

            switch (this->direction) {

                case Direction::NorthEast:

                    this->x = this->x + Constants::Bubble_Movement[static_cast<uint8_t>(this->speed)];
                    this->y = this->y - Constants::Bubble_Movement[static_cast<uint8_t>(this->speed)];

                    if (x > Constants::Bubble_X_Limit[static_cast<uint8_t>(this->size)] * 16) {
                        
                        x = Constants::Bubble_X_Limit[static_cast<uint8_t>(this->size)] * 16;
                        this->direction = Direction::NorthWest;
                        
                        if (y <= 16) {

                            y = 16;
                            this->direction = Direction::SouthWest;

                        }
                        
                    }
                    else if (y <= 16) {
                        
                        y = 16;
                        this->direction = Direction::SouthEast;
                        
                    }
                    break;

                case Direction::SouthEast:

                    this->x = this->x + Constants::Bubble_Movement[static_cast<uint8_t>(this->speed)];
                    this->y = this->y + Constants::Bubble_Movement[static_cast<uint8_t>(this->speed)];

                    if (x > Constants::Bubble_X_Limit[static_cast<uint8_t>(this->size)] * 16) {
                        
                        x = Constants::Bubble_X_Limit[static_cast<uint8_t>(this->size)] * 16;
                        this->direction = Direction::SouthWest;

                        if (y > Constants::Bubble_Y_Limit[static_cast<uint8_t>(this->size)] * 16) {

                            y = Constants::Bubble_Y_Limit[static_cast<uint8_t>(this->size)] * 16;
                            this->direction = Direction::NorthEast;

                        }
                        
                    }
                    else if (y > Constants::Bubble_Y_Limit[static_cast<uint8_t>(this->size)] * 16) {
                        
                        y = Constants::Bubble_Y_Limit[static_cast<uint8_t>(this->size)] * 16;
                        this->direction = Direction::NorthEast;
                        
                    }


                    break;

                case Direction::SouthWest:

                    this->x = this->x - Constants::Bubble_Movement[static_cast<uint8_t>(this->speed)];
                    this->y = this->y + Constants::Bubble_Movement[static_cast<uint8_t>(this->speed)];

                    if (x < 16) {
                        
                        x = 16;
                        this->direction = Direction::SouthEast;
                      
                        if (y > Constants::Bubble_Y_Limit[static_cast<uint8_t>(this->size)] * 16) {

                            y = Constants::Bubble_Y_Limit[static_cast<uint8_t>(this->size)] * 16;
                            this->direction = Direction::NorthEast;

                        }

                    }
                    else if (y > Constants::Bubble_Y_Limit[static_cast<uint8_t>(this->size)] * 16) {

                        y = Constants::Bubble_Y_Limit[static_cast<uint8_t>(this->size)] * 16;
                        this->direction = Direction::NorthWest;

                    }
                                        
                    break;

                case Direction::NorthWest:

                    this->x = this->x - Constants::Bubble_Movement[static_cast<uint8_t>(this->speed)];
                    this->y = this->y - Constants::Bubble_Movement[static_cast<uint8_t>(this->speed)];

                    if (x < 16) {
                        
                        x = 16;
                        this->direction = Direction::NorthEast;
                     
                        if (y < 16) {

                            y = 16;
                            this->direction = Direction::SouthWest;

                        }

                    }
                    else if (y < 16) {

                        y = 16;
                        this->direction = Direction::SouthWest;

                    }

                    break;
                    
            }

        }
        
};