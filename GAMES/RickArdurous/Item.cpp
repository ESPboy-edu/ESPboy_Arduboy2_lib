/*
 * This file implement the base class for all the items present in the level
*/

#include "RickArdurous.h"
#include "Item.h"
#include "MapManager.h"

void Item::CommonInit(int startX, unsigned char startY, unsigned char flags)
{
	X = startX;
	Y = startY;
	// override the property flag to reset totally the property as the specified flag
	Property = flags;
	// call the update virtual function with the Respawn step, for specific data init
	Update(UpdateStep::RESPAWN);
}

void Item::Init(int startX, unsigned char startY, unsigned char flags)
{
	// for items that can never die, add it to the manager anyway
	MapManager::AddItem(this);
	// and call the common init function
	CommonInit(startX, startY, flags);
}

Item::Side Item::CheckLethalDynamite(unsigned char width, unsigned char checkHeight)
{
	// get my y coordinate on screen
	char yOnScreen = MapManager::GetYOnScreen(Y) + checkHeight;
	// check if I'm on screen
	if ((yOnScreen >= 0) && (yOnScreen < HEIGHT))
	{
		// get my x coordinate on screen
		char xOnScreen = MapManager::GetXOnScreen(X);
		// iterate on a certain distance
		for (unsigned char i = 1; i <= EXPLOSION_DETECTION_DISTANCE; ++i)
		{
			// check on the left
			int xLeft = xOnScreen - i;
			if ((xLeft >= 0) && (xLeft < WIDTH) && (arduboy.getPixel(xLeft, yOnScreen) == WHITE))
				return Side::LEFT;
			// check on the right
			int xRight = xOnScreen + width + i;
			if ((xRight >= 0) && (xRight < WIDTH) && (arduboy.getPixel(xRight, yOnScreen) == WHITE))
				return Side::RIGHT;
		}
	}
	// by default no collision detected
	return Side::NO_SIDE;
}
