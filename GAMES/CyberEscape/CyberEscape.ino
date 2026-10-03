/*
 * CYBER ESCAPE — rail shooter for Arduboy FX
 * SKRTM 2026
 *
 * D-Pad : move    A : start / next / pause    B : restart level    A+B : restart game
 *
 * Requires the Arduboy2 library (6.0+). Integer math only, no dynamic allocation.
 */

#include <Arduboy2.h>

Arduboy2 arduboy;

#define BG_COLOR          WHITE
#define FG_COLOR          BLACK
#define SCREEN_CX         64
#define HORIZON_Y         26

#define FP_SHIFT          4

#define Z_MAX             1000
#define Z_HIT             40
#define Z_FLOOR_NEAR      28
#define Z_AIM             160
#define Z_LUT_MAX         1023
#define FLY_SPEED_X10     54
#define FLY_SPEED_AVG     ((FLY_SPEED_X10 + 5) / 10)
#define SPEED_PHASES      3

#define GRID_STEP         64
#define GRID_MASK         (GRID_STEP - 1)
#define LANE_STEP         64
#define LANE_COUNT        2
#define FLOOR_MIN_GAP     3
#define Z_LANE_FAR        480

#define PLAYER_X_LIMIT    90
#define PLAYER_Y_MIN      14
#define PLAYER_Y_MAX      100
#define PLAYER_START_Y    40
#define PLAYER_SPEED      2
#define PLAYER_RADIUS     6
#define CAM_LAG_SHIFT     2
#define MAX_HITS          3

#define MAX_OBSTACLES     10
#define MAX_BOXES_PER_TYPE 4

#define FILLED_OBSTACLES  1
#define GAP_UNIT          8
#define OBSTACLE_NONE     0xFF

#define RANDOM_LEVEL      1
#define RANDOM_LEVEL_LEN  40
#define GAP_START         48
#define GAP_END           30
#define GAP_JITTER        12
#define PAIR_CHANCE       48

#define PYRAMIDS_ON_TRACK 10
#define PYRAMIDS_REQUIRED 10
#define MAX_PICKUPS       3
#define PYR_AFTER_OBST    150
#define PYR_BEFORE_OBST   260
#define PYR_JITTER        12
#define PYR_COLLECT_R     16
#define PYR_SIZE_SHIFT    0
#define COLLECT_FX_FRAMES 10

#define PYRAMID_SPIN_AXIS_X 1

#define FINISH_GAP        70
#define FINISH_HALF_W     140
#define FINISH_Y1         0
#define FINISH_Y2         116

#define MAX_LEVELS        18
#define LEVELS_PER_PHASE  3

#define EXIT_CUBE_HALF    36
#define EXIT_CUBE_Y       52
#define EXIT_SPIN_SHIFT   2

#define HIT_FLASH_FRAMES  6
#define INTRO_FRAMES      90
#define HAND_W            43
#define HAND_H            27
#define HAND_Y            (HEIGHT - HAND_H + 2)
#define CROSS_SIZE        18
#define SHOW_HUD          1

#define TITLE_W           112
#define TITLE_H           8
#define TITLE_X           ((WIDTH - TITLE_W) / 2)
#define TITLE_Y           28
#define BG_WAVE_AMP       30
#define BG_WAVE_SPREAD    3
#define BG_SLANT          5
#define BG_XSTEP          2
#define BG_SCROLL_SHIFT   1

const uint16_t RECIP_LUT[256] PROGMEM = {
   8192,  2731,  1638,  1170,   910,   745,   630,   546,   482,   431,   390,   356,
    328,   303,   282,   264,   248,   234,   221,   210,   200,   191,   182,   174,
    167,   161,   155,   149,   144,   139,   134,   130,   126,   122,   119,   115,
    112,   109,   106,   104,   101,    99,    96,    94,    92,    90,    88,    86,
     84,    83,    81,    80,    78,    77,    75,    74,    72,    71,    70,    69,
     68,    67,    66,    65,    64,    63,    62,    61,    60,    59,    58,    57,
     56,    56,    55,    54,    54,    53,    52,    52,    51,    50,    50,    49,
     48,    48,    47,    47,    46,    46,    45,    45,    44,    44,    43,    43,
     42,    42,    42,    41,    41,    40,    40,    40,    39,    39,    38,    38,
     38,    37,    37,    37,    36,    36,    36,    35,    35,    35,    35,    34,
     34,    34,    33,    33,    33,    33,    32,    32,    32,    32,    31,    31,
     31,    31,    30,    30,    30,    30,    30,    29,    29,    29,    29,    29,
     28,    28,    28,    28,    28,    27,    27,    27,    27,    27,    27,    26,
     26,    26,    26,    26,    26,    25,    25,    25,    25,    25,    25,    24,
     24,    24,    24,    24,    24,    24,    23,    23,    23,    23,    23,    23,
     23,    23,    22,    22,    22,    22,    22,    22,    22,    22,    22,    21,
     21,    21,    21,    21,    21,    21,    21,    21,    20,    20,    20,    20,
     20,    20,    20,    20,    20,    20,    19,    19,    19,    19,    19,    19,
     19,    19,    19,    19,    19,    18,    18,    18,    18,    18,    18,    18,
     18,    18,    18,    18,    18,    18,    17,    17,    17,    17,    17,    17,
     17,    17,    17,    17,    17,    17,    17,    17,    16,    16,    16,    16,
     16,    16,    16,    16
};

const int8_t SINE_LUT[32] PROGMEM = {
  0, 0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 0,
  0, 0, -1, -1, -1, -2, -2, -2, -2, -2, -2, -2, -1, -1, -1, 0
};

const uint8_t BAYER4[16] PROGMEM = {
   0,  8,  2, 10,
  12,  4, 14,  6,
   3, 11,  1,  9,
  15,  7, 13,  5
};

const uint8_t PHASE_SPEED_X10[SPEED_PHASES] PROGMEM = { 60, 70, 80 };

const int8_t SIN127[32] PROGMEM = {
     0,   25,   49,   71,   90,  106,  117,  125,
   127,  125,  117,  106,   90,   71,   49,   25,
     0,  -25,  -49,  -71,  -90, -106, -117, -125,
  -127, -125, -117, -106,  -90,  -71,  -49,  -25
};

const uint8_t handLeft[] PROGMEM = {
  43, 27,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF0,
  0xE8, 0xF0, 0xF8, 0xD8, 0xF4, 0xF4, 0xD4, 0xD6, 0xFA, 0xDA, 0x6C, 0x6C,
  0x34, 0x14, 0x18, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  0xC0, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xF7, 0xC3, 0x81, 0x80, 0x80, 0xC0, 0xC0, 0xE0, 0x60, 0x60, 0x00,
  0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0xF8, 0xF8, 0xF8, 0xFC, 0xFC, 0xFC,
  0x7E, 0xBE, 0x5E, 0xBF, 0x5F, 0x2F, 0x17, 0x0B, 0x05, 0x07, 0x03, 0x03,
  0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x03, 0x03, 0x01, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07,
  0x07, 0x07, 0x07, 0x07, 0x03, 0x05, 0x02, 0x05, 0x02, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};
