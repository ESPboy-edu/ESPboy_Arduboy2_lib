#include "src/utils/Arduboy2Ext.h"
#include "Enums.h"
#include "Images.h"
#include "src/utils/Stack.h"


/* ----------------------------------------------------------------------------
 *  Handle hostage movements ..
 */
void hostageMovements() {


  // Update hostages ..

  HostageStance randomLowerLimit = HostageStance::Running_Left_1;
  HostageStance randomTopLimit = HostageStance::Running_Right_4;

  for (uint8_t i = 0; i < NUMBER_OF_HOSTAGES; i++) {

    Hostage *hostage = &hostages[i];


    // Collect hostages ..

    if (inHelicopter < HELICOPTER_HOSTAGE_CAPACITY &&
        hostage->stance != HostageStance::Dead && hostage->stance > HostageStance::In_Dorm && hostage->stance <= HostageStance::Leaving_Dorm &&
        heli.countDown == HELICOPTER_COUNT_DOWN_INACTIVE && heli.xPos > 50 && heli.yPos >= 40 && absT(hostage->xPos - heli.xPos) < 5
        ) {    

      sound.tones(collect_hostage);
      
      inHelicopter++;
      hostage->stance = HostageStance::In_Helicopter;
      hostage->countDown = (14 * inHelicopter);

    }


    // Deposit hostages at base ..

    else if (hostage->stance == HostageStance::In_Helicopter && 
             heli.yPos >= 40 &&                                                                                               
             ((heli.stance == HelicopterStance::Left_Level && heli.xPos < 26 && heli.xPos > -6) || 
              (heli.stance == HelicopterStance::Right_Level && heli.xPos < 24 && heli.xPos > 2) || 
              (heli.stance == HelicopterStance::Front_Level && heli.xPos < 24 && heli.xPos > -14))) {                

      hostage->countDown--;

      if (hostage->countDown == 0) {

        hostage->stance = HostageStance::Running_Right_1;
        hostage->countDown = 255;
        hostage->xPos = heli.xPos;
        inHelicopter--;

      }

    }

    // Hostages are safe ..
    
    else if (hostage->stance != HostageStance::Safe && hostage->xPos < -40) {                                                                                   

        hostage->stance = HostageStance::Safe;
        sound.tones(safety);
        safe++;

        if (dead + safe == NUMBER_OF_HOSTAGES) {

          heli.countDown = HELICOPTER_END_OF_GAME_START;

        }

    }

    // Hostages run wild!

    else {                                                                                                            

      switch (hostage->stance) {

        case HostageStance::Running_Left_1 ... HostageStance::Running_Left_3:

          hostage->stance = (HostageStance)((uint8_t)hostage->stance + 1);
          hostage->countDown--;
          hostage->xPos++;
          randomTopLimit = HostageStance::Waving_22;
          break;

        case HostageStance::Running_Left_4:

          hostage->stance = HostageStance::Running_Left_1;
          hostage->countDown--;
          hostage->xPos++;
          randomTopLimit = HostageStance::Waving_22;
          break;

        case HostageStance::Running_Right_1 ... HostageStance::Running_Right_3:

          hostage->stance = (HostageStance)((uint8_t)hostage->stance + 1);
          hostage->countDown--;
          hostage->xPos--;
          randomTopLimit = HostageStance::Waving_22;
          break;

        case HostageStance::Running_Right_4:

          hostage->stance = HostageStance::Running_Right_1;
          hostage->countDown--;
          hostage->xPos--;
          randomTopLimit = HostageStance::Waving_22;
          break;

        case HostageStance::Waving_11 ... HostageStance::Waving_12:

          hostage->stance = (HostageStance)((uint8_t)hostage->stance + 1);
          hostage->countDown--;
          randomTopLimit = HostageStance::Running_Right_4;
          break;

        case HostageStance::Waving_21 ... HostageStance::Waving_22:

          hostage->stance = (HostageStance)((uint8_t)hostage->stance + 1);
          if (hostage->stance > HostageStance::Waving_22) hostage->stance = HostageStance::Waving_11;
          hostage->countDown--;
          randomTopLimit = HostageStance::Running_Right_4;
          break;

        case HostageStance::Leaving_Dorm:

          hostage->stance = (HostageStance)((uint8_t)hostage->stance - 1);
          if (hostage->countDown == 0) {
            randomTopLimit = HostageStance::Running_Right_4;
          }
          break;

        default: break;

      }


      if (hostage->countDown == 0) {

        if (absT(hostage->xPos - heli.xPos) < 70) {

          if (random(0, 15) < 13) {

            hostage->stance = (hostage->xPos > heli.xPos ? (hostage->xPos > HOSTAGE_FAR_RIGHT_POS ? HostageStance::Running_Right_1 : HostageStance::Running_Left_1) : HostageStance::Running_Left_1);
            hostage->countDown = random(5, 15);

          }
          else {

            hostage->stance = HostageStance::Waving_11;
            hostage->countDown = random(2, 5);

          }

        }
        else {

          if (hostage->xPos < HOSTAGE_FAR_RIGHT_POS) { randomTopLimit = HostageStance::Running_Left_4; }  // Prevent hostage from crossing fence ..      
          if (hostage->xPos > HOSTAGE_FAR_LEFT_POS)  { randomLowerLimit = HostageStance::Running_Right_1; }  // Prevent hostage from crossing fence ..      

          hostage->stance = (HostageStance)random((uint8_t)randomLowerLimit, (uint8_t)randomTopLimit + 1);

          switch (hostage->stance) {

            case HostageStance::Running_Left_1 ... HostageStance::Running_Right_4:
              hostage->countDown = random(5, 25);
              break;

            case HostageStance::Waving_11 ... HostageStance::Waving_22:
              hostage->countDown = random(2, 5);
              break;

            default: break;

          }

        }

      }

    }

  }

}