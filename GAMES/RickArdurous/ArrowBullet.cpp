/*
 * Implement the logic and drawing of the Bullet fired by Rick or an Arrow thrown by the Arrow Launcher
*/

#include "RickArdurous.h"
#include "ArrowBullet.h"
#include "MapManager.h"
#include "SpriteData.h"

ArrowBullet::ArrowBullet()
{
}

ArrowBullet::ArrowBullet(bool isArrow)
{
	if (isArrow)
		SetProperty(PropertyFlags::SPECIAL_);
}

/**
 * Make this bullet alive, and start moving from the specified position in the specified direction
 * @param the x position, of the muzzle of the gun, not including the pixel of the gun (it should be the free pixel just after the gun)
 * @param the y position of the muzzle of the gun
 * @param isMovingToLeft if true, then the bullet will move to the left of the screen, otherwise to the right.
 */
void ArrowBullet::Fire(int x, unsigned char y, bool isMovingToLeft)
{
	// make the bullet alive
	SetProperty(Item::PropertyFlags::ALIVE);
	// set the starting position and orienatation
	if (isMovingToLeft)
	{
		SetProperty(Item::PropertyFlags::MIRROR_X);
		X = x - GetWidth();
	}
	else
	{
		ClearProperty(Item::PropertyFlags::MIRROR_X);
		X = x;
	}
	Y = y;
	// for the first frame we set the bullet speed to zero
	CurrentBulletSpeed = 0;
	// add myself to the map manager in order to be updated
	MapManager::AddItem(this);
}

/**
 * Get the Width of the arrow/bullet depending of its type.
 * The Arrows are usually longer than the bullets.
 * @return ARROW_WIDTH if this instance is an arrow, or BULLET_WIDTH if it is a bullet
 */
unsigned char ArrowBullet::GetWidth()
{
	return IsPropertySet(Item::PropertyFlags::SPECIAL_) ? ARROW_WIDTH : BULLET_WIDTH;
}

/**
 * Draw a full ray of the bullet with the specified color.
 * The ray length equals to the bullet length + the speed of the bullet.
 */
void ArrowBullet::DrawBulletRay(unsigned char color)
{
	// compute the start position of the raycast
	int bulletRayCastStartX = X;
	if (IsPropertySet(Item::PropertyFlags::MIRROR_X))
		bulletRayCastStartX = X - CurrentBulletSpeed;

	// draw the line of the ray cast
	arduboy.drawFastHLine(MapManager::GetXOnScreen(bulletRayCastStartX),
							MapManager::GetYOnScreen(Y), CurrentBulletSpeed + GetWidth(), color);
}

/**
 * Search for a pixel of the specified color along the bullet ray cast.
 * If that pixel of that color is found, return the world position of that pixel,
 * otherwise return the specific value NO_PIXEL_FOUND (60000)
 */
int ArrowBullet::SearchForPixelColorAlongBulletRay(unsigned char color)
{
	// get the ray cast world position and direction
	signed char startXOnScreen = 0;
	signed char endXOnScreen = CurrentBulletSpeed + GetWidth() - 1;
	signed char direction = 1;
	if (IsPropertySet(Item::PropertyFlags::MIRROR_X))
	{
		startXOnScreen = GetWidth() - 1;
		endXOnScreen = -CurrentBulletSpeed;
		direction = -1;
	}
	// get the ray cast positions on screen
	signed char xOnScreen = MapManager::GetXOnScreen(X);
	signed char yOnScreen = MapManager::GetYOnScreen(Y);
	// iterate on the frame buffer to check if any pixel is set
	for (signed char i = startXOnScreen; i != endXOnScreen; i += direction)
		if (arduboy.getPixel(xOnScreen + i, yOnScreen) == color)
				return (X + i - (SpriteData::SPARKS_SPRITE_WIDTH / 2));
	// no collision found
	return NO_PIXEL_FOUND;
}