const uint8_t handLeftMask[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8,
  0xFC, 0xFC, 0xFC, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
  0x7E, 0x3E, 0x3E, 0x1E, 0x1C, 0x1C, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0xC0,
  0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xF7, 0xC3, 0xC1, 0xC0, 0xE0, 0xE0, 0xF0, 0xF0, 0xF0, 0x60,
  0x00, 0x00, 0xF8, 0xF8, 0xF8, 0xFC, 0xFC, 0xFC, 0xFE, 0xFE, 0xFE, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x07,
  0x07, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x07, 0x07, 0x03, 0x03,
  0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

const uint8_t handRight[] PROGMEM = {
  43, 27,
  0x00, 0x08, 0x08, 0x08, 0x18, 0x14, 0x34, 0x6C, 0x6C, 0xDA, 0xFA, 0xD6,
  0xD4, 0xF4, 0xF4, 0xD8, 0xF8, 0xF0, 0xE8, 0xF0, 0xE0, 0xC0, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60,
  0xE0, 0xC0, 0xC0, 0x80, 0x80, 0x81, 0xC3, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xC0, 0x80, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
  0x01, 0x03, 0x03, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x03,
  0x03, 0x07, 0x05, 0x0B, 0x17, 0x2F, 0x5F, 0xBF, 0x5E, 0xBE, 0x7E, 0xFC,
  0xFC, 0xFC, 0xF8, 0xF8, 0xF8, 0xF0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x01, 0x02, 0x05, 0x02, 0x05, 0x03, 0x07, 0x07, 0x07,
  0x07, 0x07, 0x07, 0x07
};
const uint8_t handRightMask[] PROGMEM = {
  0x1C, 0x1C, 0x1C, 0x1E, 0x3E, 0x3E, 0x7E, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFE, 0xFE, 0xFE, 0xFE, 0xFC, 0xFC, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xF0, 0xF0,
  0xF0, 0xE0, 0xE0, 0xC0, 0xC1, 0xC3, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xC0, 0x80, 0x80,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x03, 0x03,
  0x03, 0x07, 0x07, 0x07, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x07,
  0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFE, 0xFE, 0xFE, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF8, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  0x07, 0x07, 0x07, 0x07
};

const uint8_t crosshair[] PROGMEM = {
  18, 18,
  0x7F, 0x41, 0x7D, 0x05, 0x05, 0x05, 0x07, 0x00, 0x00, 0x00, 0x00, 0x07,
  0x05, 0x05, 0x05, 0x7D, 0x41, 0x7F, 0xF8, 0x08, 0xF8, 0x80, 0x80, 0x80,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0xF8, 0x08, 0xF8,
  0x03, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x03
};
const uint8_t crosshairMask[] PROGMEM = {
  0x7F, 0x7F, 0x7F, 0x07, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x07,
  0x07, 0x07, 0x07, 0x7F, 0x7F, 0x7F, 0xF8, 0xF8, 0xF8, 0x80, 0x80, 0x80,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0xF8, 0xF8, 0xF8,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03
};

const uint8_t titleText[] PROGMEM = {
  112, 8,
  0xFF, 0x81, 0x81, 0xBD, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xE7, 0x0D,
  0x19, 0xF1, 0x83, 0x86, 0xF3, 0x19, 0x0D, 0xFF, 0x81, 0x81, 0xB5, 0xB5,
  0xB5, 0xB5, 0xB5, 0x81, 0x89, 0xFF, 0x81, 0x81, 0xB5, 0xB5, 0xB5, 0xB5,
  0xB5, 0xB5, 0xFF, 0x81, 0x81, 0xED, 0x2D, 0x2D, 0x6D, 0xCD, 0x81, 0xA1,
  0xFF, 0x00, 0x00, 0xFF, 0x81, 0x81, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5,
  0xB5, 0xFF, 0xB1, 0xB1, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0x85, 0x85, 0xFF,
  0x81, 0x81, 0xBD, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xFF, 0x81, 0x81,
  0xED, 0x2D, 0x2D, 0x2D, 0xED, 0x81, 0x81, 0xFF, 0x81, 0x81, 0xED, 0x2D,
  0x2D, 0x2D, 0x2D, 0x2D, 0x21, 0xFF, 0x81, 0x81, 0xB5, 0xB5, 0xB5, 0xB5,
  0xB5, 0xB5, 0xB5, 0xFF
};
const uint8_t titleTextMask[] PROGMEM = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0x0F,
  0x1F, 0xFF, 0xFF, 0xFE, 0xFF, 0x1F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x3F, 0x7F, 0xFF, 0xFF, 0xFF,
  0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xFF, 0xFF, 0xFF,
  0xFF, 0x3F, 0x3F, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F,
  0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF
};

struct Box {
  int8_t  x1, y1, x2, y2;
  uint8_t depth;
};

struct ObstacleType {
  uint8_t first;
  uint8_t count;
};

enum ObstacleId : uint8_t {
  OB_PILLARS = 0,
  OB_GATE,
  OB_WALL_LEFT,
  OB_WALL_RIGHT,
  OB_LOW_BAR,
  OB_HIGH_BAR,
  OB_CUBE,
  OB_CROSS,
  OB_WINDOW,
  OB_TYPE_COUNT
};

const Box OBSTACLE_BOXES[] PROGMEM = {
  // OB_PILLARS (0..1)
  { -72,  0, -60, 110, 12 }, {  60,  0,  72, 110, 12 },
  // OB_GATE (2..4)
  {-120,  0, -40, 110, 16 }, {  40,  0, 120, 110, 16 }, { -40, 64,  40, 110, 16 },
  // OB_WALL_LEFT (5)
  {-120,  0,   0, 110, 24 },
  // OB_WALL_RIGHT (6)
  {   0,  0, 120, 110, 24 },
  // OB_LOW_BAR (7)
  {-120,  0, 120,  36, 16 },
  // OB_HIGH_BAR (8)
  {-120, 56, 120, 110, 16 },
  // OB_CUBE (9)
  { -24,  0,  24,  48, 48 },
  // OB_CROSS (10..11)
  { -56,  0,  56,  28, 32 }, { -16, 28,  16,  96, 32 },
  // OB_WINDOW (12..15)
  {-120,  0, 120,  30, 16 }, {-120, 80, 120, 110, 16 },
  {-120, 30, -30,  80, 16 }, {  30, 30, 120,  80, 16 },
};

const ObstacleType OBSTACLE_TYPES[OB_TYPE_COUNT] PROGMEM = {
  {  0, 2 },  // OB_PILLARS
  {  2, 3 },  // OB_GATE
  {  5, 1 },  // OB_WALL_LEFT
  {  6, 1 },  // OB_WALL_RIGHT
  {  7, 1 },  // OB_LOW_BAR
  {  8, 1 },  // OB_HIGH_BAR
  {  9, 1 },  // OB_CUBE
  { 10, 2 },  // OB_CROSS
  { 12, 4 },  // OB_WINDOW
};

const uint8_t OBSTACLE_X_JITTER[OB_TYPE_COUNT] PROGMEM = {
  20,  // OB_PILLARS
  40,  // OB_GATE
  40,  // OB_WALL_LEFT
  40,  // OB_WALL_RIGHT
   0,  // OB_LOW_BAR
   0,  // OB_HIGH_BAR
  64,  // OB_CUBE
  40,  // OB_CROSS
  40,  // OB_WINDOW
};

struct SafeSpot {
  int8_t x, y;
};

const SafeSpot OBSTACLE_SAFE_SPOT[OB_TYPE_COUNT] PROGMEM = {
  {   0, 70 },  // OB_PILLARS
  {   0, 30 },  // OB_GATE
  {  50, 50 },  // OB_WALL_LEFT
  { -50, 50 },  // OB_WALL_RIGHT
  {   0, 76 },  // OB_LOW_BAR
  {   0, 28 },  // OB_HIGH_BAR
  {   0, 76 },  // OB_CUBE
  {  52, 62 },  // OB_CROSS
  {   0, 55 },  // OB_WINDOW
};

struct LevelEvent {
  uint8_t gap;
  uint8_t type;
  int8_t  x;
};

const LevelEvent LEVEL_1[] PROGMEM = {
  { 60, OB_PILLARS,    0 },
  { 45, OB_CUBE,     -40 },
  { 30, OB_CUBE,      40 },
  { 50, OB_GATE,       0 },
  { 45, OB_WALL_LEFT,  0 },
  { 35, OB_WALL_RIGHT, 0 },
  { 50, OB_LOW_BAR,    0 },
  { 45, OB_HIGH_BAR,   0 },
  { 50, OB_CROSS,      0 },
  { 45, OB_PILLARS,    0 },
  {  0, OB_CUBE,       0 },
  { 50, OB_WINDOW,     0 },
  { 45, OB_WALL_RIGHT, 0 },
  { 30, OB_WALL_LEFT,  0 },
  { 45, OB_GATE,     -30 },
  { 35, OB_GATE,      30 },
  { 45, OB_HIGH_BAR,   0 },
  { 30, OB_LOW_BAR,    0 },
  { 45, OB_CUBE,     -60 },
  { 20, OB_CUBE,       0 },
  { 20, OB_CUBE,      60 },
  { 50, OB_WINDOW,    20 },
  { 40, OB_CROSS,    -40 },
  { 40, OB_PILLARS,  -20 },
  { 30, OB_PILLARS,   20 },
  { 45, OB_WALL_LEFT, 20 },
  { 30, OB_HIGH_BAR,   0 },
  { 40, OB_WINDOW,   -20 },
  { 40, OB_GATE,       0 },
  {  0, OB_CUBE,       0 },
};

