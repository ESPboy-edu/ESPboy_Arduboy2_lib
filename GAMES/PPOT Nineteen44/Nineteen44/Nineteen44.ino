#include "src/Utils/Arduboy2Ext.h"
#include <ArduboyTones.h>
//#include <avr/eeprom.h> 

#define eeprom_read_byte EEPROM.get

#include "src/Entities/Enemy.h"
#include "src/Entities/Player.h"
#include "src/Entities/Obstacle.h"
#include "src/Entities/Bullet.h"
#include "src/Entities/EnemyBullet.h"
#include "src/Utils/Sequences.h"
#include "src/Utils/Sounds.h"
#include "src/Utils/Enums.h"
#include "FixedPoints.h"
#include "FixedPointsCommon.h"

#include "src/Images/Images_Enemy.h"
#include "src/Images/Images_Boat.h"
#include "src/Images/Images_Explosions.h"
#include "src/Images/Images_Obstacles.h"
#include "src/Images/Images_Player.h"
#include "src/Images/Images_Scoreboard.h"
#include "src/Images/Images_Splash.h"
#include "src/Images/Images_Scenery.h"
#include "src/Images/Images_Arrays.h"
#include "src/Images/Images_Scenery.h"
#include "src/Utils/EEPROM_Utils.h"

Arduboy2Ext arduboy;


boolean outEn() {return true;}
ArduboyTones sound(outEn);  //Can not find the audio switch in the game
//ArduboyTones sound(arduboy.audio.enabled); 

//ARDUBOY_NO_USB

#include "src/Fonts/Fonts.h"
#ifdef HIGH_SCORES
#include "src/Utils/HighScoreEditor.h"

HighScore highScore;
uint8_t alternate = 0;
#endif

const uint8_t* const missions[] =        { mission_00, mission_01, mission_02, mission_03, mission_04 };
const int8_t* const formations[] =       { formation_00, formation_01, formation_02, formation_03, formation_04, formation_06, formation_05, 
                                           formation_06, formation_07, formation_08, formation_09, formation_10, formation_11, formation_12, 
                                           formation_13 };
const int8_t* const sequences[] =        { seq_00, seq_01, seq_02, seq_03 };

const int8_t obstacleLaunchDelayInc[] =  { OBSTACLE_LAUNCH_DELAY_INC_L0, OBSTACLE_LAUNCH_DELAY_INC_L1, OBSTACLE_LAUNCH_DELAY_INC_L2 };
const int8_t frameRateInc[] =            { FRAME_RATE_INC_L0, FRAME_RATE_INC_L1, FRAME_RATE_INC_L2 };

const SQ7x8 obstacleBulletsValueDec[] =  { BULLETS_DECREMENT_L0, BULLETS_DECREMENT_L1, BULLETS_DECREMENT_L2 };
const SQ7x8 obstacleFuelValueDec[] =     { FUEL_DECREMENT_L0, FUEL_DECREMENT_L1, FUEL_DECREMENT_L2 };
const SQ7x8 obstacleHealthValueDec[] =   { HEALTH_DECREMENT_L0, HEALTH_DECREMENT_L1, HEALTH_DECREMENT_L2 };

const SQ7x8 playerHitBulletDec[] =       { PLAYER_HIT_BULLET_DEC_NORM, PLAYER_HIT_BULLET_DEC_DOUBLE, PLAYER_HIT_BULLET_DEC_NORM, PLAYER_HIT_BULLET_DEC_NORM };  // Index positions defined by EnemyType

Player player = { player_images };

Enemy enemies[NUMBER_OF_ENEMIES] = {
  { EnemyType::Fighter1, enemy_fighter1_images },
  { EnemyType::Fighter1, enemy_fighter1_images },
  { EnemyType::Fighter1, enemy_fighter1_images },
  { EnemyType::Fighter1, enemy_fighter1_images },
  { EnemyType::Fighter1, enemy_fighter1_images },
  { EnemyType::Fighter1, enemy_fighter1_images },
};

