/*
 * Implement the logic and drawing of Destroyable Blocks
*/

#include "RickArdurous.h"
#include "DestroyableBlock.h"
#include "SpriteData.h"
#include "BlockFragment.h"
#include "MapManager.h"

BlockFragment DestroyableBlock::Fragments[FRAGMENT_COUNT];

bool DestroyableBlock::Update(UpdateStep step)
{
	switch (step)
	{
		case UpdateStep::CHECK_LETHAL_BESIDE:
		{
			if (IsPropertySet(Item::PropertyFlags::ALIVE))
			{
				// spawn block fragment and remove myself from the manager
				if (CheckLethalDynamite(SpriteData::LEVEL_SPRITE_WIDTH * 2, 0))
				{
					InitDeath();
					return true;
				}
			}
			break;
		}

		case UpdateStep::DRAW_STATIC_COLLISION:
		{
			// if we are updated, that means we are alive, so no need to check the property flag
			arduboy.drawBitmapExtended(MapManager::GetXOnScreen(X), MapManager::GetYOnScreen(Y), SpriteData::DestroyableBlock, SpriteData::DESTROYABLE_BLOCK_SPRITE_WIDTH, SpriteData::DESTROYABLE_BLOCK_SPRITE_HEIGHT, WHITE, false);
			break;
		}
		
		case UpdateStep::RESPAWN:
		{
			// set again the destroyable property flag because it may have been cleared in the Init function
			SetProperty(Item::PropertyFlags::DESTROYABLE_BLOCK);
			break;
		}
	}
	return false;
}

void DestroyableBlock::InitDeath()
{
	ClearProperty(Item::PropertyFlags::ALIVE);
	for (unsigned char i = 0; i < FRAGMENT_COUNT; ++i)
		Fragments[i].Spawn(X + (i*6), Y, i);
}

bool DestroyableBlock::IsLocatedAt(unsigned char mapX, unsigned char mapY)
{
	return (mapY == (Y / SpriteData::LEVEL_SPRITE_HEIGHT)) && ((mapX == (X / SpriteData::LEVEL_SPRITE_WIDTH)) || (mapX == ((X + SpriteData::LEVEL_SPRITE_WIDTH) / SpriteData::LEVEL_SPRITE_WIDTH)));
}