const uint8_t LEVEL_1_LENGTH = sizeof(LEVEL_1) / sizeof(LEVEL_1[0]);

#if RANDOM_LEVEL
  #define LEVEL_LENGTH RANDOM_LEVEL_LEN
#else
  #define LEVEL_LENGTH LEVEL_1_LENGTH
#endif

#define PYR_SPACING (LEVEL_LENGTH / PYRAMIDS_ON_TRACK)
static_assert(LEVEL_LENGTH / PYRAMIDS_ON_TRACK >= 2,
              "Замало перешкод на рівні для такої кількості пірамідок");
static_assert(PYRAMIDS_REQUIRED <= PYRAMIDS_ON_TRACK,
              "PYRAMIDS_REQUIRED не може бути більшим за PYRAMIDS_ON_TRACK");

struct Vec3i8 {
  int8_t x, y, z;
};

const Vec3i8 PYRAMID_VERTS[4] PROGMEM = {
  {   0,  18,   0 },   // apex
  {   0,  -9,  18 },   // base
  {  15,  -9,  -9 },   // base
  { -15,  -9,  -9 },   // base
};

const uint8_t PYRAMID_FACES[4][4] PROGMEM = {
  { 0, 1, 2, 0 },   // side
  { 0, 2, 3, 1 },   // side
  { 0, 3, 1, 2 },   // side
  { 1, 3, 2, 1 },   // base
};

enum GameState : uint8_t {
  STATE_TITLE,
  STATE_PLAY,
  STATE_ENDING,
  STATE_CLEAR,
  STATE_CRASHED,
  STATE_MISSED
};

struct Obstacle {
  int16_t z;
  int8_t  x;
  uint8_t type;
};

struct Pickup {
  int16_t z;
  int8_t  x, y;
  bool    active;
};

Obstacle  obstacles[MAX_OBSTACLES];
Pickup    pickups[MAX_PICKUPS];

GameState state;
uint8_t   levelNumber;
int16_t   playerX, playerY;
int16_t   camX, camY;
uint16_t  travel;

int16_t   spawnCountdown;
uint8_t   levelIndex;
LevelEvent nextEvent;
uint16_t  rngState = 0xACE1;
uint8_t   lastType = OBSTACLE_NONE;
int8_t    lastX;

uint8_t   pyramidsPlaced;
uint8_t   nextPyramidAt;
int16_t   pyramidCountdown;
int8_t    pendingPyrX, pendingPyrY;
uint8_t   pyramidsCollected;

int16_t   finishCountdown;
int16_t   finishZ;

uint8_t   levelSpeedX10;
bool      paused;
bool      missedExit;
uint8_t   flyStep;
uint8_t   flyAcc;
uint8_t   hits;
uint8_t   hitFlash;
uint8_t   collectFx;
uint8_t   introTimer;
bool      screenInverted;

static inline uint16_t scaleAtZ(int16_t z) {
  if (z < 0) z = 0;
  if (z > Z_LUT_MAX) z = Z_LUT_MAX;
  return pgm_read_word(&RECIP_LUT[z >> 2]);
}

static inline int16_t projX(int16_t xFp, uint16_t s) {
  int32_t d = (int32_t)xFp - camX;
  return SCREEN_CX + (int16_t)((d * s) >> 12);
}

static inline int16_t projY(int16_t yFp, uint16_t s) {
  int32_t d = (int32_t)yFp - camY;
  return HORIZON_Y - (int16_t)((d * s) >> 12);
}

static inline bool isNightLevel() {
  return ((levelNumber / LEVELS_PER_PHASE) & 1) != 0;
}

static inline bool isFinalLevel() {
  return levelNumber == MAX_LEVELS - 1;
}

static void fillSpanV(int16_t x, int16_t y0, int16_t y1, uint8_t color) {
  if (x < 0 || x >= WIDTH) return;
  if (y0 > y1) { int16_t t = y0; y0 = y1; y1 = t; }
  if (y1 < 0 || y0 >= HEIGHT) return;
  if (y0 < 0) y0 = 0;
  if (y1 >= HEIGHT) y1 = HEIGHT - 1;

  const uint8_t pageFirst = y0 >> 3;
  const uint8_t pageLast  = y1 >> 3;
  uint8_t* col = arduboy.sBuffer + x;

  for (uint8_t p = pageFirst; p <= pageLast; ++p) {
    uint8_t mask = 0xFF;
    if (p == pageFirst) mask &= (uint8_t)(0xFF << (y0 & 7));
    if (p == pageLast)  mask &= (uint8_t)(0xFF >> (7 - (y1 & 7)));
    uint8_t* b = col + (uint16_t)p * WIDTH;
    if (color) *b |= mask;
    else       *b &= ~mask;
  }
}

static void fillSolidRect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color) {
  if (x0 > x1) { int16_t t = x0; x0 = x1; x1 = t; }
  if (x1 < 0 || x0 >= WIDTH) return;
  if (x0 < 0) x0 = 0;
  if (x1 >= WIDTH) x1 = WIDTH - 1;
  for (int16_t x = x0; x <= x1; ++x) fillSpanV(x, y0, y1, color);
}

static void fillTriangleSolid(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                              int16_t x2, int16_t y2, uint8_t color) {
  if ((y0 < 0 && y1 < 0 && y2 < 0) ||
      (y0 >= HEIGHT && y1 >= HEIGHT && y2 >= HEIGHT) ||
      (x0 < 0 && x1 < 0 && x2 < 0) ||
      (x0 >= WIDTH && x1 >= WIDTH && x2 >= WIDTH)) return;
  if (x0 > x1) { int16_t t = x0; x0 = x1; x1 = t; t = y0; y0 = y1; y1 = t; }
  if (x1 > x2) { int16_t t = x1; x1 = x2; x2 = t; t = y1; y1 = y2; y2 = t; }
  if (x0 > x1) { int16_t t = x0; x0 = x1; x1 = t; t = y0; y0 = y1; y1 = t; }
  if (x2 < 0 || x0 >= WIDTH || x0 == x2) return;

  const int32_t s02 = ((int32_t)(y2 - y0) << 16) / (x2 - x0);
  const int32_t s01 = (x1 != x0) ? ((int32_t)(y1 - y0) << 16) / (x1 - x0) : 0;
  const int32_t s12 = (x2 != x1) ? ((int32_t)(y2 - y1) << 16) / (x2 - x1) : 0;

  int16_t xs = (x0 < 0) ? 0 : x0;
  int16_t xe = (x1 > WIDTH) ? WIDTH : x1;
  int32_t ya = ((int32_t)y0 << 16) + s02 * (int32_t)(xs - x0);
  int32_t yb = ((int32_t)y0 << 16) + s01 * (int32_t)(xs - x0);
  for (int16_t x = xs; x < xe; ++x) {
    fillSpanV(x, (int16_t)(ya >> 16), (int16_t)(yb >> 16), color);
    ya += s02;
    yb += s01;
  }

  xs = (x1 < 0) ? 0 : x1;
  xe = (x2 >= WIDTH) ? WIDTH - 1 : x2;
  ya = ((int32_t)y0 << 16) + s02 * (int32_t)(xs - x0);
  yb = ((int32_t)y1 << 16) + s12 * (int32_t)(xs - x1);
  for (int16_t x = xs; x <= xe; ++x) {
    fillSpanV(x, (int16_t)(ya >> 16), (int16_t)(yb >> 16), color);
    ya += s02;
    yb += s12;
  }
}

static void fillQuadSolid(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                          int16_t x2, int16_t y2, int16_t x3, int16_t y3,
                          uint8_t color) {
  fillTriangleSolid(x0, y0, x1, y1, x2, y2, color);
  fillTriangleSolid(x0, y0, x2, y2, x3, y3, color);
}

static void hLine(int16_t x0, int16_t x1, int16_t y) {
  if (y < 0 || y >= HEIGHT) return;
  if (x0 > x1) { int16_t t = x0; x0 = x1; x1 = t; }
  if (x1 < 0 || x0 >= WIDTH) return;
  if (x0 < 0) x0 = 0;
  if (x1 >= WIDTH) x1 = WIDTH - 1;
  arduboy.drawFastHLine(x0, y, (uint8_t)(x1 - x0 + 1), FG_COLOR);
}

