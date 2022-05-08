/*
 * Implement the logic and drawing of fragment of a Destroyable Blocks, when it is destroyed
*/

#include "RickArdurous.h"
#include "BlockFragment.h"
#include "SpriteData.h"
#include "Physics.h"
#include "MapManager.h"

BlockFragment::BlockFragment() :
StoneFallIndex(Physics::INVALID_FALL_ID)
{
}

void BlockFragment::Spawn(int startX, unsigned char startY, unsigned char id)
{
	// choose a random velocity
	unsigned char velocityX = (id == 0) ? random(-16, -10) : ((id == 1) ? random(-6, 6) : random(10, 16));
	// randomize the mirror flag
	if (random(2))
		SetProperty(Item::PropertyFlags::MIRROR_X);
	else
		ClearProperty(Item::PropertyFlags::MIRROR_X);
	// start the trajectory
	StoneFallIndex = Physics::StartParabolicTrajectory(startX, startY, velocityX);
	MapManager::AddItem(this);
}

bool BlockFragment::Update(UpdateStep step)
{
	switch (step)
	{
		case Item::UpdateStep::DRAW_IGNORED_BY_ENEMIES:
		{
			// update the trajectory of the stone
			Physics::UpdateParabolicTrajectory(StoneFallIndex, X, Y);
			
			signed char xOnScreen = MapManager::GetXOnScreen(X);
			signed char yOnScreen = MapManager::GetYOnScreen(Y);
			arduboy.drawBitmapExtended(xOnScreen, yOnScreen,
								SpriteData::BlockFragment,
								SpriteData::BLOCK_FRAGMENT_SPRITE_WIDTH, SpriteData::BLOCK_FRAGMENT_SPRITE_HEIGHT,
								INVERT, IsPropertySet(Item::PropertyFlags::MIRROR_X));

			// check if I'm still on screen
			if (!MapManager::IsOnScreen(X, Y, SpriteData::BLOCK_FRAGMENT_SPRITE_WIDTH, SpriteData::BLOCK_FRAGMENT_SPRITE_HEIGHT))
			{
				// stop the parabolic trajectory
				Physics::StopParabolicTrajectory(StoneFallIndex);
				// we are now out of the screen we can be removed from the MapManager
				return true;
			}
		}
		break;
	}
	return false;
}
