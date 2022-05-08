#ifndef _DESTROYABLE_BLOCK_H_
#define _DESTROYABLE_BLOCK_H_

#include "LivingItem.h"
class BlockFragment;

class DestroyableBlock : public LivingItem
{
public:
	virtual bool Update(UpdateStep step);
	bool IsLocatedAt(unsigned char mapX, unsigned char mapY);
	bool IsAlive() { return IsPropertySet(Item::PropertyFlags::ALIVE); }

private:
	static constexpr int FRAGMENT_COUNT = 3;
	
	static BlockFragment Fragments[FRAGMENT_COUNT];
	
	void InitDeath();
};

#endif