static void vLine(int16_t x, int16_t y0, int16_t y1) {
  fillSpanV(x, y0, y1, FG_COLOR);
}

static void rectOutline(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
  hLine(x0, x1, y0);
  hLine(x0, x1, y1);
  vLine(x0, y0, y1);
  vLine(x1, y0, y1);
}

static uint8_t outCode(int16_t x, int16_t y) {
  uint8_t c = 0;
  if (x < 0) c |= 1; else if (x >= WIDTH)  c |= 2;
  if (y < 0) c |= 4; else if (y >= HEIGHT) c |= 8;
  return c;
}

static void clippedLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
  uint8_t c0 = outCode(x0, y0);
  uint8_t c1 = outCode(x1, y1);

  for (uint8_t guard = 0; guard < 4; ++guard) {
    if (!(c0 | c1)) {
      arduboy.drawLine(x0, y0, x1, y1, FG_COLOR);
      return;
    }
    if (c0 & c1) return;

    uint8_t c  = c0 ? c0 : c1;
    int32_t dx = (int32_t)x1 - x0;
    int32_t dy = (int32_t)y1 - y0;
    int16_t x, y;

    if (c & 8)      { y = HEIGHT - 1; x = x0 + (int16_t)(dx * (y - y0) / dy); }
    else if (c & 4) { y = 0;          x = x0 + (int16_t)(dx * (0 - y0) / dy); }
    else if (c & 2) { x = WIDTH - 1;  y = y0 + (int16_t)(dy * (x - x0) / dx); }
    else            { x = 0;          y = y0 + (int16_t)(dy * (0 - x0) / dx); }

    if (c == c0) { x0 = x; y0 = y; c0 = outCode(x0, y0); }
    else         { x1 = x; y1 = y; c1 = outCode(x1, y1); }
  }
}

#define DITHER_FG_WHITE (FG_COLOR == WHITE)

static void fillDitherRect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t phase) {
  if (x0 > x1) { int16_t t = x0; x0 = x1; x1 = t; }
  if (y0 > y1) { int16_t t = y0; y0 = y1; y1 = t; }
  if (x1 < 0 || x0 >= WIDTH || y1 < 0 || y0 >= HEIGHT) return;
  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 >= WIDTH)  x1 = WIDTH - 1;
  if (y1 >= HEIGHT) y1 = HEIGHT - 1;

  const uint8_t pageFirst = y0 >> 3;
  const uint8_t pageLast  = y1 >> 3;
  uint8_t* buf = arduboy.sBuffer;

  for (uint8_t p = pageFirst; p <= pageLast; ++p) {
    uint8_t mask = 0xFF;
    if (p == pageFirst) mask &= (uint8_t)(0xFF << (y0 & 7));
    if (p == pageLast)  mask &= (uint8_t)(0xFF >> (7 - (y1 & 7)));

    uint8_t* row = buf + (uint16_t)p * WIDTH;
    for (int16_t x = x0; x <= x1; ++x) {
      uint8_t pat = ((x + phase) & 1) ? 0xAA : 0x55;
      if (!DITHER_FG_WHITE) pat = ~pat;
      row[x] = (row[x] & ~mask) | (pat & mask);
    }
  }
}

static inline bool ditherOn(uint8_t level, uint8_t x, uint8_t y) {
  switch (level) {
    case 0:  return ((x | y) & 1) == 0;
    case 1:  return ((x ^ y) & 1) == 0;
    default: return ((x & y) & 1) == 0;
  }
}

static void ditherSpan(int16_t y, int16_t x0, int16_t x1, uint8_t level) {
  if (y < 0 || y >= HEIGHT) return;
  if (x0 > x1) { int16_t t = x0; x0 = x1; x1 = t; }
  if (x1 < 0 || x0 >= WIDTH) return;
  if (x0 < 0) x0 = 0;
  if (x1 >= WIDTH) x1 = WIDTH - 1;

  uint8_t* row = arduboy.sBuffer + (uint16_t)(y >> 3) * WIDTH;
  const uint8_t bit = 1 << (y & 7);
  for (int16_t x = x0; x <= x1; ++x) {
    const bool fg = ditherOn(level, (uint8_t)x, (uint8_t)y);
    if (fg == DITHER_FG_WHITE) row[x] |= bit;
    else                       row[x] &= ~bit;
  }
}

static void fillTriangleDither(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                               int16_t x2, int16_t y2, uint8_t level) {
  if (y0 > y1) { int16_t t = y0; y0 = y1; y1 = t; t = x0; x0 = x1; x1 = t; }
  if (y1 > y2) { int16_t t = y1; y1 = y2; y2 = t; t = x1; x1 = x2; x2 = t; }
  if (y0 > y1) { int16_t t = y0; y0 = y1; y1 = t; t = x0; x0 = x1; x1 = t; }
  if (y2 < 0 || y0 >= HEIGHT || y0 == y2) return;

  const int32_t slope02 = ((int32_t)(x2 - x0) << 16) / (y2 - y0);
  const int32_t slope01 = (y1 != y0) ? ((int32_t)(x1 - x0) << 16) / (y1 - y0) : 0;
  const int32_t slope12 = (y2 != y1) ? ((int32_t)(x2 - x1) << 16) / (y2 - y1) : 0;

  int32_t xa = (int32_t)x0 << 16;
  int32_t xb = (int32_t)x0 << 16;
  for (int16_t y = y0; y < y1; ++y) {
    ditherSpan(y, (int16_t)(xa >> 16), (int16_t)(xb >> 16), level);
    xa += slope02;
    xb += slope01;
  }
  xb = (int32_t)x1 << 16;
  for (int16_t y = y1; y <= y2; ++y) {
    ditherSpan(y, (int16_t)(xa >> 16), (int16_t)(xb >> 16), level);
    xa += slope02;
    xb += slope12;
  }
}

static void drawTitleBackground() {
  uint8_t rowOffset[HEIGHT];
  const uint8_t scroll = (uint8_t)(arduboy.frameCount >> BG_SCROLL_SHIFT);
  for (uint8_t y = 0; y < HEIGHT; ++y) {
    const int8_t wave = (int8_t)pgm_read_byte(&SIN127[(y + (scroll >> 1)) & 31]);
    rowOffset[y] = (uint8_t)(y * BG_SLANT + (((int16_t)wave * BG_WAVE_AMP) >> 7)
                             + ((uint8_t)(y >> BG_WAVE_SPREAD) << 2) + scroll);
  }

  uint8_t* buf = arduboy.sBuffer;
  for (uint8_t page = 0; page < HEIGHT / 8; ++page) {
    const uint8_t yBase = page * 8;
    for (uint8_t x = 0; x < WIDTH; ++x) {
      uint8_t column = 0;
      for (uint8_t bit = 0; bit < 8; ++bit) {
        const uint8_t y = yBase + bit;
        const uint8_t phase = (uint8_t)(x * BG_XSTEP + rowOffset[y]);
        const uint8_t tri = (phase < 128) ? phase : (uint8_t)(255 - phase);
        const uint8_t level = tri >> 3;
        if (level > pgm_read_byte(&BAYER4[((y & 3) << 2) | (x & 3)])) column |= (1 << bit);
      }
      buf[(uint16_t)page * WIDTH + x] = column;
    }
  }
}

static uint16_t rngNext() {
  rngState ^= rngState << 7;
  rngState ^= rngState >> 9;
  rngState ^= rngState << 8;
  return rngState;
}

static uint8_t rngRange(uint8_t n) {
  return (uint8_t)(((uint16_t)(rngNext() >> 8) * n) >> 8);
}

static int8_t rngSigned(uint8_t j) {
  return (int8_t)((int16_t)rngRange(j * 2 + 1) - j);
}

#if RANDOM_LEVEL
static void generateEvent(uint8_t index, LevelEvent& e) {
  const uint8_t base = GAP_START - (uint8_t)(((uint16_t)(GAP_START - GAP_END) * index) / RANDOM_LEVEL_LEN);
  e.gap = (index == 0 ? 60 : base) + rngRange(GAP_JITTER);

  if (lastType == OB_PILLARS && rngNext() < ((uint16_t)PAIR_CHANCE << 8)) {
    e.gap  = 0;
    e.type = OB_CUBE;
    e.x    = lastX;
    lastType = OB_CUBE;
    return;
  }

  uint8_t type;
  do {
    type = rngRange(OB_TYPE_COUNT);
  } while (type == lastType);

  const uint8_t jitter = pgm_read_byte(&OBSTACLE_X_JITTER[type]);
  e.type = type;
  e.x    = jitter ? rngSigned(jitter) : 0;
  lastType = type;
  lastX    = e.x;
}
#endif

