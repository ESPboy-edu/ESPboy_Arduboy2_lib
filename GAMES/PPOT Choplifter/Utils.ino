#include "src/utils/Arduboy2Ext.h"
#include "Enums.h"
#include "Images.h"

/* ----------------------------------------------------------------------------
 *  A better absolute!
 */
template<typename T> T absT(const T & v) {
  
  return (v < 0) ? -v : v;

}


AbsHelicopterStance absHelicopterStance(HelicopterStance helicopterStance) {

  return (AbsHelicopterStance)absT((int8_t)helicopterStance);

}

/* ----------------------------------------------------------------------------
 *  Draw a horizontal dotted line. 
 *  
 *  So much nicer than a solid line!
 */
void drawHorizontalDottedLine(int x1, int x2, int y) {
    
  for (int z = x1; z <= x2; z+=2) {
    arduboy.drawPixel(z, y, WHITE);
  }
  
}


/* ----------------------------------------------------------------------------
 *  Slow the helicopter down or speed it up expotentially ..
 */
int8_t calcSpeed(int8_t initValue, bool increase) {
   if(initValue<2 & initValue>-2){ 
    if(increase) return (initValue+1);
    else return (initValue-1);
   }
   else return(initValue);
/*      
  switch (initValue) {

    case 8:
      if (!increase) return 4;
    
    case 4: 
      if (increase)  return 4;
      if (!increase) return 2;
    
    case 2:
      if (increase)  return 4;
      if (!increase) return 1;

    case 1:
      if (increase)  return 2;
      if (!increase) return 0;

    case 0:
      if (increase)  return 1;
      if (!increase) return -1;

    case -1:
      if (increase)  return 0;
      if (!increase) return -2;

    case -2:
      if (increase)  return -1;
      if (!increase) return -4;

    case -4:
      if (increase)  return -2;
      if (!increase) return -4;

    case -8:
      if (increase)  return -4;

    default: return 0;

  }*/

}


/* ----------------------------------------------------------------------------
 *  Draw the helicopter ..
 */
void drawHelicopter(int8_t x, int8_t y, HelicopterStance image) {

  uint8_t index = (uint8_t)absHelicopterStance(image);
  bool mirror = ((int8_t)image < 0);

  arduboy.drawCompressedMirror(x, y, helicopter[((index - 1) * 6) + 1], BLACK, mirror);
  arduboy.drawCompressedMirror(x, y, helicopter[(index - 1) * 6], WHITE, mirror);
  arduboy.drawCompressedMirror(x, y, helicopter[((index - 1) * 6) + (frame) + 2], WHITE, mirror);

}


/* ----------------------------------------------------------------------------
 *  Reset the helicopter ready for the next sortie ..
 */
void resetSortie() {

  heli.stance = HelicopterStance::Left_Level;
  heli.xDelta = 0;
  heli.yDelta = 0;
  heli.xInc = DELTA_X_DO_NOTHING;
  heli.xPos = 0;
  heli.yPos = 40;
  heli.prevTurn = PREV_TURN_FROM_LEFT; 
  heli.hits = 0; 
  heli.countDown = HELICOPTER_COUNT_DOWN_INACTIVE;

  tankBulletExplosion.xPos = BULLET_INACTIVE_X_VALUE;
  playerBulletExplosion.xPos = BULLET_INACTIVE_X_VALUE;
  backgroundXOffset = 0;
  playerStack.clear();

}


/* ----------------------------------------------------------------------------
 *  Reset the all variables ready for the next game ..
 */
