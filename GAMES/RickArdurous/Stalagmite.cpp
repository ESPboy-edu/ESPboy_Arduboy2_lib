/*
 * Implement the logic of the Stalagmite
*/

#include "RickArdurous.h"
#include "Stalagmite.h"
#include "MapManager.h"
#include "SpriteData.h"

bool Stalagmite::Update(UpdateStep step)
{
	switch (step)
	{
		case UpdateStep::DRAW_LETHAL:
		{
			if (IsPropertySet(Item::PropertyFlags::ALIVE))
				Draw();
			break;
		}
		
		case UpdateStep::CHECK_LETHAL_BESIDE:
		{
			if (IsPropertySet(Item::PropertyFlags::ALIVE))
			{
				// spawn sparks if the dynamite kill me
				if (CheckLethalDynamite(SpriteData::STALAGMITE_SPRITE_WIDTH * 2, 0))
				{
					ClearProperty(Item::PropertyFlags::ALIVE);
					SparksAnimFrameId = 0;
				}
			}
			break;
		}

		case UpdateStep::CHECK_LETHAL:
		{
			// for check lethal, just check one pixel in between the two stalagmite,
			// if this pixel is white, that means a lethal object traversed me
			if (IsPropertySet(Item::PropertyFlags::ALIVE))
			{
				signed char xOnScreen = MapManager::GetXOnScreen(X + SpriteData::STALAGMITE_SPRITE_WIDTH);
				signed char yOnScreen = MapManager::GetYOnScreen(Y);
				// spawn sparks if the dynamite kill me
				if (arduboy.getPixel(xOnScreen, yOnScreen) == WHITE)
				{
					ClearProperty(Item::PropertyFlags::ALIVE);
					SparksAnimFrameId = 0;
				}
			}
			break;
		}
		
		case UpdateStep::DRAW_IGNORED_BY_ENEMIES:
		{
			// get my screen coords
			signed char screenX = MapManager::GetXOnScreen(X);
			signed char screenY = MapManager::GetYOnScreen(Y);
			
			if (IsPropertySet(Item::PropertyFlags::ALIVE))
			{
				// check if we are waiting for the drop to fall
				if (DropY == 255)
				{
					if (arduboy.everyXFrames(DROP_PERIOD))
						InitDrop();
				}
				else
				{
					// the drop is currently falling, so increase the drop position
					DropY++;
					
					// get the top coord of the stalagmite
					screenY += 6;
					screenX += DropX;
					
					// if we reach the stalagmite, and finished to play the sparks, then go to wait mode
					if (DropY >= screenY + SpriteData::SPARKS_SPRITE_FRAME_COUNT)
						DropY = 255;
					else if (DropY >= screenY) // otherwise play the sparks
						arduboy.drawBitmapExtended(screenX - 2, screenY - 10,
							SpriteData::Sparks[DropY % SpriteData::SPARKS_SPRITE_FRAME_COUNT],
							SpriteData::SPARKS_SPRITE_WIDTH,
							SpriteData::SPARKS_SPRITE_HEIGHT,
							INVERT, false);
					else if ((screenX >= 0) && (screenX < WIDTH) && (DropY >= 0) && (DropY < HEIGHT)) // otherwise draw the drop
						arduboy.drawPixel(screenX, DropY, WHITE);
				}
			}
			else
			{
				// increase the sparks frame counter
				if (arduboy.everyXFrames(SPARKS_ANIM_SPEED))
					SparksAnimFrameId++;

				// remove from update once the sparks anim is finished
				if (SparksAnimFrameId >= SpriteData::SPARKS_SPRITE_FRAME_COUNT)
					return true;
				
				// draw the Stalactite
				if (SparksAnimFrameId < 2)
					Draw();
				
				// draw the sparks
				for (unsigned char i = 0; i < 3; ++i)
					arduboy.drawBitmapExtended(screenX + (i*3) -1, screenY + 1,
						SpriteData::Sparks[(SparksAnimFrameId + i) % SpriteData::SPARKS_SPRITE_FRAME_COUNT],
						SpriteData::SPARKS_SPRITE_WIDTH,
						SpriteData::SPARKS_SPRITE_HEIGHT,
						INVERT, false);
			}
			break;
		}
	}
	return false;
}

void Stalagmite::InitDrop()
{
	DropX = (DropX == 1) ? 5 : 1;
	DropY = MapManager::GetCeillingScreenPositionAbove(X + DropX, Y) - 1;
}

void Stalagmite::Draw()
{
	signed char screenX = MapManager::GetXOnScreen(X);
	signed char screenY = MapManager::GetYOnScreen(Y);
	for (unsigned char i = 0; i < 2; ++i)
		arduboy.drawBitmapExtended(
			screenX + (i * (SpriteData::STALAGMITE_SPRITE_WIDTH + 1)),
			screenY + i,
			SpriteData::Stalagmite,
			SpriteData::STALAGMITE_SPRITE_WIDTH, SpriteData::STALAGMITE_SPRITE_HEIGHT,
			WHITE, i);
}