static void loadEvent(uint8_t index, LevelEvent& e) {
#if RANDOM_LEVEL
  generateEvent(index, e);
#else
  memcpy_P(&e, &LEVEL_1[index], sizeof(e));
#endif
}

static uint8_t pickPyramidSlot(uint8_t k) {
  return k * PYR_SPACING + 1 + rngRange(PYR_SPACING - 1);
}

static void startLevel() {
  for (uint8_t i = 0; i < MAX_OBSTACLES; ++i) obstacles[i].type = OBSTACLE_NONE;
  for (uint8_t i = 0; i < MAX_PICKUPS; ++i)   pickups[i].active = false;

  playerX = 0;
  playerY = PLAYER_START_Y << FP_SHIFT;
  camX    = playerX;
  camY    = playerY;
  travel  = 0;
  flyAcc  = 0;
  flyStep = FLY_SPEED_AVG;

  uint8_t phase = levelNumber / (LEVELS_PER_PHASE * 2);
  if (phase >= SPEED_PHASES) phase = SPEED_PHASES - 1;
  levelSpeedX10 = pgm_read_byte(&PHASE_SPEED_X10[phase]);
  hits    = 0;
  paused  = false;
  missedExit = false;
  hitFlash  = 0;
  collectFx = 0;
  introTimer = INTRO_FRAMES;
  levelIndex = 0;

  rngState ^= (uint16_t)arduboy.generateRandomSeed() ^ (uint16_t)micros();
  if (rngState == 0) rngState = 0xACE1;
  lastType = OBSTACLE_NONE;

  pyramidsPlaced    = 0;
  pyramidsCollected = 0;
  pyramidCountdown  = 0;
  nextPyramidAt     = pickPyramidSlot(0);

  finishCountdown = 0;
  finishZ         = 0;

  loadEvent(0, nextEvent);
  spawnCountdown = (int16_t)nextEvent.gap * GAP_UNIT;

  state = STATE_PLAY;
}

static void updatePlayer() {
  const int16_t step = PLAYER_SPEED << FP_SHIFT;

  if (arduboy.pressed(LEFT_BUTTON))  playerX -= step;
  if (arduboy.pressed(RIGHT_BUTTON)) playerX += step;
  if (arduboy.pressed(UP_BUTTON))    playerY += step;
  if (arduboy.pressed(DOWN_BUTTON))  playerY -= step;

  const int16_t xLim = PLAYER_X_LIMIT << FP_SHIFT;
  const int16_t yMin = PLAYER_Y_MIN   << FP_SHIFT;
  const int16_t yMax = PLAYER_Y_MAX   << FP_SHIFT;
  if (playerX < -xLim) playerX = -xLim;
  if (playerX >  xLim) playerX =  xLim;
  if (playerY <  yMin) playerY =  yMin;
  if (playerY >  yMax) playerY =  yMax;

  camX += (playerX - camX) >> CAM_LAG_SHIFT;
  camY += (playerY - camY) >> CAM_LAG_SHIFT;
}

static void spawnObstacle(uint8_t type, int8_t x) {
  for (uint8_t i = 0; i < MAX_OBSTACLES; ++i) {
    if (obstacles[i].type == OBSTACLE_NONE) {
      obstacles[i].z    = Z_MAX;
      obstacles[i].x    = x;
      obstacles[i].type = type;
      return;
    }
  }
}

static void spawnPickup(int8_t x, int8_t y) {
  for (uint8_t i = 0; i < MAX_PICKUPS; ++i) {
    if (!pickups[i].active) {
      pickups[i].z = Z_MAX;
      pickups[i].x = x;
      pickups[i].y = y;
      pickups[i].active = true;
      return;
    }
  }
}

static int16_t clamp16(int16_t v, int16_t lo, int16_t hi) {
  return v < lo ? lo : (v > hi ? hi : v);
}

static void schedulePyramidAfter(const LevelEvent& e) {
  SafeSpot s;
  memcpy_P(&s, &OBSTACLE_SAFE_SPOT[e.type], sizeof(s));

  const int16_t x = (int16_t)e.x + s.x + rngSigned(PYR_JITTER);
  const int16_t y = (int16_t)s.y + rngSigned(PYR_JITTER);
  pendingPyrX = (int8_t)clamp16(x, -(PLAYER_X_LIMIT - 10), PLAYER_X_LIMIT - 10);
  pendingPyrY = (int8_t)clamp16(y, PLAYER_Y_MIN + 6, PLAYER_Y_MAX - 6);
  pyramidCountdown = PYR_AFTER_OBST;

  ++pyramidsPlaced;
  if (pyramidsPlaced < PYRAMIDS_ON_TRACK) nextPyramidAt = pickPyramidSlot(pyramidsPlaced);
}

static void updateSpawner() {
  if (levelIndex < LEVEL_LENGTH) {
    spawnCountdown -= flyStep;

    while (spawnCountdown <= 0 && levelIndex < LEVEL_LENGTH) {
      spawnObstacle(nextEvent.type, nextEvent.x);

      const bool withPyramid = (pyramidsPlaced < PYRAMIDS_ON_TRACK &&
                                levelIndex == nextPyramidAt);
      if (withPyramid) schedulePyramidAfter(nextEvent);

      ++levelIndex;

      if (levelIndex < LEVEL_LENGTH) {
        loadEvent(levelIndex, nextEvent);
        if (withPyramid) {
          const uint8_t minGap = (PYR_AFTER_OBST + PYR_BEFORE_OBST + GAP_UNIT - 1) / GAP_UNIT;
          if (nextEvent.gap < minGap) nextEvent.gap = minGap;
        }
        spawnCountdown += (int16_t)nextEvent.gap * GAP_UNIT;
      } else {
        finishCountdown = (int16_t)FINISH_GAP * GAP_UNIT;
      }
    }
  }

  if (pyramidCountdown > 0) {
    pyramidCountdown -= flyStep;
    if (pyramidCountdown <= 0) {
      pyramidCountdown = 0;
      spawnPickup(pendingPyrX, pendingPyrY);
    }
  }

  if (finishCountdown > 0) {
    finishCountdown -= flyStep;
    if (finishCountdown <= 0) {
      finishCountdown = 0;
      finishZ = Z_MAX;
    }
  }
}

static bool checkCollision(const Obstacle& o) {
  ObstacleType t;
  memcpy_P(&t, &OBSTACLE_TYPES[o.type], sizeof(t));

  const int16_t px = playerX >> FP_SHIFT;
  const int16_t py = playerY >> FP_SHIFT;

  for (uint8_t i = 0; i < t.count; ++i) {
    Box b;
    memcpy_P(&b, &OBSTACLE_BOXES[t.first + i], sizeof(b));
    const int16_t x1 = (int16_t)o.x + b.x1;
    const int16_t x2 = (int16_t)o.x + b.x2;

    if (px + PLAYER_RADIUS > x1 && px - PLAYER_RADIUS < x2 &&
        py + PLAYER_RADIUS > b.y1 && py - PLAYER_RADIUS < b.y2) {
      return true;
    }
  }
  return false;
}

static void updateObstacles() {
  for (uint8_t i = 0; i < MAX_OBSTACLES; ++i) {
    Obstacle& o = obstacles[i];
    if (o.type == OBSTACLE_NONE) continue;

    o.z -= flyStep;

    if (o.z < Z_HIT) {
      if (checkCollision(o)) {
        ++hits;
        hitFlash = HIT_FLASH_FRAMES;
      }
      o.type = OBSTACLE_NONE;
    }
  }
}

