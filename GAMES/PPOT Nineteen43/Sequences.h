#pragma once

#define END_SEQUENCE      -127
#define END_SEQUENCE_UINT 129

/* -----------------------------------------------------------------------------------------------------------------------------
 *  Missions
 *
 *  Format:  [Number of formations], [Formation Index 1], [Formation Index 2], .. 
 * -----------------------------------------------------------------------------------------------------------------------------
 */

const uint8_t PROGMEM mission_00[] = { 14, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
const uint8_t PROGMEM mission_01[] = { 14, 7, 8, 9, 13, 0, 1, 2, 3, 10, 11, 12, 4, 5, 6 };
const uint8_t PROGMEM mission_02[] = { 14, 0, 2, 4, 6, 8, 10, 12, 1, 3, 5, 7, 9, 11, 13 };
const uint8_t PROGMEM mission_03[] = { 14, 2, 1, 7, 10, 4, 12, 0, 8, 9, 13, 3, 11, 5, 6,};
const uint8_t PROGMEM mission_04[] = { 14, 2, 1, 7, 10, 4, 12, 0, 8, 9, 13, 3, 11, 5, 6,};



/* -----------------------------------------------------------------------------------------------------------------------------
 *  Formations
 *
 *  Format:  [Enemy Type],             - 0 solid, 1 hollow, 2 bomber, 3 boat
 *           [Sequence Index],   
 *           [X Offset], [Y Offset],
 *           [Delay Start],           
 *           [Invert X Axis],  
 *           [Invert Y Axis],  
 *           [Number of Hits]  
 * -----------------------------------------------------------------------------------------------------------------------------
 */


const int8_t PROGMEM formation_00[] = {
  2, 
  (uint8_t)EnemyType::Zero, 0, 0, 5, 0, false, false, 1,
  (uint8_t)EnemyType::Zero, 0, 0, 0, 24, false, false, 1,
  (uint8_t)EnemyType::Zero, 0, 0, -5, 80, false, false, 1,
  (uint8_t)EnemyType::Zero, 0, 0, -10, 104, false, false, 1,
};

const int8_t PROGMEM formation_01[] = {  /* V Shape */
  5, 
  (uint8_t)EnemyType::ZeroHollow, 0, 0, -25, 36, false, false, 2,
  (uint8_t)EnemyType::Zero, 0, 0, -16, 18, false, false, 1,
  (uint8_t)EnemyType::Zero, 0, 0, -7, 0, false, false, 1,
  (uint8_t)EnemyType::Zero, 0, 0, 2, 18, false, false, 1,
  (uint8_t)EnemyType::ZeroHollow, 0, 0, 11, 36, false, false, 2,
};

const int8_t PROGMEM formation_02[] = { /* Leading two cross and disappear top and bottom, remaining three in straight line */
  5, 
  (uint8_t)EnemyType::Zero, 1, 0, 5, 0, false, true, 1,
  (uint8_t)EnemyType::Zero, 1, 0, -20, 20, false, false, 1,
  (uint8_t)EnemyType::ZeroHollow, 0, 0, -23, 50, false, false, 2,
  (uint8_t)EnemyType::ZeroHollow, 0, 0, -7, 50, false, false, 2,
  (uint8_t)EnemyType::ZeroHollow, 0, 0, 9, 50, false, false, 2,
};

const int8_t PROGMEM formation_03[] = { /* Four straight ahead */
  4, 
  (uint8_t)EnemyType::Zero, 0, 0, -16, 0, false, false, 1,
  (uint8_t)EnemyType::Zero, 0, 0, 2, 0, false, false, 1,
  (uint8_t)EnemyType::ZeroHollow, 0, 0, -16, 18, false, false, 2,
  (uint8_t)EnemyType::ZeroHollow, 0, 0, 2, 18, false, false, 2,
};

const int8_t PROGMEM formation_04[] = { /* three in row 1, followed by bomber */
  4, 
  (uint8_t)EnemyType::Zero, 0, 0, -25, 0, false, false, 1,
  (uint8_t)EnemyType::Zero, 0, 0, -7, 0, false, false, 1,
  (uint8_t)EnemyType::Zero, 0, 0, 11, 0, false, false, 1,
  (uint8_t)EnemyType::Bomber, 0, 0, -11, 22, false, false, 1,
};

const int8_t PROGMEM formation_05[] = { /* three in row 1, followed by boat */
  4, 
  (uint8_t)EnemyType::Zero, 0, 0, -16, 18, false, false, 1,
  (uint8_t)EnemyType::Zero, 0, 0, -7, 0, false, false, 1,
  (uint8_t)EnemyType::Zero, 0, 0, 2, 18, false, false, 1,
  (uint8_t)EnemyType::Boat, 0, 0, 14, 48, false, false, 10,
};

const int8_t PROGMEM formation_06[] = { /* 2, 2, 1 */
  5, 
  (uint8_t)EnemyType::Zero, 1, 0, 5, 0, false, false, 1,
  (uint8_t)EnemyType::Zero, 1, 0, 5, 20, false, false, 1,
  (uint8_t)EnemyType::Zero, 1, 0, -20, 0, false, true, 1,
  (uint8_t)EnemyType::Zero, 1, 0, -20, 20, false, true, 1,
  (uint8_t)EnemyType::Zero, 0, 0, -7, 40, false, true, 1,
};

const int8_t PROGMEM formation_07[] = {
  5, 
  (uint8_t)EnemyType::Zero, 2, -16, -48, 0, false, false, 1,
  (uint8_t)EnemyType::Zero, 2, -16, -48, 40, false, false, 1,
  (uint8_t)EnemyType::ZeroHollow, 0, 0, -7, 68, false, false, 2,
  (uint8_t)EnemyType::Zero, 2, -16, 32, 20, false, true, 1,
  (uint8_t)EnemyType::Zero, 2, -16, 32, 60, false, true, 1,
};

const int8_t PROGMEM formation_08[] = {
  6, 
  (uint8_t)EnemyType::Zero, 3, -42, -48, 0, false, false, 1,
  (uint8_t)EnemyType::Zero, 3, -26, -32, 0, false, false, 1,
  (uint8_t)EnemyType::ZeroHollow, 0, 0, -21, 20, false, false, 2,
  (uint8_t)EnemyType::ZeroHollow, 0, 0, -7, 36, false, false, 2,
  (uint8_t)EnemyType::ZeroHollow, 0, 0, 7, 52, false, false, 2,
  (uint8_t)EnemyType::ZeroHollow, 0, 0, -7, 64, false, false, 2,
};

const int8_t PROGMEM formation_09[] = {
  6, 
  (uint8_t)EnemyType::Zero, 0, 0, -16, 18, false, false, 1,
  (uint8_t)EnemyType::Zero, 0, 0, -7, 0, false, false, 1,
  (uint8_t)EnemyType::Zero, 0, 0, 2, 18, false, false, 1,
  (uint8_t)EnemyType::Bomber, 0, 0, -28, 64, false, false, 2,
  (uint8_t)EnemyType::Bomber, 0, 0, -11, 44, false, false, 2, 
  (uint8_t)EnemyType::Bomber, 0, 0, 6, 64, false, false, 2,
};

const int8_t PROGMEM formation_10[] = {
  5, 
  (uint8_t)EnemyType::Boat, 0, 0, 14, 4, false, false, 10,
  (uint8_t)EnemyType::Boat, 0, 0, -32, 62, false, false, 10,
  (uint8_t)EnemyType::Zero, 2, -16, -48, 0, false, false, 1,
  (uint8_t)EnemyType::Zero, 2, -16, -48, 35, false, false, 1,
  (uint8_t)EnemyType::Zero, 2, -16, -48, 70, false, false, 1,
};

const int8_t PROGMEM formation_11[] = {
  5, 
  (uint8_t)EnemyType::Zero, 2, -16, -48, 0, false, false, 1,
  (uint8_t)EnemyType::Zero, 2, -16, -48, 40, false, false, 1,
  (uint8_t)EnemyType::ZeroHollow, 0, 0, -7, 68, false, false, 2,
  (uint8_t)EnemyType::Zero, 2, 0, 32, 0, true, true, 1,
  (uint8_t)EnemyType::Zero, 2, 0, 32, 40, true, true, 1,
};

const int8_t PROGMEM formation_12[] = {
  5, 
  (uint8_t)EnemyType::Zero, 2, -16, -48, 0, false, false, 1,
  (uint8_t)EnemyType::Zero, 2, -16, -48, 40, false, false, 1,
  (uint8_t)EnemyType::ZeroHollow, 0, 0, -7, 68, false, false, 2,
  (uint8_t)EnemyType::Zero, 2, 0, 32, 0, true, true, 1,
  (uint8_t)EnemyType::Zero, 2, 0, 32, 40, true, true, 1,
};

const int8_t PROGMEM formation_13[] = {
  6, 
  (uint8_t)EnemyType::Zero, 2, -16, -48, 0, false, false, 1,
  (uint8_t)EnemyType::Zero, 2, -16, -48, 16, false, false, 1,
  (uint8_t)EnemyType::ZeroHollow, 0, 0, -16, 36, false, false, 2,
  (uint8_t)EnemyType::ZeroHollow, 0, 0, 2, 36, false, false, 2,
  (uint8_t)EnemyType::Zero, 2, -16, 32, 74, false, true, 1,
  (uint8_t)EnemyType::Zero, 2, -16, 32, 90, false, true, 1,
};

/* -----------------------------------------------------------------------------------------------------------------------------
 *  Sequences
 *
 *  Format:  [X Position],             - Single step
 *           [Y Position],   
 *           [Direction]        
 *
 *           [X Increment],            - Multiple steps           
 *           [Y Increment],  
 *           [Number of Repeats]       - Must be between 10 and 127
 *
 *  Note:    All sequences should be terminated by the following 0, 0, END_SEQUENCE.
 *           Arduboy screen dimensions 128 x 64.  
 *           Centre of screen is therefore 64 x 32 = position 0, 0.
 *           Top left of screen is therefore 64 x 32 = position 0, 0.
 *
 * -----------------------------------------------------------------------------------------------------------------------------
 */

const int8_t PROGMEM seq_00[] = {          /* Straight line  */
  64, 0, (int8_t)Direction::West,
  -1, 0, 64,
  -1, 0, 40,
  -1, 0, 80,
  0, 0, END_SEQUENCE,
};

const int8_t PROGMEM seq_01[] = {          /* L from right, going down */ 
  64, 0, (int8_t)Direction::West,
  -1, 0, 37,
  26, 1, (int8_t)Direction::SouthWest,
  -1, 1, 24,
  3, 24, (int8_t)Direction::South,
  0, 1, 25,
  0, 0, END_SEQUENCE,
};

const int8_t PROGMEM seq_02[] = {         /* Diagonal */ 
  64, 0, (int8_t)Direction::SouthWest,
  -1, 1, 80,
  0, 0, END_SEQUENCE,
};

const int8_t PROGMEM seq_03[] = {         /* Straight Down */ 
  64, 0, (int8_t)Direction::South,
  0, 1, 80,
  0, 0, END_SEQUENCE,
};

