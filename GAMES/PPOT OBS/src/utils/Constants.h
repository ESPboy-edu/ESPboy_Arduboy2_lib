#pragma once

#define SOUNDS

namespace Constants {

  constexpr uint8_t StarField_Size = 10;
  constexpr uint8_t SmallAsteroid_Size = 4;
  constexpr uint8_t LargeAsteroid_Size = 2;
  constexpr uint8_t Enemy_Size = 3;
  constexpr uint8_t Health_Factor = 8;
  constexpr uint8_t ScoreDistance = 96;
  constexpr uint8_t Bullet_Count = 2;
  constexpr uint8_t Bullet_None = 255;

  constexpr uint16_t EEPROM_Char1 = 60;
  constexpr uint16_t EEPROM_Char2 = EEPROM_Char1 + 1;
  constexpr uint16_t EEPROM_Scores = EEPROM_Char1 + 2;

  constexpr uint8_t Enemy_Path_Large[] = { 26, 24, 22, 20, 18, 16, 14, 12, 10, 9, 8, 7, 6, 5, 4, 4, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 42, 44, 45, 46, 47, 48, 49, 50, 50, 51, 51, 52, 52, 52, 52, 52, 52, 51, 51, 50, 50, 49, 49, 48, 47, 45, 44, 42, 41, 40, 38, 37, 35, 34, 32, 30, 28, };
  constexpr uint8_t Enemy_Path_Medium[] = { 15, 14, 13, 11, 10, 8, 7, 6, 5, 4, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 25, 26, 26, 26, 27, 27, 27, 27, 27, 26, 26, 26, 25, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, };
  constexpr uint8_t Enemy_Path_Small[] = { 18, 7, 6, 5, 4, 3, 3, 2, 2, 1, 1, 1, 1, 2, 2, 3, 3, 4, 5, 6, 7, 8, 9, 9, 10, 10, 11, 11, 11, 11, 10, 10, 9, 9, 8, 7, };

};

