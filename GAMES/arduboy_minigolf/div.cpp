#include "game.hpp"

static constexpr uint16_t DIVISORS[256] PROGMEM =
{
    65535, 65535, 32767, 21845, 16383, 13107, 10922,  9362,
     8191,  7281,  6553,  5957,  5461,  5041,  4681,  4369,
     4095,  3855,  3640,  3449,  3276,  3120,  2978,  2849,
     2730,  2621,  2520,  2427,  2340,  2259,  2184,  2114,
     2047,  1985,  1927,  1872,  1820,  1771,  1724,  1680,
     1638,  1598,  1560,  1524,  1489,  1456,  1424,  1394,
     1365,  1337,  1310,  1285,  1260,  1236,  1213,  1191,
     1170,  1149,  1129,  1110,  1092,  1074,  1057,  1040,
     1023,  1008,   992,   978,   963,   949,   936,   923,
      910,   897,   885,   873,   862,   851,   840,   829,
      819,   809,   799,   789,   780,   771,   762,   753,
      744,   736,   728,   720,   712,   704,   697,   689,
      682,   675,   668,   661,   655,   648,   642,   636,
      630,   624,   618,   612,   606,   601,   595,   590,
      585,   579,   574,   569,   564,   560,   555,   550,
      546,   541,   537,   532,   528,   524,   520,   516,
      511,   508,   504,   500,   496,   492,   489,   485,
      481,   478,   474,   471,   468,   464,   461,   458,
      455,   451,   448,   445,   442,   439,   436,   434,
      431,   428,   425,   422,   420,   417,   414,   412,
      409,   407,   404,   402,   399,   397,   394,   392,
      390,   387,   385,   383,   381,   378,   376,   374,
      372,   370,   368,   366,   364,   362,   360,   358,
      356,   354,   352,   350,   348,   346,   344,   343,
      341,   339,   337,   336,   334,   332,   330,   329,
      327,   326,   324,   322,   321,   319,   318,   316,
      315,   313,   312,   310,   309,   307,   306,   304,
      303,   302,   300,   299,   297,   296,   295,   293,
      292,   291,   289,   288,   287,   286,   284,   283,
      282,   281,   280,   278,   277,   276,   275,   274,
      273,   271,   270,   269,   268,   267,   266,   265,
      264,   263,   262,   261,   260,   259,   258,   257,
};

uint16_t inv8(uint8_t x)
{
    return pgm_read_word(&DIVISORS[x]);
}

// for x >= 256: approximates 2^24 / x
// i.e., given x: 8.8, find y=1/x: 0.16
uint16_t inv16(uint16_t x)
{
    myassert(x >= 256);
    // initial guess
    uint16_t const* p = &DIVISORS[x >> 8];
    uint16_t y = pgm_read_word(p);
    {
        // refine initial guess by linear interpolation
        uint16_t ty = pgm_read_word(p + 1);
        uint8_t t1 = uint8_t(x);
        uint8_t t0 = 255 - t1;
        y = mul_f8_u16(y, t0) + mul_f8_u16(ty, t1) + (y >> 8);
    }
    // one iter of newton raphson to refine further
    for(uint8_t i = 0; i < 1; ++i)
    {
        uint24_t xy = u24((uint32_t(y) * x) >> 8);
        // 2 - x * y
        uint24_t t = uint24_t(0x20000) - xy;
        // y' = y * (2 - x * y)
        y = uint16_t((uint32_t(t) * y) >> 16);
    }
    return y;
}