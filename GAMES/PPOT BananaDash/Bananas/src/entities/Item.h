
#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"

class Item {

    private:

        ItemType itemType;
        int16_t x;
        int8_t y;
        uint8_t frame;
        int16_t data;
        uint16_t counter;

    public:

        int16_t getX()                                  { return this->x; }
        int8_t getY()                                   { return this->y; }
        uint8_t getFrame()                              { return this->frame; }
        int16_t getData()                               { return this->data; }
        uint16_t getCounter()                           { return this->counter; }
        ItemType getItemType()                          { return this->itemType; }

        void setItemType(ItemType val)                  { this->itemType = val; }
        void setX(int16_t val)                          { this->x = val; }
        void setY(int8_t val)                           { this->y = val; }
        void setFrame(uint8_t val)                      { this->frame = val; }
        void setData(int16_t val)                       { this->data = val; }
        void setCounter(uint16_t val)                   { this->counter = val; }


        bool updateCounter(uint16_t maxVal) {

            this->counter++;

            if (this->counter == maxVal) {
                this->counter = 0;
                return true;
            }

            return false;

        }

        ItemUpdate update() {

            switch (this->getItemType()) {

                case ItemType::Banana:

                    if (this->counter > 0) {
                    
                        this->counter--;

                        if (this->counter == 0) {
                        
                            this->itemType = ItemType::Banana_Hidden;
                            return ItemUpdate::LaunchBanana;

                        }

                    }
                        
                    break;


                case ItemType::Puff:

                    if (this->frame < Constants::Puff_Max) this->frame++;
                    break;


                case ItemType::Fire:

                    updateCounter(6 * 8);
                    this->frame = this->counter >> 3;

                    break;

                case ItemType::Fruit:

                    if (this->counter > 0) {
                    
                        this->counter--;

                        if (this->counter == 0) {
                        
                            this->itemType = ItemType::Fruit_Hidden;
                            return ItemUpdate::LaunchFruit;

                        }

                    }
                        
                    break;

                case ItemType::Bananarang_0:

                    if (this->getY() >= -16) {
                    
                        this->setY(this->getY() - 3);
                        this->setX(this->getX() - 3);
                        this->setData((this->getData() + 1) % 8);
                    
                    }

                    break;

                case ItemType::Bananarang_1:

                    if (this->getY() >= -16) {
                    
                        this->setY(this->getY() - 3);
                        this->setX(this->getX() + 3);
                        this->setData((this->getData() + 1) % 8);
                    
                    }

                    break;

                case ItemType::Bananarang_2:

                    if (this->getY() <= 64) {
                    
                        this->setY(this->getY() + 3);
                        this->setX(this->getX() - 3);
                        this->setData((this->getData() + 1) % 8);
                    
                    }

                    break;

                case ItemType::Bananarang_3:

                    if (this->getY() <= 64) {
                    
                        this->setY(this->getY() + 3);
                        this->setX(this->getX() + 3);
                        this->setData((this->getData() + 1) % 8);
                    
                    }

                    break;

                default: break;

            }

            return ItemUpdate::None;

        }

        Rect getRect(int16_t worldOffset) {
        
            Rect rect;

            switch (this->getItemType()) {
        
                case ItemType::Banana:

                    rect.x = 57 + worldOffset;
                    rect.y = this->getY() + 2;
                    rect.width = 6;
                    rect.height = 6;

                    break;
        
                case ItemType::Heart:

                    rect.x = 60 + worldOffset;
                    rect.y = this->getY() + 1;
                    rect.width = 5;
                    rect.height = 5;

                    break;

                case ItemType::Fire:

                    rect.x = 63 + worldOffset;
                    rect.y = this->getY() + 11;
                    rect.width = 5;
                    rect.height = 5;

                    break;

                case ItemType::Fruit:

                    rect.x = 63 + worldOffset;
                    rect.y = this->getY() + 7;
                    rect.width = 6;
                    rect.height = 6;

                    break;

                case ItemType::Bananarang_0:
                case ItemType::Bananarang_1:
                case ItemType::Bananarang_2:
                case ItemType::Bananarang_3:

                    switch (this->getData()) {
                    
                        case 0:
                            rect.x = 62 + worldOffset;
                            rect.y = this->getY() + 10;
                            rect.width = 8;
                            rect.height = 6;
                            break;
                    
                        case 1:
                            rect.x = 58 + worldOffset;
                            rect.y = this->getY() + 6;
                            rect.width = 8;
                            rect.height = 8;
                            break;
                    
                        case 2:
                            rect.x = 58 + worldOffset;
                            rect.y = this->getY() + 4;
                            rect.width = 6;
                            rect.height = 8;
                            break;
                    
                        case 3:
                            rect.x = 58 + worldOffset;
                            rect.y = this->getY() + 2;
                            rect.width = 8;
                            rect.height = 8;
                            break;
                    
                        case 4:
                            rect.x = 62 + worldOffset; 
                            rect.y = this->getY();
                            rect.width = 8;
                            rect.height = 6;
                            break;
                    
                        case 5:
                            rect.x = 64 + worldOffset; 
                            rect.y = this->getY() + 1;
                            rect.width = 8;
                            rect.height = 8;
                            break;
                    
                        case 6:
                            rect.x = 67 + worldOffset;
                            rect.y = this->getY() + 4;
                            rect.width = 6;
                            rect.height = 8;
                            break;
                    
                        case 7:
                            rect.x = 64 + worldOffset;
                            rect.y = this->getY() + 7;
                            rect.width = 8;
                            rect.height = 8;
                            break;
                    
                    }

                    break;

            }

            return rect;

        }

};