/**
 * Kill this Instance without playing a sparks VFX.
 * This function can be called if the bullet move outside the screen,
 * or touches a soft body such as an enemy that don't generate sparks.
 */
void ArrowBullet::KillBulletWithoutSparks()
{
	ClearProperty(Item::PropertyFlags::ALIVE);
	SparksAnimFrameId = SpriteData::SPARKS_SPRITE_FRAME_COUNT;
}

bool ArrowBullet::Update(UpdateStep step)
{
	switch (step)
	{
		case UpdateStep::DRAW_LETHAL:
			// draw a line along the whole move of the bullet
			if (IsPropertySet(Item::PropertyFlags::ALIVE))
				DrawBulletRay(WHITE);
			break;

		case UpdateStep::ERASE_BULLET:
			// erase the line along the whole move of the bullet
			if (IsPropertySet(Item::PropertyFlags::ALIVE))
			{
				// before erasing first check if some pixels of the ray cast has been erased (which means the bullet has hit something)
				bool shouldKillBullet = (SearchForPixelColorAlongBulletRay(BLACK) != NO_PIXEL_FOUND);
				if (shouldKillBullet)
					KillBulletWithoutSparks();
				// now erase the bullet ray
				DrawBulletRay(BLACK);
				// return the value depending if the bullet was killed
				return shouldKillBullet;
			}
			break;
			
		case UpdateStep::CHECK_STATIC_COLLISION:
		{
			// draw the bullet or the sparks depending if the bullet is alive or not
			if (IsPropertySet(Item::PropertyFlags::ALIVE))
			{
				// first check the collition and get the impact position if any
				int impactPosition = SearchForPixelColorAlongBulletRay(WHITE);
				
				// move the bullet X according to its direction
				if (IsPropertySet(Item::PropertyFlags::MIRROR_X))
					X -= CurrentBulletSpeed;
				else
					X += CurrentBulletSpeed;
				
				// from the second frame set the current bullet speed to its nominal value
				CurrentBulletSpeed = IsPropertySet(Item::PropertyFlags::SPECIAL_) ? ARROW_SPEED : BULLET_SPEED;
				
				// draw the bullet
				signed char xOnScreen = MapManager::GetXOnScreen(X);
				arduboy.drawFastHLine(xOnScreen, MapManager::GetYOnScreen(Y), GetWidth(), WHITE);
				
				// check if the bullet collided on the static collision along its movement, if yes prepare the sparks fx for the next frame
				if (impactPosition != NO_PIXEL_FOUND)
				{
					// the bullet hit the wall, so kill it
					ClearProperty(Item::PropertyFlags::ALIVE);
					// init the postion for the sparks sprite, and reset the sparks animation frame id
					X = impactPosition;
					Y -= (SpriteData::SPARKS_SPRITE_HEIGHT / 2);
					SparksAnimFrameId = 0;
				}
				else if ((xOnScreen < 0) /*|| (xOnScreen >= WIDTH)*/)
				{
					// if the bullet is outside the screen, kill it immediately without playing sparks
					KillBulletWithoutSparks();
					return true;
				}
			}
			else
			{
				if (SparksAnimFrameId < SpriteData::SPARKS_SPRITE_FRAME_COUNT)
				{
					// draw the sparks
					arduboy.drawBitmapExtended(MapManager::GetXOnScreen(X), MapManager::GetYOnScreen(Y), SpriteData::Sparks[SparksAnimFrameId], SpriteData::SPARKS_SPRITE_WIDTH, SpriteData::SPARKS_SPRITE_HEIGHT, INVERT, IsPropertySet(Item::PropertyFlags::MIRROR_X));
					// increase the frame id of the sparks
					if (arduboy.everyXFrames(SPARKS_ANIM_SPEED))
						SparksAnimFrameId++;
				}
				else
				{
					// we reach the end of the sparks animation, so return true to be removed from the manager
					return true;
				}
			}
			break;
		}
	}
	// return false by default
	return false;
}
