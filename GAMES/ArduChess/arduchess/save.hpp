#pragma once

#include <stddef.h>

//#include <EEPROM.h>

#include "vars.hpp"

//#define DEV_EEPROM 1
#if DEV_EEPROM
static void eeprom_update(int a, uint8_t d)
{
    if(a < EEPROM_START || a >= EEPROM_END) for(;;);
    EEPROM.write(a, d);
}
#else
#define eeprom_update(a__, d__) EEPROM.write(a__, d__)
#endif

static uint16_t compute_save_checksum()
{
    // CRC16
    uint8_t x;
    uint16_t crc = 0xffff;
    for(int i = EEPROM_START; i < EEPROM_END - 2; ++i)
    {
        x = (crc >> 8) ^ EEPROM.read(i);
        x ^= x >> 4;
        crc = (crc << 8) ^
            (uint16_t(x) << 12) ^
            (uint16_t(x) <<  5) ^
            (uint16_t(x) <<  0);
    }
    return crc;
}

static constexpr int get_save_checksum_addr()
{
    return EEPROM_END - 2;
}

static void set_save_checksum(uint16_t x)
{
    int a = get_save_checksum_addr();
    eeprom_update(a + 0, *((uint8_t*)&x + 0));
    eeprom_update(a + 1, *((uint8_t*)&x + 1));
}

static uint16_t get_save_checksum()
{
    uint16_t x;
    constexpr int a = get_save_checksum_addr();
    *((uint8_t*)&x + 0) = EEPROM.read(a);
    *((uint8_t*)&x + 1) = EEPROM.read(a + 1);
    return x;
}

static constexpr int get_save_addr(uint8_t n)
{
    return EEPROM_START + sizeof(save_file_data) * n;
}

static void init_saves()
{
    checksum_valid = (compute_save_checksum() == get_save_checksum());
}

static bool save_is_valid(uint8_t n)
{
    if(!checksum_valid) return false;
    uint8_t d = EEPROM.read(get_save_addr(n) + offsetof(save_file_data, valid));
    return d == SAVE_VALID_TAG;
}

// TODO: optimize/simplify save/load by grouping things together into a struct

static void save_game(uint8_t n)
{
    if(!checksum_valid)
    {
        // mark all saves as invalid
        for(uint8_t i = 0; i < NUM_SAVE_FILES; ++i)
            if(i != n)
                eeprom_update(get_save_addr(i) + offsetof(save_file_data, valid), 0);
    }
    int a = get_save_addr(n);
    
    g.save_game_data((uint8_t*)(a + offsetof(save_file_data, data)));
    
    eeprom_update(a + offsetof(save_file_data, valid), SAVE_VALID_TAG);
    for(uint8_t i = 0; i < sizeof(save_file_grouped_data); ++i)
        eeprom_update(a + offsetof(save_file_data, grouped_data) + i, *((uint8_t*)&dd + i));
    set_save_checksum(compute_save_checksum());
    checksum_valid = true;
    game_saved = true;
    EEPROM.commit();
}

static void load_game(uint8_t n)
{
    int a = get_save_addr(n);
    
    g.load_game_data((uint8_t*)(a + offsetof(save_file_data, data)));
    
    for(uint8_t i = 0; i < sizeof(save_file_grouped_data); ++i)
        *((uint8_t*)&dd + i) = EEPROM.read(a + offsetof(save_file_data, grouped_data) + i);
}

static void update_board_cache_from_save(uint8_t n)
{
    int a = get_save_addr(n);
    for(uint8_t bi = 0, r = 0; r < 8; ++r)
    {
        for(uint8_t c = 0; c < 8; c += 2, ++bi)
        {
            uint8_t d = EEPROM.read(a + offsetof(save_file_data, data) + bi);
            b[r][c + 0] = ch2k::piece::from_save_game_nibble(d >> 4);
            b[r][c + 1] = ch2k::piece::from_save_game_nibble(d & 0xf);
        }
    }
}
