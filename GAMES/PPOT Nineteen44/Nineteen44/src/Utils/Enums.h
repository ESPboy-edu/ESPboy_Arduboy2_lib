#pragma once

// Remove comment // to free up some PROGMEM for DEBUG
#define _DEBUG

#include "FixedPoints.h"
#include "FixedPointsCommon.h"
#include "Arduboy2Ext.h"

// ---------------------------------------
//  For an Arduboy
// ---------------------------------------
// 
//  To use the original graphics:
//    SAVE_MEMORY 
//    _USE_LEDS
//
//  To use new obstacle graphics and dashboard:
//    SAVE_MEMORY 
//    USE_LEDS
//
// ---------------------------------------
//  For a Microcard
// ---------------------------------------
// 
//  To use the original graphics:
//    _SAVE_MEMORY 
//    _USE_LEDS
//
//  To use new obstacle graphics and dashboard:
//    _SAVE_MEMORY 
//    USE_LEDS
//
#define SAVE_MEMORY
#define USE_LEDS
#define USE_ISLAND_3
#define HIGH_SCORES


const int8_t PROGMEM movement_x_offset[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
const int8_t PROGMEM movement_y_offset[] = { -1, -1, 0, 1, 1, 1, 0, -1 };

// Game States ..

enum class GameState : uint8_t {
  Intro_Init,
  Intro_Loop,
  Game_Init,
  Game_Loop,
  End_Of_Mission,
  End_Of_Game,
  Credits_Init,
  Credits_Loop,
  #ifdef HIGH_SCORES
  Save_Score,
  High_Score,
  #endif
};

constexpr const static uint8_t UPLOAD_DELAY = 16;
#define MAX_BULLET_COUNTDOWN 20

#ifdef USE_LEDS             
#define LED_COUNTDOWN 4
#endif


#define PLAYER_US
#define _PLAYER_JAPANESE



// Image array offsets ..

#define IMAGES_MASK_OFFSET              8
#define IMAGES_EXPLOSION_OFFSET         16
#define IMAGES_EXPLOSION_MASK_OFFSET    20
#define IMAGES_SHRINK_OFFSET            24
#define IMAGES_SHRINK_MASK_OFFSET       28

#define PLAYER_MOVE_NONE                0
#define PLAYER_MOVE_UP                  1
#define PLAYER_MOVE_DOWN                -1

#define NUMBER_OF_ENEMIES               6
#define NUMBER_OF_MISSIONS              5

#define BULLETS_MAX                     75
#define BULLETS_MIN                     BULLETS_MAX / 2
#define BULLETS_DECREMENT_L0            (SQ7x8)0.000
#define BULLETS_DECREMENT_L1            (SQ7x8)0.500
#define BULLETS_DECREMENT_L2            (SQ7x8)1.100

#define HEALTH_MAX                      14
#define HEALTH_MIN                      HEALTH_MAX / 2
#define HEALTH_DECREMENT_L0             (SQ7x8)0.000
#define HEALTH_DECREMENT_L1             (SQ7x8)0.500
#define HEALTH_DECREMENT_L2             (SQ7x8)1.100

#define POWER_UP_MAX                    10

#define FUEL_MAX                        14
#define FUEL_MIN                        FUEL_MAX / 2
#define FUEL_DECREMENT                  0.010
#define FUEL_DECREMENT_BOOST            0.005
#define FUEL_DECREMENT_L0               (SQ7x8)0.000
#define FUEL_DECREMENT_L1               (SQ7x8)0.120
#define FUEL_DECREMENT_L2               (SQ7x8)0.400

#define OBSTACLE_LAUNCH_DELAY_MAX       300
#define OBSTACLE_LAUNCH_DELAY_MIN       125
#define OBSTACLE_LAUNCH_DELAY_INC_L0    0
#define OBSTACLE_LAUNCH_DELAY_INC_L1    10
#define OBSTACLE_LAUNCH_DELAY_INC_L2    25
#define OBSTACLE_WIDTH                  8

#define FRAME_RATE_INC_L0               2
#define FRAME_RATE_INC_L1               3
#define FRAME_RATE_INC_L2               4
#define INIT_FRAME_RATE                 50

#define PLAYER_MOVEMENT_INC_UP          0.80
#define PLAYER_MOVEMENT_INC_DOWN        0.80
#define PLAYER_MOVEMENT_INC_LEFT        0.90
#define PLAYER_MOVEMENT_INC_RIGHT       0.80
#define PLAYER_BULLETS_MAX              15
#define PLAYER_HIT_BULLET_DEC_NORM      0.25 
#define PLAYER_HIT_BULLET_DEC_DOUBLE    0.50 
#define PLAYER_HIT_PLANE_DECREMENT      0.12 

#define PLAYER_WIDTH                    18
#define PLAYER_WIDTH_HALF               9
#define PLAYER_HEIGHT                   19
#define PLAYER_HEIGHT_HALF              9

#define ENEMY_BULLETS_MAX_L1            10
#define ENEMY_BULLETS_MAX_L2            15
#define ENEMY_BULLETS_MAX_L3            20
#define ENEMY_BULLETS_MAX               ENEMY_BULLETS_MAX_L3
#define ENEMY_NO_BULLETS_FIRED_MAX      5
#define ENEMY_BOAT_TURRENT_X            24
#define ENEMY_BOAT_TURRENT_Y            3
#define ENEMY_BOAT_TURRENT_CENTER_X     ENEMY_BOAT_TURRENT_X + 5
#define ENEMY_BOAT_TURRENT_CENTER_Y     ENEMY_BOAT_TURRENT_Y + 5

#define SCOREBOARD_OUTER_RECT_X         120
#define SCOREBOARD_OUTER_RECT_WIDTH     8
#define SCOREBOARD_OUTER_RECT_Y         0
#define SCOREBOARD_OUTER_RECT_HEIGHT    HEIGHT
#define SCOREBOARD_FRAME_COUNT_MAX      200
#define SCOREBOARD_NUMBER_OF_FRAMES     2

#define SCOREBOARD_INNER_RECT_X         SCOREBOARD_OUTER_RECT_X + 1
#define SCOREBOARD_INNER_RECT_Y         SCOREBOARD_OUTER_RECT_Y

#define SCOREBOARD_FLASH_MAX            30

#define SCOREBOARD_KILLS_X              123
#define SCOREBOARD_KILLS_ICON_Y         0 
#define SCOREBOARD_KILLS_DIGIT_0_Y      6 
#define SCOREBOARD_KILLS_DIGIT_1_Y      11 
#define SCOREBOARD_KILLS_DIGIT_2_Y      16

#define SCOREBOARD_HEALTH_BAR_X         SCOREBOARD_KILLS_X
#define SCOREBOARD_HEALTH_BAR_Y         23
#define SCOREBOARD_HEALTH_BAR_TOP       30

#define SCOREBOARD_FUEL_BAR_X           SCOREBOARD_KILLS_X
#define SCOREBOARD_FUEL_BAR_Y           46
#define SCOREBOARD_FUEL_BAR_TOP         51

#define SCOREBOARD_BULLET_BAR_X         SCOREBOARD_KILLS_X
#define SCOREBOARD_BULLET_BAR_Y         34
#define SCOREBOARD_BULLET_BAR_TOP       SCOREBOARD_BULLET_BAR_Y + 5

#define SCOREBOARD_BULLET_PU_X          SCOREBOARD_KILLS_X
#define SCOREBOARD_BULLET_PU_Y          24


// Used to determine if the enemy is aiming at the player ..

#define AIM_Q0_MIN                      (SQ15x16)0
#define AIM_Q0_MAX                      (SQ15x16)0.131648
#define AIM_Q1_MIN                      (SQ15x16)0.767296
#define AIM_Q1_MAX                      (SQ15x16)1.303152
#define AIM_Q2_MIN                      (SQ15x16)7.593262
#define AIM_Q2_MAX                      (SQ15x16)999


// Used for rotating the turret ..

#define ROTATE_Q0_MIN                   (SQ15x16)0
#define ROTATE_Q0_MAX                   (SQ15x16)0.414199
#define ROTATE_Q1_MIN                   (SQ15x16)0.414199
#define ROTATE_Q1_MAX                   (SQ15x16)2.413976
#define ROTATE_Q2_MIN                   (SQ15x16)2.413976
#define ROTATE_Q2_MAX                   (SQ15x16)999

#define NUMBER_OF_SCENERY_TILES         5
#define NUMBER_OF_SCENERY_ITEMS         4
#define SCENERY_TILE_WIDTH              32


#define SCENERY_TILE_DECR               1
#define SCENERY_TILE_FLAT_BEGIN         2
#define SCENERY_TILE_FLAT_END           6
#define SCENERY_TILE_INCR               7

#define SCENERY_LOWER_OFFSET_MIN -20    
#define SCENERY_LOWER_OFFSET_MIN_PLUS_INC SCENERY_LOWER_OFFSET_MIN + 4       
#define SCENERY_LOWER_OFFSET_MAX_MINUS_INC 20       
#define SCENERY_LOWER_OFFSET_MAX SCENERY_LOWER_OFFSET_MAX_MINUS_INC + 4

#define SCENERY_UPPER_OFFSET_MIN -24    
#define SCENERY_UPPER_OFFSET_MIN_PLUS_INC SCENERY_UPPEROFFSET_MIN + 4       
#define SCENERY_UPPER_OFFSET_MAX_MINUS_INC 20       
#define SCENERY_UPPER_OFFSET_MAX SCENERY_UPPER_OFFSET_MAX_MINUS_INC + 4

#define INIT_FUEL_MIN                   4
#define INIT_HEALTH_MIN                 4
#define INIT_BULLETS_MIN                10

constexpr uint8_t DO_NOT_EDIT_SLOT             = 255;
constexpr uint8_t MAX_NUMBER_OF_SCORES         = 5;

struct SceneryInfo {
  int8_t offset;
  uint8_t tile;
};

enum class SceneryElement : uint8_t {
  Boat,
  Boat2,
  Wave1,
  Wave2,
  Cloud_AbovePlanes,
  Cloud_BelowPlanes,
  Island1,
  Island2,
  Island3,  
};

struct SceneryItem {
  int16_t x;
  uint8_t y;
  SceneryElement element;
};

enum class EnemyType : uint8_t {
  Fighter1,
  Fighter2,
  Bomber,
  Boat,
  Count,
  First = Fighter1,
  Last = Boat
};

enum class ObstacleType : uint8_t {
  Fuel,
  Bullets,
  Health,
  PowerUp,
  Count,
  First = Fuel,
  Last = Health
};

enum class Direction : uint8_t {
  North,
  NorthEast,
  East,
  SouthEast,
  South,
  SouthWest,
  West,
  NorthWest,
  Count,
  None,
};

enum class BulletImage : uint8_t {
  Single,
  Slash,
  BackSlash,
};

#define SCENERY_INACTIVE -91
struct SceneryGround {
  int16_t x = -SCENERY_INACTIVE;
  int8_t y;
  uint8_t image;
  bool enabled;
};


const Direction inverseX[] =     { Direction::North, Direction::NorthWest, Direction::West, Direction::SouthWest,
                                   Direction::South, Direction::SouthEast, Direction::East, Direction::NorthEast };
                                           
const Direction inverseY[] =     { Direction::South, Direction::SouthEast, Direction::East, Direction::NorthEast,
                                   Direction::North, Direction::NorthWest, Direction::West, Direction::SouthWest };            
                                           
const Direction inverseXandY[] = { Direction::South, Direction::SouthWest, Direction::West, Direction::NorthWest,
                                   Direction::North, Direction::NorthEast, Direction::East, Direction::SouthEast };            

inline Direction operator++( Direction & c ) {

  c = ( c == Direction::NorthWest )
  ? Direction::North
  : static_cast<Direction>( static_cast<uint8_t>(c) + 1 );
  return c;

}

inline Direction operator++( Direction & c, int ) {

  Direction result = c;
  ++c;
  return result;

}

inline Direction operator--( Direction & c ) {

  c = ( c == Direction::North )
  ? Direction::NorthWest
  : static_cast<Direction>( static_cast<uint8_t>(c) - 1 );
  return c;

}

inline Direction operator--( Direction & c, int ) {

  Direction result = c;
  ++c;
  return result;

}


inline bool operator<(const Direction  lhs, const Direction  rhs)  { 

  return (abs((uint8_t)lhs - (uint8_t)rhs) < 4 ? (uint8_t)lhs - (uint8_t)rhs : (uint8_t)lhs - (8 + (uint8_t)rhs)) < 0; 
  
} 

inline bool operator>(const Direction  lhs, const Direction  rhs)  { 
    
  return (abs((uint8_t)lhs - (uint8_t)rhs) < 4 ? (uint8_t)lhs - (uint8_t)rhs : (8 + (uint8_t)lhs) - (uint8_t)rhs) > 0;
 
} 

inline bool operator==(const Direction lhs, const Direction rhs)   { return (uint8_t)lhs == (uint8_t)rhs; }
inline bool operator!=(const Direction lhs, const Direction rhs)   { return !operator == (lhs,rhs); }
inline bool operator<=(const Direction lhs, const Direction rhs)   { return !operator >  (lhs,rhs); }
inline bool operator>=(const Direction lhs, const Direction rhs)   { return !operator <  (lhs,rhs); }



// ---------------------------------------------------------------------------------
// Extract individual digits of a uint16_t
//
template< size_t size >
void extractDigits(uint8_t (&buffer)[size], uint16_t value) {

  for(uint8_t i = 0; i < size; ++i) {
    buffer[i] = value % 10;
    value /= 10;
  }

}


// ----------------------------------------------------------------------------
//  Clamp the value into a range .. 
//
template<typename T> T clamp(const T & v, const T & min, const T & max) {
  
  return (v < min) ? min : (v > max ? max : v);

}


// ----------------------------------------------------------------------------
//  A better absolute as it uses less memory than the standard one .. 
//
template<typename T> T absT(const T & v) {
  
  return (v < 0) ? -v : v;

}