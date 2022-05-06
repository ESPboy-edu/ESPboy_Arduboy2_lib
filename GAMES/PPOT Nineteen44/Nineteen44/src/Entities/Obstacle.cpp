#include "Obstacle.h"
#include "../Utils/Arduboy2Ext.h"
#include "Sprites.h"
#include "../Utils/Enums.h"
#include "FixedPoints.h"
#include "FixedPointsCommon.h"
#include "../Images/Images_Obstacles.h"

#define pgm_read_word_near (const uint8_t*)pgm_read_ptr

Obstacle::Obstacle(const SQ15x16 x, const SQ15x16 y, const uint8_t *bitmapRef, const SQ7x8 speed, const SQ7x8 value) :
          Base(x, y) {

  _enabled = false;
  _value = value;
  _speed = speed;
  _bitmap = bitmapRef;
}            

Rect Obstacle::getRect() {

  return (Rect){ _x.getInteger() + 1, _y.getInteger() + 1, 12, 10 };

}

void Obstacle::move() {

  if (_enabled) {
    _x = _x - (SQ15x16)_speed;
  }

}

SQ7x8 Obstacle::getValue() {

  return _value;

}

void Obstacle::setValue(const SQ7x8 value) {

  _value = value;

}

SQ7x8 Obstacle::getSpeed() {

  return _value;

}

void Obstacle::setSpeed(const SQ7x8 value) {

  _speed = value;

}

ObstacleType Obstacle::getObstacleType() {

  return _obstacleType;

}

void Obstacle::setObstacleType(const ObstacleType value) {

  _obstacleType = value;
  
}

void Obstacle::setBitmap(const uint8_t *value) {

  _bitmap = value;
  
}

void Obstacle::renderImage() {
    
  if (_enabled && _x.getInteger() + OBSTACLE_WIDTH >= 0 && _x.getInteger() < WIDTH) {
    Sprites::drawExternalMask(_x.getInteger(), _y.getInteger(), _bitmap, power_up_mask, 0, 0);
  }
  else if (_x.getInteger() + OBSTACLE_WIDTH < 0) {
    _enabled = false;
  }

}
