#define ABG_IMPLEMENTATION
//#define ABG_SYNC_PARK_ROW
//#define ABG_L3_CONVERT_LIGHTEN
#include "ArduboyG.h"

//ArduboyG a;
ArduboyG_Config<ABG_Mode::L3> a;

#include <Arduino.h>
#include <Arduboy2.h>
#include <ATMlib.h>
#include "song.h"
#include "gfx.h"

unsigned char * screenBuffer;
byte px, py, pd, ps = 0;
int frameNumber = 0;
byte curLev[16 * 8];
long int myDelay;
unsigned char numTiles = 0;
unsigned char levelNum = 0;
unsigned char maxLevels = 4;
long int tempTime;
int score = 0;
int lives = 0;
int hiscore = 0;
int gameMode = 0;
char satTime = 0;
byte trkNum = 0;
byte tileAnim=0;
byte menuItem=0;
byte ok2render=0;
int fadingOut = 0;
int fadingIn = 0;
double FADESPEED = 1.5;

void renderLevel(int fn);
uint16_t nown = 0;

// explosion
byte expX[10], expY[10], expF[10], expU[10];
byte explode = 0;

#define _SFX 0
#define _MUSIC 1
byte settings[3]; // settings


// Make an instance of arduboy used for many functions
Arduboy2 arduboy;
ATMsynth ATM;


#define ROW_DRIVE_WAIT_US 150
static constexpr uint8_t FBW = 128;
static constexpr uint8_t FBH = 64;
int FULLBUFF = FBH * FBW / 8;
static constexpr uint8_t FBP = 8;

// Support a minimum panel refresh rate of 120Hz
// This also effectively limits the refresh rate to 120Hz because the controller is not allowed to start
// a new frame faster than the period we set.
static uint16_t timer_counter = ((F_CPU/64)/(120-1));

static void send_cmds(uint8_t const* d, uint8_t n)
{
    Arduboy2::LCDCommandMode();
    while(n-- != 0)
        Arduboy2::SPItransfer(*d++);
    Arduboy2::LCDDataMode();
}

static void send_cmds_prog(uint8_t const* d, uint8_t n)
{
    Arduboy2::LCDCommandMode();
    while(n-- != 0)
        Arduboy2::SPItransfer(pgm_read_byte(d++));
    Arduboy2::LCDDataMode();
}



// Button updates
#define HELD 0
#define NEW 1
#define RELEASE 2
byte CompletePad, ExPad, TempPad;
bool pA[3], pB[3], pUp[3], pDown[3], pLeft[3], pRight[3];

void UPDATEPAD(int pad, int var) {
  pB[pad] = (var & B_BUTTON) >> 2; // 4
  pA[pad] = (var & A_BUTTON) >> 3; // 8
  pDown[pad] = (var & DOWN_BUTTON) >> 4; // 16
  pLeft[pad] = (var & LEFT_BUTTON) >> 5; // 32
  pRight[pad] = (var & RIGHT_BUTTON) >> 6; // 64
  pUp[pad] = (var & UP_BUTTON) >> 7; // 128
}
void UpdatePad(int joy_code)
{
  ExPad = CompletePad;
  CompletePad = joy_code;
  UPDATEPAD(HELD, CompletePad); // held
  UPDATEPAD(RELEASE, (ExPad & (~CompletePad))); // released
  UPDATEPAD(NEW, (CompletePad & (~ExPad))); // newpress
}

void drawTile(int16_t x, int16_t y, const uint8_t *bitmap) {
  int offf = (x * 8) + (y * 128);
  for (byte t = 0; t < 8; t++) {
    screenBuffer[offf++] = pgm_read_byte(bitmap++);
  }
}

