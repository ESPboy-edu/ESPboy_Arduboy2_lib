#pragma once
 
#include "Arduboy2.h"
#include "Sprites.h"
#include "Arduino.h"
#include "../Utils/Enums.h"
#include "Base.h"
#include "FixedPoints.h"
#include "FixedPointsCommon.h"

class Obstacle : public Base {
  
  public:
   
    Obstacle(const SQ15x16 x, const SQ15x16 y, const uint8_t *bitmapRef, const SQ7x8 speed, const SQ7x8 value);

    SQ7x8 getValue();
    SQ7x8 getSpeed();
    ObstacleType getObstacleType();
    Rect getRect();
           
    void setValue(const SQ7x8 value);
    void setSpeed(const SQ7x8 value);
    void setObstacleType(const ObstacleType value);
    void setBitmap(const uint8_t *value);

    void move();
    void renderImage();  
    
  private:
             
    SQ7x8 _speed;               
    SQ7x8 _value;
    ObstacleType _obstacleType;
    const uint8_t *_bitmap;
    
};


