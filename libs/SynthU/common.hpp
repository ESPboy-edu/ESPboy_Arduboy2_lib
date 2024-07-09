#pragma once

#include <stdint.h>
#include <Arduino.h>
#include "song.h"

#define SYNTHU_NUM_CHANNELS 4
#define SYNTHU_UPDATE_EVERY_N_FRAMES 1
#define SOUNDS_SYNTHU 1
#define SYNTHU_FX_READDATABYTES_FUNC platform_fx_read_data_bytes
#define SYNTHU_IMPLEMENTATION

#define uint24_t uint32_t

void platform_fx_read_data_bytes(uint24_t addr, uint8_t *dst, size_t num){
  memcpy_P(dst, (uint8_t *)addr, num); 
};

#define SYNTHU_FX_READDATABYTES_FUNC platform_fx_read_data_bytes
#include "SynthU.hpp"
