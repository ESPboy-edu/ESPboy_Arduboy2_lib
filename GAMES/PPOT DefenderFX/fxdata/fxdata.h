#pragma once

/**** FX data header generated by fxdata-build.py tool version 1.13 ****/

using uint24_t = uint32_t;//__uint24;

// Initialize FX hardware using  FX::begin(FX_DATA_PAGE); in the setup() function.

constexpr uint16_t FX_DATA_PAGE  = 0xfe67;
constexpr uint24_t FX_DATA_BYTES = 100602;

constexpr uint16_t FX_SAVE_PAGE  = 0xfff0;
constexpr uint24_t FX_SAVE_BYTES = 2;

namespace Music
{
  constexpr uint24_t Song_MainTheme = 0x000000;
  constexpr uint24_t SFX_Death = 0x007C1C;
  constexpr uint24_t SFX_Victory = 0x007CC0;
  constexpr uint24_t SFX_XPGain = 0x007D74;
  constexpr uint24_t SFX_Three_Inc = 0x007D88;
  constexpr uint24_t SFX_Three_Dec = 0x007DA4;
  constexpr uint24_t SFX_PlayerBlip = 0x007DC0;
  constexpr uint24_t SFX_EnemyBlip = 0x007DD0;
  constexpr uint24_t SFX_Explosion = 0x007DE0;
  constexpr uint24_t SFX_DoubleBeep = 0x007E24;
}

namespace Images
{
  constexpr uint24_t Player = 0x007E34;
  constexpr uint24_t Player_Thrust = 0x008376;
  constexpr uint24_t Bullets = 0x008A38;
  constexpr uint24_t Enemy_00 = 0x008BA2;
  constexpr uint24_t Enemy_00_Pickup = 0x009384;
  constexpr uint24_t Enemy_01 = 0x00AE1A;
  constexpr uint24_t Enemy_02 = 0x00AFCC;
  constexpr uint24_t Enemy_03 = 0x00B076;
  constexpr uint24_t PPOT = 0x00B1F8;
  constexpr uint24_t Sound_Volume = 0x00E1FA;
  constexpr uint24_t Sound_Checkbox = 0x00EA3C;
  constexpr uint24_t Title = 0x00EA4A;
  constexpr uint24_t Title_Text = 0x01324C;
  constexpr uint24_t GameOver = 0x013E4E;
  constexpr uint24_t Quit = 0x014A50;
  constexpr uint24_t EnemyLanding = 0x014DB2;
  constexpr uint24_t BG_00 = 0x0159B4;
  constexpr uint24_t MG_00 = 0x0164F6;
  constexpr uint24_t FG_00 = 0x017FF8;
}

