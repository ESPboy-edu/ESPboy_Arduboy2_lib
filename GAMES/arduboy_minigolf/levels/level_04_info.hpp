#pragma once

static constexpr phys_box LEVEL_04_BOXES[14] PROGMEM =
{
    { { 80, 0, 56 }, { -48, 0, 46 }, 0, 0 },
    { { 80, 0, 128 }, { -48, 0, -32 }, 0, 0 },
    { { 48, 0, 80 }, { -16, 0, -44 }, 0, 0 },
    { { 48, 0, 80 }, { 40, 0, -44 }, 0, 0 },
    { { 16, 0, 80 }, { 64, 0, -44 }, 0, 0 },
    { { 16, 0, 32 }, { 56, 0, -56 }, 0, 0 },
    { { 16, 0, 32 }, { 56, 0, -32 }, 0, 0 },
    { { 0, 16, 80 }, { 68, 4, -44 }, 0, 0 },
    { { 80, 16, 0 }, { 48, 4, -64 }, 0, 0 },
    { { 80, 16, 0 }, { 48, 4, -24 }, 0, 0 },
    { { 128, 16, 0 }, { -36, 4, -64 }, 0, 0 },
    { { 0, 16, 128 }, { -68, 4, -32 }, 0, 0 },
    { { 64, 0, 88 }, { -40, 0, 16 }, 64, -16 },
    { { 64, 0, 88 }, { 12, 0, -36 }, 0, -16 },
};
static constexpr dvec3 LEVEL_04_BALL_POS = { -3584, 128, 3072 };
static constexpr dvec3 LEVEL_04_FLAG_POS = { 3584, -256, -2816 };