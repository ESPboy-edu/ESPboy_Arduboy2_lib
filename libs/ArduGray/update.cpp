#include "common.hpp"

static bool dir;
uint8_t ox;
uint8_t oy;

void update()
{
    uint8_t b = a.buttonsState();
    if(ox >   0 && (b & LEFT_BUTTON )) --ox;
    if(oy >   0 && (b & UP_BUTTON   )) --oy;
    if(ox < 128 && (b & RIGHT_BUTTON)) ++ox;
    if(oy <  64 && (b & DOWN_BUTTON )) ++oy;
}
