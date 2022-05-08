/*
 * Implement the logic and drawing of Stalactite
*/

#include "RickArdurous.h"
#include "Stalactite.h"
#include "SpriteData.h"
#include "MapManager.h"
#include "Rick.h"
#include "Physics.h"

Stalactite::Stalactite() :
FallAnimSpeedIndex(Physics::INVALID_FALL_ID)
{
}

bool Stalactite::Update(UpdateStep step)
{
	switch (step)
	{
		case UpdateStep::DRAW_LETHAL:
		{
			if (IsPropertySet(Item::PropertyFlags::ALIVE))
			{
				signed char shakeX = 0;
				// use the SPECIAL_ flag as an anim state: if SPECIAL_ is set then the stalactite is falling,
				// otherwise it is waiting
				if (!IsPropertySet(Item::PropertyFlags::SPECIAL_))
				{
					// check if the main character is triggering me
					CheckTrigerer(Rick::IsAlive(), Rick::GetX(), Rick::GetY());
					// check if the other trap trigerer is triggering me
					MapManager::CallMeBackForEachTrapTriggerer(this, &CheckTrigererCallback);

					// compute a shaking x variation
					signed char shakingMove[] = {1, -1, 0, 1, 0, -1, 0};
					for (unsigned char i = 0; i < sizeof(shakingMove); ++i)
						if (arduboy.everyXFrames(SHAKING_SPEED + i))
						{
							shakeX = shakingMove[i];
							break;
						}
				}
				else
				{
					// make the stalactite fall
					Physics::UpdateFall(FallAnimSpeedIndex, Y);
					if (MapManager::IsThereStaticCollisionAt(X, Y + SpriteData::STALACTITE_SPRITE_HEIGHT))
						ClearProperty(Item::PropertyFlags::ALIVE);
				}
				// draw the stalactite
				Draw(shakeX);
			}
			break;
		}
		
		case UpdateStep::DRAW_IGNORED_BY_ENEMIES:
		{
			if (!IsPropertySet(Item::PropertyFlags::ALIVE))
			{
				// increase the sparks frame counter
				if (arduboy.everyXFrames(SPARKS_ANIM_SPEED))
				{
					Y++;
					SparksAnimFrameId++;
				}

				// remove from update once the sparks anim is finished
				if (SparksAnimFrameId >= SpriteData::SPARKS_SPRITE_FRAME_COUNT)
				{
					// stop the fall
					Physics::StopFall(FallAnimSpeedIndex);
					return true;
				}
				
				// draw the Stalactite
				if (SparksAnimFrameId < 3)
					Draw(0);
				
				// draw the sparks
				for (unsigned char i = 0; i < 2; ++i)
					arduboy.drawBitmapExtended(MapManager::GetXOnScreen(X + (i*4)),
						MapManager::GetYOnScreen(Y + 2 - SparksAnimFrameId),
						SpriteData::Sparks[SparksAnimFrameId],
						SpriteData::SPARKS_SPRITE_WIDTH,
						SpriteData::SPARKS_SPRITE_HEIGHT,
						INVERT, false);
			}
			break;
		}
		
		case UpdateStep::RESPAWN:
		{
			ClearProperty(Item::PropertyFlags::SPECIAL_);
			SparksAnimFrameId = 0;
			break;
		}
	}
	return false;
}

void Stalactite::CheckTrigererCallback(Item * me, Item * trigerer)
{
	((Stalactite *)me)->CheckTrigerer(trigerer->IsPropertySet(Item::PropertyFlags::ALIVE), trigerer->GetX(), trigerer->GetY());
}

void Stalactite::CheckTrigerer(bool isAlive, int trigererX, unsigned char trigererY)
{
	// check if the trigerer is alive and inside the detection range,
	// and that the stalactite is not alreay falling
	if (isAlive && 
		(X < trigererX + SpriteData::RICK_SPRITE_WIDTH) && 
		(X + (SpriteData::STALACTITE_SPRITE_WIDTH * 3) > trigererX))
	{
		unsigned char yMap = (Y / SpriteData::LEVEL_SPRITE_HEIGHT) + 1;
		unsigned char trigererYMap = trigererY / SpriteData::LEVEL_SPRITE_HEIGHT;
		int middleX = X + (SpriteData::STALACTITE_SPRITE_WIDTH / 2);
		for (unsigned char currentY = yMap; currentY <= trigererYMap; ++currentY)
			if (MapManager::IsThereStaticCollisionAt(middleX, currentY * SpriteData::LEVEL_SPRITE_HEIGHT))
				return;
		// if we didnt find a collision, between me and the triggered, then start to fall
		SetProperty(Item::PropertyFlags::SPECIAL_);
		FallAnimSpeedIndex = Physics::StartFall();
	}
}

void Stalactite::Draw(signed char shakeX)
{
	for (unsigned char i = 0; i < 2; ++i)
		arduboy.drawBitmapExtended(
					MapManager::GetXOnScreen(X + (i * (SpriteData::STALACTITE_SPRITE_WIDTH + 1)) + shakeX),
					MapManager::GetYOnScreen(Y + i),
					SpriteData::Stalactite,
					SpriteData::STALACTITE_SPRITE_WIDTH, SpriteData::STALACTITE_SPRITE_HEIGHT,
					WHITE, i);
}
