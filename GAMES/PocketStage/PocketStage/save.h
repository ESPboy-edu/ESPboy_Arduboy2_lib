#pragma once

#include <Arduino.h>
#include <Arduboy2.h>
#include "fxdata.h"

#define SET_LEFTY 0x01
#define SET_DEMO  0x02

#define EEPROM_SAVE_OFFSET 64 // Безопасное смещение в EEPROM

// 32-битный Magic Number (Например, "PSTG" в HEX)
#define SAVE_MAGIC 0x50535447

struct SaveData {
    uint32_t magic;
    uint8_t settings;
    uint32_t best[SONG_COUNT][LEVELS];
};

static SaveData saveData;
static uint8_t settings = 0;
static uint32_t best[SONG_COUNT];
static uint8_t best_level = 0xFF;

static void save_load() {
    EEPROM.get(EEPROM_SAVE_OFFSET, saveData);
    
    // Проверка Magic Number одним сравнением 32-битного числа
    if (saveData.magic != SAVE_MAGIC) {
        saveData.magic = SAVE_MAGIC;
        saveData.settings = 0;
        
        // Надежно обнуляем все очки
        memset(saveData.best, 0, sizeof(saveData.best));
        
        EEPROM.put(EEPROM_SAVE_OFFSET, saveData);
        EEPROM.commit(); // Фиксируем изменения во флеш-памяти
    }
    settings = saveData.settings;
}

static void best_fill(uint8_t level) {
    best_level = level;
    for (uint8_t i = 0; i < SONG_COUNT; i++) {
        best[i] = saveData.best[i][level];
    }
}

static void save_settings() {
    saveData.settings = settings;
    EEPROM.put(EEPROM_SAVE_OFFSET, saveData);
    EEPROM.commit(); // Фиксируем изменения настроек
}

static void save_score(uint8_t level, uint8_t index, uint32_t score) {
    if (saveData.best[index][level] < score) {
        saveData.best[index][level] = score;
        best[index] = score;
        
        EEPROM.put(EEPROM_SAVE_OFFSET, saveData);
        EEPROM.commit(); // Фиксируем новый рекорд
    }
}

static void save_clear() {
    memset(saveData.best, 0, sizeof(saveData.best));
    for (uint8_t i = 0; i < SONG_COUNT; i++) best[i] = 0;
    
    EEPROM.put(EEPROM_SAVE_OFFSET, saveData);
    EEPROM.commit(); // Фиксируем обнуление рекордов
}
