#include "src/utils/Arduboy2Ext.h"
#include "Enums.h"
#include "Images.h"
#include "src/utils/Stack.h"


/* ----------------------------------------------------------------------------
 *  Handle tank movements ..
 */
void tankMovements() {


  // Update tank ..

  for (uint8_t i=0; i < NUMBER_OF_TANKS; i++) {

    Tank *tank = &tanks[i];

    int16_t tankDif = tank->xPos - heli.xPos;

    if (tank->state > TankState::Dead3) {

      switch (tank->state) {

        case TankState::Move_Left:
          tank->xPos+=2;
          break;

        case TankState::Move_Right:

          if (tank->xPos > TANK_FAR_RIGHT_POS) {
            tank->xPos-=2;
          }
          else {
            tank->state = TankState::Stationary;
          }
          break;

        default: break;

      }


      // Move turrent ..

      switch (tankDif) {

        case -999 ... -40:
          tank->turrentDirection = TurrentDirection::Left_Low;
          break;

        case -39 ... -13:
          tank->turrentDirection = TurrentDirection::Left_Mid;
          break;

        case -12 ... 12:
          tank->turrentDirection = TurrentDirection::Upright;
          break;

        case 13 ... 39:
          tank->turrentDirection = TurrentDirection::Right_Mid;
          break;

        case 40 ... 999:
          tank->turrentDirection = TurrentDirection::Right_Low;
          break;

      }

      
      tank->countDown--;


      // Tank has completed the last move.  Chose a new random move ..
      
      if (tank->countDown == 0) {

        switch (random(0, 3)) {

          case 0:

            if (tankDif < -30 && tank->xPos < tank->startingXPos + DORMITORY_SPACING_HALF - TANK_WIDTH) {

              tank->state = TankState::Move_Left;

            }
            else if (tankDif > 30 && tank->xPos > tank->startingXPos - DORMITORY_SPACING_HALF + TANK_WIDTH) {

              tank->state = TankState::Move_Right;
              
            }
            else {

              tank->state = TankState::Stationary;
              
            }

            break;

          case 1:

            if (tankDif > 30 && tank->xPos > tank->startingXPos - DORMITORY_SPACING_HALF + TANK_WIDTH) {

              tank->state = TankState::Move_Right;

            }
            else if (tankDif < -30 && tank->xPos < tank->startingXPos + DORMITORY_SPACING_HALF - TANK_WIDTH) {

              tank->state = TankState::Move_Left;
              
            }
            else {

              tank->state = TankState::Stationary;
              
            }

            break;

          case 2:

            tank->state = TankState::Stationary;
            
            break;

        }

        tank->countDown = random(4, 12);

      }


      // Are we in a position to fire ?

      if (absT(tankDif) < 45) {

        if (random(0, (level == LEVEL_EASY ? 40 : 25) ) == 0) {

          for (uint8_t i = 0; i < NUMBER_OF_TANK_BULLETS; i++) {
            
            Bullet *bullet = &tankBullets[i];

            if (bullet->xPos == BULLET_INACTIVE_X_VALUE) {

            sound.tones(tank_firing); 

              bullet->yPos = 40;
              bullet->yDelta = -8;
              bullet->startYPos = bullet->yPos;

              switch (tank->turrentDirection) {

                case TurrentDirection::Left_Low:
                  bullet->xPos = tank->xPos + 11;
                  bullet->xDelta = -4;
                  break;

                case TurrentDirection::Left_Mid:
                  bullet->xPos = tank->xPos + 9;
                  bullet->xDelta = -2;
                  break;

                case TurrentDirection::Upright:
                  bullet->xPos = tank->xPos;
                  bullet->xDelta = random(-1, 2);
                  break;

                case TurrentDirection::Right_Mid:
                  bullet->xPos = tank->xPos - 9;
                  bullet->xDelta = 2;
                  break;

                case TurrentDirection::Right_Low:
                  bullet->xPos = tank->xPos - 11;
                  bullet->xDelta = 4;
                  break;

              }

              break;

            }

          }

        }

      }

    }

  }

}