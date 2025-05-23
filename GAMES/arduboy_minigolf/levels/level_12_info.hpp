#pragma once

static constexpr phys_box LEVEL_12_BOXES[17] PROGMEM =
{
    { { 80, 0, 160 }, { 0, 0, 40 }, 0, 0 },
    { { 56, 0, 173 }, { 20, 0, -20 }, 224, 0 },
    { { 160, 0, 80 }, { 0, 0, -60 }, 0, 0 },
    { { 16, 0, 32 }, { -44, 0, -48 }, 0, 0 },
    { { 16, 0, 32 }, { -44, 0, -72 }, 0, 0 },
    { { 24, 0, 80 }, { -54, 0, -60 }, 0, 0 },
    { { 0, 16, 80 }, { -60, 4, -60 }, 0, 0 },
    { { 80, 16, 0 }, { -40, 4, -80 }, 0, 0 },
    { { 114, 16, 0 }, { 0, 4, -20 }, 32, 0 },
    { { 114, 16, 0 }, { 40, 4, -20 }, 32, 0 },
    { { 114, 16, 0 }, { 40, 4, -60 }, 224, 0 },
    { { 114, 16, 0 }, { -40, 28, -4 }, 224, 0 },
    { { 114, 16, 0 }, { 0, 28, -4 }, 224, 0 },
    { { 160, 0, 80 }, { -20, 24, -4 }, 0, 0 },
    { { 80, 34, 34 }, { 0, 0, 44 }, 0, 32 },
    { { 80, 0, 32 }, { 0, 24, 24 }, 0, 0 },
    { { 0, 16, 32 }, { 0, 4, -60 }, 0, 0 },
};
static constexpr dvec3 LEVEL_12_BALL_POS = { 0, 128, 4352 };
static constexpr dvec3 LEVEL_12_FLAG_POS = { -2816, -256, -3840 };