Obstacle obstacle = { -20, 24, fuel, 0, 0 };

uint8_t playerBulletIdx = 0;
uint8_t enemyBulletIdx = 0;

Bullet playerBullets[PLAYER_BULLETS_MAX];
EnemyBullet enemyBullets[ENEMY_BULLETS_MAX];
uint8_t enemyBulletsMax[] = { ENEMY_BULLETS_MAX_L1, ENEMY_BULLETS_MAX_L2, ENEMY_BULLETS_MAX_L3 };

uint16_t obstacleLaunchCountdown = OBSTACLE_LAUNCH_DELAY_MIN;
uint8_t enemyShotCountdown = 5;
uint8_t level = 0;

uint8_t mission = 0;                                        // Mission currently being played
uint8_t missionIdx = 0;                                     // Byte index within current mission
uint8_t mission_formations = 0;                             // Number of formations in the current mission.
uint8_t mission_formations_left = 0;                        // Number of formations left within current mission.
uint8_t formation = 0;
uint8_t introState;
uint8_t intro;
uint16_t frameRate = INIT_FRAME_RATE;
uint8_t restart = 0;

uint16_t obstacleLaunchDelayMin = OBSTACLE_LAUNCH_DELAY_MIN;
uint16_t obstacleLaunchDelayMax = OBSTACLE_LAUNCH_DELAY_MAX;

SQ7x8 obstacleBulletsValue = BULLETS_MAX;
SQ7x8 obstacleHealthValue = HEALTH_MAX;
SQ7x8 obstacleFuelValue = FUEL_MAX;

uint8_t sceneryRestrictions = SCENERY_NONE; 
SceneryGround upperSceneryPosition;
SceneryGround lowerSceneryPosition;
SceneryItem sceneryItems[NUMBER_OF_SCENERY_ITEMS];

GameState gameState = GameState::Intro_Init;

void initSceneryItems() {
  sceneryItems[0] = { 128, 20, SceneryElement::Boat2 };
  sceneryItems[1] = { 182, 25, SceneryElement::Wave1 };
  sceneryItems[2] = { 236, 30, SceneryElement::Wave2 };
  sceneryItems[3] = { 290, 35, SceneryElement::Boat };
}

bool renderBulletsAbove = true;
uint8_t bulletCountdown = 0;
#ifdef USE_LEDS             
uint8_t ledCountdown = 0;
#endif

