#pragma once

#include "Enums.h"

class Bullet {

  public:

    Bullet();

    void move();
     
    const uint8_t getX() const;
    const uint8_t getY() const;
    const Direction getDirection() const;
    const bool getEnabled() const;

    void setX(const uint8_t value);
    void setY(const uint8_t value);
    void setDirection(const Direction value);
    void setEnabled(const bool value);     
  
  protected:
  
    uint8_t _x;               
    uint8_t _y; 
    Direction _direction;   
    bool _enabled;           

};

