#pragma once

enum class GameState : uint8_t
{
  //Splash Screen
  SplashScreen,
  //Menu states
  MainMenu,
  CampaignMenu,
  RandomMenu,
  OptionsMenu,
  CreditsMenu,
  //Level Selection
  SelectLevel,
  SelectSeed,
  //Level Loading & Gameplay
  LoadLevel,
  GamePlay,
  Simulate,
  //End GamePlay
  EndScreen
};
