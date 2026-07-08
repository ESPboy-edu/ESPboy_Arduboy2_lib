#ifndef DEFINES_H_
#define DEFINES_H_

#define USE_SIMPLE_COLLISIONS

#define FXDATA_STREAMING

#if defined (_WIN32)

#define SHOW_FULL_PITCH 0
//#define STANDARD_FILE_STREAMING
//#define PROGMEM_MAP_STREAMING
//#define EMULATE_GAMEBUINO 1
#define EMULATE_ARDUBOY 1
//#define EMULATE_HACKVISION 1
//#define EMULATE_UZEBOX 1

#if defined(EMULATE_ARDUBOY)
// Arduboy
#if SHOW_FULL_PITCH
#define DISPLAYWIDTH 256
#define DISPLAYHEIGHT 320
#else
#define DISPLAYWIDTH 128
#define DISPLAYHEIGHT 64
#endif
#endif

#if defined(EMULATE_UZEBOX)
// Arduboy
#define DISPLAYWIDTH 240
#define DISPLAYHEIGHT 120
#endif

#else // Arduboy
//#define PETIT_FATFS_FILE_STREAMING
//#define PROGMEM_MAP_STREAMING
#define DISPLAYWIDTH 128
#define DISPLAYHEIGHT 64
#endif

#define HALF_DISPLAYWIDTH (DISPLAYWIDTH >> 1)
#define HALF_DISPLAYHEIGHT (DISPLAYHEIGHT >> 1)

// WIN32 specific
#ifdef _WIN32
#define ZOOM_SCALE 2

#define PROGMEM
#define PSTR
#define pgm_read_byte(x) (*((uint8_t*)x))
#define pgm_read_word(x) (*((uint16_t*)x))

#define pgm_read_ptr(x) (*((uintptr_t*)x))
#define strlen_P(x) strlen(x)

#include <stdint.h>
typedef uint32_t __uint24;
using uint24_t = __uint24;

#include <stdio.h>
#define WARNING(msg, ...) printf((msg), __VA_ARGS__)
#define ERROR(msg) printf(msg)
#else
#include <avr/pgmspace.h>
#define WARNING(msg, ...)
#define pgm_read_ptr(x) pgm_read_word(x)

#endif
// end

#if !defined(max) && !defined(min)
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#define min3(a, b, c) (min(min(a, b), c))
#define max3(a, b, c) (max(max(a, b), c))
#define sign(x) ((x) < 0 ? -1 : 1)
#define mabs(x) ((x) < 0 ? -(x) : (x))

#define FIRST_FONT_GLYPH 32
#define LAST_FONT_GLYPH 95
#define FONT_WIDTH 3
#define FONT_HEIGHT 5
#define FONT_GLYPH_BYTE_SIZE 2

#define TARGET_FRAMERATE 30

#define PLAYERS_PER_TEAM 5
#define NUM_PEOPLE (PLAYERS_PER_TEAM * 2 + 1)

#define TILE_SIZE 8
#define TILE_SIZE_BYTES 8

#define FIXED_SHIFT 4
#define BALL_GRAVITY 12
#define BALL_FRICTION 3
#define KICK_RECOVERY_FRAMES 6
#define TACKLE_RECOVERY_FRAMES 15
#define SLIDE_TACKLE_RECOVERY_FRAMES 30
#define SLIDE_TACKLE_FRAMES 25
#define PERSON_HEIGHT 12

#define BACKGROUND_WIDTH 256
#define BACKGROUND_HEIGHT 320

#define MAX_CAMERA_DELTA 4

#define GET_BALL_DISTANCE 6 //10
#define TACKLE_DISTANCE 6

#define CENTER_MARK_X 128
#define CENTER_MARK_Y 160

// 'Level geometry' for collision
#define LEFT_POST_X1 100
#define LEFT_POST_X2 108

#define RIGHT_POST_X1 147
#define RIGHT_POST_X2 155

#define TOP_GOAL_POST_Y1 31
#define TOP_GOAL_POST_Y2 37

#define BOTTOM_GOAL_POST_Y1 282
#define BOTTOM_GOAL_POST_Y2 289

#define GOAL_BAR_Z1 21
#define GOAL_BAR_Z2 27

#define GOAL_NET_X1 104 // 100
#define GOAL_NET_X2 151 // 155

#define TOP_GOAL_NET_Y1 27
#define TOP_GOAL_NET_Y2 33

#define BOTTOM_GOAL_NET_Y1 287
#define BOTTOM_GOAL_NET_Y2 294

#define PITCH_TOP 32
#define PITCH_LEFT 14
#define PITCH_BOTTOM 287
#define PITCH_RIGHT 241

#define PENALTY_BOX_WIDTH 138
#define PENALTY_BOX_HEIGHT 48

#define PERSON_HALF_WIDTH 5
#define GOALIE_DIVE_FRAMES 21

enum
{
	WHITE_TEAM = 0,
	BLACK_TEAM = 1,
	REFEREE_TEAM = 2
};

enum
{
	LOCAL_PLAYER = 0,
	REMOTE_PLAYER = 1
};

enum Direction
{
	North,
	NorthEast,
	East,
	SouthEast,
	South,
	SouthWest,
	West,
	NorthWest,
	NoDirection
};

#endif
