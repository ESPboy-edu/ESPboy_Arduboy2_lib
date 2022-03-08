#pragma once

#include <stdint.h>

#ifndef CH2K_NAMESPACE
#define CH2K_NAMESPACE ch2k
#endif

#ifndef CH2K_TUNABLE_EVAL
#define CH2K_TUNABLE_EVAL 0
#endif

#ifndef CH2K_USE_STD_ARRAY
#define CH2K_USE_STD_ARRAY 0
#endif

#ifndef CH2K_MAX_MOVE_STACK
#define CH2K_MAX_MOVE_STACK 0
#endif

#if CH2K_USE_STD_ARRAY
#include <array>
#endif

// expects CH2K_RAND to be uint8_t(void)
#ifndef CH2K_RAND
#include <stdlib.h>
static uint8_t ch2k_rand__INTERNAL__() { return (uint8_t)rand(); }
#define CH2K_RAND ch2k_rand__INTERNAL__
#endif

#ifndef CH2K_PRINTF
#ifdef _MSC_VER
#include <stdio.h>
#define CH2K_PRINTF(...) printf(__VA_ARGS__)
#else
#define CH2K_PRINTF(...)
#endif
#endif

#ifndef CH2K_ASSERT
#if defined(_MSC_VER) && !defined(NDEBUG)
#define CH2K_ASSERT(cond__) do if(!(cond__)) __debugbreak(); while(0)
#else
#define CH2K_ASSERT(cond__) ((void)0)
#endif
#endif

namespace CH2K_NAMESPACE
{

#ifndef CH2K_ARDUINO
#undef PROGMEM
#define PROGMEM
static inline constexpr uint8_t pgm_read_byte(void const* x) { return *(uint8_t*)x; }
static inline constexpr uint16_t pgm_read_word(void const* x) { return *(uint16_t*)x; }
#endif

template<class T> T tmin(T a, T b)
{
    return a < b ? a : b;
}
template<class T> T tmax(T a, T b)
{
    return a < b ? b : a;
}
template<class T> T tabs(T a)
{
    return a < T(0) ? -a : a;
}

template<class T> void swap(T& a, T& b)
{
    T t = a;
    a = b;
    b = t;
}

using size_t = uint16_t;

using u8 = uint8_t;
using s8 = int8_t;

using u16 = uint16_t;
using s16 = int16_t;

// i know it's not uniform i don't care
static u8 nrand(u8 n)
{
    u8 r = CH2K_RAND();
    while(r >= n) r -= n; // avoids modulo
    return r;
}

// opening book encoding:
// tree of moves, each byte is a move
// bits 0-5 (values 0-63) are index of move from move generator
//     note this cannot support opening positions with >64 legal moves
// bit 7 indicates the move has children (immediately following)
// bit 6 indicates the last child move of its parent move
// tree is serialized depth-first (children immediately follow parents)

// parameters: MAX_DEPTH=6 MIN_COUNT=13 MIN_CHILDREN=1
static constexpr u8 const OPENING_BOOK_MAX_DEPTH = 6;
static u8 const OPENING_BOOK[1022] PROGMEM =
{
    145,   5, 199, 133,  81,  71, 146,  17,
    146,   2, 133, 130, 142,  85, 134,  71,
     73, 133, 141,  13,  69,  72,   4, 136,
    142,  24,  71,   6, 201,   0,   2,  71,
    202, 142,  21,  71, 201,  85, 135,   8,
     74, 136, 199,  69, 202, 131, 213,  85,
    130, 213,  85, 135, 213,   4,   7,  73,
    202, 194,  85, 130, 202, 211,  85, 133,
      2,   4, 133, 143,  13, 197,  68, 208,
     77,   9, 202, 144,  85,  75, 132, 197,
     70, 135, 130, 153,  84, 210,  85, 133,
    132,   2, 200,  81, 135, 195,  73, 200,
    202,  82,   6, 135, 145,  25, 197,   4,
     71,   5, 196,  69, 136, 210,  69, 202,
    153, 212,  17,   4,  71, 146, 213,  68,
    133,  85, 132, 149,  26,  80,  69, 203,
    213,  83, 134, 199, 216,  72, 136, 133,
      2,  71, 135, 209,  91,  74, 139, 199,
     81, 204, 133,  83,   7,  73, 130, 146,
     83,   5, 135, 211, 153,   9, 204,  91,
     18, 197,  71, 134, 211,  72, 201, 211,
    208, 199,  82, 131,   7, 201, 211, 215,
    212,  80, 133,  17, 146, 144, 133,  83,
    135, 196,  83, 137, 148,  78, 203,  88,
    136, 148,  24,  71, 134,  88, 200,   5,
     71, 202, 203,  85, 145,   4, 200,  73,
    134,  72, 203,   4, 201,  82, 130, 144,
    211,  72, 198,  83, 133, 143, 143, 202,
     74, 144,  71,  74, 144, 143,  77, 208,
    205,   5,  70, 202, 202, 209,  81, 132,
    145, 198, 136,  81,  74, 134, 198,  81,
    136,  70,  75, 134,  70, 137, 144, 151,
    143,  22,  65,  19, 202,  90, 144, 148,
     18,  72,   7, 203,  73, 145, 148,  78,
    203,  24,  87, 198,  84, 145, 208,  78,
      5, 203, 144,  82, 145, 210,  71,  68,
    136, 144,  23, 199, 199,  81, 145,  17,
    199,  74, 134, 199,  79, 203, 199, 211,
     18,  70,  11, 204, 144, 147,  75,  17,
    197, 202,  81,  17, 134, 211,  79, 136,
    147, 198,  70, 134,  15, 198,  72, 201,
    198,  80, 203, 211, 210,  69, 135, 145,
     69, 146, 152,  74, 153, 144, 153,   5,
     71,  29,  77, 133, 216,  75, 135, 150,
     70, 144,  69, 198,  69, 134, 215,  71,
    200, 145,  74,  72, 144, 199,  88, 146,
    130,  73, 133,   5, 135,   3,  70, 200,
     71, 135, 133,   4,  71,   6,  72, 134,
     73, 136, 154,  66, 155,   5,  74, 133,
     24,   2,  69,   7, 201,  67, 202, 154,
     85, 155,  84, 133,  85, 201,  85, 133,
    143, 209,  70, 130, 209,  71, 133, 145,
     68, 197,   3,   5,  71, 132, 209,  70,
    135, 207,  73, 134, 143,  15,  72,  81,
    137, 197,  15,  80, 136, 143,  24,  71,
    145,  24,   2,   5,   4,  71, 202,  24,
      5,  71, 202, 143,  21,  71, 145,  85,
    136,  72, 202,  21,   5,  68, 136, 198,
     79, 203,  69, 133, 199, 199, 201,  69,
    132, 146, 198, 154,  74,  69, 133,  70,
     72, 135, 151, 145, 198,  69, 197,  71,
    152, 144,  86,   4,  75, 143,  81, 145,
    145, 153,  69, 133,  68, 198,  69, 133,
    199,  81, 132,  89, 198,  69, 133, 152,
    209,  71, 144, 209,  89, 146, 209,  68,
    133,  68, 132, 143,  16,   5,  70, 145,
     16,  70, 132,  68, 198,  80, 134, 144,
     18,  64, 133,  69, 198,  16,  18,   5,
     72, 136, 198,  71, 199, 143,  23,  20,
     17,   0,   5,  68, 145,  17,   4,  70,
    196,  70, 198, 209, 197,  68, 134, 146,
    152,  69, 146, 197,  73,  75, 133, 200,
    144,  71, 210,  72, 200, 210, 207,  73,
    136, 146,  17,   5,   7,  74, 133, 151,
    128,  65, 131,  74,  66, 145, 207,  88,
    130,  79, 135,  79, 202, 202,  80, 136,
    199, 143,  87,  16,   7, 198,  70,  75,
    139, 153,  11,  76, 144, 145, 217,  71,
     71, 146, 209,  73, 133, 209,  15, 202,
     72, 136, 202, 206,  81, 203, 209, 211,
      6,   8,  75, 204, 146,  19,  81, 133,
    147,  79,  81, 200, 211,  79, 137, 145,
     16,  81, 146, 144, 135, 135,  82, 198,
     83,   6,  73, 200, 210,  16, 133,  82,
    135,  70, 203,  70, 130, 199, 211,  85,
    133,  15, 144, 144, 152,  80,  18,  20,
      8, 201,  75,   0, 133, 146,  81,  72,
    199,  18,  72, 147, 144,  80, 145, 208,
     69,  69, 145, 144, 152,  19,   4,   6,
     74, 142,   5,   8,   7,  75,   4,   6,
    199,  68, 145, 206,  69, 128,  68, 133,
    152,   0,   3,  66, 151,  80, 142,  17,
     64, 132,  81, 135,  18,  68,  73, 135,
    142,  15,  64, 130,  66, 132,  70, 133,
     78, 134,  79, 135,  68, 201,  79, 203,
    199,  83,   0, 130, 208,  92, 132, 145,
    199,  81, 130, 198,  84, 134, 199,  84,
    199,  15, 198,  71, 134,  80, 135, 196,
    196,  68, 138, 197,  80,  75, 132, 144,
    198, 211,  27,  69, 147, 198,  73, 145,
    198, 207,  91, 133, 198,  72, 134, 198,
    209,   8,  75, 199, 134, 143,  69, 144,
     69, 135,  23,  68, 134,  68, 200,  70,
     75, 135, 145,   4, 199,  80, 200, 145,
     71, 213, 143,  27,  19,  85,  80, 134,
     16,  17, 199, 146, 149,  72, 143,   4,
      8,  73, 200,  72, 203, 207,  83, 137,
    149, 149, 207,  78,  15, 208,  14, 197,
     13,  68, 143, 143, 200,  86, 208, 135,
     71, 200,  86, 144, 143, 151,  21,  13,
     16,  18,   0,   3,   6,  71, 150,  22,
     81, 141,  23,  17,  72, 132,  17,  69,
    199,  70, 144, 141,  23,  77, 142,  70,
    199,  80, 134,  22, 199,  71,  73, 135,
    200,  16, 154,  79,  68, 201, 150, 208,
     69, 144,  80, 156, 192,  97, 135, 200,
     70, 200, 148,  17,  71, 207,  19,  18,
     16,   7,   6,  75, 136, 144, 199,  69,
    145,  71,  25, 134, 199, 209,  82, 199,
      2,   5, 199, 143,  23,  15,  17,  70,
    144,  20,  15,  17,   0,   5,  70, 135,
      2,  69, 198,  70, 204,  17,   5, 199,
    147, 143,   4,  70, 145,  70,  68, 198,
    143,  64, 145,  19,  81, 200,  81, 139,
    146,  81, 133,  81, 135,  81, 137, 202,
    198, 210,  70, 204, 145, 199, 203,  68,
     73, 204,  18, 135, 147,  82,  81, 137,
     19, 197, 196, 198,  72,  76,
};

static constexpr u8 const PVALS[] PROGMEM =
{
    0, 1, 1, 3, 3, 5, 9, 0,
};

#if CH2K_TUNABLE_EVAL
using score = double;
using u8score = double;
#else
using score = s16;
using u8score = u8;
#endif

static constexpr score const SCORE_LOSS = score(-32000);
static constexpr score const SCORE_WIN  = score(32000);
static constexpr score const SCORE_DRAW = score(0);

struct eval_settings
{
    score pawn[32];
    score knight[32];
    score sliders[3][32];
    score king[32];
    score connected_pawn[4];
    score knight_outpost[4];
    score rook_on_open_file[4];
    score rook_on_semi_open_file[4];
    score passed_pawn[4];
    score connected_passed_pawn[4];
    score sliders_align_to_king[3];
    score bishop_pair;
    score rook_pawn_bonus[16];
    score knight_pawn_bonus[16];
    score tempo;
    score knight_pair;
    score rook_pair;
    score king_vmob[13];
};

struct eval_piece_tables
{
    score t[6][32];
};

union eval_settings_union
{
    score data[sizeof(eval_settings) / sizeof(score)];
    eval_settings v;
    eval_piece_tables pt;
};

// 0.061181
static eval_settings_union const DEFAULT_EVAL_SETTINGS PROGMEM =
{
    {
          +0,   +0,   +0,   +0,
        +220, +219, +208, +207,
        +134, +143, +135, +133,
         +84,  +88,  +85,  +86,
         +66,  +71,  +72,  +79,
         +62,  +69,  +69,  +66,
         +65,  +78,  +76,  +58,
          +0,   +0,   +0,   +0,
        +190, +235, +249, +261,
        +245, +266, +288, +285,
        +258, +291, +303, +306,
        +273, +291, +304, +307,
        +264, +284, +294, +297,
        +253, +275, +282, +290,
        +250, +257, +271, +275,
        +234, +253, +251, +256,
        +268, +272, +268, +276,
        +273, +295, +292, +287,
        +284, +298, +302, +300,
        +287, +294, +305, +312,
        +283, +294, +301, +310,
        +285, +295, +304, +304,
        +277, +299, +292, +292,
        +267, +279, +272, +279,
        +480, +482, +489, +494,
        +481, +486, +494, +490,
        +470, +480, +485, +484,
        +467, +470, +485, +481,
        +457, +468, +475, +475,
        +449, +463, +468, +468,
        +442, +459, +468, +470,
        +455, +459, +473, +477,
        +868, +885, +905, +905,
        +880, +881, +910, +910,
        +892, +898, +911, +919,
        +886, +888, +905, +904,
        +880, +890, +895, +896,
        +878, +886, +894, +890,
        +856, +870, +893, +887,
        +863, +861, +868, +887,
         -34,   -8,  -14,  -21,
          +0,  +13,  +17,   +9,
          +3,  +25,  +24,   +9,
         -11,  +14,  +18,  +14,
         -21,   -2,  +11,  +12,
         -16,   +1,   +7,   +8,
         -16,   -1,   -1,   -3,
         -32,   -8,  -20,  -19,
         +12,  +17,  +13,  +14,
          +7,  +10,  +10,  +10,
         +27,  +22,  +14,  +15,
         +30,  +18,   +9,   +3,
         +26,  +23,  +13,   +7,
          +4,   -1,   +4,   +9,
         +15,   +6,   +7,  +53,
         -96,  -43,   -9,   -5,
          +7,   +5,  +13,  +10,
         +13,   +9,  +10,   +3,
          +0,   -1,   +1,   +1,
        -161,  -55,  -33,  -18,
         -11,   -7,   -2,   -2,
          +3,   +7,  +10,  +16,
         +20,  +32,  +30,  +43,
         +17,   +7,  -25,   +4,
         +25,  +19,  +13,   +7,
          +4,   +3,   -8,   -6,
         -13,  -14,  -14,  -18,
    }
};

template<int N> struct stack_str
{
    char x[N];
    constexpr operator char const*() const { return &x[0]; }
    constexpr char const* c_str() const { return &x[0]; }
    operator char*() { return x.data(); }
    constexpr char at(size_t n) const { return x[n]; }
    char& at(size_t n) { return x[n]; }
    void copy(size_t& n, char const* s)
    {
        char c;
        while((c = *s++) != '\0')
            x[n++] = c;
    }
};

static constexpr u8 const VECS[0x77 + 1 + 0x77] PROGMEM =
{
    239,   0,   0,   0,   0,   0,   0, 240,   0,   0,   0,   0,   0,   0, 241,   0,
      0, 239,   0,   0,   0,   0,   0, 240,   0,   0,   0,   0,   0, 241,   0,   0,
      0,   0, 239,   0,   0,   0,   0, 240,   0,   0,   0,   0, 241,   0,   0,   0,
      0,   0,   0, 239,   0,   0,   0, 240,   0,   0,   0, 241,   0,   0,   0,   0,
      0,   0,   0,   0, 239,   0,   0, 240,   0,   0, 241,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0, 239, 223, 240, 225, 241,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0, 238, 239, 240, 241, 242,   0,   0,   0,   0,   0,   0,
    255, 255, 255, 255, 255, 255, 255,   0,   1,   1,   1,   1,   1,   1,   1,   0,
      0,   0,   0,   0,   0,  14,  15,  16,  17,  18,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,  15,  31,  16,  33,  17,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,  15,   0,   0,  16,   0,   0,  17,   0,   0,   0,   0,   0,
      0,   0,   0,  15,   0,   0,   0,  16,   0,   0,   0,  17,   0,   0,   0,   0,
      0,   0,  15,   0,   0,   0,   0,  16,   0,   0,   0,   0,  17,   0,   0,   0,
      0,  15,   0,   0,   0,   0,   0,  16,   0,   0,   0,   0,   0,  17,   0,   0,
     15,   0,   0,   0,   0,   0,   0,  16,   0,   0,   0,   0,   0,   0,  17,
};

// square (position) delta
struct square_delta
{
    u8 x;
    constexpr square_delta reversed() const { return square_delta{ u8(-x) }; }

