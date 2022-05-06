#include "src/Utils/Arduboy2Ext.h"


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Introduction loop initialisation ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void introInit() {

  mission = 0;                            
  missionIdx = 0;
  introState = 0;
  restart = 0;
  gameState = GameState::Intro_Loop;
  sound.tones(score);
  player.initGame();

  obstacleLaunchDelayMin = OBSTACLE_LAUNCH_DELAY_MIN;
  obstacleLaunchDelayMax = OBSTACLE_LAUNCH_DELAY_MAX;
  obstacleBulletsValue = BULLETS_MAX;
  obstacleHealthValue = HEALTH_MAX;
  obstacleFuelValue = FUEL_MAX;
  
  frameRate = INIT_FRAME_RATE;
  arduboy.setFrameRate(frameRate);

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Credits loop ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void credits_loop() {

  Sprites::drawOverwrite(11, 10, credits_01, 0);
  for (uint8_t x = 0; x < 64; x = x + 13) {
    Sprites::drawOverwrite(x + 8, 22, credits_02, 0);
  }
  Sprites::drawOverwrite(0, 48, splash_lower, 0);

  arduboy.drawHorizontalDottedLine(4, 69, 3);
  arduboy.drawHorizontalDottedLine(4, 69, 32);

  Sprites::drawOverwrite(75, 3, filmote_01, 0);
  Sprites::drawOverwrite(91, 6, filmote_02, 0);
  Sprites::drawOverwrite(75, 17, pharap_01, 0);
  Sprites::drawOverwrite(91, 23, pharap_02, 0);
  Sprites::drawOverwrite(75, 35, vampirics_01, 0);
  Sprites::drawOverwrite(91, 40, vampirics_02, 0);
  Sprites::drawOverwrite(23, 38, splash_press_a, 0);

  if (intro < 210) {

    drawFlyingPair();
  
  }

  if (arduboy.justPressed(A_BUTTON)) {
    #ifndef HIGH_SCORES
    gameState = GameState::Intro_Init;
    #else
    arduboy.pollButtons();
    gameState = GameState::Save_Score;
    #endif
  }

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Introduction loop ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
enum class IntroState : uint8_t {
  PressA,
  Level1,
  Level2,
  Level3
};

enum class ArrowState : uint8_t {
  None,
  Lower,
  Upper
};

#define SPLASH_ARROW_MAX 7
#define SPLASH_PRESS_A_X 23
#define SPLASH_PRESS_A_Y 15
#define SPLASH_LEVEL_CAPTION_X SPLASH_PRESS_A_X + 2
#define SPLASH_LEVEL_CAPTION_Y SPLASH_PRESS_A_Y + 5
#define SPLASH_LEVEL_NUMBER_X SPLASH_LEVEL_CAPTION_X + 1
#define SPLASH_LEVEL_NUMBER_Y SPLASH_LEVEL_CAPTION_Y + 21
#define SPLASH_LEVEL_ARROW_UP_X SPLASH_LEVEL_CAPTION_X + 10
#define SPLASH_LEVEL_ARROW_UP_Y 28
#define SPLASH_LEVEL_ARROW_DOWN_X SPLASH_LEVEL_CAPTION_X - 6
#define SPLASH_LEVEL_ARROW_DOWN_Y 28

void introLoop() {

  Sprites::drawOverwrite(5, 19, title, 0);
  Sprites::drawOverwrite(71, 19, return_to_midway, 0);
  Sprites::drawOverwrite(0, -2, splash_upper, 0);
  Sprites::drawOverwrite(0, 50, splash_lower, 0);

  {
    IntroState state = static_cast<IntroState>((introState & 0b11100000) >> 5);
    ArrowState arrowState = static_cast<ArrowState>((introState & 0b00011000) >> 3);
    uint8_t arrowDelay = introState & 0b00000111;

    switch (state) {

      case IntroState::PressA:
        Sprites::drawOverwrite(83, 38, splash_press_a, 0);
        break;

      case IntroState::Level1 ... IntroState::Level3:
        {
          uint8_t leftX = 77;
          uint8_t rightX = 115;

          switch (arrowState) {

            case ArrowState::None:  
              break;

            case ArrowState::Lower:
              leftX = leftX - 2;
              break;

            case ArrowState::Upper:
              rightX = rightX + 2;
              break;

          }

          //Sprites::drawSelfMasked(94, downX, arrow_down, 0);
          //Sprites::drawSelfMasked(94, upX, arrow_up, 0);
          Sprites::drawSelfMasked(leftX, 40, arrow_left, 0);
          Sprites::drawSelfMasked(rightX, 40, arrow_right, 0);
        
          Sprites::drawOverwrite(85, 39, level_select, 0);
          Sprites::drawOverwrite(106, 40, font4x6_Full, 27 + level);

        }

        if (arrowDelay > 0) {
          arrowDelay--;
          introState = (introState & 0b11111000) | arrowDelay;
          if (arrowDelay == 0) {
            introState = introState & 0b11100000;
          }
        }

        break;

    }

  }

  //arduboy.display(true);

  {
   
    uint8_t pressed = arduboy.pressedButtons();
    uint8_t justPressed = arduboy.justPressedButtons();
    IntroState state = static_cast<IntroState>((introState & 0b11100000) >> 5);
 
    if (justPressed & RIGHT_BUTTON) {
      
      switch (state) {

        case IntroState::PressA:
          introState = ((static_cast<uint8_t>(IntroState::Level1) + level) << 5) | (static_cast<uint8_t>(ArrowState::None) << 3) | SPLASH_ARROW_MAX;
          break;

        case IntroState::Level1:
          introState = (static_cast<uint8_t>(IntroState::Level2) << 5) | (static_cast<uint8_t>(ArrowState::Upper) << 3) | SPLASH_ARROW_MAX;
          level++;
          eeprom_update_byte(EEPROM_LEVEL, level);
          break;

        case IntroState::Level2:
          introState = (static_cast<uint8_t>(IntroState::Level3) << 5) | (static_cast<uint8_t>(ArrowState::Upper) << 3) | SPLASH_ARROW_MAX;
          level++;
          eeprom_update_byte(EEPROM_LEVEL, level);
          break;

        default: break;

      }

    }

    if (justPressed & LEFT_BUTTON) {

      switch (state) {

        case IntroState::PressA:
          introState = ((static_cast<uint8_t>(IntroState::Level1) + level) << 5) | (static_cast<uint8_t>(ArrowState::None) << 3) | SPLASH_ARROW_MAX;
          break;

        case IntroState::Level2:
          introState = (static_cast<uint8_t>(IntroState::Level1) << 5) | (static_cast<uint8_t>(ArrowState::Lower) << 3) | SPLASH_ARROW_MAX;
          level--;
          eeprom_update_byte(EEPROM_LEVEL, level);
          break;

        case IntroState::Level3:
          introState = (static_cast<uint8_t>(IntroState::Level2) << 5) | (static_cast<uint8_t>(ArrowState::Lower) << 3) | SPLASH_ARROW_MAX;
          level--;
          eeprom_update_byte(EEPROM_LEVEL, level);
          break;

        default: break;

      }

    }

    if (justPressed & B_BUTTON) {
      if (state != IntroState::PressA) {
        introState = 0;
      }
      else {
        gameState = GameState::Credits_Init;   
      }
    }

    if (justPressed & A_BUTTON) { 
      
      gameState = GameState::Game_Init;
      mission = 0; 
      
    }  


    // Restart ?

    if (pressed & DOWN_BUTTON) {

      if (restart < UPLOAD_DELAY) {
        restart++;
      }
      else {
        arduboy.exitToBootloader();
      }

    }
    else {
      restart = 0;
    }

  }

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Initialize a new game ready for play.
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void gameInit() {

  for (uint8_t i = 0; i < NUMBER_OF_ENEMIES; ++i) {
    enemies[i].setEnabled(false);
  }  

  for (uint8_t i = 0; i < PLAYER_BULLETS_MAX; ++i) {
    playerBullets[i].setEnabled(false);
  }

  for (uint8_t i = 0; i < ENEMY_BULLETS_MAX; ++i) {
    enemyBullets[i].setEnabled(false);
  }
  
  {
    bool initConsumables = false;

    if (level < 2)                   { initConsumables = true; } 
    if (level == 2 && mission == 60) { initConsumables = true; }

    initSceneryItems();
    player.initMission(initConsumables);
    obstacle.setEnabled(false);

  }

  sound.tones(mission_start);
  intro = 80;
  gameState = GameState::Game_Loop;

  obstacleLaunchDelayMin = OBSTACLE_LAUNCH_DELAY_MIN;
  obstacleLaunchDelayMax = OBSTACLE_LAUNCH_DELAY_MAX;
  obstacleBulletsValue = BULLETS_MAX;
  obstacleHealthValue = HEALTH_MAX;
  obstacleFuelValue = FUEL_MAX;
  
  frameRate = INIT_FRAME_RATE;
  arduboy.setFrameRate(frameRate);

}
