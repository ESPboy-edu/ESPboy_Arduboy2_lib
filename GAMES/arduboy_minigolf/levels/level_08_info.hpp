#pragma once

static constexpr phys_box LEVEL_08_BOXES[15] PROGMEM =
{
    { { 160, 0, 64 }, { 0, 0, 56 }, 0, 0 },
    { { 160, 0, 24 }, { 0, 0, -46 }, 0, 0 },
    { { 72, 0, 16 }, { -22, 0, -56 }, 0, 0 },
    { { 72, 0, 16 }, { 22, 0, -56 }, 0, 0 },
    { { 160, 0, 24 }, { 0, 0, -66 }, 0, 0 },
    { { 160, 16, 0 }, { 0, 4, -72 }, 0, 0 },
    { { 0, 16, 64 }, { -40, 4, -56 }, 0, 0 },
    { { 0, 16, 64 }, { 40, 4, -56 }, 0, 0 },
    { { 16, 0, 160 }, { 0, -8, 0 }, 0, 0 },
    { { 160, 0, 37 }, { 12, -6, 0 }, 64, -10 },
    { { 160, 0, 37 }, { -12, -6, 0 }, 64, 10 },
    { { 160, 0, 21 }, { 24, -2, 0 }, 64, -19 },
    { { 160, 0, 21 }, { -24, -2, 0 }, 64, 19 },
    { { 160, 0, 24 }, { 32, 4, 0 }, 64, -32 },
    { { 160, 0, 24 }, { -32, 4, 0 }, 64, 32 },
};
static constexpr dvec3 LEVEL_08_BALL_POS = { 0, 128, 3840 };
static constexpr dvec3 LEVEL_08_FLAG_POS = { 0, -256, -3584 };
