#ifndef _DYNAMITE_H_
#define _DYNAMITE_H_

#include "Item.h"

class Dynamite : public Item
{
public:
	virtual bool Update(UpdateStep step);
	void LightUp(int x, unsigned char y);
	void Kill();

private:
	static constexpr int DYNAMITE_ANIM_SPEED = 20;
	static constexpr int EXPLOSION_ANIM_SPEED = 6;
	static constexpr int SPARKS_ANIM_SPEED = 5;
	signed char DynamiteAnimFrameId = -1;
	signed char SparksAnimFrameId = -1;
	
	void Draw();
};

#endif
