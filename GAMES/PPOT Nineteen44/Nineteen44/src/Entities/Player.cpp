#include "../Utils/Arduboy2Ext.h"
#include <Sprites.h>
#include "Player.h"
#include "../Utils/Enums.h"
#include "../Images/Images_Player.h"

#define ROLL_DELAY  8

#define pgm_read_word_near (const uint8_t*)pgm_read_ptr

Player::Player(const uint8_t * const * images) : Plane(images) {

  initGame();

}

void Player::setY(const SQ15x16 value) {

  if (value < _y) _movement = _movement + PLAYER_MOVE_UP;
  if (value > _y) _movement = _movement + PLAYER_MOVE_DOWN;

  _y = value;

}

void Player::initGame() {
            
  _score = 0;
  initMission(true);
  
}

void Player::initMission(bool initConsumables) {
            
  _powerUp = false;
  _fuel = FUEL_MAX;
  _health = HEALTH_MAX; 
  _bullets = BULLETS_MAX;
  _rollState = 0;
  _enabled = true;
  _x = 1;
  _y = 24;

  if (initConsumables) {

    _fuel = FUEL_MAX;
    _health = HEALTH_MAX; 
    _bullets = BULLETS_MAX;

  }
  else {

    if (_fuel < INIT_FUEL_MIN)         _fuel = INIT_FUEL_MIN;
    if (_health < INIT_HEALTH_MIN)     _health = INIT_HEALTH_MIN;
    if (_bullets < INIT_BULLETS_MIN)   _bullets = INIT_BULLETS_MIN;

  }
  
}

SQ7x8 Player::getFuel() {

  return _fuel;

}

void Player::setFuel(const SQ7x8 value) {

  _fuel = value;

}

void Player::decFuel(const SQ7x8 value) {

  if (_fuel > 0) {
    _fuel = _fuel - value;
    if (_fuel < 0) _fuel = 0; 
  }

}

uint8_t Player::getBullets() {

  return _bullets;

}

void Player::setBullets(const uint8_t value) {

  _bullets = value;

}

void Player::addBullets(const uint8_t value) {

  uint16_t newBullets = _bullets + value;
  _bullets = (newBullets > BULLETS_MAX ? BULLETS_MAX : newBullets);

}

void Player::decBullets() {

  --_bullets;

}

uint16_t Player::getScore() {

  return _score;

}

void Player::setScore(const uint16_t value) {

  _score = value;

}

void Player::startRoll() {

  _rollState = ROLL_DELAY;

}

bool Player::inRoll() {

  return (_rollState != 0);

}

void Player::setPowerUp(const bool value) {

  _powerUp = (value ? POWER_UP_MAX : 0);

}

void Player::decPowerUp() {

  if (_powerUp > 0) --_powerUp;

}

bool Player::getPowerUp() {

  return (_powerUp != 0);

}

const int8_t PROGMEM roll_x_offset[] = { 0, 7, 14, 7, 0, -1, -2, -1 };

void Player::renderImage(uint8_t frame) {

  int16_t x = _x.getInteger();
  int16_t y = _y.getInteger();

  if (x + PLAYER_WIDTH >= 0 && x < WIDTH) {

    if (_fuel > 0 && _health >= -5) {

      uint8_t roll = _rollState / ROLL_DELAY;
      int8_t rollX = x + static_cast<int8_t>(pgm_read_byte(&roll_x_offset[roll])); 

      if (roll > 0 && roll < 8) {
        ++_rollState;
      }
      if (_rollState == 64) {
        _rollState = 0;
      }

      if (_health > -3) {

        #ifdef PLAYER_US
          if (roll != 0 || (roll == 0 && absT(_movement) < 4)) {
            Sprites::drawExternalMask(rollX, y, pgm_read_word_near(&_bitmaps[static_cast<uint8_t>(roll) ]), pgm_read_word_near(&_bitmaps[IMAGES_MASK_OFFSET + (static_cast<uint8_t>(roll) )]), (roll == 0 ? frame : 0), 0);
          }
          else {
              Sprites::drawExternalMask(rollX, y, p38_move, p38_move_mask, (_movement > 0 ? 0 : 1), (_movement > 0 ? 0 : 1));
              if (_movement < 0) _movement++;
              if (_movement > 0) _movement--;
          }
        #endif

        #ifdef PLAYER_JAPANESE
          if (roll != 0 || (roll == 0 && absT(_movement) < 4)) {
            Sprites::drawExternalMask(rollX, y, pgm_read_word_near(&_bitmaps[static_cast<uint8_t>(roll) ]), pgm_read_word_near(&_bitmaps[IMAGES_MASK_OFFSET + (static_cast<uint8_t>(roll) )]), (roll == 0 ? frame : 0), 0);
          }
          else {
              Sprites::drawExternalMask(rollX, y, zero_move, zero_move_mask, (_movement > 0 ? 0 : 1), (_movement > 0 ? 0 : 1));
              if (_movement < 0) _movement++;
              if (_movement > 0) _movement--;
          }
        #endif

      }

    }
 
    if (_health <= 0) {
 
      _health = _health - static_cast<SQ7x8>(0.05);
      const auto bitmap = (abs(_health.getInteger()) - 1);

      switch (_health.getInteger()) {
  
        case -4 ... -1:
          Sprites::drawExternalMask(x, y, pgm_read_word_near(&_bitmaps[IMAGES_EXPLOSION_OFFSET + static_cast<uint8_t>(bitmap)]), pgm_read_word_near(&_bitmaps[IMAGES_EXPLOSION_MASK_OFFSET + static_cast<uint8_t>(bitmap)]), 0, 0);
          break;

        case -6 ... -5:
          Sprites::drawSelfMasked(x, y, pgm_read_word_near(&_bitmaps[IMAGES_EXPLOSION_OFFSET + 3]), 0);
          break;         

        case -10:
          _enabled = false;
          break;
        
      }

    }

    if (_fuel <= 0 && _health > 0) {

      _fuel = _fuel - (SQ7x8)0.05;

      uint8_t fuelX = x;
      uint8_t absFuel = abs(_fuel.getInteger());
      uint8_t bitmap = 0;
      uint8_t mask = 0;
      
      switch (absFuel) {
   
        case 0:
          fuelX += 6;
          bitmap = 1;
          mask = IMAGES_MASK_OFFSET + 1;
          break;
          
        case 1:
          fuelX += 11;
          bitmap = 1;
          mask = IMAGES_MASK_OFFSET + 1;
          break;

        case 2:
          fuelX += 14;
          bitmap = 1;
          mask = IMAGES_MASK_OFFSET + 1;
          break;
     
        case 3:
          fuelX += 20;
          bitmap = 6;
          mask = IMAGES_MASK_OFFSET + 6;
          break;

        case 4 ... 7:
          fuelX += 22 + (-4 - _fuel.getInteger());
          bitmap = IMAGES_SHRINK_OFFSET + (absFuel - 4);
          mask = IMAGES_SHRINK_MASK_OFFSET + (absFuel - 4);
          break;

        case 8 ... 11:
          fuelX += 20;
          bitmap = IMAGES_EXPLOSION_OFFSET + (absFuel - 8);
          mask = IMAGES_EXPLOSION_MASK_OFFSET + (absFuel - 8);
          break;
          
        case 16:
          _enabled = false;
          break;
          
      }    

      if (bitmap != 0) {
        Sprites::drawExternalMask(fuelX, y, pgm_read_word_near(&_bitmaps[bitmap]), pgm_read_word_near(&_bitmaps[mask]), 0, 0);
      }
      
    }
    
  }

}
