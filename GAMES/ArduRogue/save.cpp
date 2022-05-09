#include "game.hpp"

#include <stddef.h>

static constexpr int EEPROM_END = 1024;
static constexpr int EEPROM_START = EEPROM_END - SAVE_FILE_BYTES - 4;
static constexpr int CHECKSUM_ADDR = EEPROM_END - 2;
static constexpr int VERSION_ADDR = EEPROM_END - 4;

static constexpr uint8_t SAVE_VERSION_LO = uint8_t(SAVE_VERSION);
static constexpr uint8_t SAVE_VERSION_HI = uint8_t(SAVE_VERSION >> 8);

static uint16_t get_save_checksum()
{
    uint16_t x;
    *((uint8_t*)&x + 0) = read_persistent(CHECKSUM_ADDR);
    *((uint8_t*)&x + 1) = read_persistent(CHECKSUM_ADDR + 1);
    return x;
}

static uint16_t compute_checksum()
{
    // CRC16
    uint8_t x;
    uint16_t crc = 0xffff;
    for(uint16_t i = EEPROM_START; i < EEPROM_END - 2; ++i)
    {
        x = (crc >> 8) ^ read_persistent(i);
        x ^= x >> 4;
        crc = (crc << 8) ^
            (uint16_t(x) << 12) ^
            (uint16_t(x) << 5) ^
            (uint16_t(x) << 0);
    }
    return crc;
}

static void update_checksum()
{
    uint16_t x = compute_checksum();
    update_persistent(CHECKSUM_ADDR + 0, *((uint8_t*)&x + 0));
    update_persistent(CHECKSUM_ADDR + 1, *((uint8_t*)&x + 1));
}

bool save_valid()
{
    return
        read_persistent(VERSION_ADDR) == SAVE_VERSION &&
        compute_checksum() == get_save_checksum();
}

void destroy_save()
{
    // set player type to NONE
    update_persistent(EEPROM_START + offsetof(saved_data, ents.d_[0].type), entity::NONE);
    update_checksum();
    flush_persistent();
}

void save()
{
    for(uint16_t i = 0; i < SAVE_FILE_BYTES; ++i)
        update_persistent(EEPROM_START + i, ((uint8_t*)&globals_.saved)[i]);
    update_persistent(VERSION_ADDR + 0, SAVE_VERSION_LO);
    update_persistent(VERSION_ADDR + 1, SAVE_VERSION_HI);
    update_checksum();
    flush_persistent();
}

void load()
{
    for(uint16_t i = 0; i < SAVE_FILE_BYTES; ++i)
        ((uint8_t*)&globals_.saved)[i] = read_persistent(EEPROM_START + i);
}
