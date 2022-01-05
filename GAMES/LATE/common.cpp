#include "common.h"

void drawNumber(Arduboy2 *gr, uint8_t x, uint8_t y, uint32_t number, uint8_t color, uint8_t padding) {
	int8_t digit, maxPad, length = 1;
	uint32_t tempNum = number;
	while(tempNum /= 10) {
		length++;
	}
	maxPad = max(length, padding);
	tempNum = number;
	while(maxPad > 0) {
		maxPad--;
		digit = tempNum % 10;
		tempNum /= 10;
		if(! digit && (maxPad + length - padding < 0)) continue;
		gr->drawBitmap(x + (maxPad * 4), y, digitsBmp[digit], 3, 5, color);
	}
}

int8_t buttonPressed(Arduboy2 *gr) {
	return gr->justPressed(A_BUTTON) ||
		gr->justPressed(B_BUTTON) ||
		gr->justPressed(UP_BUTTON) ||
		gr->justPressed(DOWN_BUTTON) ||
		gr->justPressed(LEFT_BUTTON) ||
		gr->justPressed(RIGHT_BUTTON);
}

int8_t buttonReleased(Arduboy2 *gr) {
	return gr->justReleased(A_BUTTON) ||
		gr->justReleased(B_BUTTON) ||
		gr->justReleased(UP_BUTTON) ||
		gr->justReleased(DOWN_BUTTON) ||
		gr->justReleased(LEFT_BUTTON) ||
		gr->justReleased(RIGHT_BUTTON);
}
