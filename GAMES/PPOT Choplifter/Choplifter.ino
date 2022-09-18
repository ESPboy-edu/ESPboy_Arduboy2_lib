#include "src/utils/Arduboy2Ext.h"
#include <ArduboyTones.h>
#include "Enums.h"
#include "Images.h"
#include "src/utils/Stack.h"
#include "src/sounds/Sounds.h"

Arduboy2Ext arduboy;
boolean outEn() {return true;}
//ArduboyTones sound(outEn);
ArduboyTones sound(arduboy.audio.enabled);

Helicopter heli;

uint8_t frame = 0;
int8_t backgroundXOffset = 0;
uint8_t dead = 0;
uint8_t safe = 0;
uint8_t inHelicopter = 0;
uint8_t sortieNumber = 0;
uint8_t introduction_count = 0;
uint8_t level = LEVEL_EASY;

Stack <HelicopterStance, 10> playerStack;

Hostage hostages[NUMBER_OF_HOSTAGES];
Dormitory dormitories[NUMBER_OF_DORMITORIES];
Tank tanks[NUMBER_OF_TANKS];
Bullet playerBullets[NUMBER_OF_PLAYER_BULLETS];
Bullet tankBullets[NUMBER_OF_TANK_BULLETS];
BulletExplosion playerBulletExplosion;
BulletExplosion tankBulletExplosion;

GameState gameState = GameState::Initialise;


// --------------------------------------------------------------------------------------
//  Setup ..
//
void setup() {
  Serial.begin(115200);
  arduboy.boot();
  arduboy.flashlight();
  arduboy.systemButtons();
  arduboy.audio.begin();
  arduboy.setFrameRate(25);
  arduboy.initRandomSeed();

  resetGame();
  gameState = GameState::SplashScreen_Init;
  
}


// --------------------------------------------------------------------------------------
//  Main Loop ..
//
void loop() {

  if (!(arduboy.nextFrame())) return;
  frame++; if (frame == 4) frame = 0;
  arduboy.pollButtons();
  arduboy.clear();


  switch (gameState) {

    case GameState::SplashScreen_Init:
      splashScreen_Init();
      splashScreen();
      arduboy.display(false);
      break;

    case GameState::SplashScreen:
      splashScreen();
      arduboy.display(false);
      break;

    case GameState::Initialise:
      resetGame();
      sound.tones(rideOfTHeValkyries);
      break;

    case GameState::Introduction:
      introduction();
      break;

    case GameState::Sortie:
      resetSortie();
      render(sortieNumber);
      
      if (arduboy.justPressed(A_BUTTON)) { gameState = GameState::PlayGame; }
      break;

    case GameState::PlayGame:
      play();
      break;

    case GameState::GameOver:
      gameOver();
      break;

  }


  arduboy.display();

}




// --------------------------------------------------------------------------------------
//  Introduction ..
//
void introduction() {

  switch (introduction_count) {

    case 0:
      heli.xPos = 170;
      break;
    
    case 1 ... 64:
      arduboy.drawCompressedMirror(16, 23, choplifter, WHITE, false);
      break;

    case 65 ... 127:
      arduboy.drawCompressedMirror(16, 23, choplifter, WHITE, false);
      arduboy.drawCompressedMirror(36, 54, selection, WHITE, false);
      break;

    case 128:
      arduboy.drawCompressedMirror(0, 0, filmote, WHITE, false);
      arduboy.drawCompressedMirror(32, 54, selection2, WHITE, false);
      heli.xPos = 150;
      break;

    case 129 ... 191:
      arduboy.drawCompressedMirror(0, 0, filmote, WHITE, false);
      arduboy.drawCompressedMirror(32, 54, selection2, WHITE, false);
      break;

    case 192:
      arduboy.drawCompressedMirror(0, 0, keyboard_camper, WHITE, false);
      arduboy.drawCompressedMirror(36, 54, selection, WHITE, false);
      heli.xPos = 150;
      break;

    case 193 ... 254:
      arduboy.drawCompressedMirror(0, 0, keyboard_camper, WHITE, false);
      arduboy.drawCompressedMirror(36, 54, selection, WHITE, false);
      break;

    case 255:
      arduboy.drawCompressedMirror(16, 23, choplifter, WHITE, false);
      heli.xPos = 170;
      introduction_count = 0;
      break;

  }

  if (arduboy.pressed(LEFT_BUTTON)) {
    if (introduction_count % 4 <= 2){
      arduboy.drawCompressedMirror(3, 52, hostage_05, WHITE, false);
    }
    else {
      arduboy.drawCompressedMirror(3, 52, hostage_06, WHITE, false);
    }
  }

  introduction_count++;

  if (heli.xPos > -60) {
    arduboy.drawCompressedMirror(heli.xPos, 12, heli_06_intro_mask, BLACK, false);
    drawHelicopter(heli.xPos, 12, HelicopterStance::Left_Incline_Max);
    heli.xPos = heli.xPos - 3;
  }

  if (arduboy.justPressed(A_BUTTON))      { sound.tones(introduction_sound); level = LEVEL_EASY; gameState = GameState::Sortie; }
  if (arduboy.justPressed(B_BUTTON))      { sound.tones(introduction_sound); level = LEVEL_HARD; gameState = GameState::Sortie; }
  if (arduboy.justPressed(UP_BUTTON))     { turnSoundOn(); }
  if (arduboy.justPressed(DOWN_BUTTON))   { turnSoundOff(); }
}