void drawTileOffset(int16_t x, int16_t y, const uint8_t *bitmap) {
  for (byte t = 0; t < 8; t++) {
    if (y > 0) {
      byte temp = screenBuffer[(x * 8) + ((y - 1) * 128) + t];
      byte myByte = (pgm_read_byte(bitmap + t) & 0x0F) << 4;
      int offset = (x * 8) + ((y - 1) * 128) + t;
      //screenBuffer[(x * 8) + ((y - 1) * 128) + t] |= myByte;
      screenBuffer[offset] = (screenBuffer[offset]&0x0f) | myByte;
    }
    if (y < 8) {
      byte temp = screenBuffer[(x * 8) + (y * 128) + t];
      byte myByte = (pgm_read_byte(bitmap + t) & 0xF0) >> 4;
      int offset = (x * 8) + (y * 128) + t;
      //screenBuffer[(x * 8) + (y * 128) + t] |= myByte;
      screenBuffer[offset] = (screenBuffer[offset]&0xf0) | myByte;
    }
  }
}
// print text
void print(char x, char y, const char* text, bool color = 1) {
  for (char t = 0; t < strlen(text); t++) {
    char character = text[t] - 32;
    for (byte t1 = 0; t1 < 4; t1++) {
      if(x+t1>=0 && x+t1<=127){screenBuffer[x + (y * 128) + t1] = (pgm_read_byte(myFont + ((character * 3) + t1)) << 2) & 255;}
    }
    if(x+3>=0 && x+3<=127){screenBuffer[x + (y * 128) + 3] = 0;}
    x += 4;
  }
}

void titleprint(char x, char y, const char* text, bool color = 1) {
  
  for (char t = 0; t < 30; t++) {
    int character = (text[t] - 32)*3;
    int offf = x + (y * 128);
    for (byte t1 = 0; t1 < 4; t1++) {
      if(x+t1>=0 && x+t1<=123){
        screenBuffer[offf++] = pgm_read_byte(myFont + character + t1);
      }
    }
    if(x>=0 && x<=127){screenBuffer[x + (y * 128) + 3] = 0;}
    x += 4;
  }
}

// change brightness?
void setContrast(uint8_t contrast)
{
}

void paint(uint8_t image[], bool clear){}
void fadeOut() {}
void fadeIn() {}

void loadLevel(int number) {
  // wait for no dpad input
  while(pUp[HELD]||pDown[HELD]||pLeft[HELD]||pRight[HELD]){
    UpdatePad(arduboy.buttonsState());
  }
  
  numTiles = 0;
  satTime = 0;
  for (int t = 63; t > 0; t--) {
    int s = pgm_read_byte(levels + t + (66 * number));
    curLev[t * 2] = pgm_read_byte(lookUpTile + (s >> 4));
    curLev[(t * 2) + 1] = pgm_read_byte(lookUpTile + (s & 15));
    s = t * 2;
    switch (curLev[s]) {
      case 1:
        numTiles++;
        break;
      case 2:
        numTiles += 2;
        break;
      case 4:
        curLev[s + 1] = 5;
        curLev[s + 16] = 6;
        curLev[s + 17] = 7;
        break;
      case 8:
        curLev[s + 1] = 9;
        break;
      case 10:
        curLev[s + 1] = 11;
        break;
      case 12:
        curLev[s + 16] = 13;
        break;
    }

    s = (t * 2) + 1;
    switch (curLev[s]) {
      case 1:
        numTiles++;
        break;
      case 2:
        numTiles += 2;
        break;
      case 4:
        curLev[s + 1] = 5;
        curLev[s + 16] = 6;
        curLev[s + 17] = 7;
        break;
      case 8:
        curLev[s + 1] = 9;
        break;
      case 10:
        curLev[s + 1] = 11;
        break;
      case 12:
        curLev[s + 16] = 13;
        break;
    }

  }
  int t = (number + 1) * 66;
  px = pgm_read_byte(levels + (t - 2)) * 8;
  py = pgm_read_byte(levels + (t - 1)) * 8;
    ps = 0; pd = 0;
    satTime = 0;
}

void checkTile(int x, int y) {
  int t = curLev[x + 16 * y];
  switch (t) {
    case 1:
      curLev[x + 16 * y] = 0;
      numTiles--;
      for (int t = 0; t < 8; t++) {
        if (expU[t] == 0) {
          expX[t] = px / 8;
          expY[t] = py / 8;
          expF[t] = 0;
          expU[t] = 1;
          score++;
          break;
        }
      }
      break;
    case 2:
      curLev[x + 16 * y] = 1;
      numTiles--;
      score++;
      break;
  }
  if(score > 9999) {
    score = 9999;
  }
  if(score > hiscore) {
    hiscore = score;
  }
}

