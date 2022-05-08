#ifndef _SPIKE_H_
#define _SPIKE_H_

#include "Item.h"

class Spike : public Item
{
public:
	virtual bool Update(UpdateStep step);

private:
	static constexpr int ANIM_SPEED = 4;
	static constexpr int PAUSE_TIME = 90;
	unsigned char AnimFrameId = 0;
};

#endif