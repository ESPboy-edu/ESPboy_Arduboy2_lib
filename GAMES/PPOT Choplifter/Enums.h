#pragma once

#include "src/utils/Arduboy2Ext.h"

#define _DEBUG
#define _BLADE_SOUNDS

enum class GameState : uint8_t {
  Initialise,
  Introduction,
  Sortie,
  PlayGame,
  GameOver,
  SplashScreen_Init,
  SplashScreen,
};

enum class TankState : uint8_t {
  Dead1,
  Dead2,
  Dead3,
  Move_Left,
  Move_Right,
  Stationary
};

enum class TurrentDirection : uint8_t {
  Left_Low,
  Left_Mid,
  Upright,
  Right_Mid,
  Right_Low
};

enum class HelicopterStance : int8_t {

  Right_Incline_Rotate_3 = -20,
  Right_Incline_Rotate_2,
  Right_Incline_Rotate_1,
  Right_Reverse_Incline_5 = -17,
  Right_Reverse_Incline_4,
  Right_Reverse_Incline_3,
  Right_Reverse_Incline_2,
  Right_Reverse_Incline_1 = -13,
  Right_Front_Incline_2 = -12,
  Right_Front_Incline_1,
  Right_Level_Rotate_3 = -9,
  Right_Level_Rotate_2,
  Right_Level_Rotate_1,
  Right_Incline_Max = -6,
  Right_Incline_4,
  Right_Incline_3,
  Right_Incline_2,
  Right_Incline_1,
  Right_Level = -1,

  Left_Level = 1,
  Left_Incline_1,
  Left_Incline_2,
  Left_Incline_3,
  Left_Incline_4,
  Left_Incline_Max = 6,
  Left_Level_Rotate_1,
  Left_Level_Rotate_2,
  Left_Level_Rotate_3,
  Front_Level = 10,
  Left_Front_Incline_1,
  Left_Front_Incline_2,
  Left_Reverse_Incline_1 = 13,
  Left_Reverse_Incline_2,
  Left_Reverse_Incline_3,
  Left_Reverse_Incline_4,
  Left_Reverse_Incline_5,
  Left_Incline_Rotate_1 = 18,
  Left_Incline_Rotate_2,
  Left_Incline_Rotate_3,

  Set_Delta_X_Decrease = 51,
  Set_Delta_X_Increase = 52,
  Set_Delta_X_Zero = 53,
};


enum class AbsHelicopterStance : uint8_t {

  Side_Level = 1,
  Side_Incline_1,
  Side_Incline_2,
  Side_Incline_3,
  Side_Incline_4,
  Side_Incline_Max = 6,
  Side_Level_Rotate_1,
  Side_Level_Rotate_2,
  Side_Level_Rotate_3,
  Side_Front_Level = 10,
  Side_Front_Incline_1,
  Side_Front_Incline_2,
  Side_Reverse_Incline_1 = 13,
  Side_Reverse_Incline_2,
  Side_Reverse_Incline_3,
  Side_Reverse_Incline_4,
  Side_Reverse_Incline_5 = 17,
  Side_Incline_Rotate_1,
  Side_Incline_Rotate_2,
  Side_Incline_Rotate_3,

};

enum class HostageStance : uint8_t {
  In_Dorm,
  Running_Left_1,
  Running_Left_2,
  Running_Left_3,
  Running_Left_4,
  Running_Right_1,
  Running_Right_2,
  Running_Right_3,
  Running_Right_4,
  Waving_11,
  Waving_12,
  Waving_21,
  Waving_22,
  Leaving_Dorm,
  Dead,
  Dying_1,
  Dying_2,
  Safe,
  In_Helicopter
};

enum class ExplosionType : uint8_t {
  None,
  Small,
  Medium,
  Both_None,
  Both_Small,
  Both_Medium,
  Large_1,
  Large_2,
  Large_3,
  Large_4,
  Large_End
};

enum class DormitoryState : uint8_t {
  Intact,
  Open
};

struct Helicopter {
  HelicopterStance stance;
  int16_t xPos;
  int8_t yPos;
  int8_t xDelta;
  int8_t yDelta;
  uint8_t prevTurn; 
  uint8_t hits;
  uint8_t countDown;  // used when dying;
  uint8_t xInc;
};

struct Bullet {
  int16_t xPos;
  uint8_t yPos;
  int8_t xDelta;
  uint8_t yDelta;
  uint8_t startYPos;
};

struct BulletExplosion {
  int16_t xPos;
  uint16_t yPos;
  ExplosionType explosionType;
};

struct Hostage {
  HostageStance stance;   
  uint8_t countDown;
  int16_t xPos;
};

struct Dormitory {
  DormitoryState state;  
  int16_t xPos;
  uint8_t numberOfHits;
};

struct Tank {
  TankState state; 
  TurrentDirection turrentDirection;
  uint8_t countDown;
  int16_t xPos;
  int16_t startingXPos;
  bool track;
  uint8_t numberOfHits;
};
 

#define INTRODUCTION_DELAY                  65
#define LEVEL_EASY                          1
#define LEVEL_HARD                          2

#define NUMBER_OF_PLAYER_BULLETS            6
#define NUMBER_OF_HOSTAGES                  64
#define NUMBER_OF_DORMITORIES               4
#define NUMBER_OF_TANKS                     5
#define NUMBER_OF_SORTIES                   3
#define NUMBER_OF_TANK_BULLETS              2

#define BULLET_INACTIVE_X_VALUE             5000
#define BULLET_SHOOT_HORIZONTAL             255

#define HELICOPTER_MAXIMUM_HEIGHT           (int8_t)-3
#define HELICOPTER_MINIMUM_HEIGHT           (int8_t)40
#define HELICOPTER_HOSTAGE_CAPACITY         16
#define HELICOPTER_BULLET_NUMBER_OF_HITS    3

#define PREV_TURN_FROM_LEFT                 0
#define PREV_TURN_FROM_RIGHT                1

#define DELTA_X_DO_NOTHING                  0
#define DELTA_X_INCREASE                    1
#define DELTA_X_DECREASE                    2

#define DORMITORY_HITS_REQUIRED             5
#define DORMITORY_SPACING                   400
#define DORMITORY_SPACING_HALF              DORMITORY_SPACING / 2
#define DORMITORY_HOSTAGE_CAPACITY          16

#define HOSTAGE_FAR_RIGHT_POS               210
#define HOSTAGE_FAR_LEFT_POS                2130

#define TANK_SPACING                        DORMITORY_SPACING
#define TANK_BULLET_MIN_Y_VALUE             25
#define TANK_BULLET_NUMBER_OF_HITS_EASY     4
#define TANK_BULLET_NUMBER_OF_HITS_HARD     6
#define TANK_FAR_RIGHT_POS                  225
#define TANK_WIDTH                          30

#define HELICOPTER_COUNT_DOWN_INACTIVE      0
#define HELICOPTER_COUNT_DOWN_START         1
#define HELICOPTER_COUNT_DOWN_MID           3
#define HELICOPTER_COUNT_DOWN_END           30
#define HELICOPTER_END_OF_GAME_START        50
#define HELICOPTER_END_OF_GAME_END          75


