/*
 * This class will launch arrows when the player move in front of it
 */

#include "RickArdurous.h"
#include "ArrowLauncher.h"
#include "Rick.h"
#include "SpriteData.h"
#include "MapManager.h"
#include "FXManager.h"

ArrowLauncher::ArrowLauncher()
: Arrow(true)
{
}

void ArrowLauncher::Init(int startX, unsigned char startY, unsigned char flags, unsigned char detectionWidth)
{
	// set my detection
	DetectionWidth = detectionWidth;
	// and call the item init
	Item::Init(startX, startY, flags);
}

bool ArrowLauncher::Update(UpdateStep step)
{
	switch (step)
	{
		case UpdateStep::DRAW_LETHAL:
			if (LastLaunchTime == CAN_LAUNCH_ARROW)
			{
				// check if the main character is triggering me
				CheckTrigerer(Rick::IsAlive(), Rick::GetX(), Rick::GetY());
				// check if the other trap trigerer is triggering me
				MapManager::CallMeBackForEachTrapTriggerer(this, &CheckTrigererCallback);
			}
			else
			{
				LastLaunchTime++;
				if (LastLaunchTime >= LAUNCH_PERIOD)
					LastLaunchTime = CAN_LAUNCH_ARROW;
			}
			break;
		
		case UpdateStep::DRAW_STATIC_COLLISION:
			{
				// draw the statue that launch the arrow
				arduboy.drawBitmapExtended(MapManager::GetXOnScreen(X), MapManager::GetYOnScreen(Y), SpriteData::ArrowLauncherFace, SpriteData::ARROW_LAUNCHER_FACE_SPRITE_WIDTH, SpriteData::ARROW_LAUNCHER_FACE_SPRITE_HEIGHT, WHITE, IsPropertySet(Item::PropertyFlags::MIRROR_X));
			}
			break;
			
		case UpdateStep::RESPAWN:
			LastLaunchTime = CAN_LAUNCH_ARROW;
			Arrow.KillBulletWithoutSparks();
			break;
	}
	return false;
}

void ArrowLauncher::CheckTrigererCallback(Item * me, Item * trigerer)
{
	((ArrowLauncher *)me)->CheckTrigerer(trigerer->IsPropertySet(Item::PropertyFlags::ALIVE), trigerer->GetX(), trigerer->GetY());
}

void ArrowLauncher::CheckTrigerer(bool isAlive, int trigererX, unsigned char trigererY)
{
	if (LastLaunchTime == CAN_LAUNCH_ARROW)
	{
		int minX;
		int maxX;
		int throwingX;
		bool isShootingTowardLeft = IsPropertySet(Item::PropertyFlags::MIRROR_X);
		if (isShootingTowardLeft)
		{
			minX = X - DetectionWidth;
			maxX = X;
			throwingX = X - ArrowBullet::ARROW_WIDTH;
		}
		else
		{
			minX = X + 1;
			maxX = X + DetectionWidth;
			throwingX = X + 1;
		}
		// compute my triggering Y
		unsigned char throwingY = Y + 6;
		
		// check if the main character is inside the detection range
		if (isAlive && (throwingY > trigererY) && (throwingY < trigererY + 13) && (minX < trigererX + SpriteData::RICK_SPRITE_WIDTH) && (maxX > trigererX))
		{
			Arrow.Fire(throwingX, throwingY, isShootingTowardLeft);
			FXManager::StartFXCommand(FXManager::BufferId::GAME, {1, 1, 0, 0, 0, 66, 2, 8});
			LastLaunchTime = 0;
		}
	}
}