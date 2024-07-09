#pragma once

uint16_t rnd = 0xACE1;

uint8_t randomLFSR(uint8_t min, uint8_t max) {
return random(min,max);
}