    // north, east, south, west
    static square_delta const NORTH, SOUTH, WEST, EAST;
    static square_delta const NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST;
    static square_delta const ZERO;
    constexpr bool is_zero() const { return x == 0; }

    static square_delta const KNIGHT_MOVES[8] PROGMEM;
    static square_delta const KING_MOVES[8] PROGMEM;
    static square_delta const ORTHOGONAL_DIRS[4] PROGMEM;
    static square_delta const DIAGONAL_DIRS[4];

    square_delta& operator+=(square_delta d) { x += d.x; return *this; }
    square_delta& operator-=(square_delta d) { x -= d.x; return *this; }
    square_delta& operator*=(s8 m) { x *= m; return *this; }
    constexpr square_delta operator+(square_delta d) const { return square_delta{ u8(x + d.x) }; }
    constexpr square_delta operator-(square_delta d) const { return square_delta{ u8(x - d.x) }; }
    constexpr square_delta operator*(s8 m) const { return square_delta{ u8(x * m) }; }
    constexpr square_delta operator-() const { return reversed(); }

    constexpr bool operator==(square_delta d) const { return x == d.x; }
    constexpr bool operator!=(square_delta d) const { return x != d.x; }

    constexpr bool is_vertical() const { return (x & 0x7) == 0; }

public:
    inline square_delta vec() const { return square_delta{ pgm_read_byte(&VECS[0x77 + (s8)x]) }; }
};

constexpr square_delta const square_delta::NORTH     = square_delta{ 240 };
constexpr square_delta const square_delta::SOUTH     = square_delta{ 16 };
constexpr square_delta const square_delta::WEST      = square_delta{ 255 };
constexpr square_delta const square_delta::EAST      = square_delta{ 1 };
constexpr square_delta const square_delta::NORTHWEST = square_delta::NORTH + square_delta::WEST;
constexpr square_delta const square_delta::NORTHEAST = square_delta::NORTH + square_delta::EAST;
constexpr square_delta const square_delta::SOUTHWEST = square_delta::SOUTH + square_delta::WEST;
constexpr square_delta const square_delta::SOUTHEAST = square_delta::SOUTH + square_delta::EAST;
constexpr square_delta const square_delta::ZERO      = square_delta{ 0 };

constexpr square_delta const square_delta::KNIGHT_MOVES[8] PROGMEM =
{
    square_delta::NORTH + square_delta::NORTHWEST,
    square_delta::NORTH + square_delta::NORTHEAST,
    square_delta::SOUTH + square_delta::SOUTHWEST,
    square_delta::SOUTH + square_delta::SOUTHEAST,
    square_delta::WEST + square_delta::NORTHWEST,
    square_delta::WEST + square_delta::SOUTHWEST,
    square_delta::EAST + square_delta::NORTHEAST,
    square_delta::EAST + square_delta::SOUTHEAST,
};

constexpr square_delta const square_delta::KING_MOVES[8] PROGMEM =
{
    square_delta::NORTHWEST, square_delta::NORTH, square_delta::NORTHEAST, square_delta::WEST,
    square_delta::EAST, square_delta::SOUTHWEST, square_delta::SOUTH, square_delta::SOUTHEAST,
};

constexpr square_delta const square_delta::ORTHOGONAL_DIRS[4] PROGMEM =
{
    square_delta::NORTH, square_delta::SOUTH, square_delta::WEST, square_delta::EAST,
};
constexpr square_delta const square_delta::DIAGONAL_DIRS[4] PROGMEM =
{
    square_delta::NORTHWEST, square_delta::NORTHEAST, square_delta::SOUTHWEST, square_delta::SOUTHEAST,
};

// square (position)
struct square
{
    u8 x;
    square& operator+=(square_delta d) { x += s8(d.x); return *this; }
    square& operator-=(square_delta d) { x -= s8(d.x); return *this; }
    constexpr square operator+(square_delta d) const { return square{ u8(x + s8(d.x)) }; }
    constexpr square operator-(square_delta d) const { return square{ u8(x - s8(d.x)) }; }
    constexpr square_delta operator-(square s) const { return square_delta{ u8(x - s.x) }; }
    constexpr bool operator==(square s) const { return x == s.x; }
    constexpr bool operator!=(square s) const { return x != s.x; }
    constexpr bool operator<(square s) const { return x < s.x; }
    constexpr bool operator>(square s) const { return x > s.x; }
    constexpr bool operator<=(square s) const { return x <= s.x; }
    constexpr bool operator>=(square s) const { return x >= s.x; }
    constexpr bool is_valid() const { return !(x & 0x88); }
    static constexpr square from_rowcol(u8 r, u8 c)
    {
        return square{ u8(r * 16 + c) };
    }
    constexpr u8 row() const { return x >> 4; }
    constexpr u8 col() const { return x & 7; }
    void clear_row() { x &= 0x0f; }
    void clear_col() { x &= 0xf0; }
    constexpr char row_char() const { return '8' - row(); }
    constexpr char col_char() const { return 'a' + col(); }
    constexpr stack_str<3> str() const
    {
        return stack_str<3>{ {col_char(), row_char(), '\0'} };
    }
    static square const NOWHERE;
    constexpr bool is_nowhere() const { return s8(x) < 0; }

    // index into piece-column table
    inline constexpr u8 table_col_index() const
    {
        return col() > 3 ? 7 - col() : col();
    }
    // index into piece-square table
    inline constexpr u8 table_index(bool flip) const
    {
        return ((flip ? 7 - row() : row()) << 2) + table_col_index();
    }

    // returns 0 for white, 1 for black
    constexpr u8 square_color() const
    {
        return ((x >> 4) ^ x) & 1;
    }
};

constexpr square const square::NOWHERE { 255 };

// piece color
struct piece_color
{
    enum : u8 { W_ = 32, B_ = 64, X_ = 96 };
    u8 x;
    static piece_color const W;
    static piece_color const B;

    //
    // the following methods assume not X
    //

    constexpr square_delta forward() const { return square_delta{ u8(x - 48) }; }
    constexpr piece_color opposite() const { return piece_color{ u8(x ^ (W_ | B_)) }; }
    constexpr u8 promotion_row() const { return u8(32 - x) >> 5; } // 0 or 7
    constexpr u8 pawn_base_row() const { return 6 ^ promotion_row(); } // 6 or 1
    constexpr u8 pawn_ep_row()   const { return 4 ^ promotion_row(); } // 4 or 3

    // returns 0 or 1
    constexpr u8 binary_index() const { return x >> 6; }
    constexpr bool is_black() const { return (x & B_) != 0; }
    constexpr bool is_white() const { return (x & W_) != 0; }

    constexpr bool operator==(piece_color c) const { return x == c.x; }
    constexpr bool operator!=(piece_color c) const { return x != c.x; }
};

constexpr piece_color const piece_color::W { piece_color::W_ };
constexpr piece_color const piece_color::B { piece_color::B_ };

static constexpr u8 const CASTLE_WQ = (1 << 0);
static constexpr u8 const CASTLE_BQ = (1 << 1);
static constexpr u8 const CASTLE_WK = (1 << 5);
static constexpr u8 const CASTLE_BK = (1 << 6);

static constexpr u8 castle_q(piece_color c)
{
    return c.x >> 5;
}
static constexpr u8 castle_k(piece_color c)
{
    return c.x;
}

static constexpr u8 const CASTLING_SPOILERS[] PROGMEM =
{
    uint8_t(~CASTLE_BQ), 0xff, 0xff, 0xff, uint8_t(~(CASTLE_BQ | CASTLE_BK)), 0xff, 0xff, uint8_t(~CASTLE_BK),
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    uint8_t(~CASTLE_WQ), 0xff, 0xff, 0xff, uint8_t(~(CASTLE_WQ | CASTLE_WK)), 0xff, 0xff, uint8_t(~CASTLE_WK),
};

static constexpr u8 const C_ORTH    = 0x01;
static constexpr u8 const C_DIAG    = 0x02;
static constexpr u8 const C_KNIGHT  = 0x04;
static constexpr u8 const C_SIDE    = 0x08;
static constexpr u8 const C_FORW    = 0x10;
static constexpr u8 const C_FDIAG   = 0x20;
static constexpr u8 const C_BACKW   = 0x40;
static constexpr u8 const C_BDIAG   = 0x80;
static constexpr u8 const C_KING    = (C_FDIAG | C_BDIAG | C_SIDE | C_FORW | C_BACKW);
static constexpr u8 const C_BISHOP  = (C_FDIAG | C_BDIAG | C_DIAG);
static constexpr u8 const C_ROOK    = (C_SIDE | C_FORW | C_BACKW | C_ORTH);
static constexpr u8 const C_WPAWN   = (C_FDIAG);
static constexpr u8 const C_BPAWN   = (C_BDIAG);
static constexpr u8 const C_QUEEN   = (C_BISHOP | C_ROOK);
static constexpr u8 const C_CONTACT = (C_KING | C_KNIGHT);
static constexpr u8 const C_DISTANT = (C_ORTH | C_DIAG);

static constexpr u8 const CAPTS[8] PROGMEM =
{
    0, C_WPAWN, C_BPAWN, C_KNIGHT, C_BISHOP, C_ROOK, C_QUEEN, C_KING
};

static constexpr u8 const DELTA_CAPTS[0x99 + 1 + 0x77] PROGMEM =
{
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   2,   0,   0,   0,   0,   0,   0,   1,   0,   0,   0,   0,   0,   0,
    2,   0,   0,   2,   0,   0,   0,   0,   0,   1,   0,   0,   0,   0,   0,   2,
    0,   0,   0,   0,   2,   0,   0,   0,   0,   1,   0,   0,   0,   0,   2,   0,
    0,   0,   0,   0,   0,   2,   0,   0,   0,   1,   0,   0,   0,   2,   0,   0,
    0,   0,   0,   0,   0,   0,   2,   0,   0,   1,   0,   0,   2,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   2,   4,   1,   4,   2,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   4,  32,  16,  32,   4,   0,   0,   0,   0,
    0,   0,   1,   1,   1,   1,   1,   1,   8,   0,   8,   1,   1,   1,   1,   1,
    1,   0,   0,   0,   0,   0,   0,   4, 128,  64, 128,   4,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   2,   4,   1,   4,   2,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   2,   0,   0,   1,   0,   0,   2,   0,   0,   0,
    0,   0,   0,   0,   0,   2,   0,   0,   0,   1,   0,   0,   0,   2,   0,   0,
    0,   0,   0,   0,   2,   0,   0,   0,   0,   1,   0,   0,   0,   0,   2,   0,
    0,   0,   0,   2,   0,   0,   0,   0,   0,   1,   0,   0,   0,   0,   0,   2,
    0,   0,   2,   0,   0,   0,   0,   0,   0,   1,   0,   0,   0,   0,   0,   0,
    2,
};

// piece type
struct piece_type
{
    enum : u8 { X_, WP_, BP_, N_, B_, R_, Q_, K_ };
    u8 x;

    static piece_type const WPAWN;
    static piece_type const BPAWN;
    static piece_type const KNIGHT;
    static piece_type const BISHOP;
    static piece_type const ROOK;
    static piece_type const QUEEN;
    static piece_type const KING;

    static constexpr piece_type pawn(piece_color c) { return piece_type{ u8(WP_ + c.binary_index()) }; }

    // assumes it is known to not be X (nothing)
    constexpr bool is_pawn() const { return x <= BP_; }

    constexpr bool is_pawn_of_color(piece_color c) { return x == (c.x >> 5); }

