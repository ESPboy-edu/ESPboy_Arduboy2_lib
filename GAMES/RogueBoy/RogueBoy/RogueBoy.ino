#include <Arduboy2.h>
#include <ArduboyTones.h>
Arduboy2 ard;
Sprites sprites;
ArduboyTones sound(ard.audio.enabled);

int POINTS = 0;
bool Audio = false;
#include "Global.h"
#include "Game.h"





void setup() {
  ard.begin();
  //ard.flashlight();
  ard.audio.begin();
  ard.clear();
  ard.setFrameRate(30);
  ard.initRandomSeed();
  ard.setTextWrap(true);
  Level = 0;

  sprites.drawSelfMasked(CENTERX-26,CENTERY-17,MainLogo,0);
  ard.display();
  delay(1000);
}



void loop() {
  if(!ard.nextFrame())
    return;

  ard.pollButtons();
  ard.clear();

  switch(gameState)
  {
    case GameState::TextTitle: TitleText(); break;
    case GameState::MainMenu: UpdateMainMenu(); break;
    case GameState::Game: UpdateGame(); break;
    case GameState::LoadMap: NextLevelLoad(); break;
    case GameState::MapEnding: MapEnding(); break;
    case GameState::Dead: Death(); break;
    case GameState::WinState: win(); break;
  }
  ard.display();
}