void resetGame() {

  resetSortie();

  heli.xPos = 160;
  dead = 0;
  inHelicopter = 0;
  safe = 0;

  introduction_count = 0;
  sortieNumber = 1;

  dormitories[0] = { DormitoryState::Open,    DORMITORY_SPACING,       0 };
  dormitories[1] = { DormitoryState::Intact,  (DORMITORY_SPACING * 2), 0 };
  dormitories[2] = { DormitoryState::Intact,  (DORMITORY_SPACING * 3), 0 };
  dormitories[3] = { DormitoryState::Intact,  (DORMITORY_SPACING * 4), 0 };

  tanks[0] = {TankState::Stationary, TurrentDirection::Left_Low, 5, (1 * (int16_t)DORMITORY_SPACING) + (int16_t)random(-DORMITORY_SPACING_HALF, DORMITORY_SPACING_HALF), (1 * (int16_t)DORMITORY_SPACING), true, 0};
  tanks[1] = {TankState::Stationary, TurrentDirection::Left_Low, 5, (2 * (int16_t)DORMITORY_SPACING) + (int16_t)random(-DORMITORY_SPACING_HALF, DORMITORY_SPACING_HALF), (2 * (int16_t)DORMITORY_SPACING), true, 0};
  tanks[2] = {TankState::Stationary, TurrentDirection::Left_Low, 5, (3 * (int16_t)DORMITORY_SPACING) + (int16_t)random(-DORMITORY_SPACING_HALF, DORMITORY_SPACING_HALF), (3 * (int16_t)DORMITORY_SPACING), true, 0};
  tanks[3] = {TankState::Stationary, TurrentDirection::Left_Low, 5, (4 * (int16_t)DORMITORY_SPACING) + (int16_t)random(-DORMITORY_SPACING_HALF, DORMITORY_SPACING_HALF), (4 * (int16_t)DORMITORY_SPACING), true, 0};
  tanks[4] = {TankState::Stationary, TurrentDirection::Left_Low, 5, (5 * (int16_t)DORMITORY_SPACING) + (int16_t)random(-DORMITORY_SPACING_HALF, DORMITORY_SPACING_HALF), (5 * (int16_t)DORMITORY_SPACING), true, 0};
 

  for (uint8_t i = 0; i < NUMBER_OF_HOSTAGES; i++) {

      hostages[i] = { (i < 16 ? HostageStance::Leaving_Dorm : HostageStance::In_Dorm), (uint8_t)(((i % 16) * 15) + 10), dormitories[i / 16].xPos };

  }

  for (uint8_t i = 0; i < NUMBER_OF_PLAYER_BULLETS; i++) {
     
      playerBullets[i] = { BULLET_INACTIVE_X_VALUE, 0, 0, 0, 0 };

  }

  for (uint8_t i = 0; i < NUMBER_OF_TANK_BULLETS; i++) {
     
      tankBullets[i] = { BULLET_INACTIVE_X_VALUE, 0, 0, 0, 0 };

  }

  gameState = GameState::Introduction;

}


/* ----------------------------------------------------------------------------
 *  Has the nominated bullet hot anything ?
 */
