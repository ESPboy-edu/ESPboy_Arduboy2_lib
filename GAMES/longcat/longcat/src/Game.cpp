#include "Game.h"

void Game::setup()
{
  this->arduboy.begin();
  this->arduboy.setFrameRate(60);
  this->arduboy.initRandomSeed();

  ArduboyTones(this->arduboy.audio.enabled);

  Save saveValue;
  if (SaveUtil::tryGet(saveValue))
  {
    this->gameContext.stage = saveValue.lastStage;
    this->gameContext.randomDifficulty = saveValue.randomDifficulty;
  }
  else
  {
    this->gameContext.stage = 0;
    this->gameContext.randomDifficulty = 0;

    saveValue.lastStage = 0;
    saveValue.randomDifficulty = 0;
    SaveUtil::update(saveValue);
  }

  LevelUtils::copyStaticLevel(*this);
}

void Game::loop()
{
  if (!this->arduboy.nextFrame())
    return;

  this->arduboy.pollButtons();
  this->arduboy.clear();
  switch (this->gameState)
  {
  case GameState::SplashScreen:
    this->splashScreenState.update(*this);
    this->splashScreenState.render(*this);
    break;

  case GameState::MainMenu:
    this->mainMenuState.update(*this);
    this->mainMenuState.render(*this);
    break;

  case GameState::SelectLevel:
    this->selectLevelState.update(*this);
    this->selectLevelState.render(*this);
    break;

  case GameState::SelectSeed:
    this->selectSeedState.update(*this);
    this->selectSeedState.render(*this);
    break;

  case GameState::LoadLevel:
    this->loadLevelState.update(*this);
    //this->loadLevelState.render(*this);
    break;

  case GameState::GamePlay:
    this->gamePlayState.update(*this);
    this->gamePlayState.render(*this);
    break;

  case GameState::Simulate:
    this->simulateState.update(*this);
    this->simulateState.render(*this);
    break;

  case GameState::CampaignMenu:
    this->campaignMenuState.update(*this);
    this->campaignMenuState.render(*this);
    break;

  case GameState::RandomMenu:
    this->randomMenuState.update(*this);
    this->randomMenuState.render(*this);
    break;

  case GameState::CreditsMenu:
    this->creditsState.update(*this);
    this->creditsState.render(*this);
    break;

  case GameState::OptionsMenu:
    this->optionsMenuState.update(*this);
    this->optionsMenuState.render(*this);
    break;

  case GameState::EndScreen:
    this->endScreenState.update(*this);
    this->endScreenState.render(*this);
    break;

  default:
    this->setGameState(GameState::SplashScreen);
  }

  this->arduboy.display();
}
