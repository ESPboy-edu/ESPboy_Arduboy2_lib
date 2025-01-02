#pragma once

#include <Arduboy2.h>

#include "Maps.h"


enum class GameState { MainMenu, Settings, Game ,LoadMap ,GameOver, MapEnding, TextTitle, Dead, WinState};
GameState gameState = GameState::TextTitle;

class PlayerClass {
  public:
  uint16_t x;
  uint16_t y;
  byte d;
  char H;
  byte Keys;
  byte Coins;
  byte Kill;
  bool Moving;
  uint8_t Frame;
  void PlayerMovement();
};

PlayerClass playerobj;

class BulletClass{
  public:
  BulletClass(){}  
  void Update();
  void Display();
  bool GetActive() {return Active;}
  byte GetX(){return x;}
  byte GetY(){return y;}
  void Kill(){Active = false;}
  void SetBullet(uint16_t _x,uint16_t _y,byte _d){x = _x; y = _y; d = _d; Active = true;}
  private:
  uint16_t x;
  uint16_t y;
  byte d;
  bool Active;
};


void BulletClass::Update(){
  if (!Active){return;}
  unsigned rx = x;
  unsigned ry = y;
  switch(d){
    case 0: ry-=3; break;
    case 1: ry+=3; break;
    case 2: rx+=3; break;
    case 3: rx-=3; break;
    };
  if (Walkable(rx,ry)) {
    x = rx;
    y = ry;
    }
  else {
    switch(d){
    case 0: ry-=5; break;
    case 1: ry+=5; break;
    case 2: rx+=5; break;
    case 3: rx-=5; break;
    };
    
    if (GetBlock(GetTileX(rx),GetTileY(ry))==4){BarrelBreak(GetTileX(rx),GetTileY(ry));} 
    Active = false;
    }
}

void BulletClass::Display(){
  if (!Active){return;}
  int _x = (playerobj.x-x);
  int _y = (playerobj.y-y);
  sprites.drawExternalMask((CENTERX-4)-_x,(CENTERY-4)-_y,SpriteEnviroment,SpriteMask,(18+d),(18+d));
}

BulletClass Bullet[6];




void PlayerClass::PlayerMovement() {
  Moving = false;
  if (ard.pressed(UP_BUTTON)){
    if (Walkable(x,y-2)){
      y-=2;
      Moving = true;
    }
    d = 0;
    }
  if (ard.pressed(DOWN_BUTTON)){
    if (Walkable(x,y+2)){
      y+=2;
      Moving = true;
    }
    d = 1;
    }
  if (ard.pressed(RIGHT_BUTTON)){
    if (Walkable(x+2,y)){
      x+=2;
      Moving = true;
    }
    d = 2;
    }
  if (ard.pressed(LEFT_BUTTON)){
    if (Walkable(x-2,y)){
      x-=2;
      Moving = true;
    }
    d = 3;
    }   

  if (Moving) {    int relx = GetTileX(x);
                   int rely = GetTileY(y);
                   uint8_t ofx = GetTileXOffset(x);
                   uint8_t ofy = GetTileYOffset(y);
                   uint8_t bl = GetBlock(relx,rely);
                   if (Between(3,3,11,11,ofx,ofy)&&bl == PRESS_PLATE){
                    SetBlock(relx,rely,RUBBLE);
                    UpdateEBlock(relx,rely);
                    sound.tone(NOTE_D2,150,NOTE_E2,50);
                   }
              }
  /* 
  if (ard.pressed(A_BUTTON+B_BUTTON)){
    gameState = GameState::MainMenu;
    Level = 0;
  }
  */
  if (ard.justPressed(B_BUTTON)){
    for (byte i=0;i<6;i++){
      if (Bullet[i].GetActive() == false){
        Bullet[i].SetBullet(x,y,d);
        sound.tone(NOTE_F2H,50);
        break;
      }
    }
  
  }
  
  if (ard.justPressed(A_BUTTON)){
    int relx = GetTileX(x);
    int rely = GetTileY(y);
    uint8_t bl = GetBlock(relx,rely);
    if (bl == DOWN_STAIRS){
      sound.tone(NOTE_C3,100,NOTE_E3,100,NOTE_G3,100);
      gameState = GameState::MapEnding;
    } 
    else 
    {
        switch(d){
          case 0: rely--; break;
          case 1: rely++; break;
          case 2: relx++; break;
          case 3: relx--; break;
          }
        bl = GetBlock(relx,rely);
        switch(bl){
          case SWITCH_ON: SetBlock(relx,rely,SWITCH_OFF); UpdateEBlock(relx,rely); sound.tone(NOTE_D3,50,NOTE_E5,50); break;
          case SWITCH_OFF: SetBlock(relx,rely,SWITCH_ON); UpdateEBlock(relx,rely); sound.tone(NOTE_D5,50,NOTE_E3,50); break;
          case CLOSED_CHEST: SetBlock(relx,rely,OPEN_CHEST); Keys++; sound.tone(NOTE_D3,50,NOTE_E5,50); break;
          case LOCKED_DOOR: if (Keys > 0) {SetBlock(relx,rely,OPEN_DOOR); Keys--; sound.tone(NOTE_D3,50,NOTE_E5,50);} else {sound.tone(NOTE_D2,150,NOTE_E2,50);}  break;
          case LOCKED_STAIRS: if (Keys > 0) {SetBlock(relx,rely,DOWN_STAIRS); Keys--; sound.tone(NOTE_D3,50,NOTE_E5,50);} else {sound.tone(NOTE_D2,150,NOTE_E2,50);} break;
        }
    }
  
  }
}




