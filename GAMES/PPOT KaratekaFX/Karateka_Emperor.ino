#include "src/utils/Arduboy2Ext.h"
#include "src/utils/PlayerStances.h"
#include "src/utils/EnemyStances.h"
#include "src/images/images.h"
#include "Enums.h"

void emperor_loop() {

  if (arduboy.justPressed(A_BUTTON)) {
    
    enemyStack.clear();
    gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);

  }
  else {
    

    // Draw background ..

    arduboy.drawFastHLine(0, 52, WIDTH);

    drawEmperorGround();
    
    if (enemyStack.isEmpty()) {

      switch (emperorMode) {

        case EMPEROR_MODE_INIT:
        
          enemyStack.push(STANCE_RUNNING_STRAIGHTEN_UP_REV);
          
          for (int i = 0; i < 2; i++) {
            
            enemyStack.push(STANCE_RUNNING_RF_END_REV, STANCE_RUNNING_8_REV, STANCE_RUNNING_5_REV);
            enemyStack.push(STANCE_RUNNING_4_REV, STANCE_RUNNING_LF_END_REV, STANCE_RUNNING_2_REV);
            enemyStack.push(STANCE_RUNNING_7_REV, STANCE_RUNNING_4_REV);
            
          }
          
          enemy.xPosDelta = 5;
          enemy.xPos = -48;
          
          break;

        case EMPEROR_MODE_PAUSE_1:
          for (int i = 0; i < 5; i++) {
            enemyStack.push(STANCE_STANDING_UPRIGHT_REV);
          }
      
          break;

        case EMPEROR_LIFT_ARM:
        
          for (int i =0; i < 9; i++) {
            enemyStack.push(STANCE_STANDING_UPRIGHT_REV);
          }
          
          break;
          
        case EMPEROR_MODE_PAUSE_2:
        case EMPEROR_MODE_PAUSE_3:
        
          for (int i = 0; i < 3; i++) {
            enemyStack.push(STANCE_STANDING_UPRIGHT_REV);
          }
      
          break;
        
        case EMPEROR_MODE_BOW:
        
          enemyStack.push(STANCE_BOW_1, STANCE_BOW_2);
          enemyStack.push(STANCE_BOW_2, STANCE_BOW_1);
          
          break;
          
        case EMPEROR_MODE_FIGHTER_LEAVE:
        
          for (int i = 0; i < 2; i++) {

            enemyStack.push(STANCE_RUNNING_RF_END, STANCE_RUNNING_8, STANCE_RUNNING_5);
            enemyStack.push(STANCE_RUNNING_4, STANCE_RUNNING_LF_END, STANCE_RUNNING_2);
            enemyStack.push(STANCE_RUNNING_7, STANCE_RUNNING_4);
            
          }

          enemy.xPosDelta = -5;
          break;

      }

    }


    // Render screen ..
    
    if (arduboy.everyXFrames(ANIMATION_NUMBER_OF_FRAMES)) {
      
      enemy.stance = enemyStack.pop();
      enemy.xPos = enemy.xPos + enemy.xPosDelta;

      if (enemyStack.isEmpty()) {

        enemy.xPosDelta = 0;
        emperorMode++;

        if (emperorMode > EMPEROR_MODE_FIGHTER_LEAVE) { gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE); }

      }
      
    }


    // arduboy.drawCompressedMirror(92, 45, emperor_mask, BLACK, false);
    // arduboy.drawCompressedMirror(87, 4, emperor, WHITE, false);
    FX::drawBitmap(86, 3, Images::Emperor, 0, dbmMasked);

    if (emperorMode >= EMPEROR_LIFT_ARM && emperorMode < EMPEROR_MODE_FIGHTER_LEAVE) {

      FX::drawBitmap(70, 18, Images::Emperor_Arm_Out, 0, dbmMasked);
//      arduboy.drawCompressedMirror(71, 19, emperor_arm_out, WHITE, false);
      
    }
    else {

      FX::drawBitmap(86, 20, Images::Emperor_Arm_Normal, 0, dbmMasked);

      // arduboy.drawCompressedMirror(87, 21, emperor_arm_normal, WHITE, false);
      
    }

    FX::drawBitmap(16, 6, Images::ArchInt_Right, 0, dbmMasked);
    renderEnemyStance(arduboy, enemy.xPos, enemy.yPos, enemy.stance);
    FX::drawBitmap(0, 3, Images::ArchInt_Left, 0, dbmMasked);
    
  }

}
