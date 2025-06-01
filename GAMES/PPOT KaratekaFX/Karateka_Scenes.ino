#include "src/utils/Arduboy2Ext.h"
#include "src/utils/PlayerStances.h"
#include "src/utils/EnemyStances.h"
#include "src/images/images.h"
#include "Enums.h"

void drawPrincessBackground_1() {

  uint8_t x = 72;

  for (int y = 50; y < 64; y = y + 2) {
    // arduboy.drawFastHLine(x, y, WIDTH - x);   
    drawHorizontalDottedLine(x, WIDTH , y); 
    x = x + (y < 56 ? -20 : 20);
  }

  FX::drawBitmap(16, 6, Images::ArchInt_Right, 0, dbmMasked);
//   arduboy.drawCompressedMirror(89, 19, princess_seat_mask, BLACK, false);
//   arduboy.drawCompressedMirror(89, 19, princess_seat, WHITE, false);
//   FX::drawBitmap(89, 19, Images::Princess_Seat, 0, dbmMasked);
  FX::drawBitmap(87, 17, Images::Princess_Seat, 0, dbmMasked);

}

void drawPrincessBackground_2() {
  
  FX::drawBitmap(0, 3, Images::ArchInt_Left, 0, dbmMasked);

}




uint8_t dungeonIdx = 0;



void drawEmperorGround() {

   for (int i=54; i< 64; i+=2) {

      drawHorizontalDottedLine(i % 2, WIDTH, i);

    }

}



void showSceneDungeon1() {

    drawEmperorGround();
    FX::drawBitmap(99, 6, Images::ArchInt_Right, 1, dbmMasked);

    // int8_t x = dungeoon_1_x[dungeonIdx];
    // uint8_t i = dungeoon_1_i[dungeonIdx];
    int8_t x = FX::readIndexedUInt8(Constants::Dungeon_1_x, dungeonIdx);
    uint8_t i = FX::readIndexedUInt8(Constants::Dungeon_1_i, dungeonIdx);	
    uint24_t img = FX::readIndexedUInt24(Images::Princess, i);

	if (arduboy.frameCount % 8 == 0 ) {

		dungeonIdx++;

		if (dungeonIdx == 73) {
		gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);
		dungeonIdx = 0;
		}

	}

  	if (arduboy.justPressed(A_BUTTON)) {

		gameStateDetails.sequence++;
		gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);
		dungeonIdx = 0;

    }

    FX::drawBitmap(x, 11, img, 0, dbmMasked);

	if (dungeonIdx > 15) {
	    FX::drawBitmap(10, 3, Images::Emperor, 1, dbmMasked);
	}
	else {
	    FX::drawBitmap(10, 3, Images::Emperor, 3, dbmMasked);
	}

	if (dungeonIdx > 10 && dungeonIdx < 40) {
		FX::drawBitmap(37, 18, Images::Emperor_Arm_Out, 1, dbmMasked);
	}
	else {
		FX::drawBitmap(37, 20, Images::Emperor_Arm_Normal, 1, dbmMasked);
	}

    FX::drawBitmap(113, 3, Images::ArchInt_Left, 1, dbmMasked);

}



void showSceneDungeon2() {

	drawPrincessBackground_1();

    // int8_t x = dungeoon_2_x[dungeonIdx];
    // uint8_t i = dungeoon_2_i[dungeonIdx];
    int8_t x = FX::readIndexedUInt8(Constants::Dungeon_2_x, dungeonIdx);
    uint8_t i = FX::readIndexedUInt8(Constants::Dungeon_2_i, dungeonIdx);
    uint24_t img = FX::readIndexedUInt24(Images::Princess, i);

	if (arduboy.frameCount % 8 == 0 ) {

		dungeonIdx++;

		if (dungeonIdx == 55) {
		gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);
		}

	}

  	if (arduboy.justPressed(A_BUTTON)) {

        gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);
        dungeonIdx = 0;

    }

    FX::drawBitmap(x, 11, img, 0, dbmMasked);
	drawPrincessBackground_2();

}


