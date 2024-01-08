/**
 * @file ArduboyTonesFX.cpp
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

#include <Ticker.h>
Ticker tonesTicker;

#include "ArduboyTonesFX.h"

#define TONE_PIN D3

// pointer to a function that indicates if sound is enabled
static bool (*outputEnabled)();

#undef TONES_VOLUME_CONTROL

static volatile long durationToggleCount = 0;
static volatile bool tonesPlaying = false;
static volatile bool toneSilent;

static volatile uint16_t *tonesStart;
static volatile uint16_t *tonesIndex;
static volatile uint16_t toneSequence[MAX_TONES * 2 + 1];

static volatile bool inProgmem;

static volatile uint24_t tonesBufferFX_Start;
static volatile uint24_t tonesBufferFX_Curr;
static volatile uint16_t *tonesBufferFX;
static uint8_t tonesBufferFX_Len;
static uint8_t tonesBufferFX_Head;
static int8_t tonesBufferFX_Tail;
static volatile uint8_t toneMode;

static volatile uint32_t waittime;
static volatile uint32_t tmrcount;

ArduboyTonesFX::ArduboyTonesFX(boolean (*outEn)())
{
  outputEnabled = outEn;
  toneSequence[MAX_TONES * 2] = TONES_END;
  // sets the update call interval
}


void ArduboyTonesFX::checkTones(){
  if(tmrcount != 0){
    if(millis() > tmrcount + waittime){
      tmrcount = 0;
      nextTone();
    }
   }
}



ArduboyTonesFX::ArduboyTonesFX(boolean (*outEn)(), uint16_t *tonesArray, uint8_t tonesArrayLen)
{
  outputEnabled = outEn;
  tonesBufferFX = tonesArray;
  tonesBufferFX_Len = tonesArrayLen;
  toneSequence[MAX_TONES * 2] = TONES_END;
  waittime=0;
  tmrcount=0;
  tonesTicker.attach(0.01, checkTones);
}

void ArduboyTonesFX::tone(uint16_t freq, uint16_t dur)
{
  inProgmem = false;
  tonesStart = tonesIndex = toneSequence; // set to start of sequence array
  toneSequence[0] = freq;
  toneSequence[1] = dur;
  toneSequence[2] = TONES_END; // set end marker
  toneMode = TONES_MODE_NORMAL;
  nextTone(); // start playing
}

void ArduboyTonesFX::tone(uint16_t freq1, uint16_t dur1, uint16_t freq2, uint16_t dur2)
{
  inProgmem = false;
  tonesStart = tonesIndex = toneSequence; // set to start of sequence array
  toneSequence[0] = freq1;
  toneSequence[1] = dur1;
  toneSequence[2] = freq2;
  toneSequence[3] = dur2;
  toneSequence[4] = TONES_END; // set end marker
  toneMode = TONES_MODE_NORMAL;
  nextTone(); // start playing
}

void ArduboyTonesFX::tone(uint16_t freq1, uint16_t dur1,uint16_t freq2, uint16_t dur2,uint16_t freq3, uint16_t dur3)
{
  inProgmem = false;
  tonesStart = tonesIndex = toneSequence; // set to start of sequence array
  toneSequence[0] = freq1;
  toneSequence[1] = dur1;
  toneSequence[2] = freq2;
  toneSequence[3] = dur2;
  toneSequence[4] = freq3;
  toneSequence[5] = dur3;
  toneMode = TONES_MODE_NORMAL;
  nextTone(); // start playing
}

void ArduboyTonesFX::tones(const uint16_t *tones)
{
  inProgmem = true;
  tonesStart = tonesIndex = (uint16_t *)tones; // set to start of sequence array
  toneMode = TONES_MODE_NORMAL;
  nextTone(); // start playing
}

void ArduboyTonesFX::tonesInRAM(uint16_t *tones)
{
  inProgmem = false;
  tonesStart = tonesIndex = tones; // set to start of sequence array
  toneMode = TONES_MODE_NORMAL;
  nextTone(); // start playing
}

void ArduboyTonesFX::tonesFromFX(uint24_t tones)
{
  inProgmem = false;
  tonesBufferFX_Start = tonesBufferFX_Curr = tones; // set to start of sequence array
  toneMode = TONES_MODE_FX;
  tonesBufferFX_Head = 0;
  tonesBufferFX_Tail = -1;
  tonesPlaying = true;
  fillBufferFromFX();
  nextTone(); // start playing
}


void ArduboyTonesFX::fillBufferFromFX()
{
    if (tonesPlaying && tonesBufferFX_Head != tonesBufferFX_Tail) {

      uint8_t head = tonesBufferFX_Head;

      FX::seekData(tonesBufferFX_Curr);

      while ((head % tonesBufferFX_Len) != tonesBufferFX_Tail) {
        uint16_t t = FX::readPendingUInt16();
        tonesBufferFX[head % tonesBufferFX_Len] = t;
        head++;
        tonesBufferFX_Curr = tonesBufferFX_Curr + 2;

        if (t == TONES_REPEAT) {
          tonesBufferFX_Curr = tonesBufferFX_Start;
          FX::readEnd();
          FX::seekData(tonesBufferFX_Curr);
        }

        if (tonesBufferFX_Tail == -1) tonesBufferFX_Tail = 0;

      }

      tonesBufferFX_Head = head % tonesBufferFX_Len;
      FX::readEnd();
    }
}

/*
void ArduboyTonesFX::fillBufferFromFX()
{
    if (tonesPlaying && tonesBufferFX_Head != tonesBufferFX_Tail) {

      uint8_t head = tonesBufferFX_Head;

      FX::seekData(tonesBufferFX_Curr);

      while ((head % tonesBufferFX_Len) != tonesBufferFX_Tail) {
        uint16_t t = FX::readPendingUInt8();
		t |= FX::readPendingUInt8() << 8;
		
        tonesBufferFX[head % tonesBufferFX_Len] = t;
        head++;
        tonesBufferFX_Curr = tonesBufferFX_Curr + 2;

        if (t == TONES_REPEAT) {
          tonesBufferFX_Curr = tonesBufferFX_Start;
          FX::readEnd();
          FX::seekData(tonesBufferFX_Curr);
        }

        if (tonesBufferFX_Tail == -1) tonesBufferFX_Tail = 0;

      }

      tonesBufferFX_Head = head % tonesBufferFX_Len;
      FX::readEnd();

    }

}
*/

