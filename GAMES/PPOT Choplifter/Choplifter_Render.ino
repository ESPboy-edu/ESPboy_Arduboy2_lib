#include "src/utils/Arduboy2Ext.h"
#include "Enums.h"
#include "Images.h"

const uint8_t * const renderSortieLookup[] PROGMEM = { sortie_01, sortie_02, sortie_03 };

const uint8_t * const renderTankLookup[] PROGMEM = { tank_turrent_00, tank_turrent_01, tank_turrent_02, tank_turrent_01, tank_turrent_00 };

/* ----------------------------------------------------------------------------
 *  Render the screen ..
 * 
 *  If a sortie number greater than zero is supplied, the sortie number is 
 *  rendered otherwise the notmal screen os drawn.
 */
void render(uint8_t sortie) {


  // Render background ..

  for (int16_t i = backgroundXOffset; i < backgroundXOffset + 150; i = i + (int16_t)64) {
    arduboy.drawCompressedMirror(i, 43, background, WHITE, false);
  }

  drawHorizontalDottedLine(backgroundXOffset % 2, WIDTH, 51);


  // Draw base ..

  if (heli.xPos > -158 && heli.xPos < 105) {

    arduboy.fillRect(heli.xPos + 90, 43, 42, 9, BLACK);
    arduboy.drawCompressedMirror(heli.xPos + 26, 32, base, WHITE, false);

  }


  // Draw dormitories ..
  
  if (sortie > 0) {
  
    arduboy.drawCompressedMirror(38, 22, imageArrayLookup(&renderSortieLookup[sortie - 1]), WHITE, false);

  }
  else {

    for (uint8_t i = 0; i < NUMBER_OF_DORMITORIES; i++) {

      Dormitory *dormitory = &dormitories[i];

      if ((dormitory->xPos > heli.xPos - 144) && (dormitory->xPos < heli.xPos + 144)) {

        const uint8_t * mask = (dormitory->state == DormitoryState::Intact) ? dormitory_01_mask : dormitory_02_mask;
        const uint8_t * image =(dormitory->state == DormitoryState::Intact) ? dormitory_01 : dormitory_02;
      
        arduboy.drawCompressedMirror(heli.xPos - dormitory->xPos + 64 - 15, 40, mask, BLACK, false);
        arduboy.drawCompressedMirror(heli.xPos - dormitory->xPos + 64 - 15, 40, image, WHITE, false);
          

      }

    }

    // Draw hostages ..

    for (uint8_t i = 0; i < NUMBER_OF_HOSTAGES; i++) {
      
      Hostage *hostage = &hostages[i];

      if ((hostage->xPos > heli.xPos - 133) && (hostage->xPos < heli.xPos + 133)) {

        const uint8_t * image = nullptr;
        bool flag = false;

        switch (hostage->stance) {

          case HostageStance::Running_Left_1 ... HostageStance::Running_Left_4:
            image = hostage_images[(uint8_t)hostage->stance - 1];
            break;

          case HostageStance::Running_Right_1 ... HostageStance::Running_Right_4:
            image = hostage_images[(uint8_t)hostage->stance - 5];
            flag = true;
            break;

          case HostageStance::Waving_11 ... HostageStance::Waving_12:
            image = hostage_05;
            break;

          case HostageStance::Waving_21 ... HostageStance::Waving_22:
            image = hostage_06;
            break;

          case HostageStance::Dying_1:
          case HostageStance::Dying_2:
            image = hostage_07;
            hostage->stance = (HostageStance)((uint8_t)hostage->stance - 1);
            break;

          default: break;

        }

        if(image != nullptr)
          arduboy.drawCompressedMirror(heli.xPos - hostage->xPos + 64 - 3, 48, image, WHITE, flag);

      }
    }


    // Draw helicopter ..

    if (heli.countDown < HELICOPTER_COUNT_DOWN_MID || heli.countDown >= HELICOPTER_END_OF_GAME_START) {

      drawHelicopter(45, heli.yPos, heli.stance);

    }
    else {

      heli.countDown++;
      if (heli.countDown == HELICOPTER_COUNT_DOWN_END) {

        sortieNumber++;

        if (sortieNumber <= NUMBER_OF_SORTIES && dead + safe < NUMBER_OF_HOSTAGES) {
          gameState = GameState::Sortie;
        }
        else {
          gameState = GameState::GameOver;
        }

      }

    }


    // Draw tank ..

    for (int i = 0; i < NUMBER_OF_TANKS; i++) {

      Tank *tank = &tanks[i];
  
      if (tank->state > TankState::Dead1 && (tank->xPos > heli.xPos - 144) && (tank->xPos < heli.xPos + 144)) {

        arduboy.drawCompressedMirror(heli.xPos - tank->xPos + 64 - 15, 47, tank_00_mask, BLACK, false);
        arduboy.drawCompressedMirror(heli.xPos - tank->xPos + 64 - 15, 47, (tank->track ? tank_00 : tank_01), WHITE, false);

        const uint8_t * image = imageArrayLookup(&renderTankLookup[(uint8_t)tank->turrentDirection]);
        bool flag = ((uint8_t)tank->turrentDirection < (uint8_t)TurrentDirection::Upright);

        arduboy.drawCompressedMirror(heli.xPos - tank->xPos + 64 - 15, 47, image, WHITE, flag);

        if (tank->state == TankState::Move_Left || tank->state == TankState::Move_Right) tank->track = !tank->track;

        if (tank->state <= TankState::Dead3) { tank->state = (TankState)((uint8_t)tank->state - 1); }

      }

    }



    // Draw player bullets ..

    for (int i = 0; i < NUMBER_OF_PLAYER_BULLETS; i++) {

      drawBullet(playerBullets[i]);

    }



    // Draw tank bullets ..

    for (int i = 0; i < NUMBER_OF_TANK_BULLETS; i++) {

      drawBullet(tankBullets[i]);

    }



    // Draw explosion ..

    if (playerBulletExplosion.xPos != BULLET_INACTIVE_X_VALUE) {

      drawExplosion(&playerBulletExplosion);

    }



    // Draw tank explosion ..


    if (tankBulletExplosion.xPos != BULLET_INACTIVE_X_VALUE) {

      drawExplosion(&tankBulletExplosion);
      if (heli.countDown > HELICOPTER_COUNT_DOWN_INACTIVE) heli.countDown++;

    }



    // Draw fences ..

    switch (heli.xPos) {

      // Right fence ..

      case 120 ... 159:
        arduboy.drawCompressedMirror(-24 + (heli.xPos - 150), 2, fence_00_mask, BLACK, true);
        arduboy.drawCompressedMirror(-24 + (heli.xPos - 150), 2, fence_00, WHITE, true);
        break;

      case 160 ... 169:
        arduboy.drawCompressedMirror(-16 + (heli.xPos - 160), 2, fence_01_mask, BLACK, true);
        arduboy.drawCompressedMirror(-16 + (heli.xPos - 160), 2, fence_01, WHITE, true);
        break;

      case 170 ... 179:
        arduboy.drawCompressedMirror(-8 + (heli.xPos - 170), 2, fence_02_mask, BLACK, true);
        arduboy.drawCompressedMirror(-8 + (heli.xPos - 170), 2, fence_02, WHITE, true);
        break;

      case 180 ... 189:
        arduboy.drawCompressedMirror(60 + (heli.xPos - 180), 2, fence_03_mask, BLACK, false);
        arduboy.drawCompressedMirror(60 + (heli.xPos - 180), 2, fence_03, WHITE, false);
        break;

      case 190 ... 199:
        arduboy.drawCompressedMirror(68 + (heli.xPos - 190), 2, fence_02_mask, BLACK, false);
        arduboy.drawCompressedMirror(68 + (heli.xPos - 190), 2, fence_02, WHITE, false);
        break;

      case 200 ... 209:
        arduboy.drawCompressedMirror(76 + (heli.xPos - 200), 2, fence_01_mask, BLACK, false);
        arduboy.drawCompressedMirror(76 + (heli.xPos - 200), 2, fence_01, WHITE, false);
        break;

      case 210 ... 249:
        arduboy.drawCompressedMirror(84 + (heli.xPos - 210), 2, fence_00_mask, BLACK, false);
        arduboy.drawCompressedMirror(84 + (heli.xPos - 210), 2, fence_00, WHITE, false);
        break;
      

      // Left fence ..

      case 2220 ... 2259:
        arduboy.drawCompressedMirror(-24 + (heli.xPos - 2250), 2, fence_00_mask, BLACK, true);
        arduboy.drawCompressedMirror(-24 + (heli.xPos - 2250), 2, fence_00, WHITE, true);
        break;

      case 2260 ... 2269:
        arduboy.drawCompressedMirror(-16 + (heli.xPos - 2260), 2, fence_01_mask, BLACK, true);
        arduboy.drawCompressedMirror(-16 + (heli.xPos - 2260), 2, fence_01, WHITE, true);
        break;

      case 2270 ... 2279:
        arduboy.drawCompressedMirror(-8 + (heli.xPos - 2270), 2, fence_02_mask, BLACK, true);
        arduboy.drawCompressedMirror(-8 + (heli.xPos - 2270), 2, fence_02, WHITE, true);
        break;

      case 2280 ... 2289:
        arduboy.drawCompressedMirror(60 + (heli.xPos - 2280), 2, fence_03_mask, BLACK, false);
        arduboy.drawCompressedMirror(60 + (heli.xPos - 2280), 2, fence_03, WHITE, false);
        break;

      case 2290 ... 2299:
        arduboy.drawCompressedMirror(68 + (heli.xPos - 2290), 2, fence_02_mask, BLACK, false);
        arduboy.drawCompressedMirror(68 + (heli.xPos - 2290), 2, fence_02, WHITE, false);
        break;

      case 2300 ... 2309:
        arduboy.drawCompressedMirror(76 + (heli.xPos - 2300), 2, fence_01_mask, BLACK, false);
        arduboy.drawCompressedMirror(76 + (heli.xPos - 2300), 2, fence_01, WHITE, false);
        break;

      case 2310 ... 2349:
        arduboy.drawCompressedMirror(84 + (heli.xPos - 2310), 2, fence_00_mask, BLACK, false);
        arduboy.drawCompressedMirror(84 + (heli.xPos - 2310), 2, fence_00, WHITE, false);
        break;

    }


    // Draw scoreboard ..

    drawScoreBoard(true);

  }


}