/* -----------------------------------------------------------------------------------------------------------------------------
 *  Setup
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void setup() {

  EEPROM_Utils::initEEPROM(false);

  arduboy.boot();
  arduboy.flashlight(); 
  arduboy.systemButtons();
  arduboy.audio.begin();
  
  obstacleLaunchDelayMin = OBSTACLE_LAUNCH_DELAY_MIN;
  obstacleLaunchDelayMax = OBSTACLE_LAUNCH_DELAY_MAX;
  obstacleBulletsValue = BULLETS_MAX;
  obstacleHealthValue = HEALTH_MAX;
  obstacleFuelValue = FUEL_MAX;

  frameRate = INIT_FRAME_RATE;
  eeprom_read_byte(EEPROM_LEVEL,level);
  
  arduboy.setFrameRate(frameRate);
  arduboy.initRandomSeed();

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Main loop
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void loop() {

  if (!(arduboy.nextFrame())) return;

  arduboy.clear();
  arduboy.pollButtons();

  switch (gameState) {

    case GameState::Intro_Init:
      introInit();
      //break;

    case GameState::Intro_Loop:
      introLoop();
      break;

    case GameState::Game_Init:
      gameInit();
      //break;

    case GameState::Game_Loop:
      gameLoop();
      break;

    case GameState::End_Of_Mission:
    case GameState::End_Of_Game:
      endOfSequence(level);
      break;
    
    case GameState::Credits_Init:
      intro = 0;
      gameState = GameState::Credits_Loop;
    
    case GameState::Credits_Loop:
      credits_loop();
      break;

    #ifdef HIGH_SCORES
    case GameState::Save_Score:
      highScore.reset();
      highScore.setSlotNumber(EEPROM_Utils::saveScore(player.getScore()));
      gameState = GameState::High_Score;
      // break; Fall-through intentional.

    case GameState::High_Score:
      renderHighScore(highScore);
      break;
    #endif

  }

  arduboy.display();


  // Distinguish any LEDs..

  #ifdef USE_LEDS             
  if (ledCountdown > 0) {
    ledCountdown--;
    if (ledCountdown == 0) {
      arduboy.setRGBled(0, 0, 0);
    }
  }
  #endif

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Let's play !
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void gameLoop() {

  renderScenery(arduboy.getFrameCount(2));
  
  switch (intro) {

    case 80: 
      upperSceneryPosition.enabled = false;
      lowerSceneryPosition.enabled = false;

    case 2 ... 79:
      {
        uint8_t x = 26;
        Sprites::drawOverwrite(x + 3, 30, mission_number, 0);

        uint8_t digits[3] = {};
        extractDigits(digits, mission + 1);

        for (uint8_t i = 0, xOffset = x + 63; i < 3; ++i, xOffset -= 6) {
          Sprites::drawSelfMasked(xOffset, 31, font4x6_Full, digits[i] + 26);
        }

        arduboy.drawHorizontalDottedLine(x, x + 68, 26);
        arduboy.drawHorizontalDottedLine(x, x + 68, 41);

      }

      intro--;
      break;

    case 1:
      launchMission(true, missions[mission % NUMBER_OF_MISSIONS]);
      intro--;
      break;

    case 0:
      break;
      
  }


  if (player.getFuel() > 0 && player.getHealth() > 0 && !player.inRoll()) {
    
    uint8_t pressed = arduboy.pressedButtons();
    uint8_t justPressed = arduboy.justPressedButtons();
    
    player.decFuel(FUEL_DECREMENT);

    
    // Handle player movement ..
    
    if ((pressed & UP_BUTTON) && player.getY() > PLAYER_MOVEMENT_INC_UP)                                     { player.setY(player.getY() - PLAYER_MOVEMENT_INC_UP); }
    if ((pressed & DOWN_BUTTON) && player.getY() < HEIGHT - PLAYER_HEIGHT)                                   { player.setY(player.getY() + PLAYER_MOVEMENT_INC_DOWN); }
    if ((pressed & LEFT_BUTTON) && player.getX() > PLAYER_MOVEMENT_INC_LEFT)                                 { player.setX(player.getX() - PLAYER_MOVEMENT_INC_LEFT); }
    if ((pressed & RIGHT_BUTTON) && player.getX() < WIDTH - PLAYER_WIDTH - SCOREBOARD_OUTER_RECT_WIDTH)      { player.setX(player.getX() + PLAYER_MOVEMENT_INC_RIGHT); }
  
    if (justPressed & B_BUTTON)                                                                              { player.startRoll(); }
                                                                                                                    
    if (justPressed & A_BUTTON) {

      if (player.getBullets() > 0) {
        
        if (!sound.playing()) sound.tones(machine_gun_player);

        uint8_t playerX = player.getX().getInteger();
        uint8_t playerY = player.getY().getInteger();

        launchPlayerBullet(playerX, playerY, Direction::East);
        
        if (player.getPowerUp() && player.getBullets() > 1) {
        
          player.decPowerUp();
          launchPlayerBullet(playerX, playerY, Direction::NorthEast);
          launchPlayerBullet(playerX, playerY, Direction::SouthEast);
          
        }

      }
  
    }

    if (!intro) {

      checkForObstacleCollision();
      checkForEnemiesShot();
  
      if (!player.inRoll()) {
  
        checkForPlayerShot();
        checkForPlayerEnemyCollision();
    
      }
  
  
      // Enemy takes a shot?
  
      --enemyShotCountdown;
      if (enemyShotCountdown == 0) {
  
        checkCanEnemyShoot();
  
      }
  
  
      // Should we launch another obstacle?
  
      --obstacleLaunchCountdown;
      if (obstacleLaunchCountdown == 0 && !obstacle.getEnabled()) {

        launchObstacle();
  
      }

    }

  }


  // Render the screen ..

  moveAndRenderPlayerBullets();
  moveAndRenderEnemyBullets();
  moveAndRenderEnemies(true);
  moveAndRenderObstacle();
  renderScenery_BelowPlanes();
  moveAndRenderEnemies(false);
  player.renderImage(arduboy.getFrameCountHalf(6));
  renderScenery_AbovePlanes();

  renderScoreboard();
  //arduboy.display(true);


  // New wave ?

  if (intro == 0 && player.getHealth() > 0) {
      
    bool newFormation = true;
    for (uint8_t i = 0; i < NUMBER_OF_ENEMIES; ++i) {

      if (enemies[i].getEnabled()) {
        newFormation = false;
        break;
      }

    }
  
    if (newFormation) {

      if (mission_formations_left > 0) {

        launchMission(false, missions[mission % NUMBER_OF_MISSIONS]);
  
      }
      else {

        ++mission;
        intro = 40;
        sound.tones(mission_success);
        gameState = GameState::End_Of_Mission;
        renderEndOfMission();
        
      }
  
    }

  }


  // End of life or mission?

  if ((player.getHealth() <= 0 || player.getFuel() <= 0) && !sound.playing()) {
    sound.tones(mission_failure);    
  }
  
  if (!player.getEnabled()) {
    gameState = GameState::End_Of_Game;
  }

}



/* -----------------------------------------------------------------------------------------------------------------------------
 *  Launch player bullet.
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void launchPlayerBullet(const uint8_t x, const uint8_t y, const Direction direction) {

  playerBullets[playerBulletIdx].setX(x + PLAYER_WIDTH);
  playerBullets[playerBulletIdx].setY(y + PLAYER_HEIGHT_HALF);
  playerBullets[playerBulletIdx].setDirection(direction);
  playerBullets[playerBulletIdx].setEnabled(true);
  player.decBullets();
  ++playerBulletIdx;
  if (playerBulletIdx == PLAYER_BULLETS_MAX) playerBulletIdx = 0;
  bulletCountdown = MAX_BULLET_COUNTDOWN;

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Launch a new obstacle.
 *  
 *  If the player is running out of fuel or health, launch one of those first.
 *  
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void launchObstacle() {

  ObstacleType type = (ObstacleType)random((uint8_t)ObstacleType::First, (uint8_t)ObstacleType::Count);;
  SQ7x8 minValue = 0;
  SQ7x8 maxValue = 0;

  const uint8_t *bitmap = nullptr;

  if (player.getFuel() <= 4) {
    type = ObstacleType::Fuel;
  }
  else if (player.getHealth() <= 4) {
    type = ObstacleType::Health;
  }
  
  switch (type) {

    case ObstacleType::Bullets:
      minValue = (obstacleBulletsValue / (SQ7x8)2);
      maxValue = obstacleBulletsValue;
      bitmap = bullets;
      break;

    case ObstacleType::Fuel:
      minValue = (obstacleFuelValue / (SQ7x8)2);
      maxValue = obstacleFuelValue;
      bitmap = fuel;
      break;

    case ObstacleType::Health:
      minValue = (obstacleHealthValue / (SQ7x8)2);
      maxValue = obstacleHealthValue;
      bitmap = health;
      break;

    case ObstacleType::PowerUp:
      minValue = (obstacleBulletsValue / (SQ7x8)2);
      maxValue = obstacleBulletsValue;
      bitmap = power_up;
      break;
      
    case ObstacleType::Count:
      break;
     
  }

  obstacle.setObstacleType(type);
  obstacle.setEnabled(true);
  obstacle.setX(WIDTH);
  obstacle.setY(random(0, 51));
  obstacle.setSpeed(randomSFixed<7,8>(1, 2));
  obstacle.setValue(randomSFixed<7,8>(minValue, maxValue));
  obstacle.setBitmap(bitmap);

  obstacleLaunchCountdown = random(obstacleLaunchDelayMin, obstacleLaunchDelayMax);

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Launch a new mission.
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void launchMission(bool firstFormation, const uint8_t *missionRef) {

  if (firstFormation) {
    missionIdx = 0;
    mission_formations_left = pgm_read_byte(&missionRef[missionIdx++]);
    mission_formations = mission_formations_left;
  }

  {
    uint8_t formationPlusScenery = pgm_read_byte(&missionRef[missionIdx++]);
    formation = formationPlusScenery & SCENERY_MASK_NONE;
    sceneryRestrictions = formationPlusScenery & SCENERY_MASK_RESTRICTIONS;
  }

  launchFormation(formations[formation]);
  --mission_formations_left;

  if (firstFormation) {

    frameRate = frameRate + frameRateInc[level];
    obstacleLaunchDelayMin = obstacleLaunchDelayMin + obstacleLaunchDelayInc[level];
    obstacleLaunchDelayMax = obstacleLaunchDelayMax + obstacleLaunchDelayInc[level];

    if (obstacleBulletsValue > BULLETS_MIN)  obstacleBulletsValue = obstacleBulletsValue - obstacleBulletsValueDec[level];
    if (obstacleHealthValue > HEALTH_MIN)    obstacleHealthValue = obstacleHealthValue - obstacleHealthValueDec[level];
    if (obstacleFuelValue > FUEL_MIN)        obstacleFuelValue = obstacleFuelValue - obstacleFuelValueDec[level];

    arduboy.setFrameRate(frameRate);

  }

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Launch a new obstacle.
 *  
 *  If the player is running out of fuel or health, launch one of those first.
 *  
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void launchFormation(const int8_t *formation) {

  int16_t enemyIdx = 0;


  // Disable all enemies ..

  for (uint8_t i = 0; i < NUMBER_OF_ENEMIES; ++i) {
    enemies[i].setEnabled(false);
  }


  // Retrieve number of enemies to launch ..

  uint8_t number_of_enemies = pgm_read_byte(&formation[enemyIdx]);
  ++enemyIdx;

  for (uint8_t i = 0; i < number_of_enemies; ++i) {

    switch ((EnemyType)pgm_read_byte(&formation[enemyIdx])) {

      case EnemyType::Fighter1:
        enemies[i] = { EnemyType::Fighter1, enemy_fighter1_images };
        break;

      case EnemyType::Fighter2:
        enemies[i] = { EnemyType::Fighter2, enemy_fighter2_images };
        break;

      case EnemyType::Bomber:
        enemies[i] = { EnemyType::Bomber, enemy_medium_images };
        break;

      case EnemyType::Boat:
        enemies[i] = { EnemyType::Boat, enemy_boat };
        break;

      case EnemyType::Count:
        break;
      
    }

    enemies[i].setStartingPos(sequences[pgm_read_byte(&formation[enemyIdx + 1])]);
    enemies[i].setOffsetX(pgm_read_byte(&formation[enemyIdx + 2]));
    enemies[i].setOffsetY(pgm_read_byte(&formation[enemyIdx + 3]));
    enemies[i].setDelayStart(pgm_read_byte(&formation[enemyIdx + 4]));
    enemies[i].setInvertX((pgm_read_byte(&formation[enemyIdx + 5]) == 1));
    enemies[i].setInvertY((pgm_read_byte(&formation[enemyIdx + 6]) == 1));
    enemies[i].setHealth(pgm_read_byte(&formation[enemyIdx + 7]));
    enemies[i].setEnabled(true);
    enemies[i].move();

    enemyIdx = enemyIdx + 8;

  }

};


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Is the nominated enemy facing the player?
 * -----------------------------------------------------------------------------------------------------------------------------
 */