void ArduboyTonesFX::noTone()
{
  ::noTone(TONE_PIN);
}

void ArduboyTonesFX::volumeMode(uint8_t mode)
{
}

bool ArduboyTonesFX::playing()
{
  return tonesPlaying;
}


void ArduboyTonesFX::nextTone()
{
  uint16_t freq;
  
  freq = getNext(); // get tone frequency

  if (freq == TONES_END) { // if freq is actually an "end of sequence" marker
    noTone(); // stop playing
    tonesPlaying = false;
    tmrcount = 0;
    return;
  }

  tonesPlaying = true;

  if (freq == TONES_REPEAT) { // if frequency is actually a "repeat" marker
    tonesIndex = tonesStart; // reset to start of sequence
    freq = getNext();
  }

    if (freq == 0) { // if tone is silent
      freq = SILENT_FREQ;
      toneSilent = true;
    }
    else {
      toneSilent = false;
    }


  if (!outputEnabled()) { // if sound has been muted
    toneSilent = true;
  }
  
  if (toneSilent) 
	noTone();
  else 
	::tone(TONE_PIN, freq&0b11111111111111);
	
	waittime = getNext();
    tmrcount = millis();
    
 }



uint16_t ArduboyTonesFX::getNext()
{
  if (toneMode == TONES_MODE_NORMAL) {

    if (inProgmem) {
      return pgm_read_word((const void *)tonesIndex++);
    }
    return *tonesIndex++;

  }
  else {
    uint16_t t = tonesBufferFX[tonesBufferFX_Tail];
    tonesBufferFX_Tail++;
    tonesBufferFX_Tail = tonesBufferFX_Tail % tonesBufferFX_Len;
    return t;
  }
}


