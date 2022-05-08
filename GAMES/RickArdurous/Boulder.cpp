/*
 * Implement the logic and drawing of lethal spikes
*/

#include "RickArdurous.h"
#include "Boulder.h"
#include "SpriteData.h"
#include "MapManager.h"
#include "Physics.h"

bool Boulder::Update(UpdateStep step)
{
	bool isBoulderRolling = !IsPropertySet(SPECIAL_);
	bool isFalling = (PhysicsFallId != Physics::INVALID_PARABOLIC_ID);

	switch (step)
	{
		case UpdateStep::RESPAWN:
			PhysicsFallId = Physics::INVALID_PARABOLIC_ID;
			break;

		case UpdateStep::CHECK_LETHAL_BESIDE:
		{
			if (!isBoulderRolling && !isFalling)
			{
				Item::Side explosionSide = CheckLethalDynamite(SpriteData::BOULDER_SPRITE_WIDTH * 2, 8);
				if (explosionSide != Item::Side::NO_SIDE)
				{
					//start rolling
					ClearProperty(SPECIAL_ | MIRROR_X);
					// set the side
					if (explosionSide == Item::Side::RIGHT)
						SetProperty(MIRROR_X);
				}
			}
			break;
		}

		case UpdateStep::CHECK_STATIC_COLLISION:
		{
			// move the boulder
			signed char movingDirection = IsPropertySet(MIRROR_X) ? -1 : 1;
			// check if we need to start or stop to fall
			bool hasCollisionUnder = MapManager::IsThereAnyHorizontalCollisionAt(X+4, Y+12, 4);
			if (!hasCollisionUnder)
			{
				if (isFalling)
					Physics::UpdateParabolicTrajectory(PhysicsFallId, X, Y, 0);
				else
					PhysicsFallId = Physics::StartParabolicTrajectory(X, Y, movingDirection * FALL_INITIAL_VELOCITY_X);
			}
			else if (isFalling)
			{
				Physics::StopParabolicTrajectory(PhysicsFallId);
				PhysicsFallId = Physics::INVALID_PARABOLIC_ID;
				// move the boulder up outside the ground collision
				//while (MapManager::IsThereAnyHorizontalCollisionAt(X+4, Y+11, 4))
				//	Y--;
			}
			else if (arduboy.everyXFrames(ANIM_SPEED) && isBoulderRolling)
			{
				// move if we are not falling
				X += movingDirection;
				// check if I bump in a wall
				int wallCheckX = (movingDirection > 0) ? X + (SpriteData::BOULDER_SPRITE_WIDTH * 2) : X;
				if (MapManager::IsThereStaticCollisionAt(wallCheckX, Y, true) ||
					MapManager::IsThereStaticCollisionAt(wallCheckX, Y+8))
					{
						// move the boulder out of the wall
						X -= movingDirection;
						// check if there is at least 2 wall sprite under the boulder
						if (MapManager::IsThereStaticCollisionAt(X, Y+16) &&
							MapManager::IsThereStaticCollisionAt(X+8, Y+16))
						{
							// if yes, the boulder can stop
							SetProperty(SPECIAL_);
						}
						else
						{
							// if no, the boulder continue to opposite direction
							InverseProperty(MIRROR_X);
						}
					}
			}
			break;
		}
		case UpdateStep::DRAW_LETHAL_BESIDE:
		{
			if (isBoulderRolling)
				Draw();
			break;
		}

		case UpdateStep::DRAW_STATIC_COLLISION:
		{
			Draw();
			break;
		}
	}
	return false;
}

void Boulder::Draw()
{
	// get the coordinate on screen of the boulder
	signed char xOnScreen = MapManager::GetXOnScreen(X);
	signed char yOnScreen = MapManager::GetYOnScreen(Y);
	// draw the two halves of it
	arduboy.drawBitmapExtended(xOnScreen, yOnScreen, SpriteData::Boulder, SpriteData::BOULDER_SPRITE_WIDTH, SpriteData::BOULDER_SPRITE_HEIGHT, WHITE, false);
	arduboy.drawBitmapExtended(xOnScreen + SpriteData::BOULDER_SPRITE_WIDTH, yOnScreen, SpriteData::Boulder, SpriteData::BOULDER_SPRITE_WIDTH, SpriteData::BOULDER_SPRITE_HEIGHT, WHITE, true);
	
	// get the moving direction
	signed char movingDirection = IsPropertySet(MIRROR_X) ? -1 : 1;
	bool isBoulderRolling = !IsPropertySet(SPECIAL_);
	// compute the correct animation id
	if (arduboy.everyXFrames(ANIM_ROTATION_SPEED) && isBoulderRolling)
		AnimFrameId = (AnimFrameId + 1) % BOULDER_ROLLING_FRAME_COUNT;
	signed char localAnimFrameId = (movingDirection > 0) ? AnimFrameId : (BOULDER_ROLLING_FRAME_COUNT - AnimFrameId) % BOULDER_ROLLING_FRAME_COUNT;
	// draw the rotating shadow
	signed char isShadowRight = (localAnimFrameId == 0) || (localAnimFrameId == 3) ? 0 : 1;
	signed char shadowX = xOnScreen + (2 * !isShadowRight) + (SpriteData::BOULDER_SPRITE_WIDTH * isShadowRight);
	arduboy.drawBitmapExtended(shadowX, yOnScreen + 2, SpriteData::BoulderShadow[localAnimFrameId / 2], SpriteData::BOULDER_SHADOW_SPRITE_WIDTH, SpriteData::BOULDER_SHADOW_SPRITE_HEIGHT, BLACK, isShadowRight);
}
