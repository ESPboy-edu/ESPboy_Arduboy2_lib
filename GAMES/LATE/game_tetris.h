#pragma once

#include <Arduboy2.h>
#include "common.h"

void gameTetris(Arduboy2 *sgr, uint8_t *gdat, uint8_t menu, uint8_t *gameOn, uint32_t *score, uint32_t *hiScore);
const uint8_t gameTetrisLogo[] PROGMEM = {
	0b00011100,
	0b00010101,
	0b01110100,
	0b01000101,
	0b01110100,
	0b00010101,
	0b00011100,
};
