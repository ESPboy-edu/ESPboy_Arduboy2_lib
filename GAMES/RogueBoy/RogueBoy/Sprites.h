#pragma once
#include <Arduboy2.h>

class SpriteClass{
  public:
    SpriteClass() { };

    void setSprite(uint16_t _x,uint16_t _y,byte _H,byte _type,byte _offset,bool _Active){x = _x;y = _y;H = _H;type = _type;Frame = 0;offset=_offset;Active = _Active;};
    void SpriteAI();
    void Display();
    void Damage();
    byte GetFrame(){return Frame;}
    byte GetType(){return type;}
    bool IsActive(){return Active;}
    void SetActive(bool _k){Active = _k;}
    byte GetX(){return x;}
    byte GetY(){return y;}
  private:
    byte offset;
    uint16_t x;
    uint16_t y;
    short int H;
    byte type;
    byte Frame;
    bool Active;
  };


void SpriteClass::Damage(){
  H -= 10;
  if (H <= 0){Active = false; sound.tone(NOTE_E5,50);}  
  }
  
void SpriteClass::SpriteAI(){
  switch(type){
    case 1: if (ard.everyXFrames(5)) { ++Frame; Frame %= 6; } break;
    case 6:            
    case 7:     
    case 8:   
            if (GetDist(x,y,playerobj.x,playerobj.y) <= 5){
            if ((x < playerobj.x)&&(Walkable(x+1,y))) {x++;}
            if ((x > playerobj.x)&&(Walkable(x-1,y))) {x--;}
            if ((y < playerobj.y)&&(Walkable(x,y+1))) {y++;}
            if ((y > playerobj.y)&&(Walkable(x,y-1))) {y--;}
            }
            break;
    case 9: 
            if (GetDist(x,y,playerobj.x,playerobj.y) < 5){
            if ((x < playerobj.x)&&(Walkable(x+1,y))) {x++;}
            if ((x > playerobj.x)&&(Walkable(x-1,y))) {x--;}
            if ((y < playerobj.y)&&(Walkable(x,y+1))) {y++;}
            if ((y > playerobj.y)&&(Walkable(x,y-1))) {y--;}
            }
            if (ard.everyXFrames(5)) { ++Frame; Frame %= 2; } 
            break;
  }
}


void SpriteClass::Display(){
  int _x = (playerobj.x-x);
  int _y = (playerobj.y-y);
  sprites.drawExternalMask((CENTERX-4)-_x,(CENTERY-4)-_y,SpriteEnviroment,SpriteMask,(Frame+offset),(Frame+offset));
}