    // assumes it is known to be a slider
    constexpr bool is_orthogonal_slider() const { return (x & 3) != 0; }
    constexpr bool is_diagonal_slider() const { return (x & 1) == 0; }

    constexpr bool operator==(piece_type t) const { return x == t.x; }
    constexpr bool operator!=(piece_type t) const { return x != t.x; }

private:
    static constexpr char UCI[8] = { 'x', 'p', 'p', 'n', 'b', 'r', 'q', 'k' };
public:
    inline bool can_cap_delta(square_delta d) const
    {
        return (pgm_read_byte(&CAPTS[x]) & pgm_read_byte(&DELTA_CAPTS[0x99 + (s8)d.x])) != 0;
    }
    inline bool can_cap_delta_contact(square_delta d) const
    {
        return (pgm_read_byte(&CAPTS[x]) & pgm_read_byte(&DELTA_CAPTS[0x99 + (s8)d.x]) & C_CONTACT) != 0;
    }
    inline bool can_cap_delta_distant(square_delta d) const
    {
        return (pgm_read_byte(&CAPTS[x]) & pgm_read_byte(&DELTA_CAPTS[0x99 + (s8)d.x]) & C_DISTANT) != 0;
    }
    constexpr bool can_cap_orthogonal() const
    {
        return (CAPTS[x] & C_ORTH) != 0;
    }
    constexpr char uci_char() const { return UCI[x]; }
};

constexpr piece_type const piece_type::WPAWN  { piece_type::WP_ };
constexpr piece_type const piece_type::BPAWN  { piece_type::BP_ };
constexpr piece_type const piece_type::KNIGHT { piece_type::N_ };
constexpr piece_type const piece_type::BISHOP { piece_type::B_ };
constexpr piece_type const piece_type::ROOK   { piece_type::R_ };
constexpr piece_type const piece_type::QUEEN  { piece_type::Q_ };
constexpr piece_type const piece_type::KING   { piece_type::K_ };

// piece
struct piece
{
    u8 x;
    static constexpr u8 const MASK_T_ = 0x7;
    static constexpr u8 const MASK_C_ = piece_color::W_ | piece_color::B_ | piece_color::X_;
    static piece const NOTHING;
    static piece const GUARD;
    constexpr piece_type type() const { return piece_type{ u8(x & MASK_T_) }; }
    constexpr piece_color color() const { return piece_color{ u8(x & MASK_C_) }; }
    constexpr bool is_nothing() const { return x == 0; }
    constexpr bool is_pawn() const { return type().is_pawn(); }
    constexpr bool is_pawn_of_color(piece_color c) const { return type().is_pawn_of_color(c); }
    constexpr bool is_of_color(piece_color c) const { return color() == c; }
    static constexpr piece from_type_color(piece_type t, piece_color c) { return piece{ u8(t.x + c.x) }; }
    static constexpr piece pawn(piece_color c) { return from_type_color(piece_type::pawn(c), c); }

    constexpr bool operator==(piece p) const { return x == p.x; }
    constexpr bool operator!=(piece p) const { return x != p.x; }

    static char const PRINT_CHARS[];
    constexpr char print_char() const
    {
        return PRINT_CHARS[8 * color().binary_index() + type().x];
    }

    constexpr u8 save_game_nibble() const
    {
        return ((x >> 3) & 0x8) | (x & 0x7);
    }

    static constexpr piece from_save_game_nibble(u8 n)
    {
        return n == 0 ? piece::NOTHING : piece::from_type_color(
            piece_type{ u8(n & 0x7) },
            (n & 0x8) ? piece_color::B : piece_color::W);
    }

    static piece const WP, WN, WB, WR, WQ, WK;
    static piece const BP, BN, BB, BR, BQ, BK;
};

// not used on arduino
constexpr char const piece::PRINT_CHARS[] = ".PXNBRQK.xpnbrqk";

constexpr piece const piece::NOTHING { 0 };
constexpr piece const piece::GUARD   { u8(piece_color::X_) };

constexpr piece const piece::WP = piece::from_type_color(piece_type::WPAWN, piece_color::W);
constexpr piece const piece::WN = piece::from_type_color(piece_type::KNIGHT, piece_color::W);
constexpr piece const piece::WB = piece::from_type_color(piece_type::BISHOP, piece_color::W);
constexpr piece const piece::WR = piece::from_type_color(piece_type::ROOK, piece_color::W);
constexpr piece const piece::WQ = piece::from_type_color(piece_type::QUEEN, piece_color::W);
constexpr piece const piece::WK = piece::from_type_color(piece_type::KING, piece_color::W);
constexpr piece const piece::BP = piece::from_type_color(piece_type::BPAWN, piece_color::B);
constexpr piece const piece::BN = piece::from_type_color(piece_type::KNIGHT, piece_color::B);
constexpr piece const piece::BB = piece::from_type_color(piece_type::BISHOP, piece_color::B);
constexpr piece const piece::BR = piece::from_type_color(piece_type::ROOK, piece_color::B);
constexpr piece const piece::BQ = piece::from_type_color(piece_type::QUEEN, piece_color::B);
constexpr piece const piece::BK = piece::from_type_color(piece_type::KING, piece_color::B);

static_assert(sizeof(square) == 1, "bad size");
static_assert(sizeof(square_delta) == 1, "bad size");
static_assert(sizeof(piece) == 1, "bad size");
static_assert(sizeof(piece_type) == 1, "bad size");
static_assert(sizeof(piece_color) == 1, "bad size");

struct move
{
    // flags:
    //    castle q  : a<3>
    //    castle k  : a<7>
    //    pawn dmove: b<3>
    //    en passant: b<7>
    u8 a, b;
    constexpr square fr() const { return square{ u8(a & 0x77) }; }
    constexpr square to() const { return square{ u8(b & 0x77) }; }
    constexpr square nonflag_fr() const { return square{ u8(a) }; }
    constexpr square nonflag_to() const { return square{ u8(b) }; }
    move& set_fr(square s) { a &= 0x88; a |= s.x; return *this; }
    move& set_to(square s) { b &= 0x88; b |= s.x; return *this; }
    move& clear_set_fr(square s) { a = s.x; return *this; }
    move& clear_set_to(square s) { b = s.x; return *this; }
    constexpr bool is_castle() const { return (b & 0x08) != 0; }
    constexpr bool is_promotion() const { return (b & 0x80) != 0; }
    constexpr bool is_pawn_dmove() const { return (a & 0x08) != 0; } // assumes not promotion
    constexpr bool is_en_passant() const { return (a & 0x80) != 0; } // assumes not promotion
    constexpr bool is_special()    const { return ((a | b) & 0x88) != 0; }
    constexpr bool is_non_dmove_special() const
    {
        return ((a & 0x80) | (b & 0x88)) != 0;
    }
    move& clear_promotion_piece_type()
    {
        a &= 0x77;
        return *this;
    }
    move& clear_set_promotion_piece_type(piece_type t)
    {
        return clear_promotion_piece_type().set_promotion_piece_type(t);
    }
    move& set_promotion_piece_type(piece_type t)
    {
        u8 x = t.x - piece_type::N_;
        a |= (x << 3) & 0x08;
        a |= (x << 6) & 0x80;
        return *this;
    }
    constexpr piece_type promotion_piece_type() const
    {
        return piece_type{ u8((((a & 0x08) >> 3) | ((a & 0x80) >> 6)) + piece_type::N_) };
    }
    move& set_castle() { b |= 0x08; return *this; }
    move& set_promotion() { b |= 0x80; return *this; }
    move& set_pawn_dmove() { a |= 0x08; return *this; }
    move& set_en_passant() { a |= 0x80; return *this; }
    stack_str<6> uci_str() const;

    constexpr bool operator==(move m) { return a == m.a && b == m.b; }
    constexpr bool operator!=(move m) { return !(a == m.a && b == m.b); }

    static move const NO_MOVE;
    constexpr bool is_nothing() const { return (a | b) == 0; }
};

constexpr move const move::NO_MOVE { 0, 0 };

// piece list index
struct piece_index
{
    u8 x;
    static constexpr u8 const MASK_C_ = piece_color::W_ | piece_color::B_ | piece_color::X_;
    constexpr piece_color color() const { return piece_color{ u8(x & MASK_C_) }; }
    constexpr bool is_of_color(piece_color c) { return color() == c; }
    constexpr bool is_not_of_color(piece_color c) { return (x & c.x) == 0; }
    constexpr bool is_nothing() const { return x == 0; }
    constexpr bool is_guard() const { return x == 96; }
    constexpr bool is_pawn_of_color(piece_color c) const
    {
        return u8(x - c.x - 2) < 8;
    }
    static piece_index const GUARD, NOTHING;
    piece_index operator+(u8 d) const { return piece_index{ u8(x + d) }; }
    piece_index operator-(u8 d) const { return piece_index{ u8(x - d) }; }
    piece_index& operator+=(u8 d) { x += d; return *this; }
    piece_index& operator-=(u8 d) { x -= d; return *this; }
    
    constexpr bool operator==(piece_index i) const { return x == i.x; }
    constexpr bool operator!=(piece_index i) const { return x != i.x; }
    constexpr bool operator<=(piece_index i) const { return x <= i.x; }
    constexpr bool operator>=(piece_index i) const { return x >= i.x; }
    constexpr bool operator<(piece_index i) const { return x < i.x; }
    constexpr bool operator>(piece_index i) const { return x > i.x; }
};

constexpr piece_index const piece_index::GUARD   { piece::GUARD.x };
constexpr piece_index const piece_index::NOTHING { piece::NOTHING.x };

struct game
{
    // 0x88 board indexes piece lists
    // unused space used by state stack
#if CH2K_USE_STD_ARRAY
    std::array<piece_index, 256> square_to_index_; // link from position to index
#else
    piece_index square_to_index_[256];
#endif

    // piece list order:
    //    nothing: 0
    //    king: 1
    //    pawns: 2-9
    //    knights: 10-last_knight
    //    sliders: first_slider-30
    //    nothing: 31
    // indices available to reuse: 18-27

    // indices reused (for each color):
    //    last_knight: 18
    //    first_slider: 19


    // link from index to piece (with space for guard)
#if CH2K_USE_STD_ARRAY
    std::array<piece, 65> index_to_piece_;
#else
    piece index_to_piece_[65];
#endif

    // extra space for accessing piece_index::NOTHING
    // scratch space: indices 1 to 31 (31 bytes)
#if CH2K_USE_STD_ARRAY
    std::array<square, 96> index_to_square_; // link from index to position
#else
    square index_to_square_[96];
#endif

    // piece value total: reuses the "nothing" index slot in index_to_square
    inline u8& pval(piece_color c) { return *(u8*)&index_to_square_[c.x]; }

    constexpr piece_index square_to_index(square s) const { return square_to_index_[s.x]; }
    piece_index& square_to_index(square s) { return square_to_index_[s.x]; }

    // index into OPENING_BOOK (offset by 1 so zero value is no book)
    //u16 opening_index_; // moved to grouped_data
    move opening_index_move_;
    u16 opening_index_next_;

    // sets best_ to randomly selected move from opening book
    void find_opening_move();

    // state stack embedded in square_to_index_ array
    // (exactly sized to use up all available free space)
    struct state
    {
        move lastmove;
        piece_index cap;
        u8 flags;
        u8 half_move;
    };
    static constexpr size_t const STATE_STACK_SIZE = 68 / sizeof(state);
    u8 state_index;

    inline state* stack_base()
    {
        return reinterpret_cast<state*>(&square_to_index_[0x99]);
    }
    inline state& stack()
    {
        return stack_base()[state_index];
    }
    state& stack_push()
    {
        auto& prev = stack();
        ++state_index;
        CH2K_ASSERT(state_index < STATE_STACK_SIZE);
        auto& curr = stack();
        curr.flags = prev.flags;
        curr.half_move = prev.half_move;
        return curr;
    }
    state& stack_pop()
    {
        state& r = stack();
        --state_index;
        return r;
    }

    void stack_reset()
    {
        state& s = stack();
        state_index = 0;
        stack() = s;
    }

    // movegen stack: big memory hog
    static constexpr size_t const MOVEGEN_STACK_SIZE = 256;
    move mvs_[MOVEGEN_STACK_SIZE];

    // return piece at square
    constexpr piece at(square s) const { return index_to_piece(square_to_index(s)); }

    constexpr piece index_to_piece(piece_index i) const { return index_to_piece_[i.x - 32]; }
    piece& index_to_piece(piece_index i) { return index_to_piece_[i.x - 32]; }

    constexpr piece square_to_piece(square s) const { return index_to_piece(square_to_index(s)); }
    piece& square_to_piece(square s) { return index_to_piece(square_to_index(s)); }

    constexpr square index_to_square(piece_index i) const { return index_to_square_[i.x]; }
    square& index_to_square(piece_index i) { return index_to_square_[i.x]; }

    piece_index& set_last_knight(piece_color c) { return *(piece_index*)(&index_to_piece_[c.x - 32 + 18].x); }
    piece_index& set_first_slider(piece_color c) { return *(piece_index*)(&index_to_piece_[c.x - 32 + 19].x); }

    constexpr piece_index king(piece_color c) const { return piece_index{ u8(c.x + 1) }; }
    constexpr piece_index first_pawn(piece_color c) const { return piece_index{ u8(c.x + 2) }; }
    constexpr piece_index last_pawn(piece_color c) const { return piece_index{ u8(first_pawn(c).x + 8) }; }
    constexpr piece_index first_knight(piece_color c) const { return last_pawn(c); }
    piece_index last_knight(piece_color c) const { return piece_index{ index_to_piece_[c.x - 32 + 18].x }; }
    piece_index first_slider(piece_color c) const { return piece_index{ index_to_piece_[c.x - 32 + 19].x }; }
    constexpr piece_index last_slider(piece_color c) const { return piece_index{ u8(c.x + 32) }; }

    struct index_iterator
    {
        piece_index i;
        index_iterator(piece_index i) : i(i) {}
        index_iterator& operator++() { ++i.x; return *this; }
        bool operator!=(index_iterator other) const { return i.x != other.i.x; }
        constexpr piece_index operator*() const { return i; }
    };

