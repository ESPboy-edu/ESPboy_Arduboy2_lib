#ifndef _STALACTITE_H_
#define _STALACTITE_H_

#include "LivingItem.h"

class Stalactite : public LivingItem
{
public:
	Stalactite();
	virtual bool Update(UpdateStep step);

protected:
	static constexpr int SHAKING_SPEED = 120;
	static constexpr int SPARKS_ANIM_SPEED = 3;
	
	unsigned char SparksAnimFrameId;
	unsigned char FallAnimSpeedIndex;

	static void CheckTrigererCallback(Item * me, Item * trigerer);
	void CheckTrigerer(bool isAlive, int trigererX, unsigned char trigererY);
	void Draw(signed char shakeX);
};

#endif