void drawBullet(const Bullet & bullet)
{
    if (bullet.xPos != BULLET_INACTIVE_X_VALUE) {
  
        if ((bullet.xPos > heli.xPos - 144) && (bullet.xPos < heli.xPos + 144)) {

          arduboy.fillRect(heli.xPos - bullet.xPos + 64 - 1, bullet.yPos, 2, 2, WHITE);

        }
    
    }
}

void drawExplosion(BulletExplosion *bulletExplosion) {

    switch (bulletExplosion->explosionType) {

      case ExplosionType::Small:
      case ExplosionType::Medium:

      case ExplosionType::Both_Small:
      case ExplosionType::Both_Medium:

        if ((bulletExplosion->xPos > heli.xPos - 144) && (bulletExplosion->xPos < heli.xPos + 144)) {

          arduboy.drawCompressedMirror(heli.xPos - bulletExplosion->xPos + 64 - 4, bulletExplosion->yPos, explosions[(uint8_t)bulletExplosion->explosionType], WHITE, false);
          bulletExplosion->explosionType = (ExplosionType)((uint8_t)bulletExplosion->explosionType - 1);
          if (bulletExplosion->explosionType == ExplosionType::None || bulletExplosion->explosionType == ExplosionType::Both_None) bulletExplosion->xPos = BULLET_INACTIVE_X_VALUE;

        }

        break;

      case ExplosionType::Large_1 ... ExplosionType::Large_4:

        if ((bulletExplosion->xPos > heli.xPos - 144) && (bulletExplosion->xPos < heli.xPos + 144)) {

          arduboy.drawCompressedMirror(heli.xPos - bulletExplosion->xPos + 64 - 12, bulletExplosion->yPos, explosions_masks[(uint8_t)bulletExplosion->explosionType], BLACK, false);
          arduboy.drawCompressedMirror(heli.xPos - bulletExplosion->xPos + 64 - 12, bulletExplosion->yPos, explosions[(uint8_t)bulletExplosion->explosionType], WHITE, false);

          bulletExplosion->explosionType = (ExplosionType)((uint8_t)bulletExplosion->explosionType + 1);
          if (bulletExplosion->explosionType == ExplosionType::Large_End) bulletExplosion->xPos = BULLET_INACTIVE_X_VALUE;

        }

        break;

      default: break;

    }
    
}


