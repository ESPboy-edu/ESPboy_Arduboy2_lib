/*
 * Implement the logic and drawing of the statuette bonus
*/

#include "RickArdurous.h"
#include "Statuette.h"
#include "SpriteData.h"
#include "Rick.h"

bool Statuette::Update(UpdateStep step)
{
	if (PickupUpdate(step, SpriteData::Statuette, SpriteData::STATUETTE_SPRITE_WIDTH, SpriteData::STATUETTE_SPRITE_HEIGHT, false))
	{
		Rick::StatuetteCount++;
		return true;
	}
	return false;
}
