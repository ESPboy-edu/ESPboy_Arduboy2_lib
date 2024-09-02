/*
MIT License                               https://opensource.org/license/MIT

Copyright (c) 2024 Daniel C (Electro L.I.B)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
/*
      Check out my website for other interesting projects!
                      Ardumania (Arduboy)
               Email: electro_l.i.b@tinyjoypad.com
  https://github.com/phoenixbozo/TinyJoypad/tree/main/TinyJoypad
                    https://WWW.TINYJOYPAD.COM
          https://sites.google.com/view/arduino-collection

*/

#include "SpriteAMania.h"  //Part of Ardumania source code

extern LVL Level;

void UP_SET(void);
void DOWN_SET(void) ;
void RIGHT_SET(void) ;
void LEFT_SET(void);
uint8_t ExploreMapChose(int8_t x_, int8_t y_, uint8_t SriteType_);
uint8_t Select_Byte(uint8_t y_);


class TIMER{
private:
uint8_t Activ;
uint8_t startTime;
uint8_t interval;
public:
void Init(uint8_t Interval_){
startTime = 0;
interval=Interval_;
Activ=0;
}
uint8_t Get_A(void){
return Activ;
}

void Activate(void){
Activ=1;
}

void DeActivate(void){
Activ=0;
}

uint8_t Trigger(void){
if (Activ==0) return 1;
if (startTime<interval) {startTime++;return 0;}else{startTime=0; return 1;}
}
};

class SpriteAmania{
private:
uint8_t SpriteType;
  int8_t SpriteDirection;
  uint8_t DirectionX;
  uint8_t DirectionY;
  int8_t GridX;
  int8_t GridY;
  int8_t DecX;
  int8_t DecY;
  uint8_t Priority;
  uint8_t PadX;
  uint8_t PadY;