/* ----------------------------------------------------------------------------
 *  Draw the scoreboard in the top left hand corner ..
 */
void drawScoreBoard(bool all) {

  arduboy.drawCompressedMirror(-1, 0, digit_Cross, WHITE, false);
  arduboy.drawCompressedMirror(4, 0, imageArrayLookup(&digits[dead / 10]), WHITE, false);
  arduboy.drawCompressedMirror(9, 0, imageArrayLookup(&digits[dead % 10]), WHITE, false);

  if (all) {
    
    arduboy.drawCompressedMirror(14, 0, digit_Diamond, WHITE, false);
    arduboy.drawCompressedMirror(21, 0, imageArrayLookup(&digits[inHelicopter / 10]), WHITE, false);
    arduboy.drawCompressedMirror(26, 0, imageArrayLookup(&digits[inHelicopter % 10]), WHITE, false);
    
    arduboy.drawCompressedMirror(31, 0, digit_Heart, WHITE, false);
    arduboy.drawCompressedMirror(38, 0, imageArrayLookup(&digits[safe / 10]), WHITE, false);
    arduboy.drawCompressedMirror(43, 0, imageArrayLookup(&digits[safe % 10]), WHITE, false);

  }
  else {

    arduboy.drawCompressedMirror(14, 0, digit_Heart, WHITE, false);
    arduboy.drawCompressedMirror(21, 0, imageArrayLookup(&digits[safe / 10]), WHITE, false);
    arduboy.drawCompressedMirror(26, 0, imageArrayLookup(&digits[safe % 10]), WHITE, false);

  }

}