    struct pawn_collection
    {
        game const& g;
        piece_color c;
        index_iterator begin() { return g.first_pawn(c); }
        index_iterator end() { return g.last_pawn(c); }
    };
    struct knight_collection
    {
        game const& g;
        piece_color c;
        index_iterator begin() { return g.first_knight(c); }
        index_iterator end() { return g.last_knight(c); }
    };
    struct slider_collection
    {
        game const& g;
        piece_color c;
        index_iterator begin() { return g.first_slider(c); }
        index_iterator end() { return g.last_slider(c); }
    };

    pawn_collection pawns(piece_color c) const { return pawn_collection{ *this, c }; }
    knight_collection knights(piece_color c) const { return knight_collection{ *this, c }; }
    slider_collection sliders(piece_color c) const { return slider_collection{ *this, c }; }

    piece_index add_pawn(square s, piece_color color)
    {
        for(auto i : pawns(color))
            if(index_to_square(i).is_nowhere())
            {
                index_to_piece(i) = piece::pawn(color);
                index_to_square(i) = s;
                square_to_index(s) = i;
                return i;
            }
        return piece_index::NOTHING;
    }
    piece_index add_knight(square s, piece_color color)
    {
        piece_index i = last_knight(color);
        set_last_knight(color).x += 1;
        index_to_piece(i) = piece::from_type_color(piece_type::KNIGHT, color);
        index_to_square(i) = s;
        square_to_index(s) = i;
        return i;
    }
    piece_index add_slider(square s, piece_color color, piece_type type)
    {
        piece_index i = (set_first_slider(color) -= 1);
        index_to_piece(i) = piece::from_type_color(type, color);
        index_to_square(i) = s;
        square_to_index(s) = i;
        return i;
    }
    void add_king(square s, piece_color color)
    {
        piece_index i = king(color);
        index_to_piece(i) = piece::from_type_color(piece_type::KING, color);
        index_to_square(i) = s;
        square_to_index(s) = i;
    }

    u8 in_check;

    static constexpr u8 const GEN_MOVES_OUT_OF_MEM = 255;
    u8 gen_moves(move* m);
    bool en_passant_pinned(square king, square sa, square sb);

    // remove noncaptures
    u8 reduce_qsearch(move* m, u8 n);

    u8 order_moves(move* m, u8 n); // returns starting index of bad caps
    s8 see_square(square s);
    s8 see_capture(move mv);

    bool capturable(piece_color by_col, square x);

    void do_move(move m);
    void undo_move(move m);

    void do_null_move();
    void undo_null_move();

    // reset everything
    void clear();

    void save_game_data(u8* x);
    void load_game_data(u8 const* x);
    bool load_game_data_opening(u16 index, u8* mvs, u8 depth);
    piece_index add_piece(piece p, square s);
    void load_fen(char const* fen);
    void new_game();

    // evaluation settings
#if CH2K_TUNABLE_EVAL
    inline eval_settings_union const& get_eval_union() const { return *custom_eval_settings_; }
#define CH2K_GET_EVAL_SETTING(n__) custom_eval_settings_->v.n__
    eval_settings_union const* custom_eval_settings_;
    eval_settings_union eval_gradient_;
#else
    inline eval_settings_union const& get_eval_union() const { return DEFAULT_EVAL_SETTINGS; }
#define CH2K_GET_EVAL_SETTING(n__) (score)pgm_read_word(&DEFAULT_EVAL_SETTINGS.v.n__)
#endif

#ifndef CH2K_ARDUINO
    uint64_t nodes_, max_nodes_;
#else
    uint16_t nodes_, max_nodes_;
#endif
    score score_;
    uint8_t depth_, max_depth_;
    move best_;
    uint8_t stop_;
    uint8_t contempt_;

#if CH2K_MAX_MOVE_STACK
    uint16_t max_move_stack_;
#endif

    score eval()
    {
        //score r = eval_side<true>() - eval_side<false>();
        score r = eval_side(true) - eval_side(false);
        if(gd.c_.is_white())
            r += CH2K_GET_EVAL_SETTING(tempo);
        else
            r -= CH2K_GET_EVAL_SETTING(tempo);
#if CH2K_TUNABLE_EVAL
        eval_gradient_.v.tempo += gd.c_.is_white() ? 1.0 : -1.0;
#endif
        return r;
    }

    score eval_relative()
    {
        ++nodes_;
        score r;
        // endgame produces relative score
        if(endgame(r)) return r;
        r = eval();
        if(!gd.c_.is_white()) r = -r;
        return r;
    }

    score eval_relative_with_see()
    {
        score v = eval_relative();
        if(stack().cap.is_nothing()) // TODO: remove this?
            return v;
        return v + score(see_square(stack().lastmove.to())) * 25;
    }

    score eval_side(bool is_white);

    score draw_score() const
    {
        u16 c = contempt_;
        c <<= 4;
        return (state_index & 1) ? SCORE_DRAW + c : SCORE_DRAW - c;
    }

    bool check_draw_50move();
    u8 check_draw_repetition();
    bool check_draw_material();
    bool check_draw_material_side(piece_color c);

    // check for draw by material or specific endgame scenario
    bool endgame(score& s);
    // check one side for combinations
    bool endgame_side(piece_color c, score& s);

    score qsearch(move* m, score a, score b);
    score negamax(move* m, score a, score b, u8 max_depth);
    score negamax_root(score a, score b);
    score aspiration_window(u8 depth, score prev_score);
    void ai_move(); // either find_opening_move or iterative_deepening
    void iterative_deepening();

    static constexpr u8 const REP_MOVES_EXTRA = 12; // for undo
    static constexpr u8 const REP_MOVES_SIZE = 100 + REP_MOVES_EXTRA;

    struct
    {
        piece_color c_; // whose turn it is
        u16 ply_;
        u16 opening_index_;
#if CH2K_USE_STD_ARRAY
        std::array<move, REP_MOVES_SIZE> rep_moves_;
#else
        move rep_moves_[REP_MOVES_SIZE];
#endif
    } gd;

    constexpr move get_rep_move(u8 n) const
    {
        return gd.rep_moves_[n];
    }

    void execute_move(move m);
    void unexecute_move(move m, piece_index cap, u8 flags, u8 half_move);
    void reset_opening_index();

    bool move_is_reversible(move mv) const
    {
        if(mv.is_special()) return false;
        if(square_to_index(mv.nonflag_fr()).is_pawn_of_color(gd.c_))
            return false;
        return square_to_index(mv.nonflag_to()).is_nothing();
    }

    enum
    {
        STATUS_NORMAL,
        STATUS_CHECK,
        STATUS_DRAW_50MOVE,
        STATUS_DRAW_REPETITION,
        STATUS_DRAW_MATERIAL,
        STATUS_DRAW_STALEMATE,
        STATUS_MATED
    };
    // do not call from within search
    u8 check_status();

    stack_str<128> str() const;

