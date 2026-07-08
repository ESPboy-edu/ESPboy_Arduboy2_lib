#ifndef MATHSFUNCTIONS_H_
#define MATHSFUNCTIONS_H_

#include <stdint.h>
#include "Defines.h"

int16_t estimateDistance(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
int16_t estimateMagnitude(int16_t x, int16_t y);
uint8_t calculateFacingDirection(int16_t x1, int16_t y1, int16_t x2, int16_t y2);



#endif

