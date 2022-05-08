#ifndef _PICKUP_ITEM_H_
#define _PICKUP_ITEM_H_

#include "LivingItem.h"

class PickUpItem : public LivingItem
{
public:
	void UpdateShineStar(unsigned char screenX, unsigned char screenY, signed char minX, signed char maxX, signed char minY, signed char maxY);

protected:
	bool DoesRickPickMeUp(unsigned char width, unsigned char height);
	bool PickupUpdate(UpdateStep step, const unsigned char sprite[], unsigned char spriteWidth, unsigned char spriteHeight, bool isCrate);

private:
	static constexpr int ANIM_SPEED = 2;
	static constexpr int MIN_PAUSE_TIMES = 10;
	static constexpr int MAX_PAUSE_TIMES = 150;
	
	// the current time to wait before playing the next shining star
	signed char PauseTime = 1;
	
	// current frame id and direction of the shinning star
	signed char AnimFrameId = -1;
	signed char AnimDirection = 1;
	
	// the position shift for the shine star, relative to the statuette
	signed char ShineStarShiftX = 0;
	signed char ShineStarShiftY = 0;
};

#endif
