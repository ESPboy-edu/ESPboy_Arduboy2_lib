#pragma once

/**** FX data header generated by fxdata-build.py tool version 1.13 ****/

using uint24_t = uint32_t;//__uint24;

// Initialize FX hardware using  FX::begin(FX_DATA_PAGE); in the setup() function.

constexpr uint16_t FX_DATA_PAGE  = 0xfbd6;
constexpr uint24_t FX_DATA_BYTES = 268784;

constexpr uint16_t FX_SAVE_PAGE  = 0xfff0;
constexpr uint24_t FX_SAVE_BYTES = 2;

namespace Images
{
  constexpr uint24_t PPOT = 0x000000;
  constexpr uint24_t Title = 0x003002;
  constexpr uint24_t HUD_Banana = 0x009004;
  constexpr uint24_t HUD_Time = 0x022A2E;
  constexpr uint24_t HUD_Directions = 0x026D38;
  constexpr uint24_t HUD_Health = 0x02721A;
  constexpr uint24_t Font_Alpha = 0x027F3C;
  constexpr uint24_t Font_Alpha_Big = 0x028AA2;
  constexpr uint24_t Font_Numbers = 0x029EE4;
  constexpr uint24_t HUD_TimesUp = 0x02A22E;
  constexpr uint24_t HUD_GameOver = 0x02A7B8;
  constexpr uint24_t SpinningBananas = 0x02AC9A;
  constexpr uint24_t Player = 0x02B29C;
  constexpr uint24_t Tree1 = 0x02C19E;
  constexpr uint24_t Tree2 = 0x02C6F8;
  constexpr uint24_t Tree3 = 0x02CA2A;
  constexpr uint24_t Tree4 = 0x02CC9C;
  constexpr uint24_t Tree5 = 0x02CF26;
  constexpr uint24_t Tree6 = 0x02D22E;
  constexpr uint24_t Tree7 = 0x02D398;
  constexpr uint24_t Tree8 = 0x02D5FE;
  constexpr uint24_t Foreground = 0x02D756;
  constexpr uint24_t Background = 0x02EF58;
  constexpr uint24_t LowerOnly = 0x02F55A;
  constexpr uint24_t UpperOnly = 0x0303FC;
  constexpr uint24_t Both = 0x0315FE;
  constexpr uint24_t Banana = 0x035200;
  constexpr uint24_t Puff = 0x0352C2;
  constexpr uint24_t Barrel = 0x035D44;
  constexpr uint24_t Bird = 0x036046;
  constexpr uint24_t Spider = 0x036588;
  constexpr uint24_t Heart = 0x036C4A;
  constexpr uint24_t Fire = 0x036C82;
  constexpr uint24_t Fruits = 0x037104;
  constexpr uint24_t Gorilla = 0x037406;
}

namespace Music
{
  constexpr uint24_t Song_MainTheme = 0x03B788;
  constexpr uint24_t SFX_Death = 0x040325;
  constexpr uint24_t SFX_Victory = 0x0403C9;
  constexpr uint24_t SFX_LandHard = 0x04047D;
  constexpr uint24_t SFX_PlayerBlip = 0x040485;
  constexpr uint24_t SFX_EnemyBlip = 0x040495;
  constexpr uint24_t SFX_PickUpItem = 0x0404A5;
  constexpr uint24_t SFX_XPGain = 0x0404B5;
  constexpr uint24_t SFX_Glint = 0x0404C9;
  constexpr uint24_t Songs = 0x0404D1;
  constexpr uint24_t SFXs = 0x0404D4;
}

namespace Constants
{
  constexpr uint24_t StanceImgIdx = 0x0404EC;
  constexpr uint24_t StanceDetails = 0x040630;
  constexpr uint24_t Tree_X = 0x040B40;
  constexpr uint24_t Font_Width = 0x040B50;
  constexpr uint24_t GameOver = 0x040B6A;
  constexpr uint24_t TimesUp = 0x040EFA;
  constexpr uint24_t GetReady = 0x04128A;
  constexpr uint24_t BananaLaunch = 0x041992;
}

namespace Images
{
  constexpr uint24_t TreeImages = 0x0419D8;
}