static void updatePickups() {
  const int16_t px = playerX >> FP_SHIFT;
  const int16_t py = playerY >> FP_SHIFT;

  for (uint8_t i = 0; i < MAX_PICKUPS; ++i) {
    Pickup& p = pickups[i];
    if (!p.active) continue;

    p.z -= flyStep;

    if (p.z < Z_HIT) {
      const int16_t dx = px - p.x;
      const int16_t dy = py - p.y;
      if (dx > -PYR_COLLECT_R && dx < PYR_COLLECT_R &&
          dy > -PYR_COLLECT_R && dy < PYR_COLLECT_R) {
        ++pyramidsCollected;
        collectFx = COLLECT_FX_FRAMES;
      }
      p.active = false;
    }
  }
}

static void updateGame() {
  flyAcc += levelSpeedX10;
  flyStep = flyAcc / 10;
  flyAcc -= flyStep * 10;

  travel += flyStep;
  updatePlayer();
  updateSpawner();
  updateObstacles();
  updatePickups();

  if (finishZ > 0) finishZ -= flyStep;

  if (hitFlash)   --hitFlash;
  if (collectFx)  --collectFx;
  if (introTimer) --introTimer;

  if (hits >= MAX_HITS) {
    hitFlash = 0;
    state = STATE_CRASHED;
    return;
  }

  if (finishZ > 0 && finishZ < Z_HIT) {
    finishZ = 0;
    hitFlash = 0;

    bool reached = true;
    if (isFinalLevel()) {
      const int16_t px = playerX >> FP_SHIFT;
      const int16_t py = (playerY >> FP_SHIFT) - EXIT_CUBE_Y;
      reached = (px > -EXIT_CUBE_HALF && px < EXIT_CUBE_HALF &&
                 py > -EXIT_CUBE_HALF && py < EXIT_CUBE_HALF);
    }

    missedExit = !reached;
    if (!reached || pyramidsCollected < PYRAMIDS_REQUIRED) state = STATE_MISSED;
    else state = isFinalLevel() ? STATE_ENDING : STATE_CLEAR;
  }
}

static void drawFloor() {
  hLine(0, WIDTH - 1, HORIZON_Y);

  const int16_t floorFp = 0;
  const uint8_t phase   = travel & GRID_MASK;

  int16_t prevY = HEIGHT + FLOOR_MIN_GAP;
  for (int16_t z = GRID_STEP - phase; z < Z_MAX; z += GRID_STEP) {
    if (z < Z_FLOOR_NEAR) continue;
    const int16_t y = projY(floorFp, scaleAtZ(z));
    if (y >= HEIGHT) continue;
    if (prevY - y < FLOOR_MIN_GAP || y - HORIZON_Y < FLOOR_MIN_GAP) break;
    hLine(0, WIDTH - 1, y);
    prevY = y;
  }

  const uint16_t sNear = scaleAtZ(Z_FLOOR_NEAR);
  const uint16_t sFar  = scaleAtZ(Z_LANE_FAR);
  const int16_t  yNear = projY(floorFp, sNear);
  const int16_t  yFar  = projY(floorFp, sFar);

  for (int8_t i = -LANE_COUNT; i <= LANE_COUNT; ++i) {
    const int16_t xFp = (int16_t)(i * LANE_STEP) << FP_SHIFT;
    clippedLine(projX(xFp, sNear), yNear, projX(xFp, sFar), yFar);
  }
}

struct BoxScreen {
  int16_t fx1, fx2, fyT, fyB;
  int16_t bx1, bx2, byT, byB;
  uint8_t vis;
  bool    hasDepth;
  bool    offscreen;
};

static void drawObstacle(const Obstacle& o) {
  ObstacleType t;
  memcpy_P(&t, &OBSTACLE_TYPES[o.type], sizeof(t));
  if (t.count > MAX_BOXES_PER_TYPE) t.count = MAX_BOXES_PER_TYPE;

  const uint16_t sFront = scaleAtZ(o.z);
  const int16_t camXw = camX >> FP_SHIFT;
  const int16_t camYw = camY >> FP_SHIFT;

  BoxScreen bs[MAX_BOXES_PER_TYPE];

  for (uint8_t i = 0; i < t.count; ++i) {
    Box b;
    memcpy_P(&b, &OBSTACLE_BOXES[t.first + i], sizeof(b));
    BoxScreen& s = bs[i];

    const int16_t x1Fp = ((int16_t)o.x + b.x1) << FP_SHIFT;
    const int16_t x2Fp = ((int16_t)o.x + b.x2) << FP_SHIFT;
    const int16_t yBFp = (int16_t)b.y1 << FP_SHIFT;
    const int16_t yTFp = (int16_t)b.y2 << FP_SHIFT;

    s.fx1 = projX(x1Fp, sFront);
    s.fx2 = projX(x2Fp, sFront);
    s.fyT = projY(yTFp, sFront);
    s.fyB = projY(yBFp, sFront);

    s.vis = 0;
    s.offscreen = false;
    s.hasDepth = (b.depth != 0);
    if (s.hasDepth) {
      if (camXw < (int16_t)o.x + b.x1) s.vis |= 1;
      if (camXw > (int16_t)o.x + b.x2) s.vis |= 2;
      if (camYw > b.y2)                s.vis |= 4;
      if (camYw < b.y1)                s.vis |= 8;

      const uint16_t sBack = scaleAtZ(o.z + b.depth);
      s.bx1 = projX(x1Fp, sBack);
      s.bx2 = projX(x2Fp, sBack);
      s.byT = projY(yTFp, sBack);
      s.byB = projY(yBFp, sBack);
    }

    int16_t minX = s.fx1, maxX = s.fx2, minY = s.fyT, maxY = s.fyB;
    if (s.hasDepth) {
      if (s.bx1 < minX) minX = s.bx1;
      if (s.bx2 > maxX) maxX = s.bx2;
      if (s.byT < minY) minY = s.byT;
      if (s.byB > maxY) maxY = s.byB;
    }
    if (maxX < 0 || minX >= WIDTH || maxY < 0 || minY >= HEIGHT) {
      s.offscreen = true;
      continue;
    }

#if FILLED_OBSTACLES
    if (s.vis & 1) fillQuadSolid(s.fx1, s.fyT, s.bx1, s.byT, s.bx1, s.byB, s.fx1, s.fyB, BG_COLOR);
    if (s.vis & 2) fillQuadSolid(s.fx2, s.fyT, s.bx2, s.byT, s.bx2, s.byB, s.fx2, s.fyB, BG_COLOR);
    if (s.vis & 4) fillQuadSolid(s.fx1, s.fyT, s.fx2, s.fyT, s.bx2, s.byT, s.bx1, s.byT, BG_COLOR);
    if (s.vis & 8) fillQuadSolid(s.fx1, s.fyB, s.fx2, s.fyB, s.bx2, s.byB, s.bx1, s.byB, BG_COLOR);
    fillSolidRect(s.fx1, s.fyT, s.fx2, s.fyB, BG_COLOR);
#endif
  }

  for (uint8_t i = 0; i < t.count; ++i) {
    const BoxScreen& s = bs[i];
    if (s.offscreen) continue;
    rectOutline(s.fx1, s.fyT, s.fx2, s.fyB);
    if (!s.hasDepth || s.vis == 0) continue;

    if (s.vis & 1) vLine(s.bx1, s.byT, s.byB);
    if (s.vis & 2) vLine(s.bx2, s.byT, s.byB);
    if (s.vis & 4) hLine(s.bx1, s.bx2, s.byT);
    if (s.vis & 8) hLine(s.bx1, s.bx2, s.byB);

    if (s.vis & (1 | 4)) clippedLine(s.fx1, s.fyT, s.bx1, s.byT);
    if (s.vis & (2 | 4)) clippedLine(s.fx2, s.fyT, s.bx2, s.byT);
    if (s.vis & (1 | 8)) clippedLine(s.fx1, s.fyB, s.bx1, s.byB);
    if (s.vis & (2 | 8)) clippedLine(s.fx2, s.fyB, s.bx2, s.byB);
  }
}

