/**
 * @file Arduboycpp
 * \brief An Arduino library for playing tones and tone sequences,
 * intended for the Arduboy game system.
 */

/*****************************************************************************
  ArduboyTones

An Arduino library to play tones and tone sequences.

Specifically written for use by the Arduboy miniature game system
https://www.arduboy.com/
but could work with other Arduino AVR boards that have 16 bit timer 3
available, by changing the port and bit definintions for the pin(s)
if necessary.

Copyright (c) 2017 Scott Allen

Modified 2022 Simon Holmes

optimized for FX only 2023 Mr.Blinky

rewritten for ESPboy 2023 RomanS

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*****************************************************************************/

// this library is needed to get easy access to the esp82666 timer functions 
#include <Ticker.h>
Ticker tonesTicker;

#include "ArduboyTonesFX.h"

#define uint24_t uint32_t

bool tonesPlaying = false;
static uint16_t *tonesBuffer;
volatile static uint32_t waittime;
volatile static uint32_t tmrcount;

// pointer to a function that indicates if sound is enabled
static bool (*outputEnabled)();


struct TonesBufferFX {
  uint16_t *FX;
  uint8_t  Len;
  uint8_t Head;
};

static TonesBufferFX tonesBufferFX;


void checkTones(){
  if(tmrcount)
    if(millis() > tmrcount + waittime){
      tmrcount = 0;
      ArduboyTonesFX::nextTone();
    }
}


void ArduboyTonesFX::noTone(){
    tonesTicker.detach();
    delete(tonesBuffer);
    tonesPlaying = false;
    tmrcount = 0;
    ::noTone(TONES_PIN); // stop playing
};


ArduboyTonesFX::ArduboyTonesFX(boolean (*outEn)()){
  outputEnabled = outEn;
  tmrcount = 0;
}


ArduboyTonesFX::ArduboyTonesFX(boolean (*outEn)(), uint16_t *tonesArray, uint8_t tonesArrayLen){
  tonesPlaying = true;
  tonesBuffer = (uint16_t *) malloc(MAX_TONES*sizeof(uint16_t));
  tmrcount = 0;
  outputEnabled = outEn;
  tonesBufferFX.Head = 0;
  tonesBufferFX.FX = tonesArray;
  tonesBufferFX.Len = tonesArrayLen;
  tonesTicker.attach_ms(20, checkTones);
  nextTone(); 
}


void ArduboyTonesFX::tonesFromFX(uint24_t tones){
  tonesPlaying = true;
  tonesBuffer = (uint16_t *) malloc(MAX_TONES*sizeof(uint16_t));
  fillBufferFromFX(tones);
  tonesBufferFX.Head = 0;
  tonesBufferFX.FX = tonesBuffer;
  tonesTicker.attach_ms(20, checkTones);
  nextTone();
}


void ArduboyTonesFX::fillBufferFromFX(){};

void ArduboyTonesFX::fillBufferFromFX(uint24_t tones){
  tonesBufferFX.Len = 0;
  FX::seekData(tones);
  for(uint16_t i=0; i<MAX_TONES; i++){
     tonesBuffer[i] = FX::readPendingUInt16();
     tonesBufferFX.Len++;
     if (tonesBuffer[i] == TONES_END || tonesBuffer[i] == TONES_REPEAT) 
       break;
  }
}



void ArduboyTonesFX::volumeMode(uint8_t mode){}


bool ArduboyTonesFX::playing(){
  return tonesPlaying;
}


void ArduboyTonesFX::nextTone(){
  uint16_t freq;
  uint16_t dur;

  freq = getNext(); // get tone frequency

  if (freq == TONES_END || !outputEnabled()) { // if freq is actually an "end of sequence" marker
    noTone(); // stop playing
    return;
  }
  
  if (!freq) 
    ::noTone(TONES_PIN);
  else 
    ::tone(TONES_PIN, freq);
  
  dur = getNext(); // get tone duration
  
  tmrcount = millis();
  waittime = dur;
}


uint16_t ArduboyTonesFX::getNext(){
    uint16_t t;
    if (tonesBufferFX.Head > tonesBufferFX.Len)
      t = TONES_END;
    else
      t = tonesBufferFX.FX[tonesBufferFX.Head];
    if (t == TONES_REPEAT){
      tonesBufferFX.Head = 0;
      t = tonesBufferFX.FX[tonesBufferFX.Head];
    }
    tonesBufferFX.Head++;
    return t;
}

