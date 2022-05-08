/*
 * Implement the logic and drawing of the dynamite
*/

#include "RickArdurous.h"
#include "Dynamite.h"
#include "SpriteData.h"
#include "MapManager.h"

void Dynamite::LightUp(int x, unsigned char y)
{
	X = x;
	Y = y;
	DynamiteAnimFrameId = 0;
	SparksAnimFrameId = 0;
	SetProperty(Item::PropertyFlags::ALIVE);
	// add myself to the map manager in order to be updated
	MapManager::AddItem(this);
}

bool Dynamite::Update(UpdateStep step)
{
	switch (step)
	{
		case UpdateStep::DRAW_LETHAL_BESIDE:
			if (DynamiteAnimFrameId >= 6)
			{
				// increase the dynamite frame id, at different speed depending if it is the fuse consuming part or the explosion
				if (arduboy.everyXFrames(EXPLOSION_ANIM_SPEED))
					DynamiteAnimFrameId++;
				// if we reach the end of the dynamite explosion, disable the dynamite
				if (DynamiteAnimFrameId == SpriteData::DYNAMITE_SPRITE_FRAME_COUNT)
				{
					Kill();
					return true;
				}
				else
				{
					// draw the explosion
					Draw();
				}
			}
			break;
			
		case UpdateStep::DRAW_IGNORED_BY_ENEMIES:
			// check if the dynamite is alive
			if (DynamiteAnimFrameId == -1)
			{
				return true;
			}
			else if (DynamiteAnimFrameId < 6)
			{
				// We are in fuse consume part, increase the dynamite frame id for consuming the fuse
				if (arduboy.everyXFrames(DYNAMITE_ANIM_SPEED))
					DynamiteAnimFrameId++;
				// increase the sparks frame id
				if (arduboy.everyXFrames(SPARKS_ANIM_SPEED))
					SparksAnimFrameId = (SparksAnimFrameId + 1) % SpriteData::SPARKS_SPRITE_FRAME_COUNT;

				// check if we still are in the consume part or if we reach the explosion part of the anim
				if (DynamiteAnimFrameId < 6)
				{
					// draw the dynamite
					Draw();
					// draw the sparks
					arduboy.drawBitmapExtended(MapManager::GetXOnScreen(X + (DynamiteAnimFrameId >> 1)), MapManager::GetYOnScreen(Y), SpriteData::Sparks[SparksAnimFrameId], SpriteData::SPARKS_SPRITE_WIDTH, SpriteData::SPARKS_SPRITE_HEIGHT, INVERT, false);
				}
			}
			break;
	}
	return false;
}

void Dynamite::Kill()
{
	DynamiteAnimFrameId = -1;
	ClearProperty(Item::PropertyFlags::ALIVE);
}

void Dynamite::Draw()
{
	// draw the dynamite or explosion
	arduboy.drawBitmapExtended(MapManager::GetXOnScreen(X), MapManager::GetYOnScreen(Y), SpriteData::Dynamite[DynamiteAnimFrameId], SpriteData::DYNAMITE_SPRITE_WIDTH, SpriteData::DYNAMITE_SPRITE_HEIGHT, WHITE, false);
}