static void drawPickup(const Pickup& p, uint8_t slot) {
  const uint8_t a = (uint8_t)((arduboy.frameCount >> 2) + slot * 11) & 31;
  const int8_t  sn = (int8_t)pgm_read_byte(&SIN127[a]);
  const int8_t  cs = (int8_t)pgm_read_byte(&SIN127[(a + 8) & 31]);

  int16_t sx[4], sy[4];
  for (uint8_t i = 0; i < 4; ++i) {
    Vec3i8 v;
    memcpy_P(&v, &PYRAMID_VERTS[i], sizeof(v));

    int16_t rx, ry, rz;
#if PYRAMID_SPIN_AXIS_X
    rx = v.x;
    ry = ((int16_t)v.y * cs - (int16_t)v.z * sn) >> 7;
    rz = ((int16_t)v.y * sn + (int16_t)v.z * cs) >> 7;
#else
    rx = ((int16_t)v.x * cs + (int16_t)v.z * sn) >> 7;
    ry = v.y;
    rz = ((int16_t)v.z * cs - (int16_t)v.x * sn) >> 7;
#endif
    rx >>= PYR_SIZE_SHIFT;
    ry >>= PYR_SIZE_SHIFT;
    rz >>= PYR_SIZE_SHIFT;

    const uint16_t s = scaleAtZ(p.z + rz);
    sx[i] = projX(((int16_t)p.x + rx) << FP_SHIFT, s);
    sy[i] = projY(((int16_t)p.y + ry) << FP_SHIFT, s);
  }

  int16_t minX = sx[0], maxX = sx[0], minY = sy[0], maxY = sy[0];
  for (uint8_t i = 1; i < 4; ++i) {
    if (sx[i] < minX) minX = sx[i];
    if (sx[i] > maxX) maxX = sx[i];
    if (sy[i] < minY) minY = sy[i];
    if (sy[i] > maxY) maxY = sy[i];
  }
  if (maxX < 0 || minX >= WIDTH || maxY < 0 || minY >= HEIGHT) return;

  if (p.z > 700) {
    vLine(sx[0], sy[0], sy[1]);
    return;
  }

  for (uint8_t f = 0; f < 4; ++f) {
    const uint8_t i0 = pgm_read_byte(&PYRAMID_FACES[f][0]);
    const uint8_t i1 = pgm_read_byte(&PYRAMID_FACES[f][1]);
    const uint8_t i2 = pgm_read_byte(&PYRAMID_FACES[f][2]);

    const int32_t cross = (int32_t)(sx[i1] - sx[i0]) * (sy[i2] - sy[i0]) -
                          (int32_t)(sy[i1] - sy[i0]) * (sx[i2] - sx[i0]);
    if (cross <= 0) continue;

    fillTriangleDither(sx[i0], sy[i0], sx[i1], sy[i1], sx[i2], sy[i2],
                       pgm_read_byte(&PYRAMID_FACES[f][3]));
    clippedLine(sx[i0], sy[i0], sx[i1], sy[i1]);
    clippedLine(sx[i1], sy[i1], sx[i2], sy[i2]);
    clippedLine(sx[i2], sy[i2], sx[i0], sy[i0]);
  }
}

const int8_t CUBE_VERTS[8][3] PROGMEM = {
  { -1, -1, -1 }, {  1, -1, -1 }, {  1,  1, -1 }, { -1,  1, -1 },
  { -1, -1,  1 }, {  1, -1,  1 }, {  1,  1,  1 }, { -1,  1,  1 },
};

const uint8_t CUBE_FACES[6][5] PROGMEM = {
  { 0, 3, 2, 1, 1 },   // back
  { 4, 5, 6, 7, 1 },   // front
  { 0, 4, 7, 3, 0 },   // left
  { 1, 2, 6, 5, 2 },   // right
  { 0, 1, 5, 4, 2 },   // bottom
  { 3, 7, 6, 2, 0 },   // top
};

static void drawExitCube() {
  const uint8_t a  = (uint8_t)(arduboy.frameCount >> EXIT_SPIN_SHIFT) & 31;
  const uint8_t a2 = (uint8_t)(a >> 1) & 31;
  const int8_t sn  = (int8_t)pgm_read_byte(&SIN127[a]);
  const int8_t cs  = (int8_t)pgm_read_byte(&SIN127[(a + 8) & 31]);
  const int8_t sn2 = (int8_t)pgm_read_byte(&SIN127[a2]);
  const int8_t cs2 = (int8_t)pgm_read_byte(&SIN127[(a2 + 8) & 31]);

  int16_t sx[8], sy[8];
  for (uint8_t i = 0; i < 8; ++i) {
    const int16_t vx = (int16_t)(int8_t)pgm_read_byte(&CUBE_VERTS[i][0]) * EXIT_CUBE_HALF;
    const int16_t vy = (int16_t)(int8_t)pgm_read_byte(&CUBE_VERTS[i][1]) * EXIT_CUBE_HALF;
    const int16_t vz = (int16_t)(int8_t)pgm_read_byte(&CUBE_VERTS[i][2]) * EXIT_CUBE_HALF;

    const int16_t rx = (vx * cs + vz * sn) >> 7;
    const int16_t rz = (vz * cs - vx * sn) >> 7;
    const int16_t ry2 = (vy * cs2 - rz * sn2) >> 7;
    const int16_t rz2 = (vy * sn2 + rz * cs2) >> 7;

    const uint16_t s = scaleAtZ(finishZ + rz2);
    sx[i] = projX(rx << FP_SHIFT, s);
    sy[i] = projY((EXIT_CUBE_Y + ry2) << FP_SHIFT, s);
  }

  for (uint8_t f = 0; f < 6; ++f) {
    uint8_t v[4];
    for (uint8_t k = 0; k < 4; ++k) v[k] = pgm_read_byte(&CUBE_FACES[f][k]);

    const int32_t cross = (int32_t)(sx[v[1]] - sx[v[0]]) * (sy[v[2]] - sy[v[0]]) -
                          (int32_t)(sy[v[1]] - sy[v[0]]) * (sx[v[2]] - sx[v[0]]);
    if (cross <= 0) continue;

    const uint8_t level = pgm_read_byte(&CUBE_FACES[f][4]);
    fillTriangleDither(sx[v[0]], sy[v[0]], sx[v[1]], sy[v[1]], sx[v[2]], sy[v[2]], level);
    fillTriangleDither(sx[v[0]], sy[v[0]], sx[v[2]], sy[v[2]], sx[v[3]], sy[v[3]], level);

    for (uint8_t k = 0; k < 4; ++k) {
      const uint8_t n = (k + 1) & 3;
      clippedLine(sx[v[k]], sy[v[k]], sx[v[n]], sy[v[n]]);
    }
  }
}

static void drawFinish() {
  if (isFinalLevel()) { drawExitCube(); return; }

  const uint16_t s = scaleAtZ(finishZ);
  const int16_t x1 = projX(-(FINISH_HALF_W << FP_SHIFT), s);
  const int16_t x2 = projX( FINISH_HALF_W << FP_SHIFT, s);
  const int16_t yT = projY(FINISH_Y2 << FP_SHIFT, s);
  const int16_t yB = projY(FINISH_Y1 << FP_SHIFT, s);

  fillDitherRect(x1, yT, x2, yB, (arduboy.frameCount >> 3) & 1);
  rectOutline(x1, yT, x2, yB);
}

struct DrawItem {
  int16_t z;
  uint8_t kind;
  uint8_t index;
};

static void drawWorldObjects() {
  DrawItem items[MAX_OBSTACLES + MAX_PICKUPS + 1];
  uint8_t n = 0;

  if (finishZ > 0) { items[n].z = finishZ; items[n].kind = 2; items[n].index = 0; ++n; }
  for (uint8_t i = 0; i < MAX_OBSTACLES; ++i) {
    if (obstacles[i].type == OBSTACLE_NONE) continue;
    items[n].z = obstacles[i].z; items[n].kind = 0; items[n].index = i; ++n;
  }
  for (uint8_t i = 0; i < MAX_PICKUPS; ++i) {
    if (!pickups[i].active) continue;
    items[n].z = pickups[i].z; items[n].kind = 1; items[n].index = i; ++n;
  }

  for (uint8_t i = 1; i < n; ++i) {
    DrawItem cur = items[i];
    int8_t j = i - 1;
    while (j >= 0 && items[j].z < cur.z) { items[j + 1] = items[j]; --j; }
    items[j + 1] = cur;
  }

  for (uint8_t i = 0; i < n; ++i) {
    switch (items[i].kind) {
      case 0:  drawObstacle(obstacles[items[i].index]); break;
      case 1:  drawPickup(pickups[items[i].index], items[i].index); break;
      default: drawFinish(); break;
    }
  }
}

