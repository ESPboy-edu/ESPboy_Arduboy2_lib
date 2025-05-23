#pragma once

/**** FX data header generated by fxdata-build.py tool version 1.13 ****/

using uint24_t = uint32_t;//__uint24;

// Initialize FX hardware using  FX::begin(FX_DATA_PAGE); in the setup() function.

constexpr uint16_t FX_DATA_PAGE  = 0xfe82;
constexpr uint24_t FX_DATA_BYTES = 93507;

constexpr uint16_t FX_SAVE_PAGE  = 0xfff0;
constexpr uint24_t FX_SAVE_BYTES = 2;

namespace Music
{
  constexpr uint24_t Song_MainTheme = 0x000000;
  constexpr uint24_t SFX_Three_Inc = 0x00179D;
  constexpr uint24_t SFX_Three_Dec = 0x0017B9;
  constexpr uint24_t SFX_Death = 0x0017D5;
  constexpr uint24_t SFX_Victory = 0x001889;
  constexpr uint24_t SFX_XPGain = 0x00193D;
}

namespace Images
{
  constexpr uint24_t Player_01_00 = 0x001951;
  constexpr uint24_t Player_01_01 = 0x001A13;
  constexpr uint24_t Player_01_02 = 0x001ABD;
  constexpr uint24_t Player_01_03 = 0x001B43;
  constexpr uint24_t Player_01_04 = 0x001BB1;
  constexpr uint24_t Player_01_05 = 0x001BFB;
  constexpr uint24_t Player_01_06 = 0x001C39;
  constexpr uint24_t Player_01_07 = 0x001C77;
  constexpr uint24_t Player_01_08 = 0x001CB5;
  constexpr uint24_t Player_01_09 = 0x001CFF;
  constexpr uint24_t Player_01_10 = 0x001D6D;
  constexpr uint24_t Player_01_11 = 0x001DF3;
  constexpr uint24_t Player_01_12 = 0x001E9D;
  constexpr uint24_t Player_00_00 = 0x001F5F;
  constexpr uint24_t Player_00_01 = 0x002021;
  constexpr uint24_t Player_00_02 = 0x0020CB;
  constexpr uint24_t Player_00_03 = 0x002151;
  constexpr uint24_t Player_00_04 = 0x0021BF;
  constexpr uint24_t Player_00_05 = 0x002209;
  constexpr uint24_t Player_00_06 = 0x002247;
  constexpr uint24_t Player_00_07 = 0x002285;
  constexpr uint24_t Player_00_08 = 0x0022C3;
  constexpr uint24_t Player_00_09 = 0x00230D;
  constexpr uint24_t Player_00_10 = 0x00237B;
  constexpr uint24_t Player_00_11 = 0x002401;
  constexpr uint24_t Player_00_12 = 0x0024AB;
  constexpr uint24_t Player = 0x00256D;
  constexpr uint24_t HUD = 0x00258D;
  constexpr uint24_t PPOT = 0x00546F;
  constexpr uint24_t Sound_Volume = 0x008471;
  constexpr uint24_t Sound_Checkbox = 0x008CB3;
  constexpr uint24_t Title = 0x008CC1;
  constexpr uint24_t Numbers_5x3_1D_WB = 0x014CC3;
  constexpr uint24_t Numbers_5x3_2D_WB = 0x014D1F;
  constexpr uint24_t GameOver = 0x015555;
  constexpr uint24_t Plus8 = 0x015C23;
  constexpr uint24_t Quit = 0x0166ED;
  constexpr uint24_t Bubble_00 = 0x016B6F;
  constexpr uint24_t Bubble_01 = 0x016BA1;
  constexpr uint24_t Bubble_02 = 0x016C33;
  constexpr uint24_t Player_Rotate = 0x016CF5;
}

