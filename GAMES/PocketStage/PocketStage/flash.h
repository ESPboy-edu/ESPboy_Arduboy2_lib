#pragma once
#include <Arduino.h>
#include <ArduboyFX.h>


static inline void flash_init() {
    FX::begin();
}

static inline void fx_read(uint32_t addr, void *to, uint8_t n) {
    FX::readDataBytes(addr, (uint8_t*)to, n);
}
