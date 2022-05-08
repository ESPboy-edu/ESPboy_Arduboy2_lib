/*
 * Implement the logic and drawing of a bullet crate
*/

#include "RickArdurous.h"
#include "BulletCrate.h"
#include "SpriteData.h"
#include "Rick.h"

bool BulletCrate::Update(UpdateStep step)
{
	if (PickupUpdate(step, SpriteData::BulletCrate, SpriteData::CRATE_SPRITE_WIDTH, SpriteData::CRATE_SPRITE_HEIGHT, true))
	{
		Rick::BulletCount = Rick::MAX_BULLET_COUNT;
		return true;
	}
	return false;
}