#pragma once

static constexpr phys_box LEVEL_07_BOXES[16] PROGMEM =
{
    { { 160, 0, 128 }, { 40, 0, -32 }, 0, 0 },
    { { 160, 0, 128 }, { -40, 0, -32 }, 0, 0 },
    { { 64, 0, 128 }, { 64, 0, 32 }, 0, 0 },
    { { 64, 0, 88 }, { -64, 0, 22 }, 0, 0 },
    { { 64, 0, 24 }, { -64, 0, 58 }, 0, 0 },
    { { 24, 0, 16 }, { -74, 0, 48 }, 0, 0 },
    { { 24, 0, 16 }, { -54, 0, 48 }, 0, 0 },
    { { 64, 32, 0 }, { -64, 8, 64 }, 0, 0 },
    { { 0, 32, 64 }, { -80, 8, 48 }, 0, 0 },
    { { 0, 24, 32 }, { -24, 6, -8 }, 0, 0 },
    { { 0, 24, 32 }, { 0, 6, -24 }, 0, 0 },
    { { 0, 24, 32 }, { 24, 6, -40 }, 0, 0 },
    { { 91, 16, 0 }, { 64, 4, -48 }, 224, 0 },
    { { 91, 16, 0 }, { -64, 4, -48 }, 32, 0 },
    { { 64, 0, 70 }, { 32, 4, 48 }, 64, 10 },
    { { 64, 0, 51 }, { -36, 2, 48 }, 64, -7 },
};
static constexpr dvec3 LEVEL_07_BALL_POS = { 4096, 128, 3072 };
static constexpr dvec3 LEVEL_07_FLAG_POS = { -4096, -256, 3072 };