void showScene() {

  if (arduboy.justPressed(A_BUTTON)) {
    
    if (gameStateDetails.getCurrState() == GAME_STATE_THE_END) {
      gameStateDetails.sequence = 0;
    }     

    enemyStack.clear();
    gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);

  }
  else {
    
    switch (gameStateDetails.getCurrState()) {

      case GAME_STATE_TITLE_SCENE:
        FX::drawBitmap(0, 0 , Images::Title, 0, dbmNormal);
//  gameStateDetails.sequence = 18;
        break;

#ifdef JORDAN_MECHNER
      case GAME_STATE_JM_SCENE:
        FX::drawBitmap(0, 0, Images::JordanMechner, 0, dbmNormal);
        break;
#endif      

      case GAME_STATE_CASTLE_SCENE:
        FX::drawBitmap(0, 0, Images::Castle, 0, dbmNormal);
        break;
        
      case GAME_STATE_THE_END:
        outside = true;
        FX::drawBitmap(34, 26, Images::TheEnd, 0, dbmNormal);
        break;
        
      case GAME_STATE_PRINCESS:

        drawPrincessBackground_1();
        drawPrincessBackground_2();
        arduboy.drawCompressedMirror(93, 19, princess_sitting_mask, BLACK, false);
        arduboy.drawCompressedMirror(93, 19, princess_sitting, WHITE, false);
        
        break;

      case GAME_STATE_ENEMY_APPROACH:

        draw_background();
    
        //  Update player and enemy positions and stances ..
    
        if (arduboy.everyXFrames(ANIMATION_NUMBER_OF_FRAMES)) {
          
          if (!enemyStack.isEmpty()) {
            enemy.stance = enemyStack.pop();
          }
          else {
            player.xPosDelta = 0;
            gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);
          }
          
        }
    
        
        // Move scenery if needed ..
    
        if (player.xPosDelta != 0) {
    
          mainSceneX = mainSceneX + player.xPosDelta;
    
          if (mainSceneX < -MAIN_SCENE_IMG_WIDTH) { mainSceneX = 0; }
          if (mainSceneX > 0) { mainSceneX = mainSceneX - MAIN_SCENE_IMG_WIDTH; }
    
        }
    
        if (enemy.xPos < 128 && gameStateDetails.sequence != 33) renderEnemyStance(arduboy, enemy.xPos, enemy.yPos, enemy.stance);

      default:
        break;

    }
    
    if (arduboy.everyXFrames(ANIMATION_NUMBER_OF_FRAMES)) {

      if (gameStateDetails.delayInterval > 0) {

        gameStateDetails.delayInterval--;

        if (gameStateDetails.delayInterval == 0) { 

          // if (gameStateDetails.getCurrState() == GAME_STATE_THE_END) {  // Not relevant for 'The End' scene.
          //   gameStateSeq = 0;
          // }            
          gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE); 

        }

      }

    }
  
  }

}



void finalScene() {
  
  if (arduboy.justPressed(A_BUTTON)) {
    
    enemyStack.clear();
    gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);

  }
  else {
    

    // Draw background ..

    drawPrincessBackground_1();
    
    if (enemyStack.isEmpty() && playerStack.isEmpty()) {

      finalSceneMode++;

      switch (finalSceneMode) {

        case FINAL_SCENE_INIT:
     
          for (int i = 0; i < 3; i++) {
            
            playerStack.push(STANCE_RUNNING_LF_END, STANCE_RUNNING_2, STANCE_RUNNING_7);
            playerStack.push(STANCE_RUNNING_4);

            playerStack.push(STANCE_RUNNING_RF_END, STANCE_RUNNING_8, STANCE_RUNNING_5);
            playerStack.push(STANCE_RUNNING_4);

          }
          
          player.xPosDelta = 5;
          player.xPos = -48;
          
          enemyStack.push(STANCE_PRINCESS_STANDING);
          for (int i = 0; i < 12; i++) {
            enemyStack.push(STANCE_PRINCESS_SITTING);
          }
          enemy.xPos = 93;
          enemy.yPos = 14;
          enemy.stance = STANCE_PRINCESS_SITTING;

          break;

        case FINAL_SCENE_PAUSE:
          playerStack.push(STANCE_STANDING_UPRIGHT);
          playerStack.push(STANCE_RUNNING_STRAIGHTEN_UP);
          break;

        case FINAL_SCENE_PAUSE_2:
          player.xPosDelta = 0;
          enemy.xPosDelta = 0;
          for (int i = 0; i < 4; i++) {
            playerStack.push(STANCE_STANDING_UPRIGHT);
          }
          break;

        case FINAL_SCENE_KISSING:
          enemy.xPos = 90;
          enemy.yPos = 15;
          enemyStack.push(STANCE_PRINCESS_KISSING, STANCE_PRINCESS_KISSING, STANCE_PRINCESS_KISSING);
          break;

        case FINAL_SCENE_BREAK_UP:
          enemy.xPos = 93;
          enemy.yPos = 15;
          for (int i = 0; i < 18; i++) {
            enemyStack.push(STANCE_PRINCESS_STANDING);
          }
          break;
                    
      }

    }


    // Render screen ..
    
    if (arduboy.everyXFrames(ANIMATION_NUMBER_OF_FRAMES)) {
      
      if (!enemyStack.isEmpty()) {
        enemy.stance = enemyStack.pop();
        enemy.xPos = enemy.xPos + enemy.xPosDelta;
      }
           
      if (!playerStack.isEmpty()) {
        player.stance = playerStack.pop();
        player.xPos = player.xPos + player.xPosDelta;
        }

    }

    renderPlayerStance(arduboy, player.xPos, player.yPos, player.stance);
    if (gameStateDetails.sequence != 33) renderEnemyStance(arduboy, enemy.xPos, enemy.yPos, enemy.stance);

    drawPrincessBackground_2();
    
    if (finalSceneMode == FINAL_SCENE_BREAK_UP && enemyStack.isEmpty()) {

      gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);

    }
    
  }

}