static void drawHands() {
  const uint8_t t = (uint8_t)(arduboy.frameCount >> 1);
  const int8_t bobL = (int8_t)pgm_read_byte(&SINE_LUT[t & 31]);
  const int8_t bobR = (int8_t)pgm_read_byte(&SINE_LUT[(t + 8) & 31]);

  const int8_t swayX = (int8_t)((camX - playerX) >> (FP_SHIFT + 1));
  const int8_t swayY = (int8_t)((playerY - camY) >> (FP_SHIFT + 1));

  Sprites::drawExternalMask(swayX, HAND_Y + bobL + swayY,
                            handLeft, handLeftMask, 0, 0);
  Sprites::drawExternalMask(WIDTH - HAND_W + swayX, HAND_Y + bobR + swayY,
                            handRight, handRightMask, 0, 0);
}

static void drawCrosshair() {
  const uint16_t s = scaleAtZ(Z_AIM);
  int16_t cx = projX(playerX, s) - CROSS_SIZE / 2;
  int16_t cy = projY(playerY, s) - CROSS_SIZE / 2;

  if (cx < 0) cx = 0;
  if (cx > WIDTH - CROSS_SIZE)  cx = WIDTH - CROSS_SIZE;
  if (cy < 0) cy = 0;
  if (cy > HEIGHT - CROSS_SIZE) cy = HEIGHT - CROSS_SIZE;

  Sprites::drawExternalMask(cx, cy, crosshair, crosshairMask, 0, 0);

  if (collectFx) {
    const int16_t r = 10 + (COLLECT_FX_FRAMES - collectFx) * 2;
    const int16_t mx = cx + CROSS_SIZE / 2;
    const int16_t my = cy + CROSS_SIZE / 2;
    rectOutline(mx - r, my - r / 2, mx + r, my + r / 2);
  }
}

static void setTextColors() {
  arduboy.setTextColor(FG_COLOR);
  arduboy.setTextBackground(BG_COLOR);
}

static void print2(uint8_t v) {
  arduboy.print((char)('0' + (v / 10) % 10));
  arduboy.print((char)('0' + v % 10));
}

#if SHOW_HUD
static void drawHud() {
  for (uint8_t i = 0; i < MAX_HITS; ++i) {
    const int16_t x = 1 + i * 6;
    arduboy.fillRect(x - 1, 0, 7, 7, BG_COLOR);
    if (i < MAX_HITS - hits) arduboy.fillRect(x, 1, 5, 5, FG_COLOR);
    else                     arduboy.drawRect(x, 1, 5, 5, FG_COLOR);
  }

  const int16_t tx = WIDTH - 31;
  arduboy.fillRect(tx - 8, 0, 39, 8, BG_COLOR);
  arduboy.drawLine(tx - 7, 6, tx - 4, 0, FG_COLOR);
  arduboy.drawLine(tx - 4, 0, tx - 1, 6, FG_COLOR);
  arduboy.drawFastHLine(tx - 7, 6, 7, FG_COLOR);
  setTextColors();
  arduboy.setCursor(tx + 1, 0);
  print2(pyramidsCollected);
  arduboy.print('/');
  print2(PYRAMIDS_REQUIRED);
}
#endif

static void drawLevelTitle(int16_t y) {
  setTextColors();
  const int16_t w = isNightLevel() ? 14 * 6 : 12 * 6;
  arduboy.setCursor((WIDTH - w) / 2, y);
  arduboy.print(F("LEVEL "));
  print2(levelNumber + 1);
  arduboy.print(isNightLevel() ? F(" NIGHT") : F(" DAY"));
}

static void drawIntro() {
  if (introTimer == 0) return;
  if (introTimer < 30 && ((introTimer >> 2) & 1)) return;
  drawLevelTitle(12);
}

static void drawTitleScreen() {
  drawTitleBackground();
  Sprites::drawExternalMask(TITLE_X, TITLE_Y, titleText, titleTextMask, 0, 0);

  if ((arduboy.frameCount >> 4) & 1) {
    arduboy.fillRect(37, 48, 8 * 6 + 1, 9, BG_COLOR);
    setTextColors();
    arduboy.setCursor(38, 49);
    arduboy.print(F("PRESS A"));
  }
}

static void drawBannerLine(const __FlashStringHelper* s, uint8_t len, int16_t y) {
  const int16_t w = (int16_t)len * 6;
  const int16_t x = (WIDTH - w) / 2;
  arduboy.fillRect(x - 2, y - 2, (uint8_t)(w + 3), 11, BG_COLOR);
  arduboy.setCursor(x, y);
  arduboy.print(s);
}

static void drawEndingScreen() {
  drawTitleBackground();
  setTextColors();

  drawBannerLine(F("Congrats!"),      9, 12);
  drawBannerLine(F("You got out of"), 14, 23);
  drawBannerLine(F("cyberspace."),    11, 34);
  drawBannerLine(F("SKRTM 2026"),     10, 54);
}

static void drawPauseOverlay() {
  setTextColors();
  drawBannerLine(F("PAUSED"), 6, 22);
  drawBannerLine(F("A: RESUME"), 9, 34);
  drawBannerLine(F("B: RESTART LEVEL"), 16, 44);
}

static void drawEndScreen() {
  setTextColors();
  switch (state) {
    case STATE_CLEAR:
      arduboy.setCursor(31, 6);  arduboy.print(F("LEVEL CLEAR"));
      break;
    case STATE_CRASHED:
      arduboy.setCursor(40, 6);  arduboy.print(F("CRASHED!"));
      break;
    case STATE_TITLE:
    case STATE_ENDING:
      return;
    default:
      if (missedExit) { arduboy.setCursor(22, 6); arduboy.print(F("MISSED THE EXIT")); }
      else            { arduboy.setCursor(19, 6); arduboy.print(F("NOT ENOUGH KEYS")); }
      break;
  }

  drawLevelTitle(20);

  arduboy.setCursor(43, 34);
  arduboy.print(F("KEYS "));
  print2(pyramidsCollected);

  arduboy.setCursor(31 + 12, 44);
  arduboy.print(F("HITS "));
  arduboy.print((char)('0' + hits));

  arduboy.setCursor(state == STATE_CLEAR ? 40 : 37, 56);
  arduboy.print(state == STATE_CLEAR ? F("A: NEXT") : F("A: RETRY"));
}

#ifdef SHOW_DEBUG
static void drawDebug() {
  setTextColors();
  arduboy.setCursor(0, 8);
  arduboy.print(arduboy.cpuLoad());
  arduboy.print('%');
}
#endif

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(60);
  levelNumber = 0;
  startLevel();
  state = STATE_TITLE;
}

void loop() {
  if (!arduboy.nextFrame()) return;
  arduboy.pollButtons();

  if (state == STATE_PLAY) {
    if (arduboy.pressed(A_BUTTON) && arduboy.pressed(B_BUTTON)) {
      levelNumber = 0;
      startLevel();
      state = STATE_TITLE;
    } else {
      if (arduboy.justPressed(A_BUTTON)) paused = !paused;
      if (arduboy.justPressed(B_BUTTON)) startLevel();
      if (!paused) updateGame();
    }
  } else if (arduboy.justPressed(A_BUTTON)) {
    if (state == STATE_CLEAR) ++levelNumber;
    if (state == STATE_TITLE) levelNumber = 0;

    if (state == STATE_ENDING) {
      levelNumber = 0;
      startLevel();
      state = STATE_TITLE;
    } else {
      startLevel();
    }
  }

  const bool wantInvert = (state != STATE_TITLE && state != STATE_ENDING) &&
                          (isNightLevel() ^ (hitFlash > 0));
  if (wantInvert != screenInverted) {
    arduboy.invert(wantInvert);
    screenInverted = wantInvert;
  }

  if (state == STATE_TITLE || state == STATE_ENDING) {
    if (state == STATE_TITLE) drawTitleScreen();
    else                      drawEndingScreen();
    arduboy.display();
    return;
  }

  arduboy.fillScreen(BG_COLOR);

  if (state == STATE_PLAY) {
    drawFloor();
    drawWorldObjects();
    drawHands();
    drawCrosshair();
#if SHOW_HUD
    drawHud();
#endif
    if (paused) drawPauseOverlay();
    else        drawIntro();
#ifdef SHOW_DEBUG
    drawDebug();
#endif
  } else {
    drawEndScreen();
  }

  arduboy.display();
}
