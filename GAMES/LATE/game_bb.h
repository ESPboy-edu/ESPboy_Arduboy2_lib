#pragma once

#include <Arduboy2.h>
#include "common.h"

void gameBB(Arduboy2 *sgr, uint8_t *gdat, uint8_t menu, uint8_t *gameOn, uint32_t *score, uint32_t *hiScore);
const uint8_t gameBBLogo[] PROGMEM = {
	0b01101110,
	0b00100101,
	0b01101101,
	0b00000000,
	0b01000000,
	0b00111000,
	0b00000111
};
