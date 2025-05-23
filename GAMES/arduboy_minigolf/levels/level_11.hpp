#pragma once

static constexpr int8_t LEVEL_11_VERTS[165] PROGMEM =
{
     +20,   +0,  -80,  -20,   +0,  -80,   +2,   +0,  -60,   +4,   +0,  -62,
      -2,   +0,  -60,   -4,   +0,  -62,   +4,   +0,  -66,   +2,   +0,  -68,
      -4,   +0,  -66,   -2,   +0,  -68,  +20,  +20,  +80,  +20,   +0,  -60,
     -20,   +0,  -60,  +20,  +20,  +40,  -20,  +20,  +40,  -20,  +20,  +80,
     -40,   +0,  +40,  -40,   +0,  +80,  +40,   +0,  +80,  +40,   +0,  +40,
     -80,   +0,  +40,  +40,   +8,  +80,  -40,   +8,  +80,  +80,   +0,  +40,
     +20,   +0,  +20,  -20,   +0,  +20,  +20,   +0,  -20,  -20,   +0,  -20,
     -40,   +0,  +20,  -80,   +0,  +20,  +40,   +0,  +20,  +80,   +0,  +20,
     -80,   +8,  +40,  -40,   +0,  -20,  +40,   +0,  -20,  +80,   +8,  +20,
     -80,   +8,  +20,  +80,   +8,  +40,  +20,   +8,  -20,  -20,   +8,  -20,
     +20,   +0,  -20,  -20,   +0,  -20,  -40,   +0,  -20,  -20,   +8,  -60,
     -20,   +0,  -40,  +40,   +0,  -40,  +20,   +0,  -40,  -40,   +0,  -40,
     +20,   +8,  -60,  -40,   +8,  -40,  +40,   +8,  -40,  +20,   +8,  -80,
     -20,   +8,  -80,  -40,   +8,  -20,  +40,   +8,  -20, 
};

static constexpr uint8_t LEVEL_11_FACES[144] PROGMEM =
{
     23,  21,  18,  17,  32,  20,  34,  35,  31,  29,  53,  33,
     11,  50,  45,  47,  43,  12,  23,  37,  21,  17,  22,  32,
     34,  54,  35,  29,  36,  53,  11,  48,  50,  47,  49,  43,
     27,  38,  26,   1,  51,   0,  27,  39,  38,   1,  52,  51,
     15,  16,  14,  13,  18,  10,  14,  24,  13,  15,  17,  16,
     13,  19,  18,  14,  25,  24,   9,   0,   7,  14,  10,  15,
     20,  28,  17,  33,  31,  29,  27,  40,  41,  26,  45,  34,
     12,  46,  44,  27,  47,  12,  23,  30,  31,   5,   4,  12,
     12,   1,   8,   1,   0,   9,  11,   2,   3,  11,   3,   6,
      8,   5,  12,   9,   8,   1,   0,  11,   6,   0,   6,   7,
     14,  13,  10,  20,  29,  28,  33,  34,  31,  27,  26,  40,
     26,  11,  45,  12,  11,  46,  27,  33,  47,  23,  18,  30,

};

#define LEVEL_11_NUM_FACES { 0, 12, 4, 6, 26, }
