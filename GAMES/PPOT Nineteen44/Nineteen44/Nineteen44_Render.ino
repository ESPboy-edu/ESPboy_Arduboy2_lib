#include "src/Utils/Arduboy2Ext.h"

/* -----------------------------------------------------------------------------------------------------------------------------
 *  Move and render the player's active bullets ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void moveAndRenderPlayerBullets() {
  
  for (uint8_t i = 0; i < PLAYER_BULLETS_MAX; ++i) {
   
    playerBullets[i].move();
    
    if (playerBullets[i].getEnabled()) {
      Sprites::drawPlusMask(playerBullets[i].getX() - 1, playerBullets[i].getY() - 1, bullet_img, static_cast<uint8_t>(BulletImage::Single));
    }
    
  }
  
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Move and render the enemies' active bullets ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void moveAndRenderEnemyBullets() {
  
  for (uint8_t i = 0; i < enemyBulletsMax[level]; ++i) {
    
    enemyBullets[i].move();
    
    if (enemyBullets[i].getEnabled()) {

      uint8_t x = enemyBullets[i].getX() - 1;
      uint8_t y = enemyBullets[i].getY() - 1;

      Sprites::drawPlusMask(x, y, bullet_img, static_cast<uint8_t>(enemyBullets[i].getBulletImage()));

    }
    
  }
  
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Move and render the obstacle if it is active ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void moveAndRenderObstacle() {
  
  if (obstacle.getEnabled()) {

    obstacle.move();
    obstacle.renderImage();
    
  }

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Move and render the active enemies ..  Boats are handled differently as clouds should always be rendered above them.
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void moveAndRenderEnemies(bool renderBoats) {
  
  for (uint8_t i = 0; i < NUMBER_OF_ENEMIES; ++i) {
 
    if (!renderBoats && enemies[i].getEnemyType() == EnemyType::Boat) { continue; }
    if (renderBoats && enemies[i].getEnemyType()  != EnemyType::Boat) { continue; }
    
    enemies[i].move();
    Rect enemyRect = enemies[i].getRect();
    
    if (enemies[i].getEnabled() && enemies[i].getDelayStart() == 0
                                && enemyRect.x + enemyRect.width > 0 && enemyRect.x < WIDTH 
                                && enemyRect.y + enemyRect.height >= 0 && enemyRect.y < HEIGHT ) {

      if (enemies[i].getEnemyType() == EnemyType::Boat) {

        enemies[i].setTurretDirection(aimAtPlayer(i));

      }

      enemies[i].renderImage();

    }

  }

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Move player off the screen ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void renderEndOfMission() {

  for (uint8_t i = 0; i < 128; ++i) {

    renderScenery(i % 2);
    moveAndRenderObstacle();
    renderScenery_BelowPlanes();
    moveAndRenderEnemies(false);

    #ifdef PLAYER_US
    Sprites::drawExternalMask(player.getX().getInteger() + i, player.getY().getInteger(), p38_0, p38_mask_0, (i % 6 < 3), 0);
    #endif

    #ifdef PLAYER_JAPANESE
    Sprites::drawExternalMask(player.getX().getInteger() + i, player.getY().getInteger(), zero_0, zero_mask_0, (i % 6 < 3), 0);
    #endif

    renderScenery_AbovePlanes();

    renderScoreboard();
    arduboy.display(true);
    delay(20);

  }

}


/* ----------------------------------------------------------------------------
 *  Render score board gauge. 
 * ----------------------------------------------------------------------------
 */
uint16_t scoreFlash;

void renderScoreboadGauge(const uint8_t imageX, const uint8_t imageY, const uint8_t *image, const uint8_t scoreboardY, const uint8_t value) {

  Sprites::drawOverwrite(imageX, imageY, image, 0);

  if ((value <= 4 && scoreFlash >= (SCOREBOARD_FLASH_MAX / 2)) || value > 4) {
    for (uint8_t i = 0; i < (value); i += 2) {
      arduboy.drawLine(imageX, scoreboardY + i, WIDTH, scoreboardY + i);
    }
  }

}

