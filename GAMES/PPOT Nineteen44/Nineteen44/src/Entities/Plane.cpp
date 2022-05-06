#include "Plane.h"
#include "../Utils/Arduboy2Ext.h"
#include "Sprites.h"
#include "../Utils/Enums.h"
#include "FixedPoints.h"
#include "FixedPointsCommon.h"

#define pgm_read_word_near (const uint8_t*)pgm_read_ptr

Plane::Plane(const uint8_t * const * images) : Base(0, 0) {
  
  _bitmaps = images;

}

const Rect Plane::getRect() const {
  return { static_cast<int16_t>(_x.getInteger() + 1), static_cast<int16_t>(_y.getInteger() + 1), static_cast<uint8_t>(pgm_read_byte(pgm_read_word_near(_bitmaps)) - 2), static_cast<uint8_t>(pgm_read_byte(pgm_read_word_near(_bitmaps) + 1) - 2) };
}

const SQ7x8 Plane::getHealth() const {

  return _health;

}

void Plane::setHealth(const SQ7x8 value) {

  _health = value;

}

void Plane::addHealth(const SQ7x8 value) {

  _health = (_health + value > HEALTH_MAX ? HEALTH_MAX : _health + value);
  
}

void Plane::decHealth(const SQ7x8 value) {

  _health = _health - value;  
  if (_health < 0) _health = 0; 
  
}

