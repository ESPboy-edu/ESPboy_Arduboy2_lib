byte animFrame = 0;
bool catchGold = false;
bool inGold = false;
int score = 0;
int getPoint = 0;
int life = 3;
bool ingame = true;
bool subGold = false;
int subMove = 0;
int barcaState = 0;
bool moveSubButton = true;
byte dieFrame = 0;
bool endGame = false;
int HighScore = 0;

#include "OctopusSprite.h"
#include "BarcaSprite.h"
#include "SubSprite.h"

void Game(){
  Sprites::drawOverwrite(0, 0, BackgroundGame, 0);

  arduboy.setCursor(65, 2);
  arduboy.setTextSize(1);
  arduboy.setTextBackground(WHITE);
  arduboy.setTextColor(BLACK);
  arduboy.print(score);
  
  if (arduboy.justPressed(LEFT_BUTTON) && ingame == true && catchGold == false && moveSubButton == true){
    if(subMove >= 2){
      // if sub don't have gold
      --subMove;
      sound.tones(SubSound);
    }else if(subGold == true && subMove >= 1){
      //if sub have gold
      --subMove;
      sound.tones(SubSound);
    }
  }
  
  if (arduboy.justPressed(RIGHT_BUTTON) && ingame == true && catchGold == false && moveSubButton == true){
    if(subMove < 5 && subGold == false){
      ++subMove;
      sound.tones(SubSound);
    }else if(subMove == 5 && subGold == false){
      catchGold = true;
      subMove = 6;
      sound.tones(SubSound);
    }else if(subMove == 5 && subGold == true){
      catchGold = true;
      subMove = 6;
      sound.tones(SubSound);
    }else if(subMove < 5 && subGold == true){
      ++subMove;
      sound.tones(SubSound);
    }
  }

  if (arduboy.justPressed(A_BUTTON) && life  == 0 && ingame == false && endGame == true){
    state = 1;
  }
  if (arduboy.justPressed(B_BUTTON)){
  }

 if(subMove == 0 && subGold == true && inGold == false){
      inGold = true;
 }else{
      inGold = false;
 }

 if(subMove > 0 && life == 3 && subGold == false){
  barcaState = 1;
 }else if(subMove == 0 && life == 3 && subGold == true){
  barcaState = 2;
 }else if(subMove > 0 && life == 2 && subGold == false){
  barcaState = 4;
 }else if(subMove == 0 && life == 2 && subGold == true){
  barcaState = 5;
 }else if(subMove > 0 && life == 1 && subGold == false){
  barcaState = 7;
 }else if(subMove == 0 && life == 1 && subGold == true){
  barcaState = 8;
 }

 if(life == 0 && ingame == false){
  endGame = true;
  Sprites::drawPlusMask(24, 25, GameOverText_mask, 0);
  Sprites::drawPlusMask(35, 36, PressAText_gameover_mask, 0);
  if(score > HighScore){
    HighScore = score;
  }
 }

 if (endGame == false){
  Octopus();
 }
  Barca();
  if(subMove < 6){
    Sub();
  }else{
    animSubGold();
  }
  OctopusCollision();
}
