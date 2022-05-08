/*
 * This file contains an extension of the Arduboy class
 */

#include "RickArdurous.h"
#include "CustomArduboy.h"
#include "Arduboy_glcdfont.c"

unsigned int CustomArduboy::drawBitmapExtended(int8_t x, int8_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color, bool mirrorX)
{
	// no need to draw at all if we're offscreen
	if (x+w < 0 /*|| x > WIDTH-1*/ || y+h < 0 || y > HEIGHT-1)
		return 0;

	unsigned char yOffset = abs(y) % 8;
	char bufferRow = y / 8;
	if (y < 0)
	{
		bufferRow--;
		yOffset = 8 - yOffset;
	}
	unsigned char yOffsetComplement = 8 - yOffset;
	unsigned char bitmapRows = h / 8;
	if (h%8 != 0)
		bitmapRows++;
	// compute the start and end X (clamp if outside the screen)
	char startX = (x<0) ? -x : 0;
	char endX = (WIDTH-x < w) ? WIDTH-x : w;
	// a flag to check if there is white pixels under the drawn pixels of the bitmap
	unsigned int collisionDetected = 0;
	// iterate on the bitmapRows
	for (unsigned char a = 0; a < bitmapRows; ++a)
	{
		if (bufferRow > (HEIGHT/8)-1)
			break;
		if (bufferRow > -2)
		{
			// inverse the horizontal iteration inside the bitmap if we are mirrored on x
			char iCol = startX;
			char iColDirection = 1;
			if (mirrorX)
			{
				iCol = w-1-startX;
				iColDirection = -1;
			}
			int bRowShift = bufferRow*WIDTH;
			int nextBRowShift = (bufferRow+1)*WIDTH;
			for (char xCol = startX; xCol < endX; xCol++, iCol += iColDirection)
			{
				char currentX = x + xCol;
				uint8_t unshiftedByteToWrite = pgm_read_byte(bitmap+(a*w)+iCol);
				if (bufferRow >= 0)
				{
					int bufferPosition = bRowShift + currentX;
					uint8_t byteToWrite = unshiftedByteToWrite << yOffset;
					if ((sBuffer[bufferPosition] & byteToWrite) != 0)
						collisionDetected |= 1 << iCol;
					if (color == WHITE)
						sBuffer[bufferPosition] |= byteToWrite;
					else if (color == BLACK)
						sBuffer[bufferPosition] &= ~byteToWrite;
					else if (color == INVERT)
						sBuffer[bufferPosition] ^= byteToWrite;
				}
				if (yOffset && bufferRow<(HEIGHT/8)-1)
				{
					int bufferPosition = nextBRowShift + currentX;
					uint8_t byteToWrite = unshiftedByteToWrite >> yOffsetComplement;
					if ((sBuffer[bufferPosition] & byteToWrite) != 0)
						collisionDetected |= 1 << iCol;
					if (color == WHITE)
						sBuffer[bufferPosition] |= byteToWrite;
					else if (color == BLACK)
						sBuffer[bufferPosition] &= ~byteToWrite;
					else if (color == INVERT)
						sBuffer[bufferPosition] ^= byteToWrite;
				}
			}
		}
		bufferRow++;
	}
	return collisionDetected;
}

bool CustomArduboy::CheckWhitePixelsInRow(uint8_t x, uint8_t y, uint8_t w)
{
	// need to check the y, because sometimes this function can be called with a y outside the screen
	if (y < HEIGHT)
	{
		unsigned char row = y / 8;
		unsigned char yLineToCheck = 1 << (y % 8);
		int startX = (row*WIDTH) + x;
		unsigned char* localBuffer = &(sBuffer[startX]);
		for (unsigned char i = 0; i < w; ++i)
			if (localBuffer[i] & yLineToCheck)
				return true;
	}
	return false;
}
