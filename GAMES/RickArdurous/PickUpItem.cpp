/*
 * This file implement the base class for all the pickup items
*/

#include "RickArdurous.h"
#include "SpriteData.h"
#include "PickUpItem.h"
#include "MapManager.h"
#include "Rick.h"
#include "FXManager.h"

/**
 * Do a a collision box check with the specified size to see if the Main character is at the same place as this pickup,
 * and if yes, call the virtual pickup function.
 * @param width the width of this pickup
 * @param height the height of this pickup
 */
bool PickUpItem::DoesRickPickMeUp(unsigned char width, unsigned char height)
{
	int centerRickX = Rick::GetCenterX();
	int centerRickY = Rick::GetCenterY();
	return (Rick::IsAlive() && (centerRickX >= X) && (centerRickX <= X + width) && (centerRickY >= Y) && (centerRickY <= Y + height));
}

/**
 * Update and Draw a shiny star around the sprite of this Pickup Item according to the specified 
 * min and max values.
 * @param minX the included left border where the star can shine
 * @param maxX the excluded right border where the star can shine
 * @param minY the included top border where the star can shine
 * @param maxY the excluded bottom border where the star can shine 
 */
void PickUpItem::UpdateShineStar(unsigned char screenX, unsigned char screenY, signed char minX, signed char maxX, signed char minY, signed char maxY)
{
	if (AnimFrameId == -1)
	{
		// check if we need to start a shining star animation
		if (arduboy.everyXFrames(PauseTime))
		{
			// init the shining star animation id
			AnimFrameId = 0;
			
			// draw a random position for the shinning star
			// first choose the vertical position.
			ShineStarShiftY = random(minY, maxY);
			// If the y position is on top, choose a free x position,
			// otherwise we choose a x position on one of the two side of the statuette
			if (ShineStarShiftY == minY)
			{
				ShineStarShiftX = random(minX, maxX);
			}
			else
			{
				ShineStarShiftX = random(0,2);
				if (ShineStarShiftX == 0)
					ShineStarShiftX = minX - 1;
				else
					ShineStarShiftX = maxX;
			}
			
			// choose a random next pause time
			PauseTime = random(MIN_PAUSE_TIMES, MAX_PAUSE_TIMES);
		}
	}
	else
	{
		if (arduboy.everyXFrames(ANIM_SPEED))
		{
			AnimFrameId += AnimDirection;
			if ((AnimFrameId == -1) || (AnimFrameId == SpriteData::SHINE_STAR_SPRITE_FRAME_COUNT - 1))
				AnimDirection = -AnimDirection;
		}
	}
	
	//draw the star if needed
	if (AnimFrameId > -1)
		arduboy.drawBitmapExtended(screenX + ShineStarShiftX,
							screenY + ShineStarShiftY,
							SpriteData::ShineStar[AnimFrameId],
							SpriteData::SHINE_STAR_SPRITE_WIDTH,
							SpriteData::SHINE_STAR_SPRITE_HEIGHT,
							INVERT, false);
}

/**
 * Common Update function for the pickup, with some specific parameter comming from the derivated classes.
 * @param step the step parameter comming from the virtual Update function
 * @param sprite the sprite of the pickup to draw
 * @param spriteWidth width in pixel of the sprite to draw
 * @param spriteHeight height in pixel of the sprite to draw
 * @param isCrate true if this pickup is a crate (this is used to know where to draw the shining stars)
 * @return true if Rick has picked up this item during this frame
 */
bool PickUpItem::PickupUpdate(UpdateStep step, const unsigned char sprite[], unsigned char spriteWidth, unsigned char spriteHeight, bool isCrate)
{
	if ((step == UpdateStep::DRAW_IGNORED_BY_ENEMIES) && IsPropertySet(Item::PropertyFlags::ALIVE))
	{
		// check if the player pick me up
		if (DoesRickPickMeUp(spriteWidth, spriteHeight))
		{
			ClearProperty(Item::PropertyFlags::ALIVE);
			FXManager::StartFXCommand(FXManager::BufferId::GAME, {2, 0, 1, 0, 1, 78, 2, 3});
			return true;
		}
		
		// draw the sprite
		signed char screenX = MapManager::GetXOnScreen(X);
		signed char screenY = MapManager::GetYOnScreen(Y);
		arduboy.drawBitmapExtended(screenX, screenY, sprite, spriteWidth, spriteHeight, WHITE, false);
		
		// draw the shiny star
		if (isCrate)
			UpdateShineStar(screenX, screenY, -2, 9, 1, 6);
		else
			UpdateShineStar(screenX, screenY, -2, 3, -2, 4);
	}
	return false;
}