  uint8_t JmpPos;
  uint8_t JmpTrig;
  uint8_t JmpSeq;
  uint8_t SpriteMode;
public:

void Init(int8_t x_,int8_t y_,uint8_t SpriteType_){
SpriteType=SpriteType_;
SpriteDirection=0;
DirectionX=2;
DirectionY=2;
GridX=x_;
GridY=y_;
DecX=0;
DecY=0;
Priority=1;
PadX=2;
PadY=2;
SpriteMode=0;
JmpPos=0;
JmpTrig=0;
JmpSeq=0;
}

void Put_SpriteMode(uint8_t Val_){SpriteMode=Val_;}
void Put_JmpPos(uint8_t Val_){JmpPos=Val_;}
void Put_JmpTrig(uint8_t Val_){JmpTrig=Val_;}
void Put_JmpSeq(uint8_t Val_){JmpSeq=Val_;}
void Put_Priority(uint8_t Val_){Priority=Val_;}
void Put_DirectionX(uint8_t val_){DirectionX=val_;}
void Put_DirectionY(uint8_t val_){DirectionY=val_;}
void Put_GridX(int8_t val_){GridX=val_;}
void Put_GridY(int8_t val_){GridY=val_;}
void Put_DecX(int8_t val_){DecX=val_;}
void Put_DecY(int8_t val_){DecY=val_;}
void Put_PadX(int8_t val_){PadX=val_;}
void Put_PadY(int8_t val_){PadY=val_;}

uint8_t Get_SpriteMode(void){return SpriteMode;}
uint8_t Get_JmpPos(void){return JmpPos;}
uint8_t Get_JmpTrig(void){return JmpTrig;}
uint8_t Get_JmpSeq(void){return JmpSeq;}
uint8_t Get_SpriteDirection(void){return SpriteDirection;}
uint8_t Get_DirectionX(void){return DirectionX;}
uint8_t Get_DirectionY(void){return DirectionY;}
int8_t Get_GridX(void){return GridX;}
int8_t Get_GridY(void){return GridY;}
int8_t Get_DecX(void){return DecX;}
int8_t Get_DecY(void){return DecY;}

void AdjustControl(void){
if (DirectionX==1) {if (PadX==0) DirectionX=0;}
if (DirectionX==0) {if (PadX==1) DirectionX=1;}
if (DirectionY==1) {if (PadY==0) DirectionY=0;}
if (DirectionY==0) {if (PadY==1) DirectionY=1;}
if ((DecX==0) && (DecY==0)) {
if (PadX!=2) DirectionX=PadX;
if (PadY!=2) DirectionY=PadY;
switch(Priority){
  case 0:if (CheckPriorityX()) CheckPriorityY();break;
  case 1:if (CheckPriorityY()) CheckPriorityX();break;
  default:break;
}}
PadX=2;
PadY=2;
}

uint8_t CheckPriorityX(void){
if (DirectionX==0) {if (ExploreMapChose(GridX-1, GridY,SpriteType)) {DirectionX=2;return 1;}else{DirectionY=2;return 0;}}
if (DirectionX==1) {if (ExploreMapChose(GridX+1, GridY,SpriteType)) {DirectionX=2;return 1;}else{DirectionY=2;return 0;}}
return 0;
}
uint8_t CheckPriorityY(void){
if (DirectionY==0) {if (ExploreMapChose(GridX, GridY-1,SpriteType)) {DirectionY=2;return 1;}else{DirectionX=2;return 0;}}
if (DirectionY==1) {if (ExploreMapChose(GridX, GridY+1,SpriteType)) {DirectionY=2;return 1;}else{DirectionX=2;return 0;}}
return 0;
}

void RefreshJump(void){
if (JmpTrig==1) {JmpPos=pgm_read_byte(&Saut[JmpSeq]);if (JmpSeq<39){JmpSeq++;}else{JmpSeq=0;JmpTrig=2;}}
}

void RefreshMove(void){
if (JmpTrig==1) {JmpPos=pgm_read_byte(&Saut[JmpSeq]);if (JmpSeq<39){JmpSeq++;}else{JmpSeq=0;JmpTrig=2;}}
switch (DirectionX) {
  case 0:GoLeft();Priority=1;return;break;
  case 1:GoRight();Priority=1;return;break;
  case 2:break;
  default:break;
}
switch (DirectionY) {
  case 0:GoUp();Priority=0;return;break;
  case 1:GoDown();Priority=0;return;break;
  case 2:break;
  default:break;
}
}

void GoUp(void){
  if ((DecX==0)&&(DecY==0)) {if (ExploreMapChose(GridX, GridY-1,SpriteType)) {DirectionY=2;return;}}
      if (DecY>((-YSTEP)+1)) {
      DecY--;}else{
      DecY=0;
      GridY--;
      }
    DecX=-(DecY>>1);
    DirectionX=2;  
     SpriteDirection=9;
}

void GoDown(void){
    if ((DecX==0)&&(DecY==0)) {if (ExploreMapChose(GridX, GridY+1,SpriteType)) {DirectionY=2;return;}}
 if (DecY<0) {
       DecY++;}else{
      DecY=(-YSTEP)+1;
      GridY++;
      }
      DecX=-(DecY>>1);
      DirectionX=2;   
      SpriteDirection=6;
}

void GoRight(void){
    if ((DecX==0)&&(DecY==0)) {if (ExploreMapChose(GridX+1, GridY,SpriteType)) {DirectionX=2;return;}}
      if (DecX<0) {
       DecX++;}else{
      DecX=(-XSTEP)+1;
      if (GridX==(Level.W-1)) {GridX=0;}else{GridX++;}
      }
      DirectionY=2;   
      SpriteDirection=0;
}

void GoLeft(void){
      if ((DecX==0)&&(DecY==0)) {if (ExploreMapChose(GridX-1, GridY,SpriteType)) {DirectionX=2;return;}}
         if (DecX>((-XSTEP)+1)) {
       DecX--;}else{
      DecX=0;
      if (GridX==(0)) {GridX=Level.W-1;}else{GridX--;}
      }
      DirectionY=2; 
        SpriteDirection=3;
      }
};