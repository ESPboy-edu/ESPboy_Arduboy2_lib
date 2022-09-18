#include "Bullet.h"
#include "Arduboy2.h"
#include "Enums.h"

Bullet::Bullet() { }

const uint8_t Bullet::getX() const {

  return _x;

}

void Bullet::setX(const uint8_t value) {

  _x = value;

}

const uint8_t Bullet::getY() const {

  return _y;

}

void Bullet::setY(const uint8_t value) {

  _y = value;

}

const Direction Bullet::getDirection() const {

  return _direction;

}

void Bullet::setDirection(const Direction value) {

  _direction = value;
  
}

const bool Bullet::getEnabled() const {

  return _enabled;

}

void Bullet::setEnabled(const bool value) {

  _enabled = value;
  
}

void Bullet::move() {
  
  switch (_direction) {
  
    case Direction::North:
      --_y;  
      break;
  
    case Direction::NorthEast:
      ++_x;
      --_y;
      break;
  
    case Direction::East:
      ++_x;
      break;
  
    case Direction::SouthEast:
      ++_x;
      ++_y;
      break;
  
    case Direction::South:
      ++_y;
      break;
  
    case Direction::SouthWest:
      --_x;
      ++_y;
      break;
  
    case Direction::West:
      --_x;
      break;
  
    case Direction::NorthWest:
      --_x;
      --_y;
      break;
      
  }

  if (_x > WIDTH - SCOREBOARD_OUTER_RECT_WIDTH) _enabled = false;    
  if (_y > HEIGHT) _enabled = false;
  
}