void movePlayer() {
  if (ps == 0) { // not moving
    // sneeky exit check
    if (curLev[(px / 8) + 16 * (py / 8)] == 10 || curLev[(px / 8) + 16 * (py / 8)] == 11) {
      if (numTiles == 0) {
        levelNum++;
        if (levelNum >= maxLevels) {
          levelNum = 0;
        }
        gameMode=11;
        //fadeOut();
        //loadLevel(levelNum);
        //fadeIn();
      }
    }
    // sneeky water check
    int p = curLev[(px / 8) + 16 * (py / 8)];
    if (p == 0 || (p>=24 && p<=32)) {
      lives--;
      if (lives >= 0) {
        gameMode=11;
        //fadeOut();
        //loadLevel(levelNum);
        //fadeIn();
      } else {
        gameMode = 5;
      }
    }


    pd = 0;
    if (!pB[HELD]) {
      if (pUp[HELD]) {
        if (curLev[(px / 8) + 16 * (py / 8)] != 14 && curLev[(px / 8) + 16 * ((py-8) / 8)] != 14) {
          pd = 1;
          checkTile(px / 8, py / 8);
        }
      }
      if (pDown[HELD]) {
        if (curLev[(px / 8) + 16 * (py / 8)] != 14 && curLev[(px / 8) + 16 * (py / 8)] != 14) {
          pd = 2;
          checkTile(px / 8, py / 8);
        }
      }
      if (pLeft[HELD]) {
        if (curLev[(px / 8) + 16 * (py / 8)] != 15 && curLev[((px-8) / 8) + 16 * (py / 8)] != 15) {
          pd = 3;
          checkTile(px / 8, py / 8);
        }
      }
      if (pRight[HELD]) {
        if (curLev[(px / 8) + 16 * (py / 8)] != 15 && curLev[((px+8) / 8) + 16 * (py / 8)] != 15) {
          pd = 4;
          checkTile(px / 8, py / 8);
        }
      }
      static bool waitingForARelease = false;
      
      if (pA[HELD]) {
        waitingForARelease = true;
      }else{
        if (waitingForARelease==true){
          waitingForARelease = false;
          if (curLev[(px / 8) + 16 * (py / 8)] == 16) { // teleport
            int t = (px / 8) + 16 * (py / 8) + 1;
            while (curLev[t] != 16) {
              t++;
              if (t == 128) {
                t = 0;
              }
            }
            px = (t % 16) * 8;
            py = (t / 16) * 8;
          }
        }
      }
    }
  }

  switch (pd) {
    case 0:
      break;
    case 1:
      py--;
      ps++;
      break;
    case 2:
      py++;
      ps++;
      break;
    case 3:
      px--;
      ps++;
      break;
    case 4:
      px++;
      ps++;
      break;
  }
  if (ps == 8) {
    ps = 0; pd = 0;
    satTime = 0;
  }

}

void renderLevel(int fn) {

  if(gameMode>=10){
    // we clear our screen to black
    //arduboy.clear();
    for (int t = 0; t < 8; t++) {
      if (expU[t] == 1) {
        int tl = expX[t] + 16 * expY[t];
        if (curLev[tl] == 0 || curLev[tl] >= 24) {
          curLev[tl] = expF[t] + 24;
        }
        if (fn % 6 == 0) {
          expF[t]++;
          if (expF[t] == 8) {
            expU[t] = 0;
            if (curLev[tl] >= 24) {
              curLev[tl] = 0;
            }
          }
        }
      }
    }
  
    // animate tiles
    if(fn%13==0){
      tileAnim++;
      if(tileAnim==4){tileAnim=0;}
    }
  
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 16; x++) {
        char tn = (y << 4) + x;
        if (fn & 1) {
          if(curLev[tn]==2){
            drawTileOffset(x, y, animTILE + (tileAnim * 8));
          }else{
            drawTileOffset(x, y, tileGFX + (curLev[tn] * 8));
          }
        } else {
          if(curLev[tn]==2){
            drawTileOffset(x, y, animTILE + (tileAnim * 8) + 32);
          }else{
            drawTileOffset(x, y, tileGFXneg + (curLev[tn] * 8));
          }
        }
      }
    }
    // player sprite
    arduboy.drawBitmap(px, py - 4, tileGFX + (22 * 8), 8, 8, 0);
    arduboy.drawBitmap(px, py - 4, tileGFX + (20 * 8), 8, 8, 1);
  
    // the 'HUD'
    memset(&screenBuffer[7 * 128], 0, 128);
    char text[] = "                               ";
    int temp=levelNum+1;
    if(temp<0){temp=0;}
    sprintf(text, "S:%04d  LVS:%02d  LVL:%02d  Hi:%04d", score, lives, temp, hiscore);
    print(0, 7 , text);
