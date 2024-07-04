#pragma once

#include "ArduboyG.h"
extern ArduboyGBase_Config<ABG_Mode::L4_Triplane> a;

#define SPRITESU_OVERWRITE
#define SPRITESU_PLUSMASK
#define SPRITESU_RECT
#include "SpritesU.hpp"

extern uint8_t ox;
extern uint8_t oy;

void update();
void render();
