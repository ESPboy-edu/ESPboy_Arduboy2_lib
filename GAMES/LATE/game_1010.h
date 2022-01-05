#pragma once

#include <Arduboy2.h>
#include "common.h"

void game1010(Arduboy2 *sgr, uint8_t *gdat, uint8_t menu, uint8_t *gameOn, uint32_t *score, uint32_t *hiScore);
const uint8_t game1010Logo[] PROGMEM = {
	0b01110111,
	0b00000000,
	0b01110111,
	0b01010101,
	0b01010101,
	0b01010101,
	0b01110111
};
