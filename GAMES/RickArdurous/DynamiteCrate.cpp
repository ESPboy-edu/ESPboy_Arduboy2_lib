/*
 * Implement the logic and drawing of a dynamite crate
*/

#include "RickArdurous.h"
#include "DynamiteCrate.h"
#include "SpriteData.h"
#include "Rick.h"

bool DynamiteCrate::Update(UpdateStep step)
{
	if (PickupUpdate(step, SpriteData::DynamiteCrate, SpriteData::CRATE_SPRITE_WIDTH, SpriteData::CRATE_SPRITE_HEIGHT, true))
	{
		Rick::DynamiteCount = Rick::MAX_DYNAMITE_COUNT;
		return true;
	}
	return false;
}