// --------------------------------------------------------------------------------------
//  Game Over ..
//
void gameOver() {

  arduboy.drawCompressedMirror(45, 21, the_end, WHITE, false);
  drawScoreBoard(false);
  if (arduboy.justPressed(A_BUTTON)) { gameState = GameState::Initialise; }

}


// --------------------------------------------------------------------------------------
//  Play Choplifter !
//
void play() {
  // Handle player actions ..

  if (playerStack.isEmpty()) { playerMovements(); }

  if (arduboy.justPressed(A_BUTTON) && (absHelicopterStance(heli.stance) <= AbsHelicopterStance::Side_Front_Incline_2 || absHelicopterStance(heli.stance) >= AbsHelicopterStance::Side_Incline_Rotate_3)) {

    switch (absHelicopterStance(heli.stance)) {

      case AbsHelicopterStance::Side_Level ... AbsHelicopterStance::Side_Incline_2:
      case AbsHelicopterStance::Side_Reverse_Incline_1 ... AbsHelicopterStance::Side_Reverse_Incline_5:

        for (int i = 0; i < NUMBER_OF_PLAYER_BULLETS; i++) {
          
          Bullet *bullet = &playerBullets[i];

          if (bullet->xPos == BULLET_INACTIVE_X_VALUE) {

            sound.tones(player_firing);

            bullet->xPos = heli.xPos + ((int8_t)heli.stance < 0 ? -13 : 13);
            bullet->yPos = heli.yPos + 15;
            bullet->yDelta = BULLET_SHOOT_HORIZONTAL;
            bullet->xDelta = ((int8_t)heli.stance < 0 ? 8 : -8);
            bullet->startYPos = bullet->yPos;
            break;

          }

        }

        break;

      default:

        for (int i = 0; i < NUMBER_OF_PLAYER_BULLETS; i++) {
          
          Bullet *bullet = &playerBullets[i];

          if (bullet->xPos == BULLET_INACTIVE_X_VALUE) {

            sound.tones(player_firing);

            bullet->xPos = heli.xPos;
            bullet->yPos = heli.yPos + 19;
            bullet->yDelta = 1;
            bullet->xDelta = heli.xDelta * 2;
            bullet->startYPos = bullet->yPos;
            break;

          }

        }

        break;

    }

  }


  // Update the screen and object positions ..

  if (!playerStack.isEmpty()) {

    if (arduboy.everyXFrames(2)) {

      HelicopterStance oldStance = heli.stance;
      heli.stance = playerStack.pop();

      if (heli.stance == HelicopterStance::Set_Delta_X_Decrease) { heli.xInc = DELTA_X_DECREASE; heli.stance = playerStack.pop(); }
      if (heli.stance == HelicopterStance::Set_Delta_X_Increase) { heli.xInc = DELTA_X_INCREASE; heli.stance = playerStack.pop(); }
      if (heli.stance == HelicopterStance::Set_Delta_X_Zero)     { heli.xInc = 0; heli.xDelta = 0; heli.stance = oldStance; }

    }

  }

  if (arduboy.everyXFrames(4)) {
  
    if (heli.xInc == DELTA_X_INCREASE)     { heli.xDelta = calcSpeed(heli.xDelta, true); }
    if (heli.xInc == DELTA_X_DECREASE)     { heli.xDelta = calcSpeed(heli.xDelta, false); }
    if (heli.xDelta == 0)                  { heli.xInc = DELTA_X_DO_NOTHING; }
  
  }


  // Change the helicopter's position ..

  switch (heli.yDelta) {

    case -8 ... -1:                         // Going up!

      if (heli.yPos < (HELICOPTER_MAXIMUM_HEIGHT + 4) && heli.yDelta == -4) { heli.yDelta = -2; }
      if (heli.yPos < (HELICOPTER_MAXIMUM_HEIGHT + 2) && heli.yDelta == -2) { heli.yDelta = -1; }
      if (heli.yPos < (HELICOPTER_MAXIMUM_HEIGHT + 1) && heli.yDelta == -1) { heli.yDelta = 0; }

      heli.yPos = heli.yPos + heli.yDelta;
      break;

    case 0:
      break;

    case 1 ... 8:                           // We are falling ..

      if (heli.yPos < (HELICOPTER_MINIMUM_HEIGHT - heli.yDelta)) {

        heli.yPos = heli.yPos + heli.yDelta;

      }
      else {                                // Almost about to land or crash!

        switch (absHelicopterStance(heli.stance)) {

          case AbsHelicopterStance::Side_Level ... AbsHelicopterStance::Side_Incline_2:
          case AbsHelicopterStance::Side_Level_Rotate_1 ... AbsHelicopterStance::Side_Front_Level:
          case AbsHelicopterStance::Side_Reverse_Incline_1 ... AbsHelicopterStance::Side_Reverse_Incline_2:
          case AbsHelicopterStance::Side_Front_Incline_1:       
              
            if (heli.yPos > (HELICOPTER_MINIMUM_HEIGHT - 4) && heli.yDelta == 4)  { heli.yDelta = 2; }
            if (heli.yPos > (HELICOPTER_MINIMUM_HEIGHT - 2) && heli.yDelta == 2)  { heli.yDelta = 1; }
            if (heli.yPos > (HELICOPTER_MINIMUM_HEIGHT - 1) && heli.yDelta == 1)  { heli.yDelta = 0; }          

            heli.yPos = heli.yPos + heli.yDelta;
            tankBulletExplosion.yPos = tankBulletExplosion.yPos + heli.yDelta;
            break;

          default:

            if (heli.yDelta == 4 && absT(heli.xDelta) >= 2) {

              sound.tones(exploding);

              heli.yPos = (HELICOPTER_MINIMUM_HEIGHT - heli.yDelta);
              tankBulletExplosion.xPos = heli.xPos + 16;
              tankBulletExplosion.yPos = heli.yPos + 4;
              tankBulletExplosion.explosionType = ExplosionType::Large_1;
              heli.yDelta = 0;
              if (heli.countDown == HELICOPTER_COUNT_DOWN_INACTIVE) heli.countDown = HELICOPTER_COUNT_DOWN_START;


              // Kill any hostages that were in the helicopter ..

              dead = dead + inHelicopter;
              inHelicopter = 0;

              for (int i = 0; i < NUMBER_OF_HOSTAGES; i++) {

                Hostage *hostage = &hostages[i];
                if (hostage->stance == HostageStance::In_Helicopter) hostage->stance = HostageStance::Dead;

              }

            }
            else {
              
              if (heli.yPos > (HELICOPTER_MINIMUM_HEIGHT - 4) && heli.yDelta == 4)  { heli.yDelta = 2; }
              if (heli.yPos > (HELICOPTER_MINIMUM_HEIGHT - 2) && heli.yDelta == 2)  { heli.yDelta = 1; }
              if (heli.yPos > (HELICOPTER_MINIMUM_HEIGHT - 1) && heli.yDelta == 1)  { heli.yDelta = 0; }

              heli.yPos = heli.yPos + heli.yDelta;
              
            }

            break;

        }

      }

      break;

  }



  heli.yPos = heli.yPos + heli.yDelta;
  tankBulletExplosion.yPos = tankBulletExplosion.yPos + heli.yDelta;
  backgroundXOffset = backgroundXOffset - heli.xDelta;
  heli.xPos = heli.xPos - heli.xDelta;
  if (backgroundXOffset < -64) backgroundXOffset = backgroundXOffset + 64;
  if (backgroundXOffset > 0)   backgroundXOffset = backgroundXOffset - 64;



  // Did we just land on hostages?

  if (heli.yPos - heli.yDelta < 38 && heli.yPos > 38) {  // Check to see if we just landed.

    for (int i = 0; i < NUMBER_OF_HOSTAGES; i++) {

      Hostage *hostage = &hostages[i];
      int16_t diff = absT(hostage->xPos - heli.xPos);

      if (hostage->stance >= HostageStance::Running_Left_1 && hostage->stance <= HostageStance::Waving_22) {

        switch (absHelicopterStance(heli.stance)) {

          case AbsHelicopterStance::Side_Level ... AbsHelicopterStance::Side_Incline_Max:
          case AbsHelicopterStance::Side_Reverse_Incline_1 ... AbsHelicopterStance::Side_Reverse_Incline_5:
          case AbsHelicopterStance::Side_Incline_Rotate_3:
      
            if (diff < 7) {

              hostage->stance = HostageStance::Dying_2;
              dead++;

              if (dead + safe == NUMBER_OF_HOSTAGES) {

                heli.countDown = HELICOPTER_END_OF_GAME_START;

              }

            }

            break;

          case AbsHelicopterStance::Side_Level_Rotate_1 ... AbsHelicopterStance::Side_Front_Incline_2:
          case AbsHelicopterStance::Side_Incline_Rotate_1 ... AbsHelicopterStance::Side_Incline_Rotate_2:

            if (diff < 4) {

              hostages->stance = HostageStance::Dying_2;
              dead++;

              if (dead + safe == NUMBER_OF_HOSTAGES) {

                heli.countDown = HELICOPTER_END_OF_GAME_START;

              }

            }

            break;

        }

      }

    }

  }


  // Update hostages ..

  hostageMovements();


  // Update player bullets ..

  for (int i = 0; i < NUMBER_OF_PLAYER_BULLETS; i++) {

    Bullet *bullet = &playerBullets[i];

    if (bullet->xPos != BULLET_INACTIVE_X_VALUE) {


      // Update bullet position ..

      bullet->xPos = bullet->xPos - bullet->xDelta;
      if (absT(bullet->xPos - heli.xPos) > 70) { bullet->xPos = BULLET_INACTIVE_X_VALUE; }

      if (bullet->yDelta != BULLET_SHOOT_HORIZONTAL) {
        bullet->yPos = bullet->yPos + bullet->yDelta;
        bullet->yDelta = calcSpeed(bullet->yDelta, true);
      }



      // Check to see if we hit anything ..

      bulletHit(bullet, &playerBulletExplosion, true);

    }

  }




  // Update tank bullets ..

  for (int i = 0; i < NUMBER_OF_TANK_BULLETS; i++) {

    Bullet *bullet = &tankBullets[i];

    if (bullet->xPos != BULLET_INACTIVE_X_VALUE) {


      // Update bullet position ..

      bullet->xPos = bullet->xPos - bullet->xDelta;
      if (absT(bullet->xPos - heli.xPos) > 70) { bullet->xPos = BULLET_INACTIVE_X_VALUE; }

      bullet->yPos = bullet->yPos + bullet->yDelta;
      bullet->yDelta = calcSpeed(bullet->yDelta, true);


      // Check to see if we hit anything?

      bulletHit(bullet, &tankBulletExplosion, false);

    }

  }


  // Update tank movements ..

  tankMovements();


  // Render screen ..

  render(0);



  // End of game?

  if (dead + safe == NUMBER_OF_HOSTAGES && heli.countDown == HELICOPTER_COUNT_DOWN_INACTIVE) heli.countDown = HELICOPTER_END_OF_GAME_START;
  
  if (heli.countDown >= HELICOPTER_END_OF_GAME_START) {

    heli.countDown++;

    if (heli.countDown == HELICOPTER_END_OF_GAME_END) {

      gameState = GameState::GameOver;

    }

  }


  // heli blades ..

  #ifdef BLADE_SOUNDS
  if (heli.countDown == HELICOPTER_COUNT_DOWN_INACTIVE) {

    if      (absT(heli.xDelta) == 4 || absT(heli.yDelta) == 4 ) { sound.tones(blade_4); }
    else if (absT(heli.xDelta) == 2 || absT(heli.yDelta) == 2 ) { sound.tones(blade_3); }
    else if (absT(heli.xDelta) == 1 || absT(heli.yDelta) == 1 ) { sound.tones(blade_2); }
    else                                                          sound.tones(blade_1);

  }
  #endif

  // -- DEBUG -------------------------------------------------------------------------------

  #ifdef DEBUG

    arduboy.setCursor(1, 10);
    arduboy.print(image);

    arduboy.setCursor(20, 10);
    arduboy.print(deltaX);
    arduboy.setCursor(35, 10);
    arduboy.print(deltaY);
    arduboy.setCursor(50, 10);
    arduboy.print(backgroundXOffset);
    arduboy.setCursor(70, 10);
    arduboy.print(y);

  #endif

  // ----------------------------------------------------------------------------------------
  
 
}
