#include "Arduboy2Ext.h"

Arduboy2Ext::Arduboy2Ext() : Arduboy2Base() { }

uint8_t Arduboy2Ext::justPressedButtons() const {
  return (~previousButtonState & currentButtonState);
}

uint8_t Arduboy2Ext::pressedButtons() const {
  return currentButtonState;
}

void Arduboy2Ext::clearButtonState() {
  currentButtonState = previousButtonState = 0;
}


void Arduboy2Ext::resetFrameCount() {
  frameCount = 0;
}

uint16_t Arduboy2Ext::getFrameCount() const {
  return frameCount;
}

void Arduboy2Ext::setFrameCount(uint16_t val) {
  frameCount = val;
}

uint8_t Arduboy2Ext::getFrameCount(uint8_t mod, int8_t offset) const {
  return (frameCount + offset) % mod;
}

bool Arduboy2Ext::getFrameCountHalf(uint8_t mod) const {
	return getFrameCount(mod) > (mod / 2);
}

bool Arduboy2Ext::isFrameCount(uint8_t mod) const {
  return (frameCount % mod) == 0;
}

bool Arduboy2Ext::isFrameCount(uint8_t mod, uint8_t val) const {
  return (frameCount % mod) == val;
}


uint16_t rnd = 0xACE1;


uint8_t Arduboy2Ext::randomLFSR(uint8_t min, uint8_t max) {
  /*uint16_t r = rnd;
  r ^= TCNT0; // add some extra timing randomness
  (r & 1) ? r = (r >> 1) ^ 0xB400 : r >>= 1;
  rnd = r;
  return r % (max - min) + min;*/
  return (random(max-min)+min);
}

