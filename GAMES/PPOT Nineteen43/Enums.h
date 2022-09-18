#pragma once

#include "FixedPoints.h"
#include "FixedPointsCommon.h"
#include <Arduboy2.h>


// Game States ..

#define STATE_INTRO_INIT                 0
#define STATE_INTRO_LOOP                 1
#define STATE_GAME_INIT                  2
#define STATE_GAME_LOOP                  3
#define STATE_GAME_END_OF_MISSION        4
#define STATE_GAME_END_OF_GAME           5
#define STATE_CREDITS_INIT               6
#define STATE_CREDITS_LOOP               7


// Image array offsets ..

#define IMAGES_MASK_OFFSET              8
#define IMAGES_EXPLOSION_OFFSET         16
#define IMAGES_EXPLOSION_MASK_OFFSET    20
#define IMAGES_SHRINK_OFFSET            24
#define IMAGES_SHRINK_MASK_OFFSET       28


// EEPROM settings for high score ..

#define EEPROM_START_C1                 200
#define EEPROM_START_C2                 EEPROM_START_C1 + 1
#define EEPROM_SCORE                    EEPROM_START_C1 + 2
#define EEPROM_LEVEL                    EEPROM_START_C1 + 8

#define NUMBER_OF_ENEMIES               6
#define NUMBER_OF_MISSIONS              5

#define BULLETS_MAX                     75
#define BULLETS_MIN                     BULLETS_MAX / 2
#define BULLETS_DECREMENT_L0            (SQ7x8)0.000
#define BULLETS_DECREMENT_L1            (SQ7x8)0.500
#define BULLETS_DECREMENT_L2            (SQ7x8)1.000

#define HEALTH_MAX                      14
#define HEALTH_MIN                      HEALTH_MAX / 2
#define HEALTH_DECREMENT_L0             (SQ7x8)0.000
#define HEALTH_DECREMENT_L1             (SQ7x8)0.300
#define HEALTH_DECREMENT_L2             (SQ7x8)0.600

#define POWER_UP_MAX                    10

#define FUEL_MAX                        14
#define FUEL_MIN                        FUEL_MAX / 2
#define FUEL_DECREMENT                  0.010
#define FUEL_DECREMENT_BOOST            0.005
#define FUEL_DECREMENT_L0               (SQ7x8)0.000
#define FUEL_DECREMENT_L1               (SQ7x8)0.080
#define FUEL_DECREMENT_L2               (SQ7x8)0.160

#define OBSTACLE_LAUNCH_DELAY_MAX       300
#define OBSTACLE_LAUNCH_DELAY_MIN       125
#define OBSTACLE_LAUNCH_DELAY_INC_L0    0
#define OBSTACLE_LAUNCH_DELAY_INC_L1    5
#define OBSTACLE_LAUNCH_DELAY_INC_L2    10
#define OBSTACLE_WIDTH                  8

#define FRAME_RATE_INC_L0               1
#define FRAME_RATE_INC_L1               2
#define FRAME_RATE_INC_L2               3
#define INIT_FRAME_RATE                 50

#define PLAYER_MOVEMENT_INC_UP          0.80
#define PLAYER_MOVEMENT_INC_DOWN        0.80
#define PLAYER_MOVEMENT_INC_LEFT        0.90
#define PLAYER_MOVEMENT_INC_RIGHT       0.80
#define PLAYER_BULLETS_MAX              15
#define PLAYER_HIT_BULLET_DECREMENT     0.25 
#define PLAYER_HIT_PLANE_DECREMENT      0.05 
#define PLAYER_WIDTH                    16
#define PLAYER_WIDTH_HALF               8
#define PLAYER_HEIGHT                   17
#define PLAYER_HEIGHT_HALF              8

#define ENEMY_BULLETS_MAX               10
#define ENEMY_NO_BULLETS_FIRED_MAX      3
#define ENEMY_BOAT_TURRENT_X            24
#define ENEMY_BOAT_TURRENT_Y            3
#define ENEMY_BOAT_TURRENT_CENTER_X     ENEMY_BOAT_TURRENT_X + 5
#define ENEMY_BOAT_TURRENT_CENTER_Y     ENEMY_BOAT_TURRENT_Y + 5

#define SCOREBOARD_OUTER_RECT_X         120
#define SCOREBOARD_OUTER_RECT_Y         0
#define SCOREBOARD_OUTER_RECT_WIDTH     8
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

enum class EnemyType : uint8_t {
  Zero,
  ZeroHollow,
  Bomber,
  Boat,
  Count,
  First = Zero,
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