//    arduboy.display();
  
  }//gameMode==10

}


void titleScreen() {  
  static int myInt=0;
  static byte scroller=0;
  static int temp=0;
  
  for(char y=0; y<3; y++){
    for(char x=1; x<15; x++){
      int mt = pgm_read_byte(titleMap+temp);
      //if(mt!=0){
        drawTile(x,y+2, titleTiles + (mt*8));
      //}
      if(temp++>=125){temp=42;}
    }
  }

  char text1[34];
  memcpy_P(text1, &text[myInt],33);
  text1[32]=0;
  titleprint(4-scroller, 6 , text1);

  if(++frameNumber%6==0){
    scroller++;
    if(scroller==4){
      scroller=0;
      myInt++;
      if(myInt==strlen(text)){myInt=0;}
    }
  }
  // sides of scroller
  drawTile(0,6, titleTiles + (808));
  drawTile(31,5, titleTiles + (808));

  if (pA[NEW]) {
    lives = 4;
    score = 0;
    levelNum = 0;
    fadeOut();
    loadLevel(levelNum);
    gameMode = 10;
    scroller=0;
    fadeIn();
  }
  
  //arduboy.display();
}

void playLevel(int fn) {
  if (fn % 4 == 0) {
    movePlayer();
  }

  if (satTime > 16 && (curLev[px / 8 + 16 * py / 8] == 1 || curLev[px / 8 + 16 * py / 8] == 2)) {
    arduboy.setRGBled(4, 0, 0);
  } else {
    arduboy.setRGBled(0, 0, 0);
  }
  if (numTiles == 0) {
    arduboy.setRGBled(0, 4, 0);
  }
  if (pd == 0) {
    if (fn % 4 == 0) {
      satTime++;
      if (satTime == 32) {
        checkTile(px / 8, py / 8);
        satTime = 0;
      }
    }
  }


//  renderLevel();
  
}


void setup() {
  Serial.begin(74880);
  arduboy.boot();
  screenBuffer = arduboy.getBuffer();
  
  arduboy.audio.on();
  //a.begin();
  a.setFrameRate (120);
  a.startGray();

  gameMode = 5; // titlescreen
  maxLevels = sizeof(levels) / 66;

  // This method kicks off the frame ISR that handles refreshing
  // the screen. Usually you would call this at the end of setup().
}

void loop() {
  a.waitForNextPlane();

  renderLevel(nown++);
  
  if(/*a.nextFrame()*/a.needsUpdate()){
    UpdatePad(arduboy.buttonsState());
    switch (gameMode) {
      case 5:
        titleScreen();
        break;
      case 10:
        playLevel(nown);
        break;
      case 11: // loading next level
        fadingOut = 60*FADESPEED;
        gameMode=12;
        break;
      case 12:
        if(fadingOut==0){
          loadLevel(levelNum);
          fadingIn = 0;
          gameMode=14;
        }
        break;
      case 14:
        if(fadingIn==60*FADESPEED){
          gameMode = 10;
        }
        break;
    }
  }
  
  if(fadingOut>0){
    int t=0;
    for(int y=0; y<8; y++){
      for(int x=0; x<16; x++){
        for(int s=0; s<8; s++){
          screenBuffer[t++] &= pgm_read_byte(&gradient[s+(int(fadingOut/FADESPEED)*8)]);
        }
      }
    }
    fadingOut--;
  }
  if(fadingIn<60*FADESPEED){
    int t=0;
    for(int y=0; y<8; y++){
      for(int x=0; x<16; x++){
        for(int s=0; s<8; s++){
          screenBuffer[t++] &= pgm_read_byte(&gradient[s+(int(fadingIn/FADESPEED)*8)]);
        }
      }
    }
    fadingIn++;
  }

  if (!ATM.isPlay()) ATM.play(music);
}
