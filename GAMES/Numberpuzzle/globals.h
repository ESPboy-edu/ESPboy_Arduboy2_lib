#ifndef GLOBALS_H
#define GLOBALS_H


#include "Arduboy2.h"
#include "bitmaps.h"


// sound
#define NOTE_A1  110
#define NOTE_C2  131
#define NOTE_G2  196
#define NOTE_C3  262  //beam
#define NOTE_G3  392  //box
#define NOTE_C4  523


// make an instance of arduboy used for many functions
Arduboy2 arduboy;


unsigned long lTime;
unsigned long lTimeNext;

// cursor
int16_t  pWidth = 128;
int16_t  pHeight = 64;

int      pStage = 1;
int16_t  pLives = 0;

bool     pIsStageClear = false;
bool     pIsGameOver = false;




uint8_t  pX = 0;
uint8_t  pY = 0;
uint8_t  pYAdjust = 4;

uint8_t  pCharSize = 8;

uint8_t  pSpeed = 1;
uint8_t  pArrow = 0;
uint8_t  pCount = 0;
uint8_t  pLastPress = 0;

// text scroll
uint8_t  pSX = 0;
uint8_t  pSY = 0;
uint8_t  pSSpeed = 1;


bool     pWait = false;
char     pText[32];


// box
uint8_t concurrent_box_max = 30;

const uint8_t CONCURRENT_BOX_MAX = 30;

struct Box
{
  uint8_t spCount = 0;
  uint8_t qty = 0;
  uint8_t concurrent_box_max = CONCURRENT_BOX_MAX;
} box;


struct Boxs
{
  float x = 64;
  float y = 16;
  uint8_t boxSpeed = 0;
  bool active = false;
  uint8_t kind = 0;
  uint8_t arrow = 0;
};
Boxs boxes[CONCURRENT_BOX_MAX];


#endif
