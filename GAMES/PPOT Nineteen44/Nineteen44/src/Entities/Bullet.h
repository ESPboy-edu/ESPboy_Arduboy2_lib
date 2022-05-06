#pragma once

#include "../Utils/Enums.h"

class Bullet {

  public:

    Bullet();

    void move();
     
    uint8_t getX();
    uint8_t getY();
    Direction getDirection();
    bool getEnabled();

    void setX(const uint8_t value);
    void setY(const uint8_t value);
    void setDirection(const Direction value);
    void setEnabled(const bool value);     
  
  protected:
  
    uint8_t _x;               
    uint8_t _y; 
    uint8_t _details;

};