    // debug method
#ifdef NDEBUG
    inline void verify() const;
#else
    void verify() const
    {
        for(u8 ii = 1; ii < 31; ++ii)
        {
            piece_index i;
            i = piece_index{ u8(ii + 32) };
            if(!index_to_square(i).is_nowhere())
                CH2K_ASSERT(square_to_index(index_to_square(i)) == i);
            i = piece_index{ u8(ii + 64) };
            if(!index_to_square(i).is_nowhere())
                CH2K_ASSERT(square_to_index(index_to_square(i)) == i);
        }
        for(u8 r = 0; r < 8; ++r)
        {
            for(u8 c = 0; c < 8; ++c)
            {
                square s = square::from_rowcol(r, c);
                piece_index i = square_to_index(s);
                if(!i.is_nothing())
                    CH2K_ASSERT(index_to_square(i) == s);
            }
        }
    }
#endif
};

#ifdef _MSC_VER
static constexpr size_t const X = sizeof(game);
#endif

static constexpr s8 const SEE_PV[] PROGMEM = { 0, 4, 4, 12, 13, 20, 36 };

s8 game::see_square(square s)
{
    s8 v = 0;

    // get smallest attacker
    piece_index attacker = piece_index::NOTHING;

    {
        piece_color const ec = gd.c_.opposite();
        piece_index i;
        square_delta d = ec.forward();
        if((i = square_to_index(s + d + square_delta::WEST)).is_pawn_of_color(gd.c_))
        {
            if(s.row() == ec.promotion_row())
                v += SEE_PV[piece_type::Q_] - SEE_PV[piece_type::WP_];
            attacker = i;
            goto recurse;
        }
        if((i = square_to_index(s + d + square_delta::EAST)).is_pawn_of_color(gd.c_))
        {
            if(s.row() == ec.promotion_row())
                v += SEE_PV[piece_type::Q_] - SEE_PV[piece_type::WP_];
            attacker = i;
            goto recurse;
        }
    }

    for(piece_index i : knights(gd.c_))
    {
        square ts = index_to_square(i);
        if(ts.is_nowhere()) continue;
        if(piece_type::KNIGHT.can_cap_delta(s - ts))
        {
            attacker = i;
            goto recurse;
        }
    }

    // bishops
    for(piece_index i : sliders(gd.c_))
    {
        square ts = index_to_square(i);
        if(ts.is_nowhere()) continue;
        if(index_to_piece(i).type() != piece_type::BISHOP)
            continue;
        if(!piece_type::BISHOP.can_cap_delta(s - ts))
            continue;
        square_delta d = (s - ts).vec();
        while(square_to_index(ts += d).is_nothing())
            ;
        if(ts == s)
        {
            attacker = i;
            goto recurse;
        }
    }

    // rooks
    for(piece_index i : sliders(gd.c_))
    {
        square ts = index_to_square(i);
        if(ts.is_nowhere()) continue;
        if(index_to_piece(i).type() != piece_type::ROOK)
            continue;
        if(!piece_type::ROOK.can_cap_delta(s - ts))
            continue;
        square_delta d = (s - ts).vec();
        while(square_to_index(ts += d).is_nothing())
            ;
        if(ts == s)
        {
            attacker = i;
            goto recurse;
        }
    }

    // queens
    for(piece_index i : sliders(gd.c_))
    {
        square ts = index_to_square(i);
        if(ts.is_nowhere()) continue;
        if(index_to_piece(i).type() != piece_type::QUEEN)
            continue;
        if(!piece_type::QUEEN.can_cap_delta(s - ts))
            continue;
        square_delta d = (s - ts).vec();
        while(square_to_index(ts += d).is_nothing())
            ;
        if(ts == s)
        {
            attacker = i;
            goto recurse;
        }
    }

recurse:

    if(!attacker.is_nothing())
    {
        square as = index_to_square(attacker);
        CH2K_ASSERT(!as.is_nowhere());
        v += (s8)pgm_read_byte(&SEE_PV[square_to_piece(s).type().x]);
        if(state_index == STATE_STACK_SIZE - 1)
            return v;
        move mv = move{}.clear_set_fr(as).clear_set_to(s);
        CH2K_ASSERT(!square_to_index(as).is_nothing());
        CH2K_ASSERT(!square_to_index(s).is_nothing());
        CH2K_ASSERT(square_to_piece(as).type().can_cap_delta(s - as));
        do_move(mv);
        v -= see_square(s);
        undo_move(mv);
        v = tmax<s8>(0, v);
    }

    return v;
}

s8 game::see_capture(move mv)
{
    s8 v;
    if(mv.is_promotion())
    {
        v = (s8)pgm_read_byte(&SEE_PV[mv.promotion_piece_type().x]);
        v -= SEE_PV[piece_type::WP_];
    }
    else
        v = (s8)pgm_read_byte(&SEE_PV[square_to_piece(mv.to()).type().x]);
    if(state_index == STATE_STACK_SIZE - 1)
        return v;
    do_move(mv);
    v -= see_square(mv.to());
    undo_move(mv);
    return v;
}

static void sort_moves_descending(s8* scores, move* m, u8 n)
{
    for(u8 j, i = 1; i < n; ++i)
    {
        move xm = m[i];
        s8 xs = scores[i];
        for(j = i; j > 0 && scores[j - 1] < xs; --j)
        {
            m[j] = m[j - 1];
            scores[j] = scores[j - 1];
        }
        m[j] = xm;
        scores[j] = xs;
    }
}

u8 game::order_moves(move* m, u8 n)
{
    static constexpr u8 const MAX_SORT = 31;
#if 0
    s8 scores[MAX_SORT];
#else
    s8* scores = reinterpret_cast<s8*>(&index_to_square_[1]);
#endif
    for(u8 i = 0; i < n; ++i)
    {
        CH2K_ASSERT(!m[i].is_nothing());
        CH2K_ASSERT(!square_to_index(m[i].fr()).is_nothing());
    }
    constexpr u8 const ia = 0;
    u8 ib = 0, ic, ibc;
    // move all captures/promotions to the front
    for(u8 i = ia; i < n; ++i)
    {
        if(m[i].is_promotion() || !square_to_index(m[i].to()).is_nothing())
            swap(m[i], m[ib++]);
    }
    // obtain SEE scores for each capture
    ib = tmin<u8>(ib, ia + MAX_SORT);
    for(u8 i = ia; i < ib; ++i)
        scores[i - ia] = see_capture(m[i]);
    // order captures by score (insertion sort)
    sort_moves_descending(scores, m + ia, ib - ia);
    // move bad captures to end
    ic = n;
    for(u8 i = ia; i < ib; ++i)
    {
        if(scores[i] < 0)
        {
            ic = n - ib;
            for(u8 j = i; j < ib; ++j)
                swap(m[j], m[ic + j]);
            ic = n - ib + i;
            ib = i;
        }
    }
    ibc = ic;

    // skip quiet ordering if there is a good capture
    if(ib > ia)
        return ibc;

    // assign quiet scores
    ic = tmin<u8>(ic, ib + MAX_SORT);
    for(u8 i = ib; i < ic; ++i)
    {
        bool flip = gd.c_.is_black();
        square s = m[i].fr();
        u8 pti = square_to_piece(s).type().x - 2;
        if(pti > 5) pti = 0;
        s16 d = (s16)pgm_read_word(&get_eval_union().pt.t[pti][m[i].to().table_index(flip)]);
        d -= (s16)pgm_read_word(&get_eval_union().pt.t[pti][s.table_index(flip)]);
        if(d > 127) d = 127;
        if(d < -127) d = -127;
        scores[i - ib] = s8(d);
    }
    // order quiet moves
    sort_moves_descending(scores, m + ib, ic - ib);

    return ibc;
}

u8 game::reduce_qsearch(move* m, u8 n)
{
    for(u8 i = 0; i < n; ++i)
    {
        if(!square_to_index(m[i].to()).is_nothing() ||
            m[i].is_promotion() || m[i].is_en_passant())
            continue;
        m[i--] = m[--n];
    }
    return n;
}

score game::qsearch(move* m, score a, score b)
{
#if CH2K_MAX_MOVE_STACK
    max_move_stack_ = tmax(max_move_stack_, uint16_t(m - &mvs_[0]));
#endif
    //if(depth > max_depth_ * 2)
    //    return eval_relative_with_see();
    if(state_index == STATE_STACK_SIZE - 1)
        return eval_relative();
    if(stop_ || nodes_ > max_nodes_) return 0;
    u8 n = gen_moves(m);
#if CH2K_MAX_MOVE_STACK
    if(n == GEN_MOVES_OUT_OF_MEM) max_move_stack_ = MOVEGEN_STACK_SIZE;
#endif
    if(n == GEN_MOVES_OUT_OF_MEM)
        return eval_relative_with_see();
    if(n == 0)
        return in_check ? SCORE_LOSS + state_index : draw_score();
    score stand_pat = eval_relative();
    if(stand_pat >= b)
        return b;
    a = tmax<score>(a, stand_pat);
    score v = SCORE_LOSS;
    for(u8 i = 0; i < n; ++i)
    {
        CH2K_ASSERT(!m[i].is_nothing());
        CH2K_ASSERT(!square_to_index(m[i].fr()).is_nothing());
    }
    n = reduce_qsearch(m, n);
    u8 bc = order_moves(m, n);
    for(u8 i = 0; i < n; ++i)
    {
        do_move(m[i]);
        v = tmax<score>(v, -qsearch(m + n, -b, -a));
        undo_move(m[i]);
        a = tmax<score>(a, v);
        if(a >= b) return b;
    }
    return a;
}

bool game::check_draw_50move()
{
    u8 half = stack().half_move;
    if(half >= 100)
        return true;
    return false;
}

u8 game::check_draw_repetition()
{
    u8 hm = stack().half_move;
    if(hm <= 4) return 0;
#if CH2K_USE_STD_ARRAY
    std::array<square_delta, 64> d;
#else
    square_delta d[64];
#endif
    square_delta master = square_delta::ZERO;
    for(auto& t : d) t = square_delta::ZERO;
    u8 reps = 0;
    s8 n;
    // go back through move history
    for(n = 0; n < (s8)hm; n += 1)
    {
        move mv = (n < (s8)state_index ?
            stack_base()[state_index - n].lastmove :
            get_rep_move(n - state_index));
        square x = mv.nonflag_fr();
        square y = mv.nonflag_to();
        square_delta md = y - x;
        master += md;
        piece_index i = square_to_index(y);
        d[i.x - 32] += md;
        CH2K_ASSERT(!square_to_index(y).is_nothing());
        CH2K_ASSERT(square_to_index(x).is_nothing());
        square_to_index(x) = square_to_index(y);
        square_to_index(y) = piece_index::NOTHING;
        index_to_square(i) = x;
        if(master.is_zero())
        {
            bool repetition = true;
            for(u8 j = 0; j < 64; ++j)
            {
                // check if piece has returned to original position
                if(!d[j].is_zero())
                {
                    // if not, check if swapped places with identical piece
                    piece_index ji{ u8(j + 32) };
                    square other_sq = index_to_square(ji) + d[j];
                    if(!other_sq.is_nowhere())
                    {
                        piece_index other = square_to_index(other_sq);
                        if(!other.is_nothing() && index_to_piece(ji) == index_to_piece(other))
                            continue;
                    }
                    repetition = false;
                    break;
                }
            }
            if((n & 1) && repetition && ++reps >= 2)
            {
                ++n;
                break;
            }
        }
    }
    --n;
    // restore board state
    for(; n >= 0; n -= 1)
    {
        move mv = (n < (s8)state_index ?
            stack_base()[state_index - n].lastmove :
            get_rep_move(n - state_index));
        square x = mv.nonflag_fr();
        square y = mv.nonflag_to();
        piece_index i = square_to_index(x);
        CH2K_ASSERT(square_to_index(y).is_nothing());
        CH2K_ASSERT(!square_to_index(x).is_nothing());
        square_to_index(y) = square_to_index(x);
        square_to_index(x) = piece_index::NOTHING;
        index_to_square(i) = y;
    }
    return reps;
}

bool game::check_draw_material()
{
    if(pval(piece_color::W) == 3 && pval(piece_color::B) == 3)
    {
        // KBKB with same color bishops is draw
        piece_index ia = piece_index::NOTHING;
        piece_index ib = piece_index::NOTHING;
        // any slider must be a bishop
        for(auto i : sliders(piece_color::W))
            if(!index_to_square(i).is_nowhere())
                i = ia;
        for(auto i : sliders(piece_color::B))
            if(!index_to_square(i).is_nowhere())
                i = ib;
        if(ia.is_nothing() || ib.is_nothing()) return true;
        // both bishops: check same color square
        return
            index_to_square(ia).square_color() ==
            index_to_square(ib).square_color();
    }
    return
        check_draw_material_side(piece_color::W) ||
        check_draw_material_side(piece_color::B);
}

bool game::check_draw_material_side(piece_color c)
{
    u8 const pa = pval(c);
    u8 const pb = pval(c.opposite());
    if(pb == 0)
    {
        // KK
        if(pa == 0) return true;
        for(auto i : pawns(c))
            if(!index_to_square(i).is_nowhere())
                return false;
        // KNK or KBK
        if(pa < 5) return true;
        if(pa == 6)
        {
            // KNNK is not draw by material (using FIDE rules)
#if 0
            bool one_piece = false;
            for(auto i : knights(c))
            {
                if(index_to_square(i).is_nowhere()) continue;
                if(one_piece) return true;
                one_piece = true;
            }
#endif
            // KBBK with same color bishops is draw
            u8 square_color = 0xff;
            for(auto i : sliders(c))
            {
                if(index_to_square(i).is_nowhere()) continue;
                u8 sc = index_to_square(i).square_color();
                if(sc == square_color) return true;
                square_color = sc;
            }
        }
    }
    return false;
}

u8 game::check_status()
{
    u8 n = gen_moves(&mvs_[0]); // update in_check even if not used
    if(check_draw_50move()) return STATUS_DRAW_50MOVE;
    if(check_draw_material()) return STATUS_DRAW_MATERIAL;
    if(check_draw_repetition() >= 2) return STATUS_DRAW_REPETITION;
    if(n > 0) return in_check ? STATUS_CHECK : STATUS_NORMAL;
    if(in_check) return STATUS_MATED;
    return STATUS_DRAW_STALEMATE;
}

bool game::endgame_side(piece_color c, score& s)
{
    u8 const pa = pval(c);
    u8 const pb = pval(c.opposite());
    if(pb == 0)
    {
        if(pa == 0)
            return (s = draw_score()), true;
        // count pieces
        u8 nknig, nslid[3], bishc;
        for(auto i : pawns(c))
            if(!index_to_square(i).is_nowhere())
                return false;
        if(pa <= 3)
            return (s = draw_score()), true;
        if(pa < 5)
            return false;
        nknig = 0;
        nslid[0] = nslid[1] = nslid[2] = 0;
        bishc = 0;
        for(auto i : knights(c))
            if(!index_to_square(i).is_nowhere())
                ++nknig;
        for(auto i : sliders(c))
        {
            square s = index_to_square(i);
            if(s.is_nowhere()) continue;
            piece_type t = index_to_piece(i).type();
            ++nslid[t.x - 4];
            if(t == piece_type::BISHOP)
                bishc |= (s.square_color() + 1);
        }
        if(pa == 6 && nknig == 2)
            return (s = draw_score()), true;
        square ka = index_to_square(king(c));
        square kb = index_to_square(king(c.opposite()));
        u8 kbr = kb.row();
        u8 kbc = kb.col();
        u8 king_separation =
            tabs<s8>(s8(ka.row()) - s8(kbr)) +
            tabs<s8>(s8(ka.col()) - s8(kbc));
        u8 kb_edge_dist;
        if(pa == 6 && nknig == 1)
        {
            CH2K_ASSERT(nslid[0] == 1);
            CH2K_ASSERT(bishc != 0);
            // KBNK
            if(!(bishc & 1)) // bishop on light square
            {
                kb_edge_dist = tmin<u8>(
                    14 - kbr - kbc,
                    kbr + kbc);
            }
            else // bishop on dark square
            {
                kb_edge_dist = tmin<u8>(
                    7 - kbr + kbc,
                    7 + kbr - kbc);
            }
        }
        else
        {
            kb_edge_dist =
                tabs(7 - s8(kbr * 2)) +
                tabs(7 - s8(kbc * 2));
        }
        s = score(pa) * 128 + kb_edge_dist * 32 - king_separation * 16;
        return true;
    }
    return false;
}

bool game::endgame(score& s)
{
    if(endgame_side(gd.c_, s)) return true;
    if(endgame_side(gd.c_.opposite(), s)) return (s = -s), true;
    return false;
}

score game::negamax(move* m, score a, score b, u8 max_depth)
{
#if CH2K_MAX_MOVE_STACK
    max_move_stack_ = tmax(max_move_stack_, uint16_t(m - &mvs_[0]));
#endif
    if(check_draw_50move()) return draw_score();
    if(check_draw_material()) return draw_score();
    if(check_draw_repetition() >= 2) return draw_score();
    if(stop_ || nodes_ > max_nodes_) return 0;
    if(state_index >= max_depth)
    {
        // qsearch has the danger of exceeding low node limits
        // at depth 1 for noisy positions
        //if(depth == 1)
        //    return eval_relative_with_see();
        //else
            return qsearch(m, a, b);
        //return eval_relative_with_see();
    }
    u8 n = gen_moves(m);
#if CH2K_MAX_MOVE_STACK
    if(n == GEN_MOVES_OUT_OF_MEM) max_move_stack_ = MOVEGEN_STACK_SIZE;
#endif
    if(n == GEN_MOVES_OUT_OF_MEM) return eval_relative_with_see();
    if(n == 0) return in_check ? SCORE_LOSS + state_index : draw_score();
    score v = SCORE_LOSS;

    // some weird pruning
    //if(!in_check && depth == max_depth - 1)
    //{
    //    score f = eval_relative_with_see();
    //    if(f + 50 <= a)
    //        return a;
    //}

    u8 badcaps = order_moves(m, n);
    //if(in_check) ++max_depth;
    for(u8 i = 0; i < n; ++i)
    {
        do_move(m[i]);
        //u8 md = i > 12 ? max_depth - 1 : max_depth;
        u8 md = max_depth;
        //if(i >= badcaps) --md;
        v = tmax<score>(v, -negamax(m + n, -b, -a, md));
        undo_move(m[i]);
        a = tmax<score>(a, v);
        if(a >= b)
            break;
    }
    return v;
}

score game::negamax_root(score a, score b)
{
    u8 n = gen_moves(&mvs_[0]);
    best_ = move::NO_MOVE;
    if(n == 0)
        return in_check ? SCORE_LOSS : draw_score();
    score v;
    v = SCORE_LOSS;
    order_moves(&mvs_[0], n);
    for(u8 i = 0; i < n; ++i)
    {
        do_move(mvs_[i]);
        v = tmax<score>(v, -negamax(&mvs_[n], -b, -a, max_depth_));
        undo_move(mvs_[i]);
        if(v > a)
        {
            best_ = mvs_[i];
            a = v;
        }
    }
    return v;
}

score game::aspiration_window(u8 depth, score prev_score)
{
    static constexpr int8_t const ASPIRATION_MIN_DEPTH = 2;
    static constexpr score const ASPIRATION_BASE_DELTA = 10000;

    score delta = ASPIRATION_BASE_DELTA;
    score a = depth < ASPIRATION_MIN_DEPTH ? SCORE_LOSS :
        tmax<score>(SCORE_LOSS, prev_score - delta);
    score b = depth < ASPIRATION_MIN_DEPTH ? SCORE_WIN :
        tmin<score>(SCORE_WIN, prev_score + delta);

    while(!stop_)
    {
        max_depth_ = depth;
        move best = best_;
        score v = negamax_root(a, b);

        if(nodes_ > max_nodes_)
        {
            best_ = best;
            return 0;
        }

        if(v > a && v < b)
            return v;
        best_ = best;

        if(v <= a)
        {
            b = (a + b) / 2;
            a = tmax<score>(SCORE_LOSS, a - delta);
        }

        if(v >= b)
        {
            b = tmin<score>(SCORE_WIN, b + delta);
        }

        delta = delta + delta / 4 + 5;
    }

    return 0;
}

// starting at a child node, advance i to where the child's
// next sibling node would start, even if there is not a next sibling
// returns false if node i was already the final sibling
static bool book_advance_to_next_sibling(u16& i)
{
    u8 d = pgm_read_byte(&OPENING_BOOK[i - 1]);
    ++i; // advance past this node or to first child
    if(d & 0x80) // if this node has children
        while(book_advance_to_next_sibling(i))
            ; // advance past all children
    return (d & 0x40) == 0; // check if we had been at the final sibling
}

// counts the number of siblings starting at i, including i itself
static u8 book_num_siblings(u16 i)
{
    u8 n = 1;
    while(book_advance_to_next_sibling(i))
        ++n;
    return n;
}

// careful: does not check if there are at least n more siblings
static void book_advance_n_siblings(u16& i, u8 n)
{
    while(n-- > 0)
        book_advance_to_next_sibling(i);
}

void game::execute_move(move m)
{
    ++gd.ply_;
    // advance opening book index
    if(gd.opening_index_ > 0)
    {
        u8 n = gen_moves(&mvs_[0]);
        while(mvs_[--n] != m)
            ; // assumes we are not erroneously in the opening book
        // now n is the index of m
        for(;;)
        {
            u8 d = pgm_read_byte(&OPENING_BOOK[gd.opening_index_ - 1]);
            if((d & 0x3f) == n)
            {
                gd.opening_index_ = (d & 0x80) ? gd.opening_index_ + 1 : 0;
                break;
            }
            if(!book_advance_to_next_sibling(gd.opening_index_))
            {
                gd.opening_index_ = 0;
                break;
            }
        }
    }

    for(u8 i = REP_MOVES_SIZE - 1; i > 0; --i)
        gd.rep_moves_[i] = gd.rep_moves_[i - 1];
    gd.rep_moves_[0] = m;
    do_move(m);
    stack_reset();
}

void game::unexecute_move(move m, piece_index cap, u8 flags, u8 half_move)
{
    --gd.ply_;
    for(u8 i = 0; i < REP_MOVES_SIZE - 1; ++i)
        gd.rep_moves_[i] = gd.rep_moves_[i + 1];
    stack_base()[1].cap = cap;
    state_index = 1;
    undo_move(m);
    stack_base()->flags = flags;
    stack_base()->half_move = half_move;
    if(!m.is_promotion() && m.is_en_passant())
    {
        square epsq = square::from_rowcol(m.fr().row(), m.to().col());
        stack_base()->lastmove.clear_set_to(epsq).set_pawn_dmove();
    }
    reset_opening_index();
}

void game::reset_opening_index()
{
    gd.opening_index_ = 0;
    if(gd.ply_ <= OPENING_BOOK_MAX_DEPTH)
    {
        u8 ply = (u8)gd.ply_;
        new_game();
        for(u8 i = 0; i < ply; ++i)
            execute_move(gd.rep_moves_[ply - 1]);
    }
}

void game::find_opening_move()
{
    u8 n = book_num_siblings(gd.opening_index_);
    n = nrand(n);
    book_advance_n_siblings(gd.opening_index_, n);
    n = pgm_read_byte(&OPENING_BOOK[gd.opening_index_ - 1]);
    n &= 0x3f;
    gen_moves(&mvs_[0]);
    best_ = mvs_[n];
}

void game::ai_move()
{
    if(gd.opening_index_ != 0)
        find_opening_move();
    else
        iterative_deepening();
}

void game::iterative_deepening()
{
    stop_ = false;
    score_ = 0;
    nodes_ = 0;
    depth_ = 0;
    best_ = move::NO_MOVE;
    u8 depth = 1;
    u8 max_depth = max_depth_;
    //score prev_score = eval_relative();
    while(depth <= max_depth && nodes_ < max_nodes_)
    {
        //prev_score = aspiration_window(depth, prev_score);
        move best = best_;
        max_depth_ = depth;
        score s = negamax_root(SCORE_LOSS, SCORE_WIN);
        if(nodes_ < max_nodes_)
        {
            depth_ = depth;
            score_ = s;
        }
        else if(!best.is_nothing())
            best_ = best;
        else
            break;
        ++depth;
        if(depth < max_depth && nodes_ * 128 <= max_nodes_)
            ++depth;
    }
    max_depth_ = max_depth;
}

score game::eval_side(bool is_white)
{
    bool const flip = !is_white;
    piece_color const c =
        is_white ? piece_color::W : piece_color::B;
#if CH2K_TUNABLE_EVAL
    score const gradf = is_white ? score(1) : score(-1);
#endif
    score r = 0;
    u8 num_pawns = 0;
    bool has_piece;

    // analyze enemy pawn formation for passed pawn analysis
    u8 epr[10];
    for(u8 i = 0; i < 10; ++i)
        epr[i] = is_white ? 7 : 0;
    for(auto i : pawns(c.opposite()))
    {
        square x = index_to_square(i);
        if(x.is_nowhere()) continue;
        ++num_pawns;
        u8 c = x.col();
        if(is_white)
            epr[c + 1] = tmin(epr[c + 1], x.row());
        else
            epr[c + 1] = tmax(epr[c + 1], x.row());
    }

    // piece-square tables
    square_delta const forward = c.forward();
    for(auto i : pawns(c))
    {
        square x = index_to_square(i);
        if(x.is_nowhere()) continue;
        ++num_pawns;
        u8 t = x.table_index(flip);
        r += CH2K_GET_EVAL_SETTING(pawn[t]);
#if CH2K_TUNABLE_EVAL
        eval_gradient_.v.pawn[t] += gradf;
#endif
        bool connected = false;
        t = x.table_col_index();
        //connected pawns
        if(square_to_index(x - forward + square_delta::WEST).is_pawn_of_color(c) ||
            square_to_index(x - forward + square_delta::EAST).is_pawn_of_color(c))
        {
            r += CH2K_GET_EVAL_SETTING(connected_pawn[t]);
#if CH2K_TUNABLE_EVAL
            eval_gradient_.v.connected_pawn[t] += gradf;
#endif
            connected = true;
        }
        // passed pawns
        {
            u8 row = x.row();
            u8 c = x.col();
            if((!is_white && row >= epr[c] && row >= epr[c + 1] && row >= epr[c + 2]) ||
                (is_white && row <= epr[c] && row <= epr[c + 1] && row <= epr[c + 2]))
            {
                r += CH2K_GET_EVAL_SETTING(passed_pawn[t]);
#if CH2K_TUNABLE_EVAL
                eval_gradient_.v.passed_pawn[t] += gradf;
#endif
                if(connected)
                {
                    r += CH2K_GET_EVAL_SETTING(connected_passed_pawn[t]);
#if CH2K_TUNABLE_EVAL
                    eval_gradient_.v.connected_passed_pawn[t] += gradf;
#endif
                }
            }
        }
    }
    num_pawns = tmin<u8>(num_pawns, 15);
    has_piece = false;
    for(auto i : knights(c))
    {
        square x = index_to_square(i);
        if(x.is_nowhere()) continue;
        u8 t = x.table_index(flip);
        r += CH2K_GET_EVAL_SETTING(knight[t]);
#if CH2K_TUNABLE_EVAL
        eval_gradient_.v.knight[t] += gradf;
#endif
        // knight outpost
        if(square_to_index(x - forward + square_delta::WEST).is_pawn_of_color(c) ||
            square_to_index(x - forward + square_delta::EAST).is_pawn_of_color(c))
        {
            t = x.table_col_index();
            r += CH2K_GET_EVAL_SETTING(knight_outpost[t]);
#if CH2K_TUNABLE_EVAL
            eval_gradient_.v.knight_outpost[t] += gradf;
#endif
        }
        r += CH2K_GET_EVAL_SETTING(knight_pawn_bonus[num_pawns]);
#if CH2K_TUNABLE_EVAL
        eval_gradient_.v.knight_pawn_bonus[num_pawns] += gradf;
#endif
        if(has_piece)
        {
            r += CH2K_GET_EVAL_SETTING(knight_pair);
#if CH2K_TUNABLE_EVAL
            eval_gradient_.v.knight_pair += gradf;
#endif
        }
        has_piece = true;
    }

    square ek = index_to_square(king(c.opposite()));
    u8 bishop_colors = 0;
    has_piece = false;
    for(auto i : sliders(c))
    {
        square x = index_to_square(i);
        if(x.is_nowhere()) continue;
        u8 t = x.table_index(flip);
        piece_type pt = index_to_piece(i).type();
        u8 a = pt.x - piece_type::B_;
        r += CH2K_GET_EVAL_SETTING(sliders[a][t]);
#if CH2K_TUNABLE_EVAL
        eval_gradient_.v.sliders[a][t] += gradf;
#endif

        // sliders align with enemy king
        if(pt.can_cap_delta(ek - x))
        {
            r += CH2K_GET_EVAL_SETTING(sliders_align_to_king[a]);
#if CH2K_TUNABLE_EVAL
            eval_gradient_.v.sliders_align_to_king[a] += gradf;
#endif
        }
        
        // rook/queen on [semi]open file
        if(pt.is_orthogonal_slider())
        {
            if(pt == piece_type::ROOK)
            {
                r += CH2K_GET_EVAL_SETTING(rook_pawn_bonus[num_pawns]);
#if CH2K_TUNABLE_EVAL
                eval_gradient_.v.rook_pawn_bonus[num_pawns] += gradf;
#endif
                if(has_piece)
                {
                    r += CH2K_GET_EVAL_SETTING(rook_pair);
#if CH2K_TUNABLE_EVAL
                    eval_gradient_.v.rook_pair += gradf;
#endif
                }
                has_piece = true;
            }
            bool semi = true, open = true;
            x.clear_row();
            for(u8 n = 0; n < 8; ++n)
            {
                if(square_to_index(x).is_pawn_of_color(c))
                {
                    semi = open = false;
                    break;
                }
                if(square_to_index(x).is_pawn_of_color(c.opposite()))
                    open = false;
                x += square_delta::SOUTH;
            }
            if(open)
            {
                t = x.table_col_index();
                r += CH2K_GET_EVAL_SETTING(rook_on_open_file[t]);
#if CH2K_TUNABLE_EVAL
                eval_gradient_.v.rook_on_open_file[t] += gradf;
#endif
            }
            else if(semi)
            {
                t = x.table_col_index();
                r += CH2K_GET_EVAL_SETTING(rook_on_semi_open_file[t]);
#if CH2K_TUNABLE_EVAL
                eval_gradient_.v.rook_on_semi_open_file[t] += gradf;
#endif
            }
        }
        else // piece is a bishop
        {
            bishop_colors |= (x.square_color() + 1);
        }
    }
    if(!(bishop_colors ^ 3))
    {
        r += CH2K_GET_EVAL_SETTING(bishop_pair);
#if CH2K_TUNABLE_EVAL
        eval_gradient_.v.bishop_pair += gradf;
#endif
    }
    {
        square ks = index_to_square(king(c));
        u8 t = ks.table_index(flip);
        r += CH2K_GET_EVAL_SETTING(king[t]);
#if CH2K_TUNABLE_EVAL
        eval_gradient_.v.king[t] += gradf;
#endif
        t = 0;
        for(u8 j = 0; j < 8; ++j)
        {
            square_delta d{ pgm_read_byte(&square_delta::KING_MOVES[j]) };
            square x = ks;
            while(square_to_index(x += d).is_not_of_color(c))
                ++t;
        }
        t = tmin<u8>(t, 12);
        r += CH2K_GET_EVAL_SETTING(king_vmob[t]);
#if CH2K_TUNABLE_EVAL
        eval_gradient_.v.king_vmob[t] += gradf;
#endif
    }

    return r;
}

void game::do_null_move()
{
    auto& st = stack_push();
    square k = index_to_square(king(gd.c_));
    st.lastmove.clear_set_fr(k).clear_set_to(k);
    gd.c_ = gd.c_.opposite();
}

void game::undo_null_move()
{
    gd.c_ = gd.c_.opposite();
    stack_pop();
}

void game::do_move(move m)
{
    CH2K_ASSERT(!m.is_nothing());

    auto& st = stack_push();
    st.lastmove = m;
    st.half_move = move_is_reversible(m) ? st.half_move + 1 : 0;
    square x = m.fr();
    square y = m.to();
    piece_index pc = square_to_index(x);
    piece_index cap = square_to_index(y);

    CH2K_ASSERT(!pc.is_nothing());

    if(!cap.is_nothing())
        pval(gd.c_.opposite()) -= pgm_read_byte(&PVALS[index_to_piece(cap).type().x]);

    st.cap = cap;
    st.flags &= pgm_read_byte(&CASTLING_SPOILERS[x.x]);
    st.flags &= pgm_read_byte(&CASTLING_SPOILERS[y.x]);

    if(m.is_non_dmove_special())
    {
        if(m.is_promotion())
        {
            piece_type t = m.promotion_piece_type();
            index_to_piece(pc) = piece::NOTHING;
            index_to_square(pc) = square::NOWHERE;
            if(t != piece_type::KNIGHT)
            {
                pc = (set_first_slider(gd.c_) -= 1);
                index_to_piece(pc) = piece::from_type_color(t, gd.c_);
            }
            else
            {
                pc = last_knight(gd.c_);
                set_last_knight(gd.c_) += 1;
                index_to_piece(pc) = piece::from_type_color(piece_type::KNIGHT, gd.c_);
            }
            pval(gd.c_) += pgm_read_byte(&PVALS[t.x]) - PVALS[1];
        }
        else if(m.is_castle())
        {
            if(y < x) // queenside
            {
                st.cap = square_to_index(x + square_delta::WEST * 4);
                square_to_index(x + square_delta::WEST) = st.cap;
                square_to_index(x + square_delta::WEST * 4) = piece_index::NOTHING;
                index_to_square(st.cap) = x + square_delta::WEST;
            }
            else // kingside
            {
                st.cap = square_to_index(x + square_delta::EAST * 3);
                square_to_index(x + square_delta::EAST) = st.cap;
                square_to_index(x + square_delta::EAST * 3) = piece_index::NOTHING;
                index_to_square(st.cap) = x + square_delta::EAST;
            }
        }
        else if(m.is_en_passant())
        {
            // remove captured pawn
            square epcap = y - gd.c_.forward();
            piece_index eppc = st.cap = square_to_index(epcap);
            square_to_index(epcap) = piece_index::NOTHING;
            index_to_square(eppc) = square::NOWHERE;
            pval(gd.c_.opposite()) -= PVALS[1];
        }
    }

    square_to_index(y) = pc;
    square_to_index(x) = piece_index::NOTHING;
    index_to_square(pc) = y;
    index_to_square(cap) = square::NOWHERE;

    gd.c_ = gd.c_.opposite();
}

void game::undo_move(move m)
{
    gd.c_ = gd.c_.opposite();

    square x = m.fr();
    square y = m.to();
    piece_index pc = square_to_index(y);
    piece_index cap = stack_pop().cap;

    if(m.is_non_dmove_special())
    {
        if(m.is_promotion())
        {
            pval(gd.c_) -= pgm_read_byte(&PVALS[index_to_piece(pc).type().x]) - PVALS[1];
            index_to_piece(pc) = piece::NOTHING;
            index_to_square(pc) = square::NOWHERE;
            if(pc < last_knight(gd.c_))
                set_last_knight(gd.c_) -= 1;
            else
                set_first_slider(gd.c_) += 1;
            for(auto i : pawns(gd.c_))
                if(index_to_piece(i).is_nothing())
                {
                    pc = i;
                    index_to_piece(i) = piece::pawn(gd.c_);
                    break;
                }
        }
        else if(m.is_castle())
        {
            if(y < x) // queenside
            {
                square_to_index(x + square_delta::WEST * 4) = cap;
                square_to_index(x + square_delta::WEST) = piece_index::NOTHING;
                index_to_square(cap) = x + square_delta::WEST * 4;
            }
            else // kingside
            {
                square_to_index(x + square_delta::EAST * 3) = cap;
                square_to_index(x + square_delta::EAST) = piece_index::NOTHING;
                index_to_square(cap) = x + square_delta::EAST * 3;
            }
            cap = piece_index::NOTHING;
        }
        else if(m.is_en_passant())
        {
            // restore captured pawn
            square epcap = y - gd.c_.forward();
            square_to_index(epcap) = cap;
            index_to_square(cap) = epcap;
            pval(gd.c_.opposite()) += PVALS[1];
            cap = piece_index::NOTHING;
        }
    }

    if(!cap.is_nothing())
        pval(gd.c_.opposite()) += pgm_read_byte(&PVALS[index_to_piece(cap).type().x]);

    square_to_index(x) = pc;
    index_to_square(pc) = x;

    square_to_index(y) = cap;
    index_to_square(cap) = y;
}

stack_str<6> move::uci_str() const
{
    stack_str<6> r;
    r.at(0) = fr().col_char();
    r.at(1) = fr().row_char();
    r.at(2) = to().col_char();
    r.at(3) = to().row_char();
    r.at(4) = is_promotion() ? promotion_piece_type().uci_char() : '\0';
    r.at(5) = '\0';
    return r;
}

void game::clear()
{
#if CH2K_MAX_MOVE_STACK
    max_move_stack_ = 0;
#endif
    for(u8 i = 0; ; ++i)
    {
        square_to_index_[i] = (i & 0x88) ? piece_index::GUARD : piece_index::NOTHING;
        if(i == 255) break;
    }
    for(auto& p : index_to_piece_) p = piece::NOTHING;
    for(auto& s : index_to_square_) s = square::NOWHERE;
    index_to_piece_[64] = piece::GUARD;

    set_last_knight(piece_color::W) = first_knight(piece_color::W);
    set_first_slider(piece_color::W) = last_slider(piece_color::W);
    set_last_knight(piece_color::B) = first_knight(piece_color::B);
    set_first_slider(piece_color::B) = last_slider(piece_color::B);

    state_index = 0;
    state& s = stack();
    s.flags = 0;
    s.half_move = 0;
    s.lastmove = move::NO_MOVE;
    s.cap = piece_index::NOTHING;

    gd.c_ = piece_color::W;
    pval(piece_color::W) = 0;
    pval(piece_color::B) = 0;
}

void game::new_game()
{
    clear();

    gd.opening_index_ = 1;
    gd.ply_ = 0;

    // add pawns
    {
        square ws = square::from_rowcol(6, 0);
        square bs = square::from_rowcol(1, 0);
        for(u8 i = 0; i < 8; ++i)
        {
            add_pawn(ws, piece_color::W);
            add_pawn(bs, piece_color::B);
            ws += square_delta::EAST;
            bs += square_delta::EAST;
        }
    }

    // add other pieces
    {
        square s = square::from_rowcol(0, 0);
        for(u8 i = 0; i < 2; ++i)
        {
            piece_color const c = (i == 0 ? piece_color::B : piece_color::W);
            add_slider(s + square_delta::EAST * 0, c, piece_type::ROOK);
            add_knight(s + square_delta::EAST * 1, c);
            add_slider(s + square_delta::EAST * 2, c, piece_type::BISHOP);
            add_slider(s + square_delta::EAST * 3, c, piece_type::QUEEN);
            add_king(s + square_delta::EAST * 4, c);
            add_slider(s + square_delta::EAST * 5, c, piece_type::BISHOP);
            add_knight(s + square_delta::EAST * 6, c);
            add_slider(s + square_delta::EAST * 7, c, piece_type::ROOK);
            s += square_delta::SOUTH * 7;
        }
    }

    pval(piece_color::W) = pval(piece_color::B) = 8 * 1 + 2 * (5 + 3 + 3) + 9;
    stack().flags = (CASTLE_WK | CASTLE_WQ | CASTLE_BK | CASTLE_BQ);

    // set last move to black king
    stack().lastmove.clear_set_to(square::from_rowcol(0, 4));
}

#ifdef CH2K_ARDUINO
#define CH2K_EEPROM_WR(a__, d__) EEPROM.write((int)(a__), (d__))
#define CH2K_EEPROM_RD(a__)      EEPROM.read((int)(a__))
#else
#define CH2K_EEPROM_WR(a__, d__) (*(a__) = (d__))
#define CH2K_EEPROM_RD(a__)      (*(a__))
#endif

/*
GAME DATA FORMAT
 32 - board data (4 bits per piece type/color)
  2 - last move (contains en passant info)
  1 - half move clock
??? - repetition history   \
  1 - side to move         | grouped
  2 - opening book index   |  data
  2 - ply                  /
??? - total
*/
static constexpr int const EEPROM_GAME_DATA_SIZE = 35 + sizeof(game::gd);

void game::save_game_data(u8* x)
{
    // board data
    for(u8 r = 0; r < 8; ++r)
    {
        for(u8 c = 0; c < 8; c += 2)
        {
            u8 d = 0;
            square s = square::from_rowcol(r, c);
            piece_index i = square_to_index(s);
            if(!i.is_nothing())
                d = index_to_piece(i).save_game_nibble();
            d <<= 4;
            s += square_delta::EAST;
            i = square_to_index(s);
            if(!i.is_nothing())
                d |= index_to_piece(i).save_game_nibble();
            CH2K_EEPROM_WR(x++, d);
        }
    }

    // last move
    CH2K_EEPROM_WR(x++, stack_base()->lastmove.a);
    CH2K_EEPROM_WR(x++, stack_base()->lastmove.b);

    // half move clock
    CH2K_EEPROM_WR(x++, stack_base()->half_move);

    // grouped data
    for(u8 i = 0; i < sizeof(gd); ++i)
        CH2K_EEPROM_WR(x++, *((u8*)&gd + i));
}

piece_index game::add_piece(piece p, square s)
{
    piece_color const c = p.color();
    piece_type const t = p.type();
    switch(t.x)
    {
    case piece_type::WP_: case piece_type::BP_:
        pval(c) += PVALS[1];
        return add_pawn(s, c);
    case piece_type::N_:
        pval(c) += PVALS[3];
        return add_knight(s, c);
    case piece_type::B_: case piece_type::R_: case piece_type::Q_:
        pval(c) += pgm_read_byte(&PVALS[t.x]);
        return add_slider(s, c, t);
    case piece_type::K_:
        add_king(s, c);
        return king(c);
    default:
        break;
    }
    return piece_index::NOTHING;
}

bool game::load_game_data_opening(u16 index, u8* mvs, u8 depth)
{
    if(depth == 0)
        return index == gd.opening_index_;
    for(;;)
    {
        u8 d = pgm_read_byte(&OPENING_BOOK[index - 1]);
        if((d & 0x80) && load_game_data_opening(index + 1, mvs, depth - 1))
        {
            mvs[depth] = d;
            return true;
        }
        if(!book_advance_to_next_sibling(index))
            break;
    }
    return false;
}

void game::load_game_data(u8 const* x)
{
    clear();

    // board data
    for(u8 r = 0; r < 8; ++r)
    {
        for(u8 c = 0; c < 8; c += 2)
        {
            uint8_t d = CH2K_EEPROM_RD(x++);
            square s = square::from_rowcol(r, c);
            add_piece(piece::from_save_game_nibble(d >> 4), s);
            s += square_delta::EAST;
            add_piece(piece::from_save_game_nibble(d & 0xf), s);
        }
    }

    // last move
    stack_base()->lastmove.a = CH2K_EEPROM_RD(x++);
    stack_base()->lastmove.b = CH2K_EEPROM_RD(x++);

    // half move clock
    stack_base()->half_move = CH2K_EEPROM_RD(x++);

    // grouped data
    for(u8 i = 0; i < sizeof(gd); ++i)
        *((u8*)&gd + i) = CH2K_EEPROM_RD(x++);

    // re-establish opening book index by executing moves over again
    reset_opening_index();
}

void game::load_fen(char const* fen)
{
    clear();

    // board
    char c;
    square x = square::from_rowcol(0, 0);
    pval(piece_color::W) = 0;
    pval(piece_color::B) = 0;
    while(' ' != (c = *fen++) && c)
    {
        switch(c)
        {
        case 'p': pval(piece_color::B) += PVALS[1]; add_pawn  (x, piece_color::B);                     x += square_delta::EAST; break;
        case 'n': pval(piece_color::B) += PVALS[3]; add_knight(x, piece_color::B);                     x += square_delta::EAST; break;
        case 'b': pval(piece_color::B) += PVALS[4]; add_slider(x, piece_color::B, piece_type::BISHOP); x += square_delta::EAST; break;
        case 'r': pval(piece_color::B) += PVALS[5]; add_slider(x, piece_color::B, piece_type::ROOK);   x += square_delta::EAST; break;
        case 'q': pval(piece_color::B) += PVALS[6]; add_slider(x, piece_color::B, piece_type::QUEEN);  x += square_delta::EAST; break;
        case 'k': pval(piece_color::B) += PVALS[7]; add_king  (x, piece_color::B);                     x += square_delta::EAST; break;
        case 'P': pval(piece_color::W) += PVALS[1]; add_pawn  (x, piece_color::W);                     x += square_delta::EAST; break;
        case 'N': pval(piece_color::W) += PVALS[3]; add_knight(x, piece_color::W);                     x += square_delta::EAST; break;
        case 'B': pval(piece_color::W) += PVALS[4]; add_slider(x, piece_color::W, piece_type::BISHOP); x += square_delta::EAST; break;
        case 'R': pval(piece_color::W) += PVALS[5]; add_slider(x, piece_color::W, piece_type::ROOK);   x += square_delta::EAST; break;
        case 'Q': pval(piece_color::W) += PVALS[6]; add_slider(x, piece_color::W, piece_type::QUEEN);  x += square_delta::EAST; break;
        case 'K': pval(piece_color::W) += PVALS[7]; add_king  (x, piece_color::W);                     x += square_delta::EAST; break;
        case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8':
            for(u8 n = 0; n < u8(c - '0'); ++n)
            {
                square_to_index(x) = piece_index::NOTHING;
                x += square_delta::EAST;
            }
            break;
        case '/':
            x += square_delta{ 8 };
            break;
        default:
            break;
        }
    }

    // side to move
    gd.c_ = piece_color::W;
    while(' ' != (c = *fen++) && c)
    {
        switch(c)
        {
        case 'w': case 'W': gd.c_ = piece_color::W; break;
        case 'b': case 'B': gd.c_ = piece_color::B; break;
        default: break;
        }
    }

    state_index = 0;

    // castling
    stack().flags = 0;
    while(' ' != (c = *fen++) && c)
    {
        switch(c)
        {
        case 'K': stack().flags |= CASTLE_WK; break;
        case 'Q': stack().flags |= CASTLE_WQ; break;
        case 'k': stack().flags |= CASTLE_BK; break;
        case 'q': stack().flags |= CASTLE_BQ; break;
        default: break;
        }
    }

    // by default set last move to king
    stack().lastmove = move{}.clear_set_to(index_to_square(king(gd.c_.opposite())));

    // find if king is in contact check, set last move to be checker
    {
        x = index_to_square(king(gd.c_));
        for(u8 j = 0; j < 8; ++j)
        {
            square_delta d{ pgm_read_byte(&square_delta::KING_MOVES[j]) };
            square y = x - d;
            piece_index i = square_to_index(y);
            if(i.is_nothing()) continue;
            if(i.is_of_color(gd.c_)) continue;
            if(index_to_piece(i).type().can_cap_delta_contact(d))
                stack().lastmove.clear_set_to(y);
        }
        for(u8 j = 0; j < 8; ++j)
        {
            square_delta d{ pgm_read_byte(&square_delta::KNIGHT_MOVES[j]) };
            square y = x - d;
            piece_index i = square_to_index(y);
            if(i.is_nothing()) continue;
            if(i.is_of_color(gd.c_)) continue;
            if(index_to_piece(i).type() == piece_type::KNIGHT)
                stack().lastmove.clear_set_to(y);
        }
    }

    // en passant pawn
    while(' ' != (c = *fen++) && c)
    {
        if(c >= 'a' && c <= 'h')
        {
            x = square::from_rowcol(u8('8' - *fen), u8(c - 'a'));
            stack().lastmove =
                move{}.clear_set_to(x - gd.c_.forward()).set_pawn_dmove();
            break;
        }
    }

    stack().half_move = 0;
}

bool game::en_passant_pinned(square king, square sa, square sb)
{
    if(king.row() != sa.row())
        return false;
    square x = king;
    piece_index i;
    if(king < sa)
    {
        while(square_to_index(x += square_delta::EAST).is_nothing())
            ;
        if(x != sa) return false;
        x = sb;
        while((i = square_to_index(x += square_delta::EAST)).is_nothing())
            ;
        return i.is_not_of_color(gd.c_) && index_to_piece(i).type().can_cap_orthogonal();
    }
    else
    {
        while(square_to_index(x += square_delta::WEST).is_nothing())
            ;
        if(x != sb) return false;
        x = sa;
        while((i = square_to_index(x += square_delta::WEST)).is_nothing())
            ;
        return i.is_not_of_color(gd.c_) && index_to_piece(i).type().can_cap_orthogonal();
    }
}

static move* add_pawn_move(move* r, move mv, uint8_t pro_row)
{
    if(mv.to().row() != pro_row)
        *r++ = mv;
    else
    {
        mv.set_promotion();
        *r++ = mv.clear_set_promotion_piece_type(piece_type::QUEEN);
        *r++ = mv.clear_set_promotion_piece_type(piece_type::KNIGHT);
        *r++ = mv.clear_set_promotion_piece_type(piece_type::ROOK);
        *r++ = mv.clear_set_promotion_piece_type(piece_type::BISHOP);
    }
    return r;
}

u8 game::gen_moves(move* m)
{
    move* r = m;

    move* const m28 = &mvs_[MOVEGEN_STACK_SIZE - 28];
    move* const m14 = &mvs_[MOVEGEN_STACK_SIZE - 14];
    move* const m8 = &mvs_[MOVEGEN_STACK_SIZE - 8];
    move* const m_out_of_mem = m + MOVEGEN_STACK_SIZE;

    auto const my_color = gd.c_;
    auto const enemy_color = my_color.opposite();
    auto const forward = my_color.forward();

    u8 const pawn_row_base = my_color.pawn_base_row();
    u8 const pawn_row_pro = my_color.promotion_row();

    move mv;

    square const king_pos = index_to_square(king(my_color));

    in_check = 0;
    square checker = square::NOWHERE;
    square_delta check_dir;

    // pinned piece temp stack
    square pin_sq[12];
    piece_index pin_index[12];
    u8 pin_st = 0;

    // locate pinned pieces
    for(piece_index i : sliders(enemy_color))
    {
        square const enemy_pos = index_to_square(i);
        if(enemy_pos.is_nowhere()) continue;
        piece pc = index_to_piece(i);
        piece_type const pt = pc.type();
        square_delta const d = enemy_pos - king_pos;
        if(!pt.can_cap_delta_distant(d)) continue;
        square_delta const v = d.vec();
        piece_index pci;
        square x = king_pos;
        while((pci = square_to_index(x += v)).is_nothing())
            ;
        pc = index_to_piece(pci);
        if(x == enemy_pos)
        {
            in_check += 2;
            checker = enemy_pos;
            check_dir = v;
        }
        else if(pc.is_of_color(my_color))
        {
            if(r >= m8) goto out_of_mem;
            square y = x;
            while(square_to_index(y += v).is_nothing())
                ;
            if(y == enemy_pos)
            {
                // piece at x is pinned!
                pin_sq[pin_st] = x;
                index_to_square(pci) = square::NOWHERE;
                pin_index[pin_st++] = pci;

                // generate moves along pin
                mv.clear_set_fr(x);
                if(pc.is_pawn())
                {
                    y = x + forward;
                    mv.clear_set_to(y);
                    if(v.is_vertical())
                    {
                        if(square_to_index(y).is_nothing())
                        {
                            //*r++ = mv.clear_set_to(y);
                            r = add_pawn_move(r, mv.clear_set_to(y), pawn_row_pro);
                            y += forward;
                            if(square_to_index(y).is_nothing() && x.row() == pawn_row_base)
                                *r++ = mv.clear_set_to(y).set_pawn_dmove();
                        }
                    }
                    else
                    {
                        y += square_delta::WEST;
                        if(y == enemy_pos)
                            //*r++ = mv.clear_set_to(y);
                            r = add_pawn_move(r, mv.clear_set_to(y), pawn_row_pro);
                        y += square_delta::EAST * 2;
                        if(y == enemy_pos)
                            //*r++ = mv.clear_set_to(y);
                            r = add_pawn_move(r, mv.clear_set_to(y), pawn_row_pro);
                    }
                }
                else if(pc.type().can_cap_delta(d))
                {
                    y = x;
                    do
                    {
                        y += v;
                        *r++ = mv.clear_set_to(y);
                    } while(y != enemy_pos);
                    y = x;
                    while((y -= v) != king_pos)
                        *r++ = mv.clear_set_to(y);
                }
            }
        }
    }

    {
        square y = stack().lastmove.to();
        square_delta d = king_pos - y;
        if(square_to_piece(y).type().can_cap_delta_contact(d))
        {
            checker = y;
            ++in_check;
            check_dir = d.reversed().vec();
        }
    }

    if(in_check)
    {
        r = m;
        if(in_check > 2) goto king_moves;
        if(checker == stack().lastmove.to())
            goto en_passant_moves;
        goto regular_moves;
    }

    // generate castlings
    {
        if(r >= m8) goto out_of_mem;
        auto f = stack().flags;
        if((f & castle_q(my_color)) &&
            square_to_index(king_pos + square_delta::WEST).is_nothing() &&
            square_to_index(king_pos + square_delta::WEST * 2).is_nothing() &&
            square_to_index(king_pos + square_delta::WEST * 3).is_nothing() &&
            !capturable(enemy_color, king_pos + square_delta::WEST) &&
            !capturable(enemy_color, king_pos + square_delta::WEST * 2))
        {
            *r++ = mv.clear_set_fr(king_pos).
                clear_set_to(king_pos + square_delta::WEST * 2).set_castle();
        }
        if((f & castle_k(my_color)) &&
            square_to_index(king_pos + square_delta::EAST).is_nothing() &&
            square_to_index(king_pos + square_delta::EAST * 2).is_nothing() &&
            !capturable(enemy_color, king_pos + square_delta::EAST) &&
            !capturable(enemy_color, king_pos + square_delta::EAST * 2))
        {
            *r++ = mv.clear_set_fr(king_pos).
                clear_set_to(king_pos + square_delta::EAST * 2).set_castle();
        }
    }

en_passant_moves:
    if(r >= m8) goto out_of_mem;
    if(stack().lastmove.is_pawn_dmove())
    {
        square y = stack().lastmove.to();
        if(in_check && y != checker)
            goto regular_moves;
        square x = y + square_delta::WEST;
        piece_index i = square_to_index(x);
        if(i.is_pawn_of_color(my_color) &&
            !index_to_square(i).is_nowhere() &&
            !en_passant_pinned(king_pos, x, y))
        {
            *r++ = mv.clear_set_fr(x).clear_set_to(y + forward).set_en_passant();
        }
        x += square_delta::EAST * 2;
        i = square_to_index(x);
        if(i.is_pawn_of_color(my_color) &&
            !index_to_square(i).is_nowhere() &&
            !en_passant_pinned(king_pos, y, x))
        {
            *r++ = mv.clear_set_fr(x).clear_set_to(y + forward).set_en_passant();
        }
    }

regular_moves:

    if(in_check & 1)
    {
        // contact check: only king move or capture
        mv.clear_set_to(checker);

        // pawns capture checker
        {
            square x = checker - forward;
            x += square_delta::WEST;
            piece_index i = square_to_index(x);
            if(i.is_pawn_of_color(my_color) && !index_to_square(i).is_nowhere())
                //*r++ = mv.clear_set_fr(x);
                r = add_pawn_move(r, mv.clear_set_fr(x), pawn_row_pro);
            x += square_delta::EAST * 2;
            i = square_to_index(x);
            if(i.is_pawn_of_color(my_color) && !index_to_square(i).is_nowhere())
                //*r++ = mv.clear_set_fr(x);
                r = add_pawn_move(r, mv.clear_set_fr(x), pawn_row_pro);
        }

        // knights capture checker
        if(r >= m8) goto out_of_mem;
        for(piece_index i : knights(my_color))
        {
            square x = index_to_square(i);
            if(x.is_nowhere()) continue;
            if(piece_type::KNIGHT.can_cap_delta(checker - x))
                *r++ = mv.clear_set_fr(x);
        }

        // sliders capture checker
        if(r >= m8) goto out_of_mem;
        for(piece_index i : sliders(my_color))
        {
            square x = index_to_square(i);
            if(x.is_nowhere()) continue;
            piece pc = index_to_piece(i);
            square_delta d = checker - x;
            if(pc.type().can_cap_delta(d))
            {
                square_delta v = d.vec();
                square y = x;
                while(square_to_index(y += v).is_nothing())
                    ;
                if(y == checker)
                    *r++ = mv.clear_set_fr(x);
            }
        }

        goto king_moves;
    }

    // pawn moves
    {
        if(r >= m28) goto out_of_mem;
        for(piece_index i : pawns(my_color))
        {
            square x = index_to_square(i);
            if(x.is_nowhere()) continue;
            square y = x + forward;
            mv.clear_set_fr(x);
            if(square_to_index(y + square_delta::WEST).is_of_color(enemy_color))
                //*r++ = mv.clear_set_to(y + square_delta::W);
                r = add_pawn_move(r, mv.clear_set_to(y + square_delta::WEST), pawn_row_pro);
            if(square_to_index(y + square_delta::EAST).is_of_color(enemy_color))
                //*r++ = mv.clear_set_to(y + square_delta::E);
                r = add_pawn_move(r, mv.clear_set_to(y + square_delta::EAST), pawn_row_pro);
            if(square_to_index(y).is_nothing())
            {
                //*r++ = mv.clear_set_to(y);
                r = add_pawn_move(r, mv.clear_set_to(y), pawn_row_pro);
                y += forward;
                if(square_to_index(y).is_nothing() && x.row() == pawn_row_base)
                {
                    *r++ = mv.clear_set_to(y).set_pawn_dmove();
                    // no need to clear dmove flag because it is stored
                    // in 'to' slot and the next set_to will clear it
                }
            }
        }
    }

    // knight moves
    for(piece_index i : knights(my_color))
    {
        square x = index_to_square(i);
        if(x.is_nowhere()) continue;
        if(r >= m8) goto out_of_mem;
        mv.clear_set_fr(x);
        for(u8 j = 0; j < 8; ++j)
        {
            square_delta d{ pgm_read_byte(&square_delta::KNIGHT_MOVES[j]) };
            square y = x + d;
            if(square_to_index(y).is_not_of_color(my_color))
                *r++ = mv.clear_set_to(y);
        }
    }

    // bishop/rook/queen moves
    for(piece_index i : sliders(my_color))
    {
        square x = index_to_square(i);
        if(x.is_nowhere()) continue;
        if(r >= m28) goto out_of_mem;
        piece piece = index_to_piece(i);
        piece_type pt = piece.type();
        piece_index j;
        mv.clear_set_fr(x);
        if(pt.is_orthogonal_slider())
        {
            for(u8 k = 0; k < 4; ++k)
            {
                square_delta d{ pgm_read_byte(&square_delta::ORTHOGONAL_DIRS[k]) };
                square y = x;
                do
                {
                    j = square_to_index(y += d);
                    if(j.is_not_of_color(my_color))
                        *r++ = mv.clear_set_to(y);
                } while(j.is_nothing());
            }
        }
        if(pt.is_diagonal_slider())
        {
            for(u8 k = 0; k < 4; ++k)
            {
                square_delta d{ pgm_read_byte(&square_delta::DIAGONAL_DIRS[k]) };
                square y = x;
                do
                {
                    j = square_to_index(y += d);
                    if(j.is_not_of_color(my_color))
                        *r++ = mv.clear_set_to(y);
                } while(j.is_nothing());
            }
        }
    }

    // remove moves that don't block a distant check
    if(in_check)
    {
        for(move* mp = m; mp < r; ++mp)
        {
            square y = mp->to();
            if((y - king_pos).vec() != check_dir)
                *mp-- = *--r;
            else
            {
                square x = king_pos;
                do
                {
                    x += check_dir;
                    if(x == y) break;
                } while(x != checker);
                if(x != y)
                    *mp-- = *--r;
            }
        }
    }

king_moves:
    {
        if(m >= m8) goto out_of_mem;
        piece_index i = king(my_color);
        square x = index_to_square(i);
        mv.clear_set_fr(x);

        // temporarily remove king to catch moving
        // away in the ray of a slider's attack
        square_to_index(x) = piece_index::NOTHING;

        for(u8 j = 0; j < 8; ++j)
        {
            square_delta d{ pgm_read_byte(&square_delta::KING_MOVES[j]) };
            square y = x + d;
            if(square_to_index(y).is_not_of_color(my_color))
            {
                if(!capturable(enemy_color, y))
                    *r++ = mv.clear_set_to(y);
            }
        }

        // put king back
        square_to_index(x) = king(my_color);
    }

    // pop pieces back off of the pin stack
    while(pin_st > 0)
    {
        piece_index i = pin_index[--pin_st];
        index_to_square(i) = pin_sq[pin_st];
    }

    return u8(r - m);

out_of_mem:

    // pop pieces back off of the pin stack
    while(pin_st > 0)
    {
        piece_index i = pin_index[--pin_st];
        index_to_square(i) = pin_sq[pin_st];
    }

    return GEN_MOVES_OUT_OF_MEM;
}

bool game::capturable(piece_color by_col, square x)
{
    // pawns
    {
        square_delta forward = by_col.forward();
        square y = x - forward;
        if(square_to_index(y + square_delta::WEST).is_pawn_of_color(by_col)) return true;
        if(square_to_index(y + square_delta::EAST).is_pawn_of_color(by_col)) return true;
    }

    // knights
    for(piece_index i : knights(by_col))
    {
        square y = index_to_square(i);
        if(y.is_nowhere()) continue;
        if(piece_type::KNIGHT.can_cap_delta(y - x)) return true;
    }

    // sliders
    piece_index t = square_to_index(x);
    square_to_index(x) = piece_index::GUARD;
    for(piece_index i : sliders(by_col))
    {
        square y = index_to_square(i);
        if(y.is_nowhere()) continue;
        piece pc = index_to_piece(i);
        square_delta d = x - y;
        if(pc.type().can_cap_delta(d))
        {
            square_delta v = d.vec();
            while(square_to_index(y += v).is_nothing())
                ;
            if(y == x)
            {
                square_to_index(x) = t;
                return true;
            }
        }
    }
    square_to_index(x) = t;

    // king
    return piece_type::KING.can_cap_delta(x - index_to_square(king(by_col)));
}

stack_str<128> game::str() const
{
    stack_str<128> rs;
    size_t j = 0;
    rs.copy(j, gd.c_.is_white() ? "WHITE" : "BLACK");
    rs.copy(j, " to move\n");
    for(u8 r = 0; r < 8; ++r)
    {
        for(u8 c = 0; c < 8; ++c)
        {
            auto i = square_to_index(square::from_rowcol(r, c));
            rs.at(j++) = i.is_nothing() ? ' ' : index_to_piece(i).print_char();
        }
        rs.at(j++) = '\n';
    }
    rs.at(j) = '\0';
    return rs;
}

}
