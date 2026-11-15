#pragma once
// PC stand-in for avr/eeprom.h: 1 KB of fake EEPROM the harness can wipe.
#include <cstdint>
#include <cstring>
inline uint8_t* simEeprom() { static uint8_t e[1024]; return e; }
inline uint8_t eeprom_read_byte(const uint8_t* a) { return simEeprom()[(uintptr_t)a]; }
inline void eeprom_update_byte(uint8_t* a, uint8_t v) { simEeprom()[(uintptr_t)a] = v; }
inline void eeprom_update_block(const void* src, void* a, size_t n) { memcpy(simEeprom() + (uintptr_t)a, src, n); }
inline void eeprom_read_block(void* dst, const void* a, size_t n) { memcpy(dst, simEeprom() + (uintptr_t)a, n); }
inline uint16_t eeprom_read_word(const uint16_t* a) { uint16_t v; memcpy(&v, simEeprom() + (uintptr_t)a, 2); return v; }
inline void eeprom_update_word(uint16_t* a, uint16_t v) { memcpy(simEeprom() + (uintptr_t)a, &v, 2); }