/* -----------------------------------------------------------------------------------------------------------------------------
 *  Render the score board.
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void renderScoreboard() {


  // Increment the frame count

  ++scoreFlash;        if (scoreFlash > SCOREBOARD_FLASH_MAX) { scoreFlash = 0; }
  if (bulletCountdown > 0) bulletCountdown--;

  uint16_t player_score = player.getScore();


  // Clear the board space ..

  arduboy.fillRect(SCOREBOARD_OUTER_RECT_X, SCOREBOARD_OUTER_RECT_Y, SCOREBOARD_OUTER_RECT_WIDTH, SCOREBOARD_OUTER_RECT_HEIGHT, BLACK);
  arduboy.drawLine(SCOREBOARD_INNER_RECT_X, SCOREBOARD_INNER_RECT_Y, SCOREBOARD_INNER_RECT_X, HEIGHT, WHITE);
    

  // Score ..
  {
    uint8_t player_score_digits[4] = {};
    extractDigits(player_score_digits, player_score);
  
    for (uint8_t i = 0, y2 = 15; i < 4; ++i, y2 = y2 - 5) {
      Sprites::drawExternalMask(SCOREBOARD_KILLS_X, y2, digits, digit_mask, player_score_digits[i], 0);
    }
    
  }

  renderScoreboadGauge(SCOREBOARD_HEALTH_BAR_X, SCOREBOARD_HEALTH_BAR_Y, health_gauge, SCOREBOARD_HEALTH_BAR_TOP, (player.getHealth() < 0 ? 0 : player.getHealth().getInteger()));
  renderScoreboadGauge(SCOREBOARD_FUEL_BAR_X, SCOREBOARD_FUEL_BAR_Y, fuel_gauge, SCOREBOARD_FUEL_BAR_TOP, (player.getFuel() < 0 ? 0 : player.getFuel().getInteger()));


  // Render ammo gauge ..

  if (bulletCountdown > 0) {

    uint8_t bullets = player.getBullets();
    uint8_t x = player.getX().getInteger();
    uint8_t y = player.getY().getInteger();

    if (y < 7  && renderBulletsAbove)  { renderBulletsAbove = false;}
    if (y > 42 && !renderBulletsAbove) { renderBulletsAbove = true;}

    int8_t yOffset = (renderBulletsAbove ? -6 : 19);

    Sprites::drawExternalMask(x, y + yOffset, ammo_gauge, ammo_gauge_mask, 0, 0);
    
    if ((bullets <= 16 && scoreFlash >= (SCOREBOARD_FLASH_MAX / 2)) || bullets > 16) {

      for (int i = 0, xOffset = x; i < bullets; i = i + 10, xOffset = xOffset + 2) {

        Sprites::drawExternalMask(xOffset, y + yOffset + 2, ammo_gauge_item, ammo_gauge_item_mask, 0, 0);

      }

    }

  }

}


void renderScenery(const uint8_t frame) {


  // Draw scenery elements ..

  for (uint8_t x = 0; x < NUMBER_OF_SCENERY_ITEMS; x++) {

    switch (sceneryItems[x].element) {

      case SceneryElement::Wave1:
        if (arduboy.getFrameCount(5 + x) != 0) {
          Sprites::drawSelfMasked(sceneryItems[x].x, sceneryItems[x].y, wave_01, 0);
        }
        break;

      case SceneryElement::Wave2:
        if (arduboy.getFrameCount(5 + x) != 0) {
          Sprites::drawSelfMasked(sceneryItems[x].x, sceneryItems[x].y, wave_02, 0);
        }
        break;

      case SceneryElement::Boat:
        Sprites::drawSelfMasked(sceneryItems[x].x, sceneryItems[x].y, sail_boat_01, 0);
        break;

      case SceneryElement::Boat2:
        Sprites::drawSelfMasked(sceneryItems[x].x, sceneryItems[x].y, sail_boat_02, 0);
        break;

      case SceneryElement::Island1:
        Sprites::drawSelfMasked(sceneryItems[x].x, sceneryItems[x].y, island_01, 0);
        break;

      case SceneryElement::Island2:
        Sprites::drawSelfMasked(sceneryItems[x].x, sceneryItems[x].y, island_02, 0);
        break;

      #ifdef USE_ISLAND_3
        case SceneryElement::Island3:
          Sprites::drawSelfMasked(sceneryItems[x].x, sceneryItems[x].y, island_03, 0);
          break;
      #endif

      default: break;

    }

  }


  // Draw ground ..

  if (upperSceneryPosition.enabled) { Sprites::drawOverwrite(upperSceneryPosition.x, upperSceneryPosition.y, ground_upper, upperSceneryPosition.image); }
  if (lowerSceneryPosition.enabled) { Sprites::drawOverwrite(lowerSceneryPosition.x, lowerSceneryPosition.y, ground_lower, lowerSceneryPosition.image); }

  if (frame == 0) {


    // Update scenery element positions ..

    for (uint8_t x = 0; x < NUMBER_OF_SCENERY_ITEMS; x++) {

      sceneryItems[x].x--;

      #ifdef USE_ISLAND_3
        #define NUMBER_OF_ELEMENTS 9
      #else
        #define NUMBER_OF_ELEMENTS 8
      #endif
      #define NUMBER_OF_COMMON_ELEMENTS (static_cast<uint8_t>(SceneryElement::Cloud_BelowPlanes) + 1)

      if (sceneryItems[x].x < -90) {

        sceneryItems[x].x = 162;
        SceneryElement previousElement = (x > 0 ? sceneryItems[x - 1].element : sceneryItems[NUMBER_OF_SCENERY_ITEMS - 1].element);
        uint8_t element = 0;

        switch (previousElement) {

          case SceneryElement::Boat ... SceneryElement::Wave2:
            element = random(
              (sceneryRestrictions == 0 ? static_cast<uint8_t>(SceneryElement::Boat) : static_cast<uint8_t>(SceneryElement::Wave1)), 
              (sceneryRestrictions == 0 ? NUMBER_OF_ELEMENTS : NUMBER_OF_COMMON_ELEMENTS));
            break;

          default:
            element = random(0, NUMBER_OF_COMMON_ELEMENTS);
            break;

        }

        switch (static_cast<SceneryElement>(element)) {

          case SceneryElement::Boat ... SceneryElement::Wave2:
            sceneryItems[x].element = static_cast<SceneryElement>(element);
            sceneryItems[x].y = random((upperSceneryPosition.enabled ? upperSceneryPosition.y + 24 : 4), (lowerSceneryPosition.enabled ? lowerSceneryPosition.y - 24 : HEIGHT - 24));
            break;

          case SceneryElement::Cloud_AbovePlanes ... SceneryElement::Cloud_BelowPlanes:
            sceneryItems[x].element = static_cast<SceneryElement>(element);
            sceneryItems[x].y = random(-16, HEIGHT - 16);
            break;

          default: //case SceneryElement::Island1 ... SceneryElement::Island3:
            sceneryItems[x].element = static_cast<SceneryElement>(element);
            sceneryItems[x].y = random((upperSceneryPosition.enabled ? upperSceneryPosition.y + 24 : -6), (lowerSceneryPosition.enabled ? lowerSceneryPosition.y - 38 : HEIGHT - 16));

            if (!upperSceneryPosition.enabled)  { launchScenery(random(200, 250), 0, &upperSceneryPosition); }
            if (!lowerSceneryPosition.enabled)  { launchScenery(random(200, 250), 53, &lowerSceneryPosition); }

            break;

        }

      }

    }


    // Update ground positions ..

    if (upperSceneryPosition.enabled) {
      upperSceneryPosition.x--;
      if (upperSceneryPosition.x < -90) { 
        upperSceneryPosition.enabled = false;
      }
    }
    else {
      if (random(0, 40) == 0) { launchScenery(162, 0, &upperSceneryPosition); }
    }

    if (lowerSceneryPosition.enabled) {
      lowerSceneryPosition.x--;
      if (lowerSceneryPosition.x < -90) { 
        lowerSceneryPosition.enabled = false;
      }
    }
    else {
      if (random(0, 40) == 0) { launchScenery(162, 53, &lowerSceneryPosition); }
    }

  }

}

void launchScenery(int16_t xOffset, uint8_t yPos, SceneryGround *sceneryItem) {

  sceneryItem->enabled = true;
  sceneryItem->x = xOffset;
  sceneryItem->y = yPos;
  sceneryItem->image = random(0, 2);

}

void renderScenery_BelowPlanes() {


  // Draw scenery elements ..
  
  for (uint8_t x = 0; x < NUMBER_OF_SCENERY_ITEMS; x++) {

    switch (sceneryItems[x].element) {

      case SceneryElement::Cloud_BelowPlanes:
        Sprites::drawExternalMask(sceneryItems[x].x, sceneryItems[x].y, cloud, cloud_Mask, 0, 0);
        break;

      default: break;

    }

  }

}


void renderScenery_AbovePlanes() {


  // Draw scenery elements ..
  
  for (uint8_t x = 0; x < NUMBER_OF_SCENERY_ITEMS; x++) {

    switch (sceneryItems[x].element) {

      case SceneryElement::Cloud_AbovePlanes:
        Sprites::drawExternalMask(sceneryItems[x].x, sceneryItems[x].y, cloud, cloud_Mask, 0, 0);
        break;

      default: break;

    }

  }

}