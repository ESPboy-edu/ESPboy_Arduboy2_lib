#include "EnemyBullet.h"
#include "Enums.h"

EnemyBullet::EnemyBullet() { }

void EnemyBullet::move() {
  
  switch (_direction) {
  
    case Direction::North:
      _y = _y - 2;
      break;
  
    case Direction::NorthEast:
      _x = _x + 2;
      _y = _y - 2;
      break;
  
    case Direction::East:
      _x = _x + 2;
      break;
  
    case Direction::SouthEast:
      _x = _x + 2;
      _y = _y + 2;
      break;
  
    case Direction::South:
      _y = _y + 2;
      break;
  
    case Direction::SouthWest:
      _x = _x - 2;
      _y = _y + 2;
      break;
  
    case Direction::West:
      _x = _x - 2;
      break;
  
    case Direction::NorthWest:
      _x = _x - 2;
      _y = _y - 2;
      break;
      
  }
  
  if (_x > WIDTH - SCOREBOARD_OUTER_RECT_WIDTH) _enabled = false;  
  if (_y > HEIGHT) _enabled = false;
  
}

