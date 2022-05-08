/*
 * This file implement the base class for all the living items present in the level.
 * LivingItem are item that can die (such as Enemies) or be destroyed (like destroyable blocks),
 * or Picked up by the player (like Statuette).
 * They store their alive status in each puzzle screen
*/

#include "RickArdurous.h"
#include "LivingItem.h"
#include "MapManager.h"

void LivingItem::Init(int startX, unsigned char startY, unsigned char flags, bool shouldRespawn)
{
	// if we are not forced to respaw, check our alive status that as been correctly set by the progress
	if (!shouldRespawn)
		shouldRespawn = IsPropertySet(Item::PropertyFlags::ALIVE);
	// set the alive flag if we should respawn
	if (shouldRespawn)
	{
		// set the alive flag in the flags variable
		flags |= Item::PropertyFlags::ALIVE;
		// add the item to the manager
		MapManager::AddItem(this);
		// and call the common init function
		CommonInit(startX, startY, flags);
	}
	else
	{
		// remove the item from the manager
		MapManager::RemoveItem(this);
	}
}

/**
 * Save the current alive status of this item in the bit dedicated to the specified screen id to save,
 * then patch the current alive status of this item with the bit dedicated to the specified screen id to load.
 * However, there's a SPECIAL_ case, if the current screen id to save is 0, then all the flags for all the
 * screens are reset to 1.
*/
void LivingItem::SaveAndLoadAliveStatus(unsigned char currentScreenIdToSave, unsigned char newScreenIdToLoad)
{
	// SPECIAL_ case, if the currentScreenIdToSave is null, then we reset all
	if (currentScreenIdToSave == 0)
	{
		for (unsigned char i = 0; i < MapManager::PUZZLE_SCREEN_BYTE_COUNT; ++i)
			AlivePerPuzzleScreen[i] = 0xFF;
	}
	else
	{
		// First save my current alive status
		unsigned char screenByteNumber = (currentScreenIdToSave / 8);
		unsigned char screenBitValue = 1 << (currentScreenIdToSave % 8);
		if (IsPropertySet(Item::PropertyFlags::ALIVE))
			AlivePerPuzzleScreen[screenByteNumber] |= screenBitValue;
		else
			AlivePerPuzzleScreen[screenByteNumber] &= ~screenBitValue;

		// Then patch my alive status with the value of the screen to load
		screenByteNumber = (newScreenIdToLoad / 8);
		screenBitValue = 1 << (newScreenIdToLoad % 8);
		if (AlivePerPuzzleScreen[screenByteNumber] & screenBitValue)
			SetProperty(Item::PropertyFlags::ALIVE);
		else
			ClearProperty(Item::PropertyFlags::ALIVE);
	}
}
