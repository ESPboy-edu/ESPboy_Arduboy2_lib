#include "src/utils/Arduboy2Ext.h"
#include "Enums.h"
#include "Images.h"
#include "src/utils/Stack.h"


/* ----------------------------------------------------------------------------
 *  Hanlde player movements ..
 */
void playerMovements() {


  // Up and down movements ..

  if (arduboy.pressed(UP_BUTTON)) {

    if (heli.yPos > HELICOPTER_MAXIMUM_HEIGHT) {

      heli.yDelta = calcSpeed(heli.yDelta, false);  

    }

  } 
  
  else {
    
    if (heli.yPos < HELICOPTER_MINIMUM_HEIGHT) {

      if (level == LEVEL_HARD) {
        heli.yDelta = calcSpeed(heli.yDelta, true);  
      }
      else {
        
        if (heli.yDelta < 1) {
          heli.yDelta = calcSpeed(heli.yDelta, true);  
        }

      }

    }

  }


  // Turn the helicoper ..

  if (arduboy.pressed(B_BUTTON)) {

    if (heli.xDelta == 0) {

      switch (heli.stance) {

        case HelicopterStance::Left_Level:
          playerStack.push(HelicopterStance::Front_Level, HelicopterStance::Left_Level_Rotate_3);
          playerStack.push(HelicopterStance::Left_Level_Rotate_2, HelicopterStance::Left_Level_Rotate_1);
          heli.prevTurn = PREV_TURN_FROM_LEFT;
          break;

        case HelicopterStance::Right_Level:
          playerStack.push(HelicopterStance::Front_Level, HelicopterStance::Right_Level_Rotate_3);
          playerStack.push(HelicopterStance::Right_Level_Rotate_2, HelicopterStance::Right_Level_Rotate_1);
          heli.prevTurn = PREV_TURN_FROM_RIGHT;
          break;

        case HelicopterStance::Front_Level:

          if (heli.prevTurn == PREV_TURN_FROM_LEFT) {
            playerStack.push(HelicopterStance::Right_Level, HelicopterStance::Right_Level_Rotate_1);
            playerStack.push(HelicopterStance::Right_Level_Rotate_2, HelicopterStance::Right_Level_Rotate_3);
          }
          else {
            playerStack.push(HelicopterStance::Left_Level, HelicopterStance::Left_Level_Rotate_1);
            playerStack.push(HelicopterStance::Left_Level_Rotate_2, HelicopterStance::Left_Level_Rotate_3);
          }

          break;

          default: break;
      }

    }
    else {

      switch (heli.stance) {

        case HelicopterStance::Right_Reverse_Incline_5:
          playerStack.push(HelicopterStance::Left_Incline_Max, HelicopterStance::Left_Incline_Rotate_3);
          playerStack.push(HelicopterStance::Left_Incline_Rotate_2, HelicopterStance::Left_Incline_Rotate_1);
          break;

        case HelicopterStance::Right_Front_Incline_2:
          playerStack.push(HelicopterStance::Right_Incline_Max, HelicopterStance::Right_Incline_Rotate_3);
          break;

        case HelicopterStance::Left_Front_Incline_2:
          playerStack.push(HelicopterStance::Left_Incline_Max, HelicopterStance::Left_Incline_Rotate_3);
          break;
      
        case HelicopterStance::Left_Reverse_Incline_5:
          playerStack.push(HelicopterStance::Right_Incline_Max, HelicopterStance::Right_Incline_Rotate_3);
          playerStack.push(HelicopterStance::Right_Incline_Rotate_2, HelicopterStance::Right_Incline_Rotate_1);
          break;

        default: break;

      }

    }

  }


  // Left and right movements ..

  if (arduboy.pressed(LEFT_BUTTON)) {

    if (heli.yPos < HELICOPTER_MINIMUM_HEIGHT || arduboy.pressed(UP_BUTTON)) {

      switch (heli.stance) {

        case HelicopterStance::Left_Level:
          playerStack.push(HelicopterStance::Left_Incline_Max, HelicopterStance::Left_Incline_4, HelicopterStance::Left_Incline_3);
          playerStack.push(HelicopterStance::Left_Incline_2, HelicopterStance::Left_Incline_1);
          heli.xInc = DELTA_X_DECREASE;          
          break;

        case HelicopterStance::Front_Level:
          playerStack.push(HelicopterStance::Left_Front_Incline_2, HelicopterStance::Left_Front_Incline_1);
          heli.xInc = DELTA_X_DECREASE;          
          break;

        case HelicopterStance::Left_Reverse_Incline_5:
          playerStack.push(HelicopterStance::Left_Incline_Max, HelicopterStance::Left_Incline_4, HelicopterStance::Left_Incline_3);
          playerStack.push(HelicopterStance::Left_Incline_2, HelicopterStance::Left_Incline_1, HelicopterStance::Left_Level);
          playerStack.push(HelicopterStance::Left_Reverse_Incline_1, HelicopterStance::Left_Reverse_Incline_2);
          playerStack.push(HelicopterStance::Left_Reverse_Incline_3, HelicopterStance::Left_Reverse_Incline_4);
          heli.xInc = DELTA_X_DECREASE;          
          break;

        case HelicopterStance::Right_Level:
          playerStack.push(HelicopterStance::Right_Reverse_Incline_5, HelicopterStance::Right_Reverse_Incline_4, HelicopterStance::Right_Reverse_Incline_3);
          playerStack.push(HelicopterStance::Right_Reverse_Incline_2, HelicopterStance::Right_Reverse_Incline_1);
          heli.xInc = DELTA_X_DECREASE;          
          break;

        case HelicopterStance::Right_Incline_Max:
          playerStack.push(HelicopterStance::Right_Reverse_Incline_5, HelicopterStance::Right_Reverse_Incline_4, HelicopterStance::Right_Reverse_Incline_3);
          playerStack.push(HelicopterStance::Right_Reverse_Incline_2, HelicopterStance::Right_Reverse_Incline_1);
          playerStack.push(HelicopterStance::Right_Level, HelicopterStance::Right_Incline_1, HelicopterStance::Right_Incline_2);
          playerStack.push(HelicopterStance::Right_Incline_3, HelicopterStance::Right_Incline_4);
          heli.xInc = DELTA_X_DECREASE;          
          break;

        case HelicopterStance::Right_Front_Incline_2:
          playerStack.push(HelicopterStance::Left_Front_Incline_2, HelicopterStance::Left_Front_Incline_1);
          playerStack.push(HelicopterStance::Front_Level, HelicopterStance::Right_Front_Incline_1);
          heli.xInc = DELTA_X_INCREASE;          
          break;

        default:
          heli.xInc = DELTA_X_DECREASE;          
          break;
        
      }

    }
    else {    // We are on the ground.

      switch (heli.stance) {

        case HelicopterStance::Left_Level:
          playerStack.push(HelicopterStance::Set_Delta_X_Zero);
          playerStack.push(HelicopterStance::Left_Level, HelicopterStance::Left_Incline_1, HelicopterStance::Left_Incline_2);
          playerStack.push(HelicopterStance::Left_Incline_2, HelicopterStance::Set_Delta_X_Increase);
          playerStack.push(HelicopterStance::Left_Incline_2, HelicopterStance::Left_Incline_2, HelicopterStance::Left_Incline_1);
          heli.xInc = DELTA_X_DECREASE;          
          break;

        case HelicopterStance::Left_Incline_Max:
          playerStack.push(HelicopterStance::Set_Delta_X_Zero);
          playerStack.push(HelicopterStance::Left_Level, HelicopterStance::Left_Incline_1);
          playerStack.push(HelicopterStance::Left_Incline_2, HelicopterStance::Left_Incline_3, HelicopterStance::Left_Incline_4);
          heli.xInc = DELTA_X_DECREASE;          
          break;

        case HelicopterStance::Front_Level:
          playerStack.push(HelicopterStance::Set_Delta_X_Zero);
          playerStack.push(HelicopterStance::Front_Level, HelicopterStance::Left_Front_Incline_1, HelicopterStance::Left_Front_Incline_1);
          playerStack.push(HelicopterStance::Set_Delta_X_Increase);
          playerStack.push(HelicopterStance::Left_Front_Incline_2, HelicopterStance::Left_Front_Incline_1, HelicopterStance::Left_Front_Incline_1);
          heli.xInc = DELTA_X_DECREASE;          
          break;

        case HelicopterStance::Left_Front_Incline_2:
          playerStack.push(HelicopterStance::Set_Delta_X_Zero);
          playerStack.push(HelicopterStance::Front_Level, HelicopterStance::Left_Front_Incline_1, HelicopterStance::Left_Front_Incline_1);
          playerStack.push(HelicopterStance::Set_Delta_X_Increase);
          playerStack.push(HelicopterStance::Left_Front_Incline_1);
          heli.xInc = DELTA_X_DECREASE;          
          break;

        case HelicopterStance::Right_Reverse_Incline_5:
          playerStack.push(HelicopterStance::Set_Delta_X_Zero);
          playerStack.push(HelicopterStance::Right_Level);
          playerStack.push(HelicopterStance::Right_Reverse_Incline_2, HelicopterStance::Right_Reverse_Incline_1);
          playerStack.push(HelicopterStance::Right_Reverse_Incline_4, HelicopterStance::Right_Reverse_Incline_3);
          heli.xInc = DELTA_X_DECREASE;          
          break;

        case HelicopterStance::Right_Level:
          playerStack.push(HelicopterStance::Set_Delta_X_Zero);
          playerStack.push(HelicopterStance::Right_Level, HelicopterStance::Right_Reverse_Incline_1);
          playerStack.push(HelicopterStance::Right_Reverse_Incline_2, HelicopterStance::Right_Reverse_Incline_3);
          playerStack.push(HelicopterStance::Set_Delta_X_Increase);
          playerStack.push(HelicopterStance::Right_Reverse_Incline_3, HelicopterStance::Right_Reverse_Incline_2, HelicopterStance::Right_Reverse_Incline_1);
          heli.xInc = DELTA_X_DECREASE;          
          break;

        default: break;

      }

    }

  } 

  else if (arduboy.pressed(RIGHT_BUTTON)) {

    if (heli.yPos < HELICOPTER_MINIMUM_HEIGHT || arduboy.pressed(UP_BUTTON)) {

      switch (heli.stance) {

        case HelicopterStance::Right_Level:
          playerStack.push(HelicopterStance::Right_Incline_Max, HelicopterStance::Right_Incline_4, HelicopterStance::Right_Incline_3);
          playerStack.push(HelicopterStance::Right_Incline_2, HelicopterStance::Right_Incline_1);
          heli.xInc = DELTA_X_INCREASE; 
          break;

        case HelicopterStance::Front_Level:
          playerStack.push(HelicopterStance::Right_Front_Incline_2, HelicopterStance::Right_Front_Incline_1);
          heli.xInc = DELTA_X_INCREASE; 
          break;

        case HelicopterStance::Right_Reverse_Incline_5:
          playerStack.push(HelicopterStance::Right_Incline_Max, HelicopterStance::Right_Incline_4, HelicopterStance::Right_Incline_3);
          playerStack.push(HelicopterStance::Right_Incline_2, HelicopterStance::Right_Incline_1, HelicopterStance::Right_Level);
          playerStack.push(HelicopterStance::Right_Reverse_Incline_1, HelicopterStance::Right_Reverse_Incline_2);
          playerStack.push(HelicopterStance::Right_Reverse_Incline_3, HelicopterStance::Right_Reverse_Incline_4);
          heli.xInc = DELTA_X_INCREASE; 
          break;

        case HelicopterStance::Left_Level:
          playerStack.push(HelicopterStance::Left_Reverse_Incline_5, HelicopterStance::Left_Reverse_Incline_4, HelicopterStance::Left_Reverse_Incline_3);
          playerStack.push(HelicopterStance::Left_Reverse_Incline_2, HelicopterStance::Left_Reverse_Incline_1);
          heli.xInc = DELTA_X_INCREASE;       
          break;

        case HelicopterStance::Left_Incline_Max:
          playerStack.push(HelicopterStance::Left_Reverse_Incline_5, HelicopterStance::Left_Reverse_Incline_4, HelicopterStance::Left_Reverse_Incline_3);
          playerStack.push(HelicopterStance::Left_Reverse_Incline_2, HelicopterStance::Left_Reverse_Incline_1);
          playerStack.push(HelicopterStance::Left_Level, HelicopterStance::Left_Incline_1, HelicopterStance::Left_Incline_2);
          playerStack.push(HelicopterStance::Left_Incline_3, HelicopterStance::Left_Incline_4);
          heli.xInc = DELTA_X_INCREASE;       
          break;

        case HelicopterStance::Left_Front_Incline_2:
          playerStack.push(HelicopterStance::Right_Front_Incline_2, HelicopterStance::Right_Front_Incline_1);
          playerStack.push(HelicopterStance::Front_Level, HelicopterStance::Left_Front_Incline_1);
          heli.xInc = DELTA_X_DECREASE;       
          break;

        default:
          heli.xInc = DELTA_X_INCREASE; 
          break;

      }

    }
    else {    // We are on the ground.

      switch (heli.stance) {

        case HelicopterStance::Right_Level:
          playerStack.push(HelicopterStance::Set_Delta_X_Zero);
          playerStack.push(HelicopterStance::Right_Level, HelicopterStance::Right_Incline_1, HelicopterStance::Right_Incline_2);
          playerStack.push(HelicopterStance::Right_Incline_2, HelicopterStance::Set_Delta_X_Decrease);
          playerStack.push(HelicopterStance::Right_Incline_2, HelicopterStance::Right_Incline_2, HelicopterStance::Right_Incline_1);
          heli.xInc = DELTA_X_INCREASE;          
          break;

        case HelicopterStance::Right_Incline_Max:
          playerStack.push(HelicopterStance::Set_Delta_X_Zero);
          playerStack.push(HelicopterStance::Right_Level, HelicopterStance::Right_Incline_1);
          playerStack.push(HelicopterStance::Right_Incline_2, HelicopterStance::Right_Incline_3, HelicopterStance::Right_Incline_4);
          heli.xInc = DELTA_X_INCREASE;          
          break;

        case HelicopterStance::Front_Level:
          playerStack.push(HelicopterStance::Set_Delta_X_Zero);
          playerStack.push(HelicopterStance::Front_Level, HelicopterStance::Right_Front_Incline_1, HelicopterStance::Right_Front_Incline_1);
          playerStack.push(HelicopterStance::Set_Delta_X_Decrease);
          playerStack.push(HelicopterStance::Right_Front_Incline_2, HelicopterStance::Right_Front_Incline_1, HelicopterStance::Right_Front_Incline_1);
          heli.xInc = DELTA_X_INCREASE;          
          break;

        case HelicopterStance::Right_Front_Incline_2:
          playerStack.push(HelicopterStance::Set_Delta_X_Zero);
          playerStack.push(HelicopterStance::Front_Level, HelicopterStance::Right_Front_Incline_1);
          playerStack.push(HelicopterStance::Set_Delta_X_Decrease);
          playerStack.push(HelicopterStance::Right_Front_Incline_1, HelicopterStance::Right_Front_Incline_1);
          heli.xInc = DELTA_X_INCREASE;          
          break;

        case HelicopterStance::Left_Reverse_Incline_5:
          playerStack.push(HelicopterStance::Set_Delta_X_Zero);
          playerStack.push(HelicopterStance::Left_Level);
          playerStack.push(HelicopterStance::Left_Reverse_Incline_2, HelicopterStance::Left_Reverse_Incline_1);
          playerStack.push(HelicopterStance::Left_Reverse_Incline_4, HelicopterStance::Left_Reverse_Incline_3);
          heli.xInc = DELTA_X_INCREASE;          
          break;

        case HelicopterStance::Left_Level:
          playerStack.push(HelicopterStance::Set_Delta_X_Zero);
          playerStack.push(HelicopterStance::Left_Level, HelicopterStance::Left_Reverse_Incline_1);
          playerStack.push(HelicopterStance::Left_Reverse_Incline_2, HelicopterStance::Left_Reverse_Incline_3);
          playerStack.push(HelicopterStance::Set_Delta_X_Decrease);
          playerStack.push(HelicopterStance::Left_Reverse_Incline_3, HelicopterStance::Left_Reverse_Incline_2, HelicopterStance::Left_Reverse_Incline_1);
          heli.xInc = DELTA_X_INCREASE;          
          break;

        default: break;

      }

    }

  } 
  else {  // Return from current position to upright ..

    switch (heli.stance) {

      case HelicopterStance::Left_Front_Incline_2:
        playerStack.push(HelicopterStance::Set_Delta_X_Zero);
        playerStack.push(HelicopterStance::Front_Level, HelicopterStance::Left_Front_Incline_1);
        if (heli.xDelta != 0) heli.xInc = (heli.xDelta > 0 ? DELTA_X_DECREASE : DELTA_X_INCREASE);
        break;

      case HelicopterStance::Right_Front_Incline_2:
        playerStack.push(HelicopterStance::Set_Delta_X_Zero);
        playerStack.push(HelicopterStance::Front_Level, HelicopterStance::Right_Front_Incline_1);
        if (heli.xDelta != 0) heli.xInc = (heli.xDelta > 0 ? DELTA_X_DECREASE : DELTA_X_INCREASE);
        break;

      case HelicopterStance::Left_Incline_Max:
        playerStack.push(HelicopterStance::Set_Delta_X_Zero);
        playerStack.push(HelicopterStance::Left_Level, HelicopterStance::Left_Incline_1, HelicopterStance::Left_Incline_2);
        playerStack.push(HelicopterStance::Left_Incline_3, HelicopterStance::Left_Incline_4);
        if (heli.xDelta != 0) heli.xInc = (heli.xDelta > 0 ? DELTA_X_DECREASE : DELTA_X_INCREASE);
        break;

      case HelicopterStance::Right_Incline_Max:
        playerStack.push(HelicopterStance::Set_Delta_X_Zero);
        playerStack.push(HelicopterStance::Right_Level, HelicopterStance::Right_Incline_1, HelicopterStance::Right_Incline_2);
        playerStack.push(HelicopterStance::Right_Incline_3, HelicopterStance::Right_Incline_4);
        if (heli.xDelta != 0) heli.xInc = (heli.xDelta > 0 ? DELTA_X_DECREASE : DELTA_X_INCREASE);
        break;
      
      case HelicopterStance::Left_Reverse_Incline_5:
        playerStack.push(HelicopterStance::Set_Delta_X_Zero);
        playerStack.push(HelicopterStance::Left_Level, HelicopterStance::Left_Reverse_Incline_1, HelicopterStance::Left_Reverse_Incline_2);
        playerStack.push(HelicopterStance::Left_Reverse_Incline_3, HelicopterStance::Left_Reverse_Incline_4);
        if (heli.xDelta != 0) heli.xInc = (heli.xDelta > 0 ? DELTA_X_DECREASE : DELTA_X_INCREASE);
        break;

      case HelicopterStance::Right_Reverse_Incline_5:
        playerStack.push(HelicopterStance::Set_Delta_X_Zero);
        playerStack.push(HelicopterStance::Right_Level, HelicopterStance::Right_Reverse_Incline_1, HelicopterStance::Right_Reverse_Incline_2);
        playerStack.push(HelicopterStance::Right_Reverse_Incline_3, HelicopterStance::Right_Reverse_Incline_4);
        if (heli.xDelta != 0) heli.xInc = (heli.xDelta > 0 ? DELTA_X_DECREASE : DELTA_X_INCREASE); //inc
        break;

      default: break;
      
    }

  }

}