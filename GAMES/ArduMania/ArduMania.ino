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

#include <Arduboy2.h>         // https://github.com/MLXXXp/Arduboy2
#include <Sprites.h>          // https://github.com/MLXXXp/Arduboy2
#include <ESPboyPlaytune.h>  //https://github.com/Arduboy/ArduboyPlaytune
#include "EngineAMania.h"     //Part of Ardumania source code

#define _delay_ms delay

Arduboy2 arduboy;
ESPboyPlaytune tunes(arduboy.audio.enabled);
Sprites sprites;

const uint8_t* Level_Use[TOTAL_LEVEL] = { &level0[0], &level1[0], &level2[0], &level3[0], &level0[0], &level1[0], &level2[0], &level3[0], &level4[0] };
const uint8_t* Level_Add[TOTAL_LEVEL] = { &level0_Add[0], &level1_Add[0], &level2_Add[0], &level3_Add[0], &level0_Add[0], &level1_Add[0], &level2_Add[0], &level3_Add[0], &level4_Add[0] };
const uint8_t* Blocks[TOTAL_LEVEL] = { &Block0[0], &Block1[0], &Block2[0], &Block3[0], &Block0[0], &Block1[0], &Block2[0], &Block3[0], &Block4[0] };
const uint8_t* BlocksM[TOTAL_LEVEL] = { &Block0m[0], &Block1m[0], &Block2m[0], &Block3m[0], &Block0m[0], &Block1m[0], &Block2m[0], &Block3m[0], &Block4m[0] };

const uint8_t* GhostModeMask[3] = { &ghostmask[0], &ghostmask[0], &GhostEyeMask[0] };
const uint8_t* GhostMode[3] = { &Ghost[0], &GhostBlack[0], &GhostEyes[0] };

const uint8_t* AVATAR[3] = { &ardumania[0], &Ghost[0], &fruit[0] };

SpriteAmania SPKM[ABSOLUTEMAXGHOST];
LVL Level;
uint8_t LevelBuffer[256];
CAM_AMANIA CamGrid;
GameVar GVar;
VAR_GAME_PLAY VGP;
SEQUENTIALANIM SeqAnim;
NUM Num;
FRUIT_VAR FV;
HIGH_SCORE HS[3];

void InitCamGrid(void) {
  CamGrid.IsoScrollX = 0;   //Déplacement fluide en X
  CamGrid.IsoScrollY = 0;   //Déplacement fluide en Y
  CamGrid.ScanScreenX = 0;  //balayage d'écran sur l'axe des X
  CamGrid.ScanScreenY = 0;  //balayage d'écran sur l'axe des Y
  CamGrid.IsoShift = 0;     //calcule le Décalage pour chaque balayage d'affichage isométrique
  CamGrid.DriftGridX = 0;   //Déplacement de la Caméra de la grille en X
  CamGrid.DriftGridY = 0;   //Déplacement de la Caméra de la grille en Y
  CamGrid.CamScanX = 0;     //Portion x de la map à aficher sur le Screen
  CamGrid.CamScanY = 0;     //Portion y de la map à aficher sur le Screen
}

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(50);

  // audio setup
  tunes.initChannel(PIN_SPEAKER_1);
#ifndef AB_DEVKIT
  // if not a DevKit
  tunes.initChannel(PIN_SPEAKER_2);
#else
  // if it's a DevKit
  tunes.initChannel(PIN_SPEAKER_1);  // use the same pin for both channels
  tunes.toneMutesScore(true);        // mute the score when a tone is sounding
#endif
}

void loop() {
  InitFirstBoot();
  LoadHighScoreData();
  //BootIntro();
MAINMENU:;
  Menu();
  GVar.DemiTimer = 0;
  uint8_t Leveluse = 0;
  int16_t TPX = 0;
  int16_t TPY = 0;
  TIMER SHORTSTART;
  TIMER LowCheck;
  SHORTSTART.Init(100);
  SHORTSTART.Activate();
  LowCheck.Init(100);
  LowCheck.Activate();
  arduboy.clear();
  arduboy.display();
  InitNewGame();//!
NEXTLEVEL:;
  uint8_t ShortStart = 1;
  Load_Selected_Level(Leveluse);//!
  Num.ScoresFruit = 0;//!
  InitCamGrid();//!
  AnimLvlChange();//!
RESETPOS:;
  InitResetPos();//!
  ProgramExitMode(4, 1);//!
  Center_Screen();//!
  while (1) {
    ESP.wdtFeed();

    CamGrid.IsoShift = 0;

    arduboy.fillScreen(BLACK);
    for (CamGrid.ScanScreenY = -2; CamGrid.ScanScreenY < SCREEN_BLOCK_H; CamGrid.ScanScreenY++) {
      for (CamGrid.ScanScreenX = 0; CamGrid.ScanScreenX < SCREEN_BLOCK_W; CamGrid.ScanScreenX++) {

        CamGrid.CamScanX = CamGrid.ScanScreenX + CamGrid.DriftGridX;
        CamGrid.CamScanY = CamGrid.ScanScreenY + CamGrid.DriftGridY;
        LoopScreen(&CamGrid.CamScanX);
        uint8_t SPKscan = Read2Bits(CamGrid.CamScanX, CamGrid.CamScanY);
        int16_t PosY = (CamGrid.ScanScreenY * YSTEP) + CamGrid.IsoScrollY;
        int16_t PosX = (CamGrid.ScanScreenX * XSTEP) + CamGrid.IsoShift + CamGrid.IsoScrollX;
        TPX = PosX + FinetuneCenterX;
        TPY = PosY + FinetuneCenterY;
        Out_Check(&SPKscan, CamGrid.CamScanY);

        if (Level.InvertBlock) {
          if (SPKscan != 1) {
            sprites.drawSelfMasked(TPX, TPY, Blocks[Leveluse], 0);
          }
        }
        switch (SPKscan) {
          case (0):
            if ((GVar.GateX == CamGrid.CamScanX) && (GVar.GateY == CamGrid.CamScanY)) { sprites.drawSelfMasked(TPX, TPY, &Gate[0], 0); }
            break;
          case (1):
            if (Level.InvertBlock == 0) {
              sprites.drawErase(TPX, TPY, BlocksM[Leveluse], 0);
              sprites.drawSelfMasked(TPX, TPY, Blocks[Leveluse], 0);
            }
            break;
          case (2):
            sprites.drawErase(TPX + 6, TPY + 4, dotmask, 0);
            sprites.drawSelfMasked(TPX + 6, TPY + 4, tidot, 0);
            break;

          case (3):
            sprites.drawErase(TPX + 4, TPY + 2, BigDotMask, 0);
            sprites.drawSelfMasked(TPX + 4, TPY + 2, bigdot, 0);
            break;

          default:
            break;
        }


        if ((GVar.FruitTimeLeft != 0) && ((CamGrid.CamScanY == Level.fruit.Y) && (CamGrid.CamScanX == Level.fruit.X))) {
          sprites.drawErase(TPX, TPY, fruitMask, FV.FruitUse[FV.RF]);
          sprites.drawSelfMasked(TPX, TPY, fruit, FV.FruitUse[FV.RF]);
        }

        for (uint8_t L = 1; L < GVar.TotalGhost; L++) {
          if (((CamGrid.CamScanY == SPKM[L].Get_GridY())) && (CamGrid.CamScanX == SPKM[L].Get_GridX())) {
            sprites.drawErase(TPX + SPKM[L].Get_DecX() + 2, TPY + SPKM[L].Get_DecY() - SPKM[L].Get_JmpPos(), GhostModeMask[GhostTime(L)], RCupAnimMask(L));
            sprites.drawSelfMasked(TPX + SPKM[L].Get_DecX() + 2, TPY + SPKM[L].Get_DecY() - SPKM[L].Get_JmpPos(), GhostMode[GhostTime(L)], RCupAnim(L));
          }
        }

        if (((CamGrid.CamScanY == SPKM[0].Get_GridY())) && (CamGrid.CamScanX == SPKM[0].Get_GridX())) {
          if ((SPKM[0].Get_DecX() == 0) && (SPKM[0].Get_DecY() == 0)) {
            if (SPKM[0].Get_JmpPos() < 3) {
              switch (SPKscan) {
                case (0): CheckGob((SPKM[0].Get_GridX() == Level.fruit.X), (SPKM[0].Get_GridY() == Level.fruit.Y)); break;
                case (2):
                  CheckGob((SPKM[0].Get_GridX() == Level.fruit.X), (SPKM[0].Get_GridY() == Level.fruit.Y));
                  RemoveDot();
                  SoundSystem(SPKscan);
                  Num.Scores += 15;
                  break;
                case (3):
                  Num.Scores += 37;
                  SwitchModeGhost(0, 1);
                  GVar.GhostTimer = VGP.GDuration;
                  RemoveDot();
                  SoundSystem(SPKscan);
                  break;
                default: break;
              }
            }
          }

          sprites.drawErase(TPX + SPKM[0].Get_DecX() + 2, (TPY + SPKM[0].Get_DecY()) - SPKM[0].Get_JmpPos(), ardumania_m, MaskUse());
          sprites.drawSelfMasked(TPX + SPKM[0].Get_DecX() + 2, (TPY + SPKM[0].Get_DecY()) - SPKM[0].Get_JmpPos(), ardumania, AnimateDeadMania());
        }
      }
      CamGrid.IsoShift = CamGrid.IsoShift - 4;
    }
    if (GVar.FruitTimeLeft != 0) {
      sprites.drawErase(114, 0, fruitMask, FV.FruitUse[FV.RF]);
      sprites.drawSelfMasked(114, 0, fruit, FV.FruitUse[FV.RF]);
    }

    if (GVar.SEQFRM.ActivateSequence == 0) {
      if (collision()) {
        if (SeqAnim.Latch1 == 0) {
          CalculateNum();//!
          Pannel();
          arduboy.display();
          SoundSystem(4);
          _delay_ms(1000);
          SeqAnim.Latch1 = 1;
        }
      }

      if (SeqAnim.Latch1 == 0) {
        for (uint8_t r = 0; r < GVar.HighSpeed; r++) {
          CheckPadFunction();
          RefreshMovingArdu();
        }

        GhostDirectionChoser();
        RefreshMovingGhost();
        GhostJumpCalculate();
      } else {
        RCupDeadAnim();
        _delay_ms(25);
      }
    }

    Center_Screen();//!
    RefreshTimers();//!
    if (LowCheck.Trigger()) {
      ONEUP();
      CalculateNum();//!
    }
    if (ShortStart) {
      sprites.drawErase(43, 36, &player1readyMask[0], 0);
      sprites.drawSelfMasked(43, 36, &player1ready[0], 0);
      if (SHORTSTART.Trigger()) { ShortStart = 0; }
    }
    Pannel();
    FrameRedundancy();
    arduboy.display();


    if ((Level.DotCollected == Level.TotalDot)) {
      ProgramExitMode(2, 0);
    }

    switch (ExitTime()) {
      case 1:
        _delay_ms(400);
        if (GVar.Live > 0) {
          GVar.Live--;
          goto RESETPOS;
        } else {
          CheckNewHighScore();
          goto MAINMENU;
        }
        break;
      case 2:
        _delay_ms(400);
        if (Leveluse < (TOTAL_LEVEL - 1)) {
          Leveluse++;
        } else {
          CheckNewHighScore();
          goto MAINMENU;
        }

        goto NEXTLEVEL;
        break;

      case 4: InitFrameRepeat(); break;//!
      default: break;
    }


    while (!arduboy.nextFrame()) {ESP.wdtFeed();}
  }
}

