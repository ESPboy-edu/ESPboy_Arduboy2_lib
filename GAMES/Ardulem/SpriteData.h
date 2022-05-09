#ifndef SPRITE_DATA_H
#define SPRITE_DATA_H

/*
 * HUD Sprites and anims
 */

extern const unsigned char sprite_HUDEnter[];
extern const unsigned char sprite_HUDExit[];
extern const unsigned char sprite_HUDFlagDown[];
extern const unsigned char sprite_HUDVelocityCursor[];
extern const unsigned char sprite_Cursor[][7];
extern const unsigned char sprite_CursorArrow[][5];

const uint16_t ANIM_HUD_FLAG_FRAME_COUNT = 4;
extern const unsigned char anim_HUDFlag[][6];
 
const uint16_t SPRITE_TINY_NUMBERS_COUNT = 10;
extern const unsigned char sprite_TinyNumbers[][3];

const uint16_t ANIM_PARA_BUTTON_FRAME_COUNT = 4;
extern const unsigned char anim_ParaButton[][5];


/*
 * Animation for the lems
 */

// WALK
const uint16_t ANIM_LEM_WALK_FRAME_COUNT = 4;
extern const unsigned char anim_LemWalk[][3];

// BLOCKER
const uint16_t ANIM_LEM_BLOCKER_FRAME_COUNT = 1;
extern const unsigned char anim_LemBlocker[][5];

// BOMB
const uint16_t ANIM_LEM_BOMB_FRAME_COUNT = 7;
extern const unsigned char anim_LemBomb[][4];

// DIG DIAGONAL
const uint16_t ANIM_LEM_DIG_DIAGONAL_FRAME_COUNT = 6;
extern const unsigned char anim_LemDigDiagonal[][5];

// DIG HORIZONTAL
const uint16_t ANIM_LEM_DIG_HORIZONTAL_FRAME_COUNT = 6;
extern const unsigned char anim_LemDigHorizontal[][5];

// DIG HORIZONTAL
const uint16_t ANIM_LEM_DIG_VERTICAL_FRAME_COUNT = 8;
extern const unsigned char anim_LemDigVertical[][5];

// STAIR
const uint16_t ANIM_LEM_STAIR_FRAME_COUNT = 6;
extern const unsigned char anim_LemStair[][5];

// CLIMB
const uint16_t ANIM_LEM_CLIMB_FRAME_COUNT = 3;
extern const unsigned char anim_LemClimb[][2];

// CLIMB TOP
const uint16_t ANIM_LEM_CLIMB_TOP_FRAME_COUNT = 4;
extern const unsigned char anim_LemClimbTop[][4];

// START FALL
const uint16_t ANIM_LEM_START_FALL_FRAME_COUNT = 3;
extern const unsigned char anim_LemStartFall[][5];

// FALL
const uint16_t ANIM_LEM_FALL_FRAME_COUNT = 4;
extern const unsigned char anim_LemFall[][5];

// FALL TO DEATH
const uint16_t ANIM_LEM_FALL_TO_DEATH_FRAME_COUNT = 3;
extern const unsigned char anim_LemFallToDeath[][5];

// CRASH
const uint16_t ANIM_LEM_CRASH_FRAME_COUNT = 6;
extern const unsigned char anim_LemCrash[][5];

// PARA
const uint16_t ANIM_LEM_PARA_FRAME_COUNT = 4;
extern const unsigned char anim_LemPara[][3];

// PARACHUTE
const uint16_t ANIM_PARACHUTE_FRAME_COUNT = 4;
extern const unsigned char anim_Parachute[][7];

// all the anim width and frame count ordered by state
extern const unsigned char LemAnimWidthPerLemState[];
extern const unsigned char LemAnimFrameCountPerLemState[];
extern const unsigned char LemAnimRootPosition[];

/*
 * Start and Home
 */
const uint16_t ANIM_START_FRAME_COUNT = 6;
extern const unsigned char anim_Start[][8];

extern const unsigned char sprite_HomeTop[];
extern const unsigned char sprite_HomeBottom[];

#endif
