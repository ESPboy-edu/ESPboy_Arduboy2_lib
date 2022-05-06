#include "Bullet.h"
#include "../Utils/Arduboy2Ext.h"
#include "../Utils/Enums.h"

Bullet::Bullet() { }

uint8_t Bullet::getX() {

  return _x;

}

void Bullet::setX(const uint8_t value) {

  _x = value;

}

uint8_t Bullet::getY() {

  return _y;

}

void Bullet::setY(const uint8_t value) {

  _y = value;

}

Direction Bullet::getDirection() {

  return static_cast<Direction>(_details & 0x0F);

}

void Bullet::setDirection(const Direction value) {

  _details = (_details & 0xF0) | static_cast<uint8_t>(value);
  
}

bool Bullet::getEnabled() {

  return (_details & 0xF0) > 0;

}

void Bullet::setEnabled(const bool value) {

  _details = (_details & 0x0F) | (value ? 0xF0 : 0x00);
  
}

void Bullet::move() {
  
  uint8_t direction = static_cast<uint8_t>(getDirection());
  _x = _x + static_cast<int8_t>(pgm_read_byte(&movement_x_offset[direction]));
  _y = _y + static_cast<int8_t>(pgm_read_byte(&movement_y_offset[direction]));

  if (_x > WIDTH - SCOREBOARD_OUTER_RECT_WIDTH || _y > HEIGHT) setEnabled(false);

}