uint16_t Mymap(float x, float in_min, float in_max, float out_min, float out_max) {
  return roundf((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

void InitFirstBoot(void) { //! démarage
  GVar.Audio = 1;//!
  GVar.Avatar = 0;//!
  InitHSVar();//!
}

void InitFrameRepeat(void) {//!
  GVar.SEQFRM.ActivateSequence = 0;//!
  GVar.SEQFRM.ExitTrigger = 0;//!
  GVar.SEQFRM.ExitCounter = 0;//!
  GVar.SEQFRM.ExitType = 0;//!
  GVar.SEQFRM.FadeInOut = 0;//!
}

void InitNewGame(void) { //new game
  GVar.Live = 3;//!
  Num.OneUp = 0;//!
  Num.Scores = 0;//!
  Num.M10000 = 0;//!
  Num.M1000 = 0;//!
  Num.M100 = 0;//!
  Num.M10 = 0;//!
  Num.M1 = 0;//!
  CalculateNum();//!
}

void InitResetPos(void) {//!
  GVar.FrameAnim = 0;//!
  GVar.Rolling = 0;//!
  GVar.SpeedGhost = 0;//!
  GVar.SpeedJump = 0;//!
  GVar.timer1 = 0;//!
  GVar.FruitTimeLeft = 0;//!
  InitFrameRepeat();//!

  SeqAnim.FrameDraw = 0;//!
  SeqAnim.Latch1 = 0;//!
  SeqAnim.NumPass = 0;//!

  Reset_SpritesPos();//!

  FV.Flip = 0;//!
  GVar.HighSpeed = 1;//!
  GVar.HighSpeedTimer = 0;//!
}

void InitHSVar(void) {//!
  for (uint8_t t = 0; t < 3; t++) {
    HS[t].Avatar = 0;
    HS[t].Score = 0;
  }
}

void InitGamePlay(uint8_t Lvl_) {
  uint8_t ML = TOTAL_LEVEL - 1;
  VGP.ActivateGJump = (Lvl_ > 1) ? 1 : 0;
  VGP.GJump = Mymap(Lvl_, 0, ML, 80, 10);
  VGP.GSpeed = Mymap(Lvl_, 0, ML, 2, 0);
  VGP.GDuration = Mymap(Lvl_, 0, ML, 700, 200);
  GVar.TotalGhost = Mymap(Lvl_, 0, ML, 6, (ABSOLUTEMAXGHOST));
  FV.FruitUse[0] = (Lvl_ < 6) ? Lvl_ : (rand() % 7);
  FV.FruitUse[1] = 6;
  FV.FruitUse[2] = 7;
}

void FILLSCREENSHADE(void) {
  arduboy.fillScreen(0);
  for (uint8_t y = 0; y < 8; y++) {
    for (uint8_t x = 0; x < 64; x++) {
      sprites.drawSelfMasked((x << 1), (y << 3), &demiton[0], 0);
    }
  }
  arduboy.display();
  _delay_ms(1000);
}

void GreenOn(void) {
  digitalWrite(GREEN_LED, LOW);
}
void GreenOff(void) {
  digitalWrite(GREEN_LED, HIGH);
}
void RedOn(void) {
  digitalWrite(RED_LED, LOW);
}
void RedOff(void) {
  digitalWrite(RED_LED, HIGH);
}

void LEDOFF(void) {
  digitalWrite(RED_LED, HIGH);
  digitalWrite(BLUE_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
}

void Snd_Drive(void) {
  tunes.tone(100, 20);
}

void Delay_SND(uint16_t Tm_) {
  TIMER t1;
  t1.Init(200);
  t1.Activate();
  Snd_Drive();
  for (uint16_t t = 0; t < Tm_; t++) {
    if (t1.Trigger()) Snd_Drive();
    _delay_ms(1);
  }
}

void FakeLoad(void) {
  TIMER tm;
  tm.Init(150);
  tm.Activate();
  RedOn();
  _delay_ms(1000);
  FILLSCREENSHADE();
  Snd_Drive();
  arduboy.fillScreen(WHITE);
  arduboy.display();
  _delay_ms(1000);
  RedOff();
  _delay_ms(100);
  GreenOn();
  Delay_SND(100);
  GreenOff();
  _delay_ms(100);
  while (1) {
    ESP.wdtFeed();
    sprites.drawErase(41, 9, BOOTINTRO, 0);
    arduboy.display();
    if ((arduboy.pressed(A_BUTTON) == 1) || (arduboy.pressed(B_BUTTON) == 1)) {
      Snd_Drive();
      GreenOn();
      _delay_ms(10);
      break;
    }
    if (tm.Trigger()) {
      Snd_Drive();
      GreenOn();
    } else {
      GreenOff();
    }
    while (!arduboy.nextFrame()) {ESP.wdtFeed();}
  }
  GreenOff();
  _delay_ms(1400);
  GreenOn();
  Delay_SND(300);
  GreenOff();
  _delay_ms(400);
  arduboy.fillScreen(WHITE);
  arduboy.display();
  _delay_ms(400);
  GreenOn();
  Delay_SND(600);
  FILLSCREENSHADE();
  GreenOff();
  _delay_ms(300);
  GreenOn();
  Delay_SND(100);
  GreenOff();
  _delay_ms(500);
  GreenOn();
  _delay_ms(200);
  GreenOff();
  _delay_ms(800);
  GreenOn();
  Delay_SND(800);
  GreenOff();
  arduboy.fillScreen(WHITE);
  arduboy.display();
  _delay_ms(1400);
}

void BootIntro(void) {
  _delay_ms(100);
  FakeLoad();
  Fail();
}

void Fail(void) {
  TIMER tm1;
  uint8_t Flip = 1;
  tm1.Init(40);
  tm1.Activate();

  while (1) {
    ESP.wdtFeed();
    if (tm1.Trigger()) { Flip = (Flip) ? 0 : 1; }
    arduboy.fillScreen(0);
    sprites.drawSelfMasked(0, 0, TEXT, 0);
    if (Flip) {
      RedOn();
      sprites.drawSelfMasked(0, 0, FRAME, 0);
    } else {
      RedOff();
    }
    arduboy.display();
    if ((arduboy.pressed(A_BUTTON) == 1) || (arduboy.pressed(B_BUTTON) == 1)) {
      while (1) {
        ESP.wdtFeed();
        if ((arduboy.pressed(A_BUTTON) == 0) && (arduboy.pressed(B_BUTTON) == 0)) {
          LEDOFF();
          goto Start_;
        }
      }
    }
    while (!arduboy.nextFrame())ESP.wdtFeed();;
  }
Start_:;

  LEDOFF();
  arduboy.fillScreen(0);
}

void DrawBar(uint8_t Scr) {
  sprites.drawSelfMasked(8, pgm_read_byte(&wave[Scr]) + 2, &bar[0], 0);
  sprites.drawErase(8, pgm_read_byte(&wave[Scr]) + 2, &bar[0], 1);
}

void DrawArduMania(uint8_t Scr) {
  sprites.drawSelfMasked(26 + (pgm_read_byte(&wave[Scr]) >> 2), 6, &ArduMania[0], 0);
  sprites.drawErase(26 + (pgm_read_byte(&wave[Scr]) >> 2), 6, &ArduMania[0], 1);
}

void Menu(void) {
  uint8_t OneClick = 0;
  uint8_t OneClick2 = 0;
  uint8_t x_ = 0;
  uint8_t Scr = 0;
  uint8_t Scr2 = 11;
  uint8_t CursorPos = 31;
  uint8_t Pos = 31;
  uint8_t Order[3];
  while (1) {
    ESP.wdtFeed();
    x_ = (x_ < 7) ? x_ + 1 : 0;
    arduboy.fillScreen(0);
    sprites.drawSelfMasked(6, 0, &MAIN[0], 0);
    for (uint8_t y_ = 0; y_ < 63; y_ += 8) {
      sprites.drawSelfMasked(0, y_, &scroll[0], x_);
      sprites.drawSelfMasked(123, y_, &scroll[0], x_);
    }

    if ((Scr < 14) && (Scr2 < 14)) {
      Order[0] = 1;
      Order[1] = 2;
      Order[2] = 0;
    } else if (Scr < 14) {
      Order[0] = 1;
      Order[1] = 0;
      Order[2] = 2;
    } else if (Scr2 < 14) {
      Order[0] = 2;
      Order[1] = 0;
      Order[2] = 1;
    } else {
      Order[0] = 0;
      Order[1] = 1;
      Order[2] = 2;
    }
    for (uint8_t t = 0; t < 3; t++) {
      switch (Order[t]) {
        case 0: DrawArduMania(Scr); break;
        case 1: DrawBar(Scr); break;
        case 2: DrawBar(Scr2); break;
        default: break;
      }
    }
    _delay_ms(26);

    Scr = (Scr < 28) ? Scr + 1 : 0;
    Scr2 = (Scr2 < 28) ? Scr2 + 1 : 0;
    RandVar();
    if ((Pos == CursorPos)) {
      if ((arduboy.pressed(DOWN_BUTTON) == 1)) {

        if (CursorPos < 49) {
          SoundSystem(5);
          CursorPos = CursorPos + 9;
        }
      }
      if ((arduboy.pressed(UP_BUTTON) == 1)) {

        if (CursorPos > 31) {
          SoundSystem(5);
          CursorPos = CursorPos - 9;
        }
      }
    } else {

      Pos = (Pos < CursorPos) ? Pos + 1 : Pos;
      Pos = (Pos > CursorPos) ? Pos - 1 : Pos;
    }

    sprites.drawSelfMasked(26, Pos, AVATAR[GVar.Avatar], 0);
    if (GVar.Audio) { sprites.drawSelfMasked(77, 51, &audio[0], 0); }
    arduboy.display();
    if (((arduboy.pressed(A_BUTTON) == 1) || (arduboy.pressed(B_BUTTON) == 1)) && OneClick) {
      OneClick = 0;
      SoundSystem(5);
      _delay_ms(30);
      switch (Pos) {
        case 31: goto Start_; break;
        case 40:
          ScoreMenu();
          goto Continue;
          break;
        case 49:
          if (GVar.Audio) {
            GVar.Audio = 0;
            arduboy.audio.off();
          } else {
            GVar.Audio = 1;
            arduboy.audio.on();
          }
          goto Continue;
          break;
        default: goto Continue; break;
      }
    }
    if ((arduboy.pressed(A_BUTTON) == 0) && (arduboy.pressed(B_BUTTON) == 0)) { OneClick = 1; }
Continue:;

    if ((arduboy.pressed(RIGHT_BUTTON) == 1) && (OneClick2)) {
      OneClick2 = 0;
      if (GVar.Avatar < 2) {
        SoundSystem(5);
        GVar.Avatar++;
      }
    }

    else if ((arduboy.pressed(LEFT_BUTTON) == 1) && (OneClick2)) {
      OneClick2 = 0;
      if (GVar.Avatar > 0) {
        SoundSystem(5);
        GVar.Avatar--;
      }
    }
    if ((arduboy.pressed(RIGHT_BUTTON) == 0) && (arduboy.pressed(LEFT_BUTTON) == 0)) {
      OneClick2 = 1;
    }

    while (!arduboy.nextFrame()) {ESP.wdtFeed();}
  }
Start_:;
}

void ForcedSaveEEPROM(void) {
  ClassementHighScore();
  SaveMarqueurEEPROM();
  for (uint8_t t = 0; t < 3; t++) {
    SaveSlotEEPROM(t, HS[t].Avatar, HS[t].Score);
  }
  EEPROM.commit();
}

void CheckNewHighScore(void) {
  if (Num.Scores > HS[2].Score) {
    HS[2].Score = Num.Scores;
    HS[2].Avatar = GVar.Avatar;
    ForcedSaveEEPROM();
    ScoreMenu();
  }
}

void ClassementHighScore() {
  uint8_t i, j;
  HIGH_SCORE temp;

  for (i = 0; i < 3; i++) {
    for (j = i + 1; j < 3; j++) {
      if (HS[j].Score > HS[i].Score) {
        temp = HS[i];
        HS[i] = HS[j];
        HS[j] = temp;
      }
    }
  }
}

uint8_t READEEPROM(uint8_t Adress_) {
#define ADUMANIA_START_ADRESS (EEPROM_STORAGE_SPACE_START + 20)
  return EEPROM.read(ADUMANIA_START_ADRESS + Adress_);
}

uint8_t CheckMarqueurEEPROM(void) {
#define MARQEUR 84
#define ARDUMANIA_START_ADRESS (EEPROM_STORAGE_SPACE_START + 20)
  if (EEPROM.read(ADUMANIA_START_ADRESS) == MARQEUR) {
    return 1;
  } else {
    return 0;
  }
}

void SaveMarqueurEEPROM(void) {
#define MARQEUR 84
#define ARDUMANIA_START_ADRESS (EEPROM_STORAGE_SPACE_START + 20)
  EEPROM.write(ADUMANIA_START_ADRESS, MARQEUR);
  EEPROM.commit();
}

void LoadSlotEEPROM(uint8_t Slot_) {
#define Slot ((Slot_ * 3) + 1)
#define ARDUMANIA_START_ADRESS (EEPROM_STORAGE_SPACE_START + 20)
  HS[Slot_].Avatar = EEPROM.read(ADUMANIA_START_ADRESS + Slot);
  HS[Slot_].Score = EEPROM.read(ADUMANIA_START_ADRESS + Slot + 1);
  HS[Slot_].Score |= EEPROM.read(ADUMANIA_START_ADRESS + (Slot + 2)) << 8;
}

void SaveSlotEEPROM(uint8_t Slot_, uint8_t Avatar_, uint16_t Valeur) {
#define Slot ((Slot_ * 3) + 1)
#define ARDUMANIA_START_ADRESS (EEPROM_STORAGE_SPACE_START + 20)

  EEPROM.write(ADUMANIA_START_ADRESS + Slot, (Avatar_));
  EEPROM.write(ADUMANIA_START_ADRESS + Slot + 1, (Valeur & 0x00ff));
  EEPROM.write(ADUMANIA_START_ADRESS + (Slot + 2), (Valeur & 0xff00) >> 8);
  EEPROM.commit();
}

void LoadHighScoreData(void) {
  if (CheckMarqueurEEPROM()) {
    for (uint8_t t = 0; t < 3; t++) {
      LoadSlotEEPROM(t);
    }
  }
}

void RestHighScore(void) {
  InitHSVar();//!
  ForcedSaveEEPROM();
}

void ScoreMenu(void) {
  uint8_t OneClick2 = 0;
  uint8_t x_ = 0;
  while (1) {
    ESP.wdtFeed();
    x_ = (x_ > 0) ? x_ - 1 : 7;
    arduboy.fillScreen(0);

    for (uint8_t y_ = 0; y_ < 63; y_ += 8) {
      sprites.drawSelfMasked(0, y_, &scroll[0], x_);
      sprites.drawSelfMasked(123, y_, &scroll[0], x_);
    }
    sprites.drawSelfMasked(26, 4, &HighScore[0], 0);
    Num.Scores = HS[0].Score;
    ScorePannel(49, 26);
    Num.Scores = HS[1].Score;
    ScorePannel(49, 40);
    Num.Scores = HS[2].Score;
    ScorePannel(49, 54);
    if (HS[0].Score != 0) sprites.drawSelfMasked(36, 22, AVATAR[HS[0].Avatar], 0);
    if (HS[1].Score != 0) sprites.drawSelfMasked(36, 36, AVATAR[HS[1].Avatar], 0);
    if (HS[2].Score != 0) sprites.drawSelfMasked(36, 49, AVATAR[HS[2].Avatar], 0);

    arduboy.display();
    if ((arduboy.pressed(LEFT_BUTTON) == 1) && (arduboy.pressed(RIGHT_BUTTON) == 1)) {
      TIMER trig;
      trig.Init(125);
      trig.Activate();
      while (1) {
        ESP.wdtFeed();
        SoundSystem(5);
        if ((arduboy.pressed(LEFT_BUTTON) == 0) || (arduboy.pressed(RIGHT_BUTTON) == 0)) { goto Continue; }
        if (trig.Trigger()) {
          RestHighScore();
          goto Continue;
        }
        while (!arduboy.nextFrame())ESP.wdtFeed();;
      }
    }

    if (((arduboy.pressed(A_BUTTON) == 1) || (arduboy.pressed(B_BUTTON) == 1)) && (OneClick2 == 1)) {
      SoundSystem(5);
      goto Start_;
    }
Continue:;
    if ((arduboy.pressed(A_BUTTON) == 0) && (arduboy.pressed(B_BUTTON) == 0)) {
      OneClick2 = 1;
    }

    while (!arduboy.nextFrame()) {ESP.wdtFeed();}
  }
Start_:;
}

void AnimLvlChange(void) {//!
  uint8_t x_ = 0;
  uint8_t D = 0;
  uint8_t anim_ = 0;
  int16_t A = 127;
  TIMER tm_;
  tm_.Init(3);
  tm_.Activate();
  tunes.playScore(score);
  while (1) {
    ESP.wdtFeed();
    while (!arduboy.nextFrame()) {ESP.wdtFeed();}
    x_ = (x_ < 7) ? x_ + 1 : 0;
    arduboy.fillScreen(0);

    for (uint8_t y_ = 0; y_ < 10; y_++) {
      sprites.drawSelfMasked((5 + (y_ * 12)), 20, &Plate[0], 0);
    }

    for (uint8_t y_ = 0; y_ < 8; y_++) {
      sprites.drawErase(5, y_ << 3, &Black[0], 0);
      sprites.drawErase(122, y_ << 3, &Black[0], 0);
    }


    sprites.drawErase(A, 23, &ardumania_m[0], 0);
    sprites.drawSelfMasked(A, 23, &ardumania[0], anim_ + ((D == 0) ? 3 : 0));
    if (D == 1) {
      for (uint8_t y_ = 0; y_ < (GVar.TotalGhost - 1); y_++) {
        sprites.drawErase((A - 30) - (y_ * 20), 23, &ghostmask[0], 0);
        sprites.drawSelfMasked((A - 30) - (y_ * 20), 23, &Ghost[0], anim_ + ((D == 0) ? 3 : 0));
      }
    }

    for (uint8_t y_ = 0; y_ < 63; y_ += 8) {
      sprites.drawOverwrite(0, y_, &scroll[0], x_);
      sprites.drawOverwrite(123, y_, &scroll[0], x_);
    }
    if (D == 0) {
      if (A > -16) {
        A--;
      } else {
        D = 1;
      }
    } else {
      if (A < (157 + ((GVar.TotalGhost - 1) * 20))) {
        A += 2;
      } else {
        break;
      }
    }
    if (tm_.Trigger()) {
      if (anim_ < 2) {
        anim_++;
      } else {
        anim_ = 0;
      }
    }
    arduboy.display();
  }
}

void ScorePannel(uint8_t x_, uint8_t y_) {
  CalculateNum();//!
  sprites.drawSelfMasked(22 + x_, y_, police, Num.M1);
  sprites.drawSelfMasked(18 + x_, y_, police, Num.M10);
  sprites.drawSelfMasked(14 + x_, y_, police, Num.M100);
  sprites.drawSelfMasked(10 + x_, y_, police, Num.M1000);
  sprites.drawSelfMasked(6 + x_, y_, police, Num.M10000);
}

void Pannel(void) {
  const uint8_t y_ = 57;
  const uint8_t y2_ = 54;
  sprites.drawOverwrite(102, y2_, Digital, 0);
  sprites.drawOverwrite(0, y2_, Dlive, 0);
  sprites.drawSelfMasked(10, y_, police, GVar.Live);
  ScorePannel(100, y_);
  UpdateFruitDelivery();
}

void CalculateNum(void) {//!
  Num.M10000 = (Num.Scores / 10000);
  Num.M1000 = (((Num.Scores) - (Num.M10000 * 10000)) / 1000);
  Num.M100 = (((Num.Scores) - (Num.M1000 * 1000) - (Num.M10000 * 10000)) / 100);
  Num.M10 = (((Num.Scores) - (Num.M100 * 100) - (Num.M1000 * 1000) - (Num.M10000 * 10000)) / 10);
  Num.M1 = ((Num.Scores) - (Num.M10 * 10) - (Num.M100 * 100) - (Num.M1000 * 1000) - (Num.M10000 * 10000));
}

void CheckGob(uint8_t a, uint8_t b) {
  if (!a) return;
  if (!b) return;
  if (GVar.FruitTimeLeft) GobFruit();
}

void ActivateHighSpeed(void) {
  GVar.HighSpeed = 2;
  GVar.HighSpeedTimer = 500;
}

void ProgramExitMode(uint8_t EXITMODE, uint8_t FADE_INOUT) {
  if (GVar.SEQFRM.ActivateSequence == 0) {
    _delay_ms(400);
    GVar.SEQFRM.ActivateSequence = 1;
    GVar.SEQFRM.ExitType = EXITMODE;
    GVar.SEQFRM.ExitCounter = (FADE_INOUT) ? 7 : 0;
    GVar.SEQFRM.FadeInOut = FADE_INOUT;
  }
}

uint8_t ExitTime(void) {
  if (GVar.SEQFRM.ExitTrigger == 1) { return GVar.SEQFRM.ExitType; }
  return 0;
}

void FrameRedundancy(void) {
  if (GVar.SEQFRM.ActivateSequence) {
    switch (GVar.SEQFRM.FadeInOut) {
      case 0:
        if (GVar.SEQFRM.ExitCounter < 7) {
          GVar.SEQFRM.ExitCounter++;
        } else {
          GVar.SEQFRM.ExitTrigger = 1;
        }
        break;
      case 1:
        if (GVar.SEQFRM.ExitCounter > 0) {
          GVar.SEQFRM.ExitCounter--;
        } else {
          GVar.SEQFRM.ExitTrigger = 1;
        }
        break;
    }
    Fade_SCR();
  }
}

void Fade_SCR(void) {
  if (GVar.SEQFRM.ActivateSequence) {
    for (uint8_t y = 0; y < 8; y++) {
      for (uint8_t x = 0; x < 128; x++) {
        sprites.drawErase(x, y << 3, fadescreen, GVar.SEQFRM.ExitCounter);
      }
    }
  }
}

void GobFruit(void) {
  GVar.FruitTimeLeft = 0;
  SoundSystem(0);
  Num.Scores += 250;
  switch (FV.RF) {
    case 1:
      SwitchModeGhost(0, 1);
      GVar.GhostTimer = VGP.GDuration;
      break;
    case 2: ActivateHighSpeed(); break;
    default: break;
  }
}

void ONEUP(void) {
  if (Num.OneUp != Num.M10000) {
    Num.OneUp = Num.M10000;
    GVar.Live++;
  }
}

void UpdateFruitDelivery(void) {
  if ((Level.DotCollected - Num.ScoresFruit) > 35) {
    Num.ScoresFruit += 35;
    LiberateFruit();
  }
}

void LiberateFruit(void) {
  GVar.FruitTimeLeft = FRUITTIMELEFT;
  if (FV.Flip == 0) {
    FV.RF = 0;
    FV.Flip = 1;
  } else {
    FV.RF = (RandVar()) ? 2 : 1;
    FV.Flip = 0;
  }
}

void SoundSystem(uint8_t Val_) {
  switch (Val_) {
    case 0:
      for (uint16_t Sn1 = 10; Sn1 < 4000; Sn1 += 20) { tunes.tone(Sn1, 10); }
      break;
    case 1:
      for (uint16_t Sn1 = 10; Sn1 < 800; Sn1++) {
        tunes.tone(Sn1, 10);
        tunes.tone(Sn1 + 100, 10);
      }
      break;
    case 2:
      tunes.tone(1000, 14);
      tunes.tone(200, 14);
      tunes.tone(1000, 14);
      break;
    case 3:
      tunes.tone(1400, 8);
      tunes.tone(400, 8);
      tunes.tone(1400, 8);
      break;
    case 4: deadSound(); break;
    case 5: tunes.tone(2000, 10); break;
    default: break;
  }
}

void deadSound(void) {
  for (uint16_t Sn1 = 1000; Sn1 < 2000; Sn1++) {
    tunes.tone(Sn1, 2);
    tunes.tone(Sn1 + 100, 2);
  }
}

void GhostJumpCalculate(void) {
  if (VGP.ActivateGJump == 0) return;
  if (GVar.SpeedJump != 0) {
    return;
  }
  for (uint8_t t = 1; t < 3; t++) {  //2 firsts ghost only
    int8_t Val_ = abs(SPKM[t].Get_GridX() - SPKM[0].Get_GridX());
    int8_t Val2_ = abs(SPKM[t].Get_GridY() - SPKM[0].Get_GridY());
    if (Val_ <= 1 && Val2_ <= 1 && SPKM[0].Get_JmpSeq() > 3) {
      if (SPKM[t].Get_JmpTrig() == 0) {
        SPKM[t].Put_JmpTrig(1);
        return;
      }
    }
    if (SPKM[t].Get_JmpTrig() == 2) {
      SPKM[t].Put_JmpTrig(0);
    }
  }
}

uint8_t AnimateDeadMania(void) {

  if (SeqAnim.Latch1 == 0) {
    return RCupAnim(0);
  } else {
    if (GVar.SEQFRM.ExitCounter == 0) tunes.tone((20 - SeqAnim.FrameDraw) * 100, 20);
    return SeqAnim.FrameDraw + 12;
  }
}

void RCupDeadAnim(void) {
  if (SeqAnim.NumPass < 25) {

    SeqAnim.Latch1 = (SeqAnim.Latch1 < 4) ? SeqAnim.Latch1 + 1 : 1;
    SeqAnim.FrameDraw = (SeqAnim.Latch1 - 1);
    SeqAnim.NumPass++;
    return;
  } else {

    if (SeqAnim.Latch1 < 13) { SeqAnim.Latch1++; }
    SeqAnim.FrameDraw = (SeqAnim.Latch1 - 1);
    if (SeqAnim.Latch1 == 13) { ProgramExitMode(1, 0); }
    return;
  }
}

void RefreshMovingArdu(void) {
  SPKM[0].AdjustControl();
  SPKM[0].RefreshMove();
}

void CheckPadFunction(void) {
  if (arduboy.pressed(UP_BUTTON) == 1) {
    SPKM[0].Put_PadY(0);
  } else if (arduboy.pressed(DOWN_BUTTON) == 1) {
    SPKM[0].Put_PadY(1);
  } else {
    SPKM[0].Put_PadY(2);
  }
  if (arduboy.pressed(RIGHT_BUTTON) == 1) {
    SPKM[0].Put_PadX(1);
  } else if (arduboy.pressed(LEFT_BUTTON) == 1) {
    SPKM[0].Put_PadX(0);
  } else {
    SPKM[0].Put_PadX(2);
  }

  if ((arduboy.pressed(A_BUTTON) == 1) || (arduboy.pressed(B_BUTTON) == 1)) {
    if (SPKM[0].Get_JmpTrig() == 0) { SPKM[0].Put_JmpTrig(1); }
  } else {
    if (SPKM[0].Get_JmpTrig() == 2) SPKM[0].Put_JmpTrig(0);
  }
}

uint8_t MaskUse(void) {
  if (SeqAnim.Latch1 > 4) { return 2; }
  return Level.InvertBlock;
}

void LoopScreen(int8_t* x_) {
  if (*x_ > (Level.W - 1)) { *x_ = *x_ - Level.W; }
  if (*x_ < 0) { *x_ = Level.W + *x_; }
}

uint8_t GhostTime(uint8_t t_) {
  if ((GVar.GhostTimer > 0) && (GVar.GhostTimer < 125) && (SPKM[t_].Get_SpriteMode() == 1)) {
    if (GVar.DemiTimer < 3) { return 0; }
  }
  return SPKM[t_].Get_SpriteMode();
}

void RefreshTimers(void) {

  if (GVar.SpeedJump < VGP.GJump) {
    GVar.SpeedJump++;
  } else {
    GVar.SpeedJump = 0;
  }
  if (GVar.SpeedGhost < VGP.GSpeed) {
    GVar.SpeedGhost++;
  } else {
    GVar.SpeedGhost = 0;
  }
  if (GVar.timer1 < 3) {
    GVar.timer1++;
  } else {
    GVar.timer1 = 0;
    if (GVar.FrameAnim != 2) {
      GVar.FrameAnim++;
    } else {
      GVar.FrameAnim = 0;
    }
  }

  if (GVar.GhostTimer > 1) { GVar.GhostTimer--; }
  {
    if (GVar.GhostTimer == 1) { SwitchModeGhost(1, 0); }
  }
  GVar.DemiTimer = (GVar.DemiTimer < 5) ? GVar.DemiTimer + 1 : 0;

  if (GVar.FruitTimeLeft > 0) {
    GVar.FruitTimeLeft--;
  }

  if (GVar.HighSpeedTimer > 0) {
    GVar.HighSpeedTimer--;
    if (GVar.HighSpeedTimer == 0) { GVar.HighSpeed = 1; }
  }
}

void RemoveDot(void) {
  Initialize2Bits(CamGrid.CamScanX, CamGrid.CamScanY);
  Level.DotCollected++;
}

void SwitchModeGhost(uint8_t Tested_, uint8_t Set_) {
  for (uint8_t t = 1; t < GVar.TotalGhost; t++) {
    if (SPKM[t].Get_SpriteMode() == Tested_) {
      SPKM[t].Put_SpriteMode(Set_);
      GhostUturn(t);
    }
  }
}

void GhostUturn(uint8_t t) {
  switch (SPKM[t].Get_SpriteDirection()) {
    case 0:
      SPKM[t].Put_PadX(0);
      break;
    case 3:
      SPKM[t].Put_PadX(1);
      break;
    case 6:
      SPKM[t].Put_PadY(0);
      break;
    case 9:
      SPKM[t].Put_PadY(1);
      break;
    default: break;
  }
}

uint8_t collision(void) {
#define sprite1x ((SPKM[0].Get_GridX() * (XSTEP - 1)) + SPKM[0].Get_DecX())
#define sprite1y ((SPKM[0].Get_GridY() * (YSTEP - 1)) + SPKM[0].Get_DecY())
#define sprite2x(T_) ((SPKM[T_].Get_GridX() * (XSTEP - 1)) + SPKM[T_].Get_DecX())
#define sprite2y(T_) ((SPKM[T_].Get_GridY() * (YSTEP - 1)) + SPKM[T_].Get_DecY())

  for (uint8_t t = 1; t < GVar.TotalGhost; t++) {

    int16_t x_distance = sprite1x - sprite2x(t);
    int16_t y_distance = sprite1y - sprite2y(t);
    int8_t z_distance = (SPKM[0].Get_JmpPos()) - (SPKM[t].Get_JmpPos());
    if (abs(x_distance) <= 6 && abs(y_distance) <= 6) {
      if (abs(z_distance) <= 4) {
        if (SPKM[t].Get_SpriteMode() == 0) {
          return 1;
        } else {
          if (SPKM[t].Get_SpriteMode() != 2) {
            SoundSystem(1);
            _delay_ms(250);
            Num.Scores += 125;
            SPKM[t].Put_SpriteMode(2);
            return 0;
          }
        }
      }
    }
  }
  return 0;
}

void GhostRespawn(uint8_t val_) {
  if ((SPKM[val_].Get_GridX() == pgm_read_byte(&Level.level_add[14])) && (SPKM[val_].Get_GridY() == pgm_read_byte(&Level.level_add[15]))) {
    if (SPKM[val_].Get_SpriteMode() == 2) SPKM[val_].Put_SpriteMode(0);
  }
}

void GhostDirectionChoser(void) {
  for (uint8_t t = 1; t < GVar.TotalGhost; t++) {
    if ((SPKM[t].Get_DecX() == 0) && (SPKM[t].Get_DecY() == 0)) {
      GhostRespawn(t);
      switch (SPKM[t].Get_SpriteDirection()) {
        case (0): GhostRight(t); break;
        case (3): GhostLeft(t); break;
        case (6): GhostDown(t); break;
        case (9): GhostUp(t); break;
        default: break;
      }
    }
  }
}

uint8_t RNDGhost(uint8_t t_) {
  if (SPKM[t_].Get_SpriteMode() == 1) {
    return RandVar();
  }
  return 0;
}

uint8_t TrackObjectifX(uint8_t Val_) {

  if ((SPKM[Val_].Get_SpriteMode() == 2) || (RNDGhost(Val_))) {
    return pgm_read_byte(&Level.level_add[12]);
  } else {
    return SPKM[0].Get_GridX();
  }
}

uint8_t TrackObjectifY(uint8_t Val_) {
  if ((SPKM[Val_].Get_SpriteMode() == 2) || (RNDGhost(Val_))) {
    return pgm_read_byte(&Level.level_add[13]);
  } else {
    return SPKM[0].Get_GridY();
  }
}

uint8_t GateExit(uint8_t Val_) {
  if ((SPKM[Val_].Get_GridX() == Level.GateExit.X) && (SPKM[Val_].Get_GridY() == Level.GateExit.Y)) {
    return 0;
  }
  return 1;
}

void GhostRight(uint8_t t) {
  uint8_t ADd_ = 0;
  ADd_ += (Read2Bits(SPKM[t].Get_GridX() + 1, SPKM[t].Get_GridY()) == 1) ? 1 : 0;
  ADd_ += (Read2Bits(SPKM[t].Get_GridX(), SPKM[t].Get_GridY() - 1) == 1) ? 3 : 0;
  ADd_ += (Read2Bits(SPKM[t].Get_GridX(), SPKM[t].Get_GridY() + 1) == 1) ? 5 : 0;

  if (ADd_ == 9) {
    SPKM[t].Put_PadX(0);
    return;
  }
  if (ADd_ == 8) {
    SPKM[t].Put_PadX(1);
    return;
  }
  if (ADd_ == 4) {
    SPKM[t].Put_PadY(1);
    return;
  }
  if (ADd_ == 6) {
    SPKM[t].Put_PadY(0);
    return;
  }
  if (ADd_ == 1) {
    SPKM[t].Put_PadY(RandVar());
    return;
  }

  if (ADd_ == 0) {
    if (RandVar()) { SPKM[t].Put_PadY((SPKM[t].Get_GridY() < TrackObjectifY(t)) ? GateExit(t) : 0); }
  }
  if (ADd_ == 3) {
    if ((SPKM[t].Get_GridY() < TrackObjectifY(t)) ? 0 : 1) {
      SPKM[t].Put_PadX(1);
      return;
    } else {
      SPKM[t].Put_PadY(1);
      return;
    }
  }
  if (ADd_ == 5) {
    if ((SPKM[t].Get_GridY() > TrackObjectifY(t)) ? 0 : 1) {
      SPKM[t].Put_PadX(1);
      return;
    } else {
      SPKM[t].Put_PadY(0);
      return;
    }
  }
}

void GhostLeft(uint8_t t) {
  uint8_t ADd_ = 0;
  ADd_ += (Read2Bits(SPKM[t].Get_GridX() - 1, SPKM[t].Get_GridY()) == 1) ? 1 : 0;
  ADd_ += (Read2Bits(SPKM[t].Get_GridX(), SPKM[t].Get_GridY() - 1) == 1) ? 3 : 0;
  ADd_ += (Read2Bits(SPKM[t].Get_GridX(), SPKM[t].Get_GridY() + 1) == 1) ? 5 : 0;

  if (ADd_ == 9) {
    SPKM[t].Put_PadX(1);
    return;
  }
  if (ADd_ == 8) {
    SPKM[t].Put_PadX(0);
    return;
  }
  if (ADd_ == 4) {
    SPKM[t].Put_PadY(1);
    return;
  }
  if (ADd_ == 6) {
    SPKM[t].Put_PadY(0);
    return;
  }
  if (ADd_ == 1) { SPKM[t].Put_PadY(RandVar()); }
  if (ADd_ == 0) {
    if (RandVar()) { SPKM[t].Put_PadY((SPKM[t].Get_GridY() < TrackObjectifY(t)) ? GateExit(t) : 0); }
  }
  if (ADd_ == 3) {
    if ((SPKM[t].Get_GridY() < TrackObjectifY(t)) ? 0 : 1) {
      SPKM[t].Put_PadX(0);
      return;
    } else {
      SPKM[t].Put_PadY(1);
      return;
    }
  }
  if (ADd_ == 5) {
    if ((SPKM[t].Get_GridY() > TrackObjectifY(t)) ? 0 : 1) {
      SPKM[t].Put_PadX(0);
      return;
    } else {
      SPKM[t].Put_PadY(0);
      return;
    }
  }
}

void GhostUp(uint8_t t) {
  uint8_t ADd_ = 0;
  ADd_ += (Read2Bits(SPKM[t].Get_GridX(), SPKM[t].Get_GridY() - 1) == 1) ? 1 : 0;
  ADd_ += (Read2Bits(SPKM[t].Get_GridX() - 1, SPKM[t].Get_GridY()) == 1) ? 3 : 0;
  ADd_ += (Read2Bits(SPKM[t].Get_GridX() + 1, SPKM[t].Get_GridY()) == 1) ? 5 : 0;

  if (ADd_ == 9) {
    SPKM[t].Put_PadY(1);
    return;
  }
  if (ADd_ == 8) {
    SPKM[t].Put_PadY(0);
    return;
  }
  if (ADd_ == 4) {
    SPKM[t].Put_PadX(1);
    return;
  }
  if (ADd_ == 6) {
    SPKM[t].Put_PadX(0);
    return;
  }
  if (ADd_ == 1) { SPKM[t].Put_PadX(RandVar()); }
  if (ADd_ == 0) {
    if (RandVar()) { SPKM[t].Put_PadX((SPKM[t].Get_GridX() < TrackObjectifX(t)) ? 1 : 0); }
  }
  if (ADd_ == 3) {
    if ((SPKM[t].Get_GridX() < TrackObjectifX(t)) ? 0 : 1) {
      SPKM[t].Put_PadY(0);
      return;
    } else {
      SPKM[t].Put_PadX(1);
      return;
    }
  }
  if (ADd_ == 5) {
    if ((SPKM[t].Get_GridX() > TrackObjectifX(t)) ? 0 : 1) {
      SPKM[t].Put_PadY(0);
      return;
    } else {
      SPKM[t].Put_PadX(0);
      return;
    }
  }
}

void GhostDown(uint8_t t) {
  uint8_t ADd_ = 0;
  ADd_ += (Read2Bits(SPKM[t].Get_GridX(), SPKM[t].Get_GridY() + 1) == 1) ? 1 : 0;
  ADd_ += (Read2Bits(SPKM[t].Get_GridX() - 1, SPKM[t].Get_GridY()) == 1) ? 3 : 0;
  ADd_ += (Read2Bits(SPKM[t].Get_GridX() + 1, SPKM[t].Get_GridY()) == 1) ? 5 : 0;

  if (ADd_ == 9) {
    SPKM[t].Put_PadY(0);
    return;
  }
  if (ADd_ == 8) {
    SPKM[t].Put_PadY(1);
    return;
  }
  if (ADd_ == 4) {
    SPKM[t].Put_PadX(1);
    return;
  }
  if (ADd_ == 6) {
    SPKM[t].Put_PadX(0);
    return;
  }
  if (ADd_ == 1) { SPKM[t].Put_PadX(RandVar()); }
  if (ADd_ == 0) {
    if (RandVar()) { SPKM[t].Put_PadX((SPKM[t].Get_GridX() < TrackObjectifX(t)) ? 1 : 0); }
  }
  if (ADd_ == 3) {
    if ((SPKM[t].Get_GridX() < TrackObjectifX(t)) ? 0 : 1) {
      SPKM[t].Put_PadY(1);
      return;
    } else {
      SPKM[t].Put_PadX(1);
      return;
    }
  }
  if (ADd_ == 5) {
    if ((SPKM[t].Get_GridX() > TrackObjectifX(t)) ? 0 : 1) {
      SPKM[t].Put_PadY(1);
      return;
    } else {
      SPKM[t].Put_PadX(0);
      return;
    }
  }
}

void RefreshMovingGhost(void) {
  for (uint8_t L = 1; L < GVar.TotalGhost; L++) {
    if ((GVar.SpeedGhost == 0) || (SPKM[L].Get_SpriteMode() == 2)) {
      SPKM[L].AdjustControl();
      SPKM[L].RefreshMove();
    } else {
      SPKM[L].RefreshJump();
    }
  }
}

uint8_t RandVar(void) {
  GVar.Rolling = (GVar.Rolling > 0) ? GVar.Rolling - 1 : 23;
  return pgm_read_byte(&rndmove[GVar.Rolling]);
}

uint8_t FruitActif(int8_t x_, int8_t y_) {
  if (Level.fruit.X == x_ && Level.fruit.Y == y_) { return 1; }
  return 0;
}

void InitLevelBuffer(void) {
  for (uint16_t t = 0; t < 256; t++) {
    LevelBuffer[t] = 0x00;
  }
}

uint16_t Total_Dot_Count_test(void) {
  uint16_t Add_ = 0;
  uint8_t MaxX = Level.W;
  uint8_t MaxY = Level.H;
  for (uint16_t y_ = 0; y_ < MaxY; y_++) {
    for (uint16_t x_ = 0; x_ < MaxX; x_++) {
      uint8_t val_ = Read2Bits(x_, y_);
      if ((val_ == 2) || (val_ == 3)) Add_++;
    }
  }
  return Add_;
}

void BigDotAssign(void) {
  uint8_t level_add_offset = 0;
  for (uint8_t t = 0; t < 4; t++) {
    Level.bigdot[t].X = pgm_read_byte(&Level.level_add[level_add_offset]);
    Level.bigdot[t].Y = pgm_read_byte(&Level.level_add[level_add_offset + 1]);
    Define2Bits(Level.bigdot[t].X, Level.bigdot[t].Y, 3);
    level_add_offset += 2;
  }
}

void Initialize2Bits(int8_t x_, int8_t y_) {
  if (x_ > (Level.W - 1) || y_ > (Level.H - 1) || x_ < 0 || y_ < 0) return;

  uint8_t ByteNo = Select_Byte_2Bits(y_);
  uint8_t BitScrool = ((y_ - (ByteNo << 2)) << 1);
  LevelBuffer[x_ + (ByteNo * Level.W)] &= ~(0b11 << BitScrool);
}

uint8_t Read2Bits(int8_t x_, int8_t y_) {
  int8_t x1 = (x_ < 0) ? (Level.W + x_) : (x_ > Level.W - 1) ? (x_ - Level.W): x_;

  if (y_ > (Level.H - 1) || y_ < 0) return 1;
  uint8_t ByteNo = Select_Byte_2Bits(y_);
  uint8_t ByTe = LevelBuffer[x1 + (ByteNo * Level.W)];
  uint8_t BitScrool = ((y_ - (ByteNo << 2)) << 1);
  uint8_t BitValue = (ByTe & (0b11 << BitScrool));
  return (BitValue >> BitScrool);
}

void Define2Bits(int8_t x_, int8_t y_, uint8_t initValue) {
  if (x_ > (Level.W - 1) || y_ > (Level.H - 1) || x_ < 0 || y_ < 0) return;
  uint8_t ByteNo = Select_Byte_2Bits(y_);
  uint8_t BitScrool = ((y_ - (ByteNo << 2)) << 1);
  uint8_t mask = ~(0b11 << BitScrool);
  uint8_t valueToSet = (initValue << BitScrool) & (0b11 << BitScrool);
  LevelBuffer[x_ + (ByteNo * Level.W)] = (LevelBuffer[x_ + (ByteNo * Level.W)] & mask) | valueToSet;
}

void CopyLevelToMem_test(void) {
  uint8_t MaxX = Level.W;
  uint8_t MaxY = Level.H;
  for (uint8_t y = 0; y < MaxY; y++) {
    for (uint8_t x = 0; x < MaxX; x++) {
      Define2Bits(x, y, (ExploreMap(x, y) == 1) ? 1 : 2);
    }
  }
}

void Reset_SpritesPos(void) {//!
  SPKM[0].Init(Level.Main.X, Level.Main.Y, 1);
  SetGhost();
  GVar.GhostTimer = 0;
}

void RemoveDotNotUse(void) {
  Define2Bits(Level.Main.X, Level.Main.Y, 0);
}

void Load_Selected_Level(uint8_t level_to_Load) {
  InitLevelBuffer();
  Level.TotalDot = 0;
  Level.DotCollected = 0;
  Level.level = Level_Use[level_to_Load];
  Level.level_add = Level_Add[level_to_Load];
  Level.W = pgm_read_byte(&Level.level[0]);
  Level.H = pgm_read_byte(&Level.level[1]);
  InitGamePlay(level_to_Load);
  Level.InvertBlock = InvertBlock(level_to_Load);
  CopyLevelToMem_test();
  Level.fruit.X = pgm_read_byte(&Level.level_add[8]);
  Level.fruit.Y = pgm_read_byte(&Level.level_add[9]);
  Level.Main.X = pgm_read_byte(&Level.level_add[10]);
  Level.Main.Y = pgm_read_byte(&Level.level_add[11]);
  BigDotAssign();
  Reset_SpritesPos();//!
  DeleteSerialDot();
  RemoveDotNotUse();
  Level.TotalDot = Total_Dot_Count_test();
  GVar.GateX = pgm_read_byte(&Level.level_add[14]);
  GVar.GateY = pgm_read_byte(&Level.level_add[15]);
  Level.GateExit.X = GVar.GateX;
  Level.GateExit.Y = GVar.GateY + 1;
}

void DeleteSerialDot(void) {
  uint8_t max_ = pgm_read_byte(&Level.level_add[16]);
  for (uint8_t x = 0; x < max_; x++) {
    uint8_t Val_ = pgm_read_byte(&Level.level_add[17 + x]);
    uint8_t Vertical = trunc(Val_ / Level.W);
    uint8_t Horizontal = (Val_ - (Vertical * Level.W));
    Define2Bits(Horizontal, Vertical, 0);
  }
}

void SetGhost(void) {
  for (uint8_t t = 1; t < GVar.TotalGhost; t++) {
    SPKM[t].Init(pgm_read_byte(&Level.level_add[12]), pgm_read_byte(&Level.level_add[13]), 0);
  }
}

void Center_Screen(void) {//!
  CamGrid.IsoScrollY = -SPKM[0].Get_DecY();
  CamGrid.DriftGridY = (SPKM[0].Get_GridY() - 3);
  if (CamGrid.IsoScrollY != 0) CamGrid.IsoScrollX = -(CamGrid.IsoScrollY >> 1);
  CamGrid.IsoScrollX = -SPKM[0].Get_DecX();
  CamGrid.DriftGridX = (SPKM[0].Get_GridX() - 6);
  RandVar();
}

uint8_t RCupAnimMask(uint8_t d_) {
  if (SPKM[d_].Get_SpriteMode() == 2) {
    return RCupAnim(d_);
  }
  return 0;
}

int8_t RCupAnim(uint8_t d_) {
  return GVar.FrameAnim + SPKM[d_].Get_SpriteDirection();
}

uint8_t InvertBlock(uint8_t Lvl_) {
  switch (Lvl_) {
    case 3:
    case 7: return 1; break;
    default: return 0; break;
  }
}

void Out_Check(uint8_t* val, int8_t y) {
  if ((y < 0) || (y > (Level.H - 1))) *val = 0;
}

uint8_t ExploreMapChose(int8_t x_, int8_t y_, uint8_t SriteType_) {
  if ((SriteType_) && (x_ == GVar.GateX) && (y_ == GVar.GateY)) {
    return 1;
  }
  return ExploreMap(x_, y_);
}

uint8_t ExploreMap(int8_t x_, int8_t y_) {
  uint8_t MaxX = Level.W;

  int8_t x1 = (x_ < 0) ? (Level.W + x_) : (x_ > Level.W - 1) ? (x_ - Level.W)
                                                             : x_;

  uint8_t ByteNo = Select_Byte(y_);
  uint8_t ByTe = pgm_read_byte(&Level.level[(x1 + (ByteNo * MaxX)) + 2]);

  return ((ByTe & (1 << (y_ - (ByteNo << 3)))) == 0) ? 0 : 1;
}

uint8_t Select_Byte(uint8_t y_) {
  return (y_ >> 3);
}

uint8_t Select_Byte_2Bits(uint8_t y_) {
  return (y_ >> 2);
}
