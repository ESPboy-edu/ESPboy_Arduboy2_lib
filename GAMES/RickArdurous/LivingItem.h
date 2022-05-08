#ifndef _LIVING_ITEM_H_
#define _LIVING_ITEM_H_

#include "Item.h"
#include "MapDataConstVariables.h"

class LivingItem: public Item
{
public:
	void Init(int startX, unsigned char startY, unsigned char flags, bool shouldRespawn);
	void SaveAndLoadAliveStatus(unsigned char currentScreenIdToSave, unsigned char newScreenIdToLoad);

private:
	unsigned char AlivePerPuzzleScreen[MapManager::PUZZLE_SCREEN_BYTE_COUNT];
};

#endif