#include "EnemyBullet.h"
#include "../Utils/Enums.h"

EnemyBullet::EnemyBullet() { }

void EnemyBullet::move() {

  _x = _x + static_cast<int8_t>(pgm_read_byte(&movement_x_offset[static_cast<uint8_t>(getDirection())]) * 2);
  _y = _y + static_cast<int8_t>(pgm_read_byte(&movement_y_offset[static_cast<uint8_t>(getDirection())]) * 2);

  if (_x > WIDTH - SCOREBOARD_OUTER_RECT_WIDTH) setEnabled(false);  
  if (_y > HEIGHT) setEnabled(false);
  
}

EnemyType EnemyBullet::getEnemyType() {

  return _enemyType;

}

void EnemyBullet::setEnemyType(const EnemyType value) {

  _enemyType = value;
  
}

uint8_t EnemyBullet::getX() {

  switch (_enemyType) {

    case EnemyType::Fighter2:
      return _x - 1;

    default:
      return _x;

  }

}


uint8_t EnemyBullet::getY() {

  switch (_enemyType) {

    case EnemyType::Fighter2:
      return _y - 1;

    default:
      return _y;

  }

}

BulletImage EnemyBullet::getBulletImage() {

  switch (_enemyType) {

    case EnemyType::Fighter2:

      switch (Bullet::getDirection()) {

        case Direction::North:
        case Direction::NorthEast:
        case Direction::South:
        case Direction::SouthWest:
        case Direction::NorthWest:
        case Direction::West:
          return BulletImage::Slash;

        case Direction::East:
        case Direction::SouthEast:
          return BulletImage::BackSlash;

      }
      
      break;

    default:
      return BulletImage::Single;

  }

  return BulletImage::Single;

}