void bulletHit(Bullet *bullet, BulletExplosion *explosion, bool playerBullet) {


  // Check to see if a tank bullet hit the helicopter (it can be at any height) ..
  
  {
    Rect hitZone = {50, 0, 0, 0};

    if (!playerBullet) {

      switch (absHelicopterStance(heli.stance)) {

        case AbsHelicopterStance::Side_Level ... AbsHelicopterStance::Side_Incline_2:
        case AbsHelicopterStance::Side_Reverse_Incline_1 ... AbsHelicopterStance::Side_Reverse_Incline_5:
        case AbsHelicopterStance::Side_Incline_Rotate_3:
    
          hitZone.x = 50;
          hitZone.y = heli.yPos + 5;
          hitZone.width = 28;
          hitZone.height = 12;

          break;

        case AbsHelicopterStance::Side_Incline_3 ... AbsHelicopterStance::Side_Incline_Max:
    
          hitZone.x = 50;
          hitZone.y = heli.yPos + 7;
          hitZone.width = 28;
          hitZone.height = 12;

          break;

        case AbsHelicopterStance::Side_Level_Rotate_1 ... AbsHelicopterStance::Side_Front_Incline_2:
        case AbsHelicopterStance::Side_Incline_Rotate_1 ... AbsHelicopterStance::Side_Incline_Rotate_2:

          hitZone.x = 57;
          hitZone.y = heli.yPos + 5;
          hitZone.width = 14;
          hitZone.height = 13;

          break;

      }

      Point pt = { 64 + bullet->xPos - heli.xPos, bullet->yPos };

      if (arduboy.collide(pt, hitZone)) {

        explosion->xPos = bullet->xPos;
        explosion->yPos = hitZone.y - 4;
        explosion->explosionType = ExplosionType::Both_Medium;
        bullet->xPos = BULLET_INACTIVE_X_VALUE;

        heli.hits++;

        if (heli.hits == HELICOPTER_BULLET_NUMBER_OF_HITS) {

          sound.tones(exploding);

          explosion->explosionType = ExplosionType::Large_1;
          if (heli.countDown == HELICOPTER_COUNT_DOWN_INACTIVE) {
            
            heli.countDown++;
            sound.tones(exploding);

          }
          

          // Kill any hostages that were in the helicopter ..

          dead = dead + inHelicopter;
          inHelicopter = 0;

          for (int i = 0; i < NUMBER_OF_HOSTAGES; i++) {

            Hostage *hostage = &hostages[i];
            if (hostage->stance == HostageStance::In_Helicopter) hostage->stance = HostageStance::Dead;

          }

        }

      }

    }

  }



  // Check to see if we hit anything ..

  switch (bullet->yPos) {

    case 40 ... 46:               // Hitting a dormitory?

      for (uint8_t i = 0; i < NUMBER_OF_DORMITORIES; i++) {

        Dormitory *dormitory = &dormitories[i];
        uint16_t diff = absT(dormitory->xPos - bullet->xPos);
 
        if (dormitory->state == DormitoryState::Intact && diff < 16) {

            dormitory->numberOfHits++;
            
            explosion->xPos = bullet->xPos;
            explosion->yPos = 36 + (diff / 3);
            explosion->explosionType = ExplosionType::Medium;

            if (dormitory->numberOfHits == DORMITORY_HITS_REQUIRED) {

              sound.tones(exploding);

              dormitory->state = DormitoryState::Open;
              explosion->xPos = bullet->xPos;
              explosion->yPos = 36 + (diff / 3);
              explosion->explosionType = ExplosionType::Large_1;


              // Release the hostages ..

              uint8_t count = 0;

              for (uint8_t i = 0; i < NUMBER_OF_HOSTAGES; i++) {

                Hostage *hostage = &hostages[i];

                if (hostage->stance == HostageStance::In_Dorm) {
                  
                  hostage->stance = HostageStance::Leaving_Dorm;
                  hostage->xPos = dormitory->xPos + 16;
                  hostage->countDown = (count * 15) + 10;
                  count++;

                  if (count == DORMITORY_HOSTAGE_CAPACITY) { break; }

                }

              }

            } 

            bullet->xPos = BULLET_INACTIVE_X_VALUE;

        }

      }

      break;

    case 47 ... 52:               // Hitting a hostage or the tank?
      {
        bool hit = false;

        for (uint8_t i = 0; i < NUMBER_OF_HOSTAGES; i++) {

          Hostage *hostage = &hostages[i];
          uint16_t diff = hostage->xPos - bullet->xPos;

          if (hostage->stance >= HostageStance::Running_Left_1 && 
              hostage->stance <= HostageStance::Waving_22 && 
              diff < 3) {

            sound.tones(kill_hostage);

            explosion->xPos = bullet->xPos;
            explosion->yPos = 52;
            explosion->explosionType = ExplosionType::Medium;
            bullet->xPos = BULLET_INACTIVE_X_VALUE;

            hostage->stance = HostageStance::Dying_2;
            dead++;

            if (dead + safe == NUMBER_OF_HOSTAGES) {

              heli.countDown = HELICOPTER_END_OF_GAME_START;

            }

            hit = true;
            break;

          }

        }

        if (!hit && playerBullet) {

          for (uint8_t i = 0; i < NUMBER_OF_TANKS; i++) {

            Tank *tank = &tanks[i];
            uint16_t diff = absT(tank->xPos - bullet->xPos);

            if (bullet->startYPos > TANK_BULLET_MIN_Y_VALUE && diff < 16) {  // Bullets that hit tank must be fired from down low ..

              explosion->xPos = bullet->xPos;
              explosion->yPos = (diff < 8 ? 46 : 51);
              explosion->explosionType = ExplosionType::Medium;
              bullet->xPos = BULLET_INACTIVE_X_VALUE;

              tank->numberOfHits++;

              if (tank->numberOfHits == (level == LEVEL_EASY ? TANK_BULLET_NUMBER_OF_HITS_EASY : TANK_BULLET_NUMBER_OF_HITS_HARD) ) {

                sound.tones(exploding);

                explosion->yPos = 50;
                explosion->explosionType = ExplosionType::Large_1;
                tank->state = TankState::Dead3;

              }

            }

          }

        }

      }
      break;

    case 61 ... 74:               //  Hitting the ground ..

      sound.tones(miss); 

      explosion->xPos = bullet->xPos;
      explosion->yPos = 60;
      explosion->explosionType = ExplosionType::Small;
      bullet->xPos = BULLET_INACTIVE_X_VALUE;
      break;

  }

}

/* ----------------------------------------------------------------------------
 *  Turn sound off and commit to the EEPROM.
 */
void turnSoundOff() {

  arduboy.audio.off(); 
  arduboy.audio.saveOnOff();
    
}



/* ----------------------------------------------------------------------------
 *  Turn sound on and commit to the EEPROM.
 */
void turnSoundOn() {

  arduboy.audio.on(); 
  arduboy.audio.saveOnOff();
    
}

/* ----------------------------------------------------------------------------
 *  Helper function for using image lookup arrays
 */
const uint8_t * imageArrayLookup(const uint8_t * const * const array) {

    return reinterpret_cast<const uint8_t *>(pgm_read_ptr(array));

}

/* ----------------------------------------------------------------------------
 *  Alternate helper function for using image lookup arrays
 */
template< typename IntT > const uint8_t * imageArrayLookup(const uint8_t * const * const array, IntT index) {

    return reinterpret_cast<const uint8_t *>(pgm_read_ptr(&array[index]));

}
