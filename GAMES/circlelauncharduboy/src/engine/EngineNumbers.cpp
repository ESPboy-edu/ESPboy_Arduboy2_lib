#include "Engine.h"


#include "Engine.h"


// Исправлено: Ассемблер AVR заменен на 32-битную арифметику С++
int16_t MULHI(int16_t a, int16_t b) {
    return (int16_t)(((int32_t)a * (int32_t)b) >> 16);
}


// Computes sin(pi*x) for signed x=[-1,1] in Q15
//                   or unsigned x=[ 0,2] in Q15
// |error| < 2.3 ULP
int16_t sinpi(int16_t x) {

    // reduce to quadrant, and x in [-0.25,0.25]
    uint16_t q = (x + 0x2000) & 0xc000;     // quadrant [0,3] in 2 MSBs
    x <<= 2;                                // normalize to Q15
    if (x == -32768) x++;                   // saturate to avoid x*x overflow

    int16_t xx = MULHI(x, x) << 1;  // Q15
    int16_t y;

    // reconstruct using quadrant
    if (q & 0x4000) {
        // cos approx, as even polynomial
        y = 2039;                   // Q17
        y = MULHI(y, xx) - 20209;   // Q16
        y = MULHI(y, xx) + 32767;   // Q15
    } else {
        // sin approx, as odd polynomial
        y = 322;                    // Q17
        y = MULHI(y, xx) - 5291;    // Q16
        y = MULHI(y, xx) + 25736;   // Q15
        y = MULHI(y, x);            // Q14
        y = (y << 1) + 1;           // Q15
    }
    return (q & 0x8000) ? -y : y;
}

// Computes cos(pi*x) for signed x=[-1,1] in Q15
//                   or unsigned x=[ 0,2] in Q15
// |error| < 2.3 ULP
int16_t cospi(int16_t x) {
    return sinpi(0x4000 - x);
}

// Random numbers
int Engine::randomInt(int min, int max) {
  int diff = max - min;
  return (random() + millisNow()) % diff + min;
}

int Engine::randomIntFromMillis(int max) {
  return millisNow() % max;
}

// Number helpers
int Engine::digitsInNumber(int n) {
  int t = abs(n);
  int places;

  if (t < 10)
    places = 1;
  else if (t < 100)
    places = 2;
  else if (t < 1000)
    places = 3;
  else if (t < 10000)
    places = 4;
  else if (t < 100000)
    places = 5;

  if (n < 0)
    places++;

  return places;
}

float Engine::deg2rad(int degrees) {
  return degrees * 2 * M_PI / 360;
}

int Engine::xDirectionsN(int i, int n) {
  int16_t increment = UNSIGNED_SHORT_MAX / n * -1;
  int16_t radians = increment * i;
  return -100 * (sinpi(radians) * 2.0 / UNSIGNED_SHORT_MAX);
}

int Engine::yDirectionsN(int i, int n) {
  int16_t increment = UNSIGNED_SHORT_MAX / n * -1;
  int16_t radians = increment * i;
  return -100 * (cospi(radians) * 2.0 / UNSIGNED_SHORT_MAX);
}

int16_t Engine::xDirectionsN16(int i, int n) {
  int16_t increment = UNSIGNED_SHORT_MAX / n * -1;
  int16_t radians = increment * i;
  return (sinpi(radians) * -1);
}

int16_t Engine::yDirectionsN16(int i, int n) {
  int16_t increment = UNSIGNED_SHORT_MAX / n * -1;
  int16_t radians = increment * i;
  return (cospi(radians) * -1);
}

int Engine::distanceBetween(int x0, int y0, int x1, int y1) {
  return sqrt(pow(x0 - x1, 2) + pow(y0 - y1, 2));
}

float Engine::distanceBetween(float x0, float y0, float x1, float y1) {
  return sqrt(pow(x0 - x1, 2) + pow(y0 - y1, 2));
}

int32_t Engine::timeScaledDelta(int32_t delta) {
  if (speedFactorInt == 0) return delta;
  return (delta * 256) / speedFactorInt;
}

int32_t Engine::timeScaledDelay(int32_t delay) {
  return (s32)(delay) * speedFactorInt >> R8;
}


void Engine::clampBetween(int& value, int minValue, int maxValue, bool loopAround) {
  int diff = maxValue - minValue;
  if (value > maxValue)
    value = loopAround ? minValue : maxValue;
  if (value < minValue)
    value = loopAround ? maxValue : minValue;
}

void Engine::wrapBetween(float& value, float minValue, float maxValue) {
  float diff = maxValue - minValue;
  while (value >= maxValue)
    value -= diff;
  while (value < minValue)
    value += diff;
}
