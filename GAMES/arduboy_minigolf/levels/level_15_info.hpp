#pragma once

static constexpr phys_box LEVEL_15_BOXES[18] PROGMEM =
{
    { { 80, 0, 40 }, { 0, 8, 70 }, 0, 0 },
    { { 160, 0, 80 }, { -40, 8, 40 }, 0, 0 },
    { { 160, 0, 80 }, { 40, 8, 40 }, 0, 0 },
    { { 80, 0, 80 }, { -60, -8, -40 }, 0, 0 },
    { { 80, 0, 80 }, { 60, -8, -40 }, 0, 0 },
    { { 24, 0, 80 }, { -10, 0, -40 }, 0, 0 },
    { { 24, 0, 80 }, { 10, 0, -40 }, 0, 0 },
    { { 16, 0, 32 }, { 0, 0, -52 }, 0, 0 },
    { { 16, 0, 32 }, { 0, 0, -28 }, 0, 0 },
    { { 80, 0, 88 }, { -60, 0, 0 }, 0, -15 },
    { { 80, 0, 88 }, { 60, 0, 0 }, 0, -15 },
    { { 80, 0, 50 }, { -28, -4, -40 }, 64, -13 },
    { { 80, 0, 50 }, { 28, -4, -40 }, 64, 13 },
    { { 64, 16, 0 }, { 0, 4, -60 }, 0, 0 },
    { { 114, 16, 0 }, { -60, -4, -40 }, 32, 0 },
    { { 114, 16, 0 }, { 60, -4, -40 }, 224, 0 },
    { { 114, 16, 0 }, { -60, 12, 40 }, 224, 0 },
    { { 114, 16, 0 }, { 60, 12, 40 }, 32, 0 },
};
static constexpr dvec3 LEVEL_15_BALL_POS = { 0, 640, 4352 };
static constexpr dvec3 LEVEL_15_FLAG_POS = { 0, -256, -2560 };