bool isAimingAtPlayer(const uint8_t enemyIdx) {
  
  const int8_t playerX = player.getX().getInteger() + PLAYER_WIDTH_HALF;
  const int8_t playerY = player.getY().getInteger() + PLAYER_HEIGHT_HALF;

  const int16_t enemyX = enemies[enemyIdx].getX().getInteger() + (enemies[enemyIdx].getEnemyType() == EnemyType::Boat ? ENEMY_BOAT_TURRENT_CENTER_X : enemies[enemyIdx].getWidth() / 2);
  const int16_t enemyY = enemies[enemyIdx].getY().getInteger() + (enemies[enemyIdx].getEnemyType() == EnemyType::Boat ? ENEMY_BOAT_TURRENT_CENTER_Y : enemies[enemyIdx].getHeight() / 2);

  const int16_t deltaX = playerX - enemyX;
  const int16_t deltaY = playerY - enemyY;

  SQ15x16 heading = AIM_Q2_MAX;
  if (deltaX != 0) {
    heading = ((SQ15x16)deltaY / (SQ15x16)deltaX);
    if(heading<0)heading=-heading;
  }

  Direction direction = (enemies[enemyIdx].getEnemyType() == EnemyType::Boat ? enemies[enemyIdx].getTurretDirection() : enemies[enemyIdx].getDirection());

  if (deltaY <= 0) {
    
    if (deltaX > 0) {
  
            if (heading >= AIM_Q0_MIN && heading <= AIM_Q0_MAX && direction == Direction::East)             return true;
       else if (heading >= AIM_Q1_MIN && heading <= AIM_Q1_MAX && direction == Direction::NorthEast)        return true;
  
    }
    
    if (deltaX < 0) {
  
           if (heading >= AIM_Q0_MIN && heading <= AIM_Q0_MAX && direction == Direction::West)              return true;
      else if (heading >= AIM_Q1_MIN && heading <= AIM_Q1_MAX && direction == Direction::NorthWest)         return true;
  
    }
  
    if (heading >= AIM_Q2_MIN && heading <= AIM_Q2_MAX && direction == Direction::North)                    return true;

  }
  
  if (deltaY > 0) {
    
    if (deltaX > 0) {

           if (heading >= AIM_Q0_MIN && heading <= AIM_Q0_MAX && direction == Direction::East)              return true;
      else if (heading >= AIM_Q1_MIN && heading <= AIM_Q1_MAX && direction == Direction::SouthEast)         return true;
    
    }
    
    if (deltaX < 0) {
  
           if (heading >= AIM_Q0_MIN && heading <= AIM_Q0_MAX && direction == Direction::West)              return true;
      else if (heading >= AIM_Q1_MIN && heading <= AIM_Q1_MAX && direction == Direction::SouthWest)         return true;
      
    }
    
    if (heading >= AIM_Q2_MIN && heading <= AIM_Q2_MAX && direction == Direction::South)                    return true;
  
  }
  
  return false;
  
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Rotate the boat's turret to aim at the player.
 * -----------------------------------------------------------------------------------------------------------------------------
 */
Direction aimAtPlayer(const uint8_t enemyIdx) {

  int8_t playerX = player.getX().getInteger() + PLAYER_WIDTH_HALF;
  int8_t playerY = player.getY().getInteger() + PLAYER_HEIGHT_HALF;

  int16_t enemyX = enemies[enemyIdx].getX().getInteger() + ENEMY_BOAT_TURRENT_X + 5;
  int16_t enemyY = enemies[enemyIdx].getY().getInteger() + (enemies[enemyIdx].getHeight() / 2);

  int16_t deltaX = playerX - enemyX;
  int16_t deltaY = playerY - enemyY;

  SQ15x16 heading = (SQ15x16)ROTATE_Q2_MAX;
  if (deltaX != 0) {
    heading = ((SQ15x16)deltaY / (SQ15x16)deltaX);
    if (heading<0) heading=-heading;
  }

  Direction aimDirection = getAimDirection(deltaX, deltaY, heading);
  Direction curDirection = enemies[enemyIdx].getTurretDirection();

  if (aimDirection != Direction::None && aimDirection != curDirection) {
  
    if (aimDirection > curDirection) ++curDirection;
    if (aimDirection < curDirection) --curDirection;
  
  }
  
  return curDirection;

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Get the direction to turn turret towards.
 * -----------------------------------------------------------------------------------------------------------------------------
 */
Direction getAimDirection(const int8_t deltaX, const int8_t deltaY, const SQ15x16 heading) {

  if (deltaY <= 0) {

    if (deltaX > 0) {
  
      if (heading > ROTATE_Q0_MIN && heading < ROTATE_Q0_MAX)      return Direction::East;
      if (heading > ROTATE_Q1_MIN && heading < ROTATE_Q1_MAX)      return Direction::NorthEast;

    }
      
    if (deltaX < 0) {
  
      if (heading > ROTATE_Q0_MIN && heading < ROTATE_Q0_MAX)      return Direction::West;
      if (heading > ROTATE_Q1_MIN && heading < ROTATE_Q1_MAX)      return Direction::NorthWest;
  
    }

    if (heading > ROTATE_Q2_MIN && heading < ROTATE_Q2_MAX)        return Direction::North;
  
  }

  if (deltaY > 0) {
    
    if (deltaX > 0) {
  
      if (heading > ROTATE_Q0_MIN && heading < ROTATE_Q0_MAX)      return Direction::East;      
      if (heading > ROTATE_Q1_MIN && heading < ROTATE_Q1_MAX)      return Direction::SouthEast;
      
    }
  
    if (deltaX < 0) {
  
      if (heading > ROTATE_Q0_MIN && heading < ROTATE_Q0_MAX)      return Direction::West;
      if (heading > ROTATE_Q1_MIN && heading < ROTATE_Q1_MAX)      return Direction::SouthWest;
      
    }

    if (heading > ROTATE_Q2_MIN && heading <= ROTATE_Q2_MAX)       return Direction::South;

  }

  return Direction::None;
  
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Check to see if the player has run over an obstacle.  If so, process the action ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void checkForObstacleCollision() {

  if (obstacle.getEnabled()) {

    if (arduboy.collide(player.getRect(), obstacle.getRect())) {

      switch (obstacle.getObstacleType()) {

        case ObstacleType::Fuel:
          player.setFuel(obstacle.getValue());
          break;

        case ObstacleType::Bullets:
          player.addBullets(obstacle.getValue().getInteger());
          break;

        case ObstacleType::Health:
          player.addHealth(obstacle.getValue());
          break;

        case ObstacleType::PowerUp:
          player.setPowerUp(true);
          player.addBullets(obstacle.getValue().getInteger());
          break;

        case ObstacleType::Count:
          break;

      }

      #ifdef USE_LEDS             
      arduboy.setRGBled(GREEN_LED, 8);
      ledCountdown = LED_COUNTDOWN;
      #endif

      obstacle.setEnabled(false);
      if (!sound.playing()) sound.tones(collect_obstacle);

    }

  }

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Check to see if the player has shot and enemy.  Reduce the enemy's health and if this kills the enemy, increase the score.
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void checkForEnemiesShot() {
  
  for (uint8_t i = 0; i < PLAYER_BULLETS_MAX; ++i) {

    if (playerBullets[i].getEnabled()) {
  
      Point bulletPoint = { playerBullets[i].getX(), playerBullets[i].getY() };
      
      for (uint8_t j = 0; j < NUMBER_OF_ENEMIES; ++j) {
  
        if (enemies[j].getEnabled() && enemies[j].getHealth() > 0) {
  
          if (arduboy.collide(bulletPoint, enemies[j].getRect())) {
  
            playerBullets[i].setEnabled(false);
            enemies[j].decHealth((SQ7x8)1.0);
  
            if (enemies[j].getHealth().getInteger() == 0) player.setScore(player.getScore() + 1);
            if (!sound.playing()) sound.tones(hit_by_bullets);

            break;
  
          }
  
        }
  
      }
  
    }
  
  }

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Check to see if the player has been shot by an enemy bullet.  If so, reduce the player's health ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void checkForPlayerShot() {
  
  Rect playerRect = player.getRect();
  
  for (uint8_t i = 0; i < enemyBulletsMax[level]; ++i) {
  
    if (enemyBullets[i].getEnabled()) {
  
      Point bulletPoint = { enemyBullets[i].getX(), enemyBullets[i].getY() };
    
      if (arduboy.collide(bulletPoint, playerRect)) {
  
        enemyBullets[i].setEnabled(false);
        player.decHealth(playerHitBulletDec[static_cast<uint8_t>(enemyBullets[i].getEnemyType())]);

        #ifdef USE_LEDS             
        arduboy.setRGBled(RED_LED, 8);
        ledCountdown = LED_COUNTDOWN;
        #endif

        break;
  
      }
  
    }
  
  }

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Check to see if the player and an enemy have collided.  If so, reduce the player's health ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void checkForPlayerEnemyCollision() {

  Rect playerRect = player.getRect(); 

  for (uint8_t i = 0; i < NUMBER_OF_ENEMIES; ++i) {

    if (enemies[i].getEnabled() && enemies[i].getDelayStart() == 0) {

      if (enemies[i].getEnemyType() != EnemyType::Boat && arduboy.collide(playerRect, enemies[i].getRect())) {

        player.decHealth(PLAYER_HIT_PLANE_DECREMENT);
        if (!sound.playing()) sound.tones(hit_by_plane);

        #ifdef USE_LEDS             
        arduboy.setRGBled(RED_LED, 8);
        ledCountdown = LED_COUNTDOWN;
        #endif

        break;

      }

    }

  }
      
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Check to see if an active enemy is point towards the player, if so fire a bullet ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void checkCanEnemyShoot() {
  
  for (uint8_t i = 0; i < NUMBER_OF_ENEMIES; ++i) {

    Rect enemyRect = enemies[i].getRect();
    
    if (enemies[i].getEnabled() && enemies[i].getDelayStart() == 0 && enemies[i].getNumberOfBulletsFired() > 0
                                && enemyRect.x + enemyRect.width > 0 && enemyRect.x < WIDTH 
                                && enemyRect.y + enemyRect.height >= 0 && enemyRect.y < HEIGHT ) {
  
      if (isAimingAtPlayer(i)) {

        enemies[i].decNumberOfBulletsFired();
  
        enemyBullets[enemyBulletIdx].setX(enemies[i].getX().getInteger() + (enemies[i].getEnemyType() == EnemyType::Boat ? ENEMY_BOAT_TURRENT_X : (enemies[i].getWidth() / 2)));
        enemyBullets[enemyBulletIdx].setY(enemies[i].getY().getInteger() + (enemies[i].getEnemyType() == EnemyType::Boat ? ENEMY_BOAT_TURRENT_CENTER_Y : (enemies[i].getHeight() / 2)));
        enemyBullets[enemyBulletIdx].setDirection(enemies[i].getEnemyType() == EnemyType::Boat ? enemies[i].getTurretDirection() : enemies[i].getDirection());
        enemyBullets[enemyBulletIdx].setEnabled(true);
        enemyBullets[enemyBulletIdx].setEnemyType(enemies[i].getEnemyType());

        ++enemyBulletIdx;
        if (enemyBulletIdx == enemyBulletsMax[level]) enemyBulletIdx = 0;

        if (!sound.playing()) sound.tones(machine_gun_enemy);

        break;
  
      }
  
    }
  
  }


  // Reset the countdown to induce a delay before the next shot is fired ..
  
  enemyShotCountdown = random(10, 20);

}
