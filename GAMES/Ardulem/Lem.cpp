#include "Ardulem.h"
#include "Lem.h"
#include "SpriteData.h"
#include "MapManager.h"
#include "LemManager.h"
#include "HUD.h"

// static functions
int16_t Lem::sRecursiveUpdateStateCount = 0;
int16_t Lem::sStateThatNeedsUpdateInTheLastFrame = 
	(1 << Lem::CLIMB_TOP) | 
	(1 << Lem::START_FALL) |
	(1 << Lem::DIG_HORIZ) |
	(1 << Lem::CRASH) | 
	(1 << Lem::BYE_BYE_BOOM) |
	(1 << Lem::EXPLOSION_FX) |
	(1 << Lem::FALL);

void Lem::Spawn()
{
	// set the position
	mPosX = MapManager::GetStartX() + 6;
	mPosY = MapManager::GetStartY();
	mIsAClimber = 0;
	mIsAParachuter = 0;
	mIsDirectionMirrored = 0;
	mCurrentAnimFrame = 0;
	mCurrentState = StateId::FALL;
}

bool Lem::Update()
{
	// do nothing if the Lem is not alive
	if (GetCurrentState() > StateId::DEAD)
	{
		// reinit the recursive update count for the current lem
		sRecursiveUpdateStateCount = 0;

		// update the current animation (which will make the lem position move)
		bool doesNeedUpdate = UpdateCurrentAnim();
		
		// update the ai of the lem depending on the new position if it has moved
		if (doesNeedUpdate)
			return UpdateState();
	}
	return false;
}

bool Lem::UpdateState()
{
	// increase the update count, and early exit if there's too many changes
	sRecursiveUpdateStateCount++;
	if (sRecursiveUpdateStateCount > 3)
		return false;

	// check if the lem arrived in home
	if ((mCurrentState != StateId::DEAD) && (mPosX+3 >= MapManager::GetHomeX()) && (mPosX <= MapManager::GetHomeX()) && (mPosY+5 >= MapManager::GetHomeY()) && (mPosY <= MapManager::GetHomeY()))
	{
		LemManager::NotifyInHomeLem();
		SetCurrentState(StateId::DEAD, 0, 0);
		return true;
	}

	// then update its current state
	switch (GetCurrentState())
	{
		case StateId::EXPLOSION_FX: UpdateExplosion(); break;
		case StateId::CRASH: UpdateCrash(); break;
		case StateId::BYE_BYE_BOOM: UpdateByeByeBoom(); break;
		case StateId::WALK: UpdateWalk(); break;
		case StateId::BLOCKER: UpdateBlocker(); break;
		case StateId::DIG_DIAG: UpdateDigDiag(); break;
		case StateId::DIG_HORIZ: UpdateDigHoriz(); break;
		case StateId::DIG_VERT: UpdateDigVert(); break;
		case StateId::STAIR: return UpdateStair();
		case StateId::CLIMB: UpdateClimb(); break;
		case StateId::CLIMB_TOP: UpdateClimbTop(); break;
		case StateId::START_FALL: UpdateStartFall(); break;
		case StateId::FALL: UpdateFall(); break;
		case StateId::FALL_TO_DEATH: UpdateFallToDeath(); break;
		case StateId::PARACHUTE: UpdateParachute(); break;
	}
	return false;
}

bool Lem::IsXInsideWorld(int16_t x)
{
	return (x >=0) && (x <= 255);
}

bool Lem::IsYInsideWorld(int16_t y)
{
	return (y >=0) && (y <= 63);
}

/*
 * Compute how far is the ground from the specified coordinates. Return 0 if there's ground
 * at the specified coordinate, and a big number if the ground is farer than 8 pixels away.
 */
int16_t Lem::GetGroundDepth(int16_t x, int16_t y, bool checkInFront, bool checkBehind)
{
	const int16_t CHECK_DEPTH = 3;
	const int16_t INFINITE_DEPTH = 255;

	// if y is outside the map, that means there's no ground
	if (!IsXInsideWorld(x) || !IsYInsideWorld(y))
		return INFINITE_DEPTH;

	// the return value (we will take the min value)
	int16_t depth = INFINITE_DEPTH;

	// pixel under
	unsigned char pixelsUnder = MapManager::GetPixelsColumn(x, y, CHECK_DEPTH, true);
	if (pixelsUnder != 0)
	{
		// search the first set pixel
		for (int16_t i = 0; i < CHECK_DEPTH; i++)
			if (pixelsUnder & (1<<i))
			{
				depth = i;
				break;
			}
	}

	// pixel in front
	if (checkInFront)		
	{
		int16_t frontDepth = GetGroundDepth(mIsDirectionMirrored ? x-1 : x+1, y, false, false);
		if (frontDepth < depth)
			depth = frontDepth;
	}
	
	// pixel behind
	if (checkBehind)
	{
		int16_t behindDepth = GetGroundDepth(mIsDirectionMirrored ? x+1 : x-1, y, false, false);
		if (behindDepth < depth)
			depth = behindDepth;
	}

	return depth;
}

bool Lem::IsThereRoofAt(int16_t x, int16_t y)
{ 
	return IsXInsideWorld(x) && IsYInsideWorld(y) && ((MapManager::GetPixelsColumn(x, y, 1, false)/*&1*/) != 0);
}

/*
 * x: x world pos of the wall
 * y: y world pos of the top of the wall.
 * height: the height of the wall (downward from y) that you want to test
 */
int16_t Lem::GetWallHeight(int16_t x, int16_t y, int16_t height, bool shouldCheckAddedStairs)
{
	// if the x is outside the world, there's no wall
	if (!IsXInsideWorld(x))
		return 0;
	
	// clamp y and height for staying inside the world
	if (y < 0)
	{
		height += y;
		y == 0;
	}
	else if (y + height > 63)
	{
		height = 64 - y;
	}
	// if there's no height anymore, return 0
	if (height <= 0)
		return 0;
	
	// get the column of pixels
	unsigned char pixelColumn = MapManager::GetPixelsColumn(x, y, height, shouldCheckAddedStairs);
	// and try to find the highest pixel to determines the height of the wall
	for (int16_t i = 0; i < height; ++i)
		if ((pixelColumn >> i) & 0x01)
			return height-i;

	// no wall at all was found
	return 0;
}

// check 3 column in front of the lem (depending of its walking direction) if there's a wall
// taller than a step.
bool Lem::IsThereAWallInFrontOfYou(int16_t minHeight, bool shouldCheckAddedStairs)
{
	int16_t y = mPosY+1;
	int16_t x = mPosX+3;
	int16_t incX = 1;
	if (mIsDirectionMirrored)
	{
		x = mPosX-1;
		incX = -1;
	}

	// check for the 3 column in front
	for (int16_t i = 0; i < 3; ++i)
	{
		// check if the wall is taller than a step
		int16_t wallHeight = GetWallHeight(x, y, 5, shouldCheckAddedStairs);
		if (wallHeight > minHeight)
			return true;

		// increment x for the test of next column
		x += incX;
	}
	return false;
}

/*
 * Tell if it is the last frame of the last anim frame for the current anim 
 *(i.e. next loop, the anim will return to the first frame)
 */
bool Lem::IsLastFrame()
{
	return (mCurrentAnimFrame == GetFrameCountForCurrentAnim()-1) &&
			!((arduboy.frameCount+1) % GetFrameRateForCurrentAnim(true));
}

void Lem::UpdateByeByeBoom()
{
	// if it's the last frame, make a hole in the level, and change state for the explosion
	if (IsLastFrame())
	{
		// carve the map with a round explosion
		int16_t exploX = mPosX - 3;
		int16_t exploY = mPosY - 1;
		unsigned char lineY = (exploY >= 0) ? exploY >> 3 : 0;
		unsigned char bitShift = exploY % 8;
		
		// iterate on all the columns
		for (int16_t i = 0; i < 11; ++i)
		{
			if ((i == 0) || (i == 10))
				MapManager::Delete16AlignedPixels(exploX + i, lineY, 0x007C << bitShift);
			else if ((i < 3) || (i > 7))
				MapManager::Delete16AlignedPixels(exploX + i, lineY, 0x00FE << bitShift);
			else
				MapManager::Delete16AlignedPixels(exploX + i, lineY, 0x01FF << bitShift);
		}
		
		// and set the new state of the lem to explosion, in order to display the nice VFX
		mIsAClimber = 0;
		mIsAParachuter = 0;
		mIsDirectionMirrored = 0;
		SetCurrentState(StateId::EXPLOSION_FX, 0, 0);
	}
}

void Lem::CheckGroundAndStepDownOrFall(bool checkInFront, int16_t shiftXIfFall)
{
	// check the pixel under
	int16_t groundDepth = GetGroundDepth(GetPosXPlus(1), mPosY+6, checkInFront, true);
	if (groundDepth > 2)
	{
		// no pixel in front of me (inside), and no pixel just under, so fall
		SetCurrentState(StateId::START_FALL, shiftXIfFall, 1);
		return;
	}
	else
	{
		// there's a ground a bit under (like a step down) so adjust the y
		mPosY += groundDepth;
	}
}

void Lem::UpdateWalk()
{
	bool isMirrored = mIsDirectionMirrored;

	// check if there's a stair in front of me, or a ground under my feet, if no ground, I start to fall
	int16_t wallHeight = GetWallHeight(GetPosXPlus(2), mPosY+4, 3, true);
	if (wallHeight > 0)
	{
		// step on the stair
		mPosY -= (wallHeight-1);
	}
	else
	{
		// no need to check in front, cause we have already check with the step up, with the wall height
		CheckGroundAndStepDownOrFall(false, -1);
	}

	// get the x pixel in front of me
	int16_t inFrontXOutside = GetPosXPlus(3);

	// check if there's no blocker in front of me, then I would need to reverse my direction
	if (LemManager::IsThereABlockerAt(inFrontXOutside, mPosY+1, isMirrored))
	{
		ReverseMirroredDirection();
		return;
	}
	
	// now check if there's a wall in front (don't check the added stairs to go through)
	wallHeight = GetWallHeight(inFrontXOutside, mPosY+1, 5, false);
	if (wallHeight > 2)
	{
		// but if the wall is taller, either we climb or we need to reverse direction
		if (mIsAClimber)
			SetCurrentState(StateId::CLIMB, isMirrored ? 0 : 1, 0);
		else
			ReverseMirroredDirection();
	}
}

/*
 * Tell if this lem is blocking the specified position.
 */
bool Lem::IsBlockingPosition(unsigned char worldX, unsigned char worldY, bool isWalkingInMirror)
{
	if ((mCurrentState == StateId::BLOCKER) && (worldY+4 >= mPosY+1) && (worldY <= mPosY+5) && (worldX >= mPosX) && (worldX < mPosX+5))
	{
		// The second test is to avoid to block lems inside a blocker (for example if a lem falls on a blocker)
		int16_t xBefore = worldX + (isWalkingInMirror ? 1 : -1);
		return ((xBefore < mPosX) || (xBefore > mPosX+4));
	}
	return false;
}

void Lem::UpdateBlocker()
{
	// get the pixel under my feet, if no ground, I start to fall
	if (GetGroundDepth(mPosX+2, mPosY+6, false, false) > 2)
	{
		// check if I need to fall toward left or right (if no ground on my left, set mirror to true)
		mIsDirectionMirrored = (GetGroundDepth(mPosX+1, mPosY+6, false, false) > 0) ? 1 : 0;
		SetCurrentState(StateId::START_FALL, mIsDirectionMirrored ? -1 : 0, 1);
	}
}

void Lem::UpdateExplosion()
{
	// check if it is a climber because we use the 3 flags to extends the frame count of the explosion
	if (mIsAClimber && IsLastFrame())
		SetCurrentState(StateId::DEAD, 0, 0);
}

void Lem::Dig8Pixels(int16_t x, int16_t y, uint16_t pixels)
{
	MapManager::Delete16AlignedPixels(x, (y >> 3), pixels << (y % 8));
}

void Lem::UpdateDigDiag()
{
	// remove specific pixels depending on the frame num
	switch (mCurrentAnimFrame)
	{
		case 0:
		{
			if (GetGroundDepth(mPosX+2, mPosY+6, true, true) > 0)
				SetCurrentState(StateId::START_FALL, mIsDirectionMirrored ? -1 : 0, 1);
			break;
		}
		case 1:
		case 2:
		{
			Dig8Pixels(GetPosXPlus(2 + mCurrentAnimFrame), mPosY, 0x3F);
			break;
		}
		case 3:
		{
			Dig8Pixels(GetPosXPlus(4), mPosY, 0x3F);
			break;
		}
	}
}

void Lem::UpdateDigHoriz()
{
	// only test ground in specific frame id (when the lem is moving)
	bool shouldTestGround = false;	
	
	// remove specific pixels depending on the frame num
	switch (mCurrentAnimFrame)
	{
		case 0:
		case 1:
		case 2:
		{
			Dig8Pixels(GetPosXPlus(2 + mCurrentAnimFrame), mPosY, 0x003F);
			break;
		}
		case 3:
		{
			shouldTestGround = true;
			break;
		}
		case 5:
		{
			shouldTestGround = true;
			// in last frame check if there's some pixel in front to continue to dig
			if ((GetWallHeight(GetPosXPlus(3), mPosY+1, 5, true) == 0) &&
				(GetWallHeight(GetPosXPlus(4), mPosY+1, 5, true) == 0))
				SetCurrentState(StateId::WALK, mIsDirectionMirrored ? 2 : 0, 0);
			break;
		}
	}
	
	// get the pixel under my feet, if no ground, I start to fall
	// we need to use the same function as walk, because the dig horiz can be triggered
	// by the player 3 pixels before the wall, so if the ground is going down, the lem
	// need to go down while reaching the wall, or he will fall before reaching the wall and stop the digging
	if (shouldTestGround)
		CheckGroundAndStepDownOrFall(true, mIsDirectionMirrored ? -1 : 0);
}

void Lem::UpdateDigVert()
{
	// only test ground in specific frame id (when the lem is moving)
	bool shouldTestGround = false;
	
	// remove specific pixels depending on the frame num
	switch (mCurrentAnimFrame)
	{
		case 2:
		{
			int16_t y = mPosY + 6;
			MapManager::SetPixel(mPosX, y, false);
			MapManager::SetPixel(mPosX+4, y, false);
			break;
		}
		case 3:
		{
			int16_t y = mPosY + 5;
			MapManager::SetPixel(mPosX+1, y, false);
			MapManager::SetPixel(mPosX+3, y, false);
			break;
		}
		case 4:
		{
			int16_t y = mPosY + 5;
			MapManager::SetPixel(mPosX+2, y, false);
			shouldTestGround = true;
			break;
		}
	}
	
	// get the pixel under my feet, if no ground, I start to fall
	if (shouldTestGround)
	{
		if (GetGroundDepth(mPosX+2, mPosY+6, true, true) > 0)
			SetCurrentState(StateId::START_FALL, mIsDirectionMirrored ? -1 : 0, 1);
	}
}

bool Lem::UpdateStair()
{
	// remove specific pixels depending on the frame num
	switch (mCurrentAnimFrame)
	{
		case 1:
		{
			int16_t inFrontX2 = mPosX + 2;
			int16_t inFrontX3 = GetPosXPlus(3);
			// check if there's a blocker or a wall, after steping on the step
			// (do not consider the stairs added by me or another stairer)
			if (LemManager::IsThereABlockerAt(inFrontX2, mPosY+1, mIsDirectionMirrored) || 
				LemManager::IsThereABlockerAt(inFrontX3, mPosY+1, mIsDirectionMirrored) || 
				(GetWallHeight(inFrontX2, mPosY+1, 5, false) > 2) || (GetWallHeight(inFrontX3, mPosY+1, 5, false) > 2))
			{
				ReverseMirroredDirection();
				if (mIsDirectionMirrored)
					mPosX--;
				else
					mPosX++;
				return false;
			}
		}
		break;

		case 3:
			int16_t x1 = GetPosXPlus(3);
			int16_t x2 = GetPosXPlus(4);
			int16_t y = mPosY + 5;
			MapManager::SetPixel(x1, y,  true);
			MapManager::SetPixel(x2, y,  true);
			MapManager::SetPixel(x1, y+1, true);
		break;
	}
	
	// get the pixel under my feet, if no ground, I start to fall
	if (GetGroundDepth(GetPosXPlus(1), mPosY+6, true, true) > 0)
	{
		SetCurrentState(StateId::START_FALL, mIsDirectionMirrored ? -1 : 0, 1);
		return true;
	}
	
	// no need to cancel my timer
	return false;
}

void Lem::UpdateClimb()
{
	unsigned char posY = mPosY;
	bool isMirrored = mIsDirectionMirrored;

	// if we reach the top of the screen or if we reach a roof, we fall down opposite to the wall
	if ((posY == 0) || IsThereRoofAt(GetPosXPlus(1), posY))
	{
		ReverseMirroredDirection();
		SetCurrentState(StateId::START_FALL, isMirrored ? 0 : -2, 0);
		return;
	}
	
	// then check if we reach the top of the climb
	// (we can consider the stairs for climber to climb above the stairs, or not to stop through the stairs)
	int16_t wallHeight = GetWallHeight(GetPosXPlus(2), posY+1, 5, true);
	if (wallHeight <= 3)
	{  
		SetCurrentState(StateId::CLIMB_TOP, isMirrored ? -2 : 0, 0);
		return;
	}
}

void Lem::UpdateClimbTop()
{ 
	// no need to check the ground during that anim, because the whole anim is played on the wall edge
	// and when I have finished the climb top anim, I go to Walk
	if (IsLastFrame()){
		SetCurrentState(StateId::WALK, 0, 0);}
}

void Lem::UpdateStartFall()
{
	// get the pixel under my feet, if I touch ground, go back to walk
	if (GetGroundDepth(mPosX+2, mPosY+6, true, true) == 0)
	{
		SetCurrentState(StateId::WALK, 1, 0);
		return;
	}
	
	// and when I have finished the start to Fall anim, I go to Fall
	if (IsLastFrame())
		SetCurrentState(StateId::FALL, 0, 0);
}

void Lem::UpdateFall()
{
	// get the pixel under my feet, if I touch ground, go back to walk
	if (GetGroundDepth(mPosX+2, mPosY+6, true, true) == 0)
		SetCurrentState(StateId::WALK, 1, 0);
	
	// if there's no ground and it is the last frame, we move to the fall to death anim
	if (IsLastFrame())
	{
		// check if I got a parachute, or not
		if (mIsAParachuter)
			SetCurrentState(StateId::PARACHUTE, 1, 0);
		else
			SetCurrentState(StateId::FALL_TO_DEATH, 0, 0);
	}
}

void Lem::UpdateFallToDeath()
{
	// if I got a parachute, change my state
	if (mIsAParachuter)
	{
		SetCurrentState(StateId::PARACHUTE, 1, 0);
		return;
	}

	// get the pixel under my feet, if I touch ground, I just crash
	if (GetGroundDepth(mPosX+2, mPosY+6, true, true) == 0)
		SetCurrentState(StateId::CRASH, 0, -1);
}

void Lem::UpdateCrash()
{
	// if it's the last frame, we are dead
	if (IsLastFrame())
		SetCurrentState(StateId::DEAD, 0, 0);
}

void Lem::UpdateParachute()
{
	// get the pixel under my feet, if I touch ground, I go back to walk
	if (GetGroundDepth(mPosX+2, mPosY+6, true, true) == 0)
		SetCurrentState(StateId::WALK, 0, -1);
}

/*
 * Get the frame rate for the current anim id
 */
uint16_t Lem::GetFrameRateForCurrentAnim(bool useSlowAnimation)
{
	if (HUD::IsGameSpeedUp())
		return (useSlowAnimation && (mCurrentState == StateId::FALL)) ? 2 : 1;

	switch (mCurrentState)
	{
		case StateId::EXPLOSION_FX:
			return 1;
		case StateId::FALL:
			return useSlowAnimation ? 4 : 2;
		case StateId::FALL_TO_DEATH:
			return 2;
		case StateId::START_FALL:
		case StateId::CRASH:
			return 3;
		case StateId::PARACHUTE:
			return 6;
		case StateId::CLIMB_TOP:
			return 7;
		default:
			return 10;
	}
}

/*
 * This function return the m_XPos + the specified parameter as if the lem is not mirrored, 
 * but adjust the parameter with the correct value if the lem is mirrored 
 * (using the width of the sprite)
 */ 
int16_t Lem::GetPosXPlus(int16_t plus)
{
	return mPosX + (mIsDirectionMirrored ? GetFrameWidthForCurrentAnim() - 1 - plus : plus);
}

/*
 * Get the number of frame of the current animation
 */
uint16_t Lem::GetFrameCountForCurrentAnim()
{
	return pgm_read_byte_near(LemAnimFrameCountPerLemState + mCurrentState);
}

/*
 * Get the width of the current animation
 */
uint16_t Lem::GetFrameWidthForCurrentAnim()
{
	return pgm_read_byte_near(LemAnimWidthPerLemState + mCurrentState);
}

bool Lem::UpdateCurrentAnim()
{
	// declare the return value
	bool doesNeedUpdate = false;
	int16_t frameRateForCurrentAnim = GetFrameRateForCurrentAnim(false);
	bool checkLastFrame = (frameRateForCurrentAnim == 1); // if we change anim frame every game frame, we need to check the last frame

	// check if we need to change the animation frame
	if (arduboy.everyXFrames(frameRateForCurrentAnim))
	{
		// get the current frame counter and increase it
		unsigned char currentFrame = (mCurrentAnimFrame + 1) % GetFrameCountForCurrentAnim();
		mCurrentAnimFrame = currentFrame;
		
		// and find the correct animation frame
		switch (mCurrentState)
		{
			case StateId::EXPLOSION_FX:
				if (currentFrame == 0)
				{
					// simulate a bigger frame count number (up to 63, using 6 bits) by using the flag single bits
					if (mIsDirectionMirrored == 0)
					{
						mIsDirectionMirrored = 1;
					}
					else if (mIsAParachuter == 0)
					{
						mIsAParachuter = 1;
						mIsDirectionMirrored = 0;
					}
					else
					{
						mIsAClimber = 1;
						mIsAParachuter = 0;
						mIsDirectionMirrored = 0;
					}
				}
				doesNeedUpdate = true; // the explosion FX does need update for every frame of the frame rate
				break;
			case StateId::CRASH:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemCrash[currentFrame], sizeof(anim_LemCrash[0]));
				break;
			case StateId::BYE_BYE_BOOM:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemBomb[currentFrame], sizeof(anim_LemBomb[0]));
				break;
			case StateId::WALK:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemWalk[currentFrame], sizeof(anim_LemWalk[0]));
				break;
			case StateId::BLOCKER:
				UpdateOneAnimFrame(anim_LemBlocker[currentFrame], sizeof(anim_LemBlocker[0]));
				doesNeedUpdate = true; // blocker never move, but we can dig under their feet, so they have to check their state some times to time
				break;
			case StateId::DIG_DIAG:
				UpdateOneAnimFrame(anim_LemDigDiagonal[currentFrame], sizeof(anim_LemDigDiagonal[0]));
				// need to check ground on frame 0, need to dig on frame (1,2,3), need to move on frame 5
				doesNeedUpdate = (currentFrame != 4);
				break;
			case StateId::DIG_HORIZ:
				UpdateOneAnimFrame(anim_LemDigHorizontal[currentFrame], sizeof(anim_LemDigHorizontal[0]));
				// need to dig on the first frames (0,1,2), on frame 3 because we move 
				// on frame 4, we move so we ought to update, but in fact we will check ground in last frame
				// so update only on frame 0 to 3, and the 5 will be updated in the last frame
				doesNeedUpdate = (currentFrame < 4);
				break;
			case StateId::DIG_VERT:
				UpdateOneAnimFrame(anim_LemDigVertical[currentFrame], sizeof(anim_LemDigVertical[0]));
				// need to dig on frame 2,3,4, and move on frame 3
				doesNeedUpdate = ((currentFrame >= 2) && (currentFrame <= 4));
				break;
			case StateId::STAIR:
				UpdateOneAnimFrame(anim_LemStair[currentFrame], sizeof(anim_LemStair[0]));
				// need to reverse direction in 1, add pixel in 3, and move in 3 and 5
				// so update on all the odd frames
				doesNeedUpdate = (currentFrame % 2);
				break;
			case StateId::CLIMB:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemClimb[currentFrame], sizeof(anim_LemClimb[0]));
				break;
			case StateId::CLIMB_TOP:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemClimbTop[currentFrame], sizeof(anim_LemClimbTop[0]));
				break;
			case StateId::START_FALL:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemStartFall[currentFrame], sizeof(anim_LemStartFall[0]));
				break;
			case StateId::FALL:
				// special case for the fall we want to change the frame less often than we can the update one frame to move
				if (!arduboy.everyXFrames(GetFrameRateForCurrentAnim(true)))
				{
					// so cancel the increase of the current frame in that case
					currentFrame--;
					mCurrentAnimFrame = currentFrame;
				}
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemFall[currentFrame % ANIM_LEM_FALL_FRAME_COUNT], sizeof(anim_LemFall[0]));
				break;
			case StateId::FALL_TO_DEATH:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemFallToDeath[currentFrame], sizeof(anim_LemFallToDeath[0]));
				break;
			case StateId::PARACHUTE:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemPara[currentFrame], sizeof(anim_LemPara[0]));
				break;
		}
	}
	else
	{
		// if we didn't just switch to the last frame, so we can check it
		checkLastFrame = true;
	}

	// special case for the anims that doesn't loop, if it is their last frame, then need to also update
	// but do not override doesNeedUpdate, if it is already true
	if (checkLastFrame && !doesNeedUpdate && IsLastFrame())
		doesNeedUpdate = sStateThatNeedsUpdateInTheLastFrame & (1 << mCurrentState);
	
	// return the flag
	return doesNeedUpdate;
}

/*
 * This function draw the specified frame, and may move the lem if we found some data pixel
 * inside the last row of the frame. The movement is based on the information stored in the last row of the animation frame.
 * Each pixel set have this meaning:
 * +-----+-----+-----+-----+
 * | x+1 | y-1 | y+1 | x+2 |   (y+1) = down   and   (y-1) = up
 * +-----+-----+-----+-----+
 * All the animation are stored with a movement to the right. If the Lem is walking to the left,
 * The move x should be reversed.
 */
bool Lem::UpdateOneAnimFrame(const unsigned char animFrame[], int16_t animFrameWidth)
{
	bool hasMoved = false;
	
	// first get the move from the animation frame
	int16_t xMove = 0;
	int16_t yMove = 0;
	if (pgm_read_byte_near(animFrame) & 0x80)
		xMove += 1;
	if (pgm_read_byte_near(animFrame + 1) & 0x80)
		yMove -= 1;
	if ((animFrameWidth > 2) && (pgm_read_byte_near(animFrame + 2) & 0x80))
		yMove += 1;
	if ((animFrameWidth > 3) && (pgm_read_byte_near(animFrame + 3) & 0x80))
		xMove += 2;

	// check if there's any movement
	hasMoved = (xMove != 0) || (yMove != 0);
	
	// reverse xMove if I'm mirrored
	if (mIsDirectionMirrored)
		xMove = -xMove;

	// cast the mPosX and PosY in int16_t before removing, because if the map is 256 pixel wide, mPosX will reboot, and same for y
	bool isInsideMap = MapManager::IsInMapBoundary((int)mPosX + xMove, (int)mPosY + yMove);

	// first move the x
	mPosX += xMove;

	// then move the lem Y position according to the move found in the animation
	mPosY += yMove;
	
	// if the position are outside the boundary of the Map, the lem is dead
	if (!isInsideMap)
	{
		SetCurrentState(StateId::DEAD, 0, 0);
		return false;
	}
	
	// return the flag telling if the lem has moved
	return hasMoved;
}

void Lem::Draw()
{
	// call the draw function to draw the anim frame if alive and not ouside the screen
	if ((GetCurrentState() > StateId::DEAD) && MapManager::IsOnScreen(mPosX))
	{
		// get the screen coordinate and the current anim frame
		unsigned char screenX = MapManager::ConvertToScreenCoord(mPosX);
		unsigned char screenY = mPosY;
		unsigned char currentFrame = mCurrentAnimFrame;
		bool isMirrored = mIsDirectionMirrored;
		
		// and draw the correct animation frame
		switch (mCurrentState)
		{
			case StateId::EXPLOSION_FX:
				DrawExplosiontFx();
				break;
			case StateId::CRASH:
				arduboy.drawBitmapExt(screenX, screenY, anim_LemCrash[currentFrame], sizeof(anim_LemCrash[0]), WHITE, isMirrored, 0x7F);
				break;
			case StateId::BYE_BYE_BOOM:
				arduboy.drawBitmapExt(screenX, screenY, anim_LemBomb[currentFrame], sizeof(anim_LemBomb[0]), WHITE, isMirrored, 0x7F);
				break;
			case StateId::WALK:
				arduboy.drawBitmapExt(screenX, screenY, anim_LemWalk[currentFrame], sizeof(anim_LemWalk[0]), WHITE, isMirrored, 0x7F);
				break;
			case StateId::BLOCKER:
				arduboy.drawBitmapExt(screenX, screenY, anim_LemBlocker[currentFrame], sizeof(anim_LemBlocker[0]), WHITE, isMirrored, 0x7F);
				break;
			case StateId::DIG_DIAG:
				arduboy.drawBitmapExt(screenX, screenY, anim_LemDigDiagonal[currentFrame], sizeof(anim_LemDigDiagonal[0]), WHITE, isMirrored, 0x7F);
				break;
			case StateId::DIG_HORIZ:
				arduboy.drawBitmapExt(screenX, screenY, anim_LemDigHorizontal[currentFrame], sizeof(anim_LemDigHorizontal[0]), WHITE, isMirrored, 0x7F);
				break;
			case StateId::DIG_VERT:
				arduboy.drawBitmapExt(screenX, screenY, anim_LemDigVertical[currentFrame], sizeof(anim_LemDigVertical[0]), WHITE, isMirrored, 0x7F);
				break;
			case StateId::STAIR:
				arduboy.drawBitmapExt(screenX, screenY, anim_LemStair[currentFrame], sizeof(anim_LemStair[0]), WHITE, isMirrored, 0x7F);
				break;
			case StateId::CLIMB:
				arduboy.drawBitmapExt(screenX, screenY, anim_LemClimb[currentFrame], sizeof(anim_LemClimb[0]), WHITE, isMirrored, 0x7F);
				break;
			case StateId::CLIMB_TOP:
				arduboy.drawBitmapExt(screenX, screenY, anim_LemClimbTop[currentFrame], sizeof(anim_LemClimbTop[0]), WHITE, isMirrored, 0x7F);
				break;
			case StateId::START_FALL:
				arduboy.drawBitmapExt(screenX, screenY, anim_LemStartFall[currentFrame], sizeof(anim_LemStartFall[0]), WHITE, isMirrored, 0x7F);
				break;
			case StateId::FALL:
				arduboy.drawBitmapExt(screenX, screenY, anim_LemFall[currentFrame % ANIM_LEM_FALL_FRAME_COUNT], sizeof(anim_LemFall[0]), WHITE, isMirrored, 0x7F);
				break;
			case StateId::FALL_TO_DEATH:
				arduboy.drawBitmapExt(screenX, screenY, anim_LemFallToDeath[currentFrame], sizeof(anim_LemFallToDeath[0]), WHITE, isMirrored, 0x7F);
				break;
			case StateId::PARACHUTE:
				arduboy.drawBitmapExt(screenX-2, screenY-4, anim_Parachute[currentFrame], sizeof(anim_Parachute[0]), WHITE, isMirrored, 0x7F);
				arduboy.drawBitmapExt(currentFrame==1 ? screenX-1: currentFrame==3 ? screenX+1 : screenX, screenY, anim_LemPara[currentFrame], sizeof(anim_LemPara[0]), WHITE, isMirrored, 0x7F);
				break;
		}
	}
}

void Lem::DrawTimerAboveHead(unsigned char timer)
{
	if (MapManager::IsOnScreen(mPosX) || MapManager::IsOnScreen(mPosX+5))
		arduboy.drawBitmapExt(MapManager::ConvertToScreenCoord(mCurrentState == StateId::WALK ? mPosX : mPosX+1), mPosY-5, sprite_TinyNumbers[timer], 3, WHITE);
}

void Lem::DrawExplosiontFx()
{
	const int16_t T_NORMALIZATION_SHIFT = 3;
	const int16_t EXPLOSION_PIXEL_COUNT = 20;
	const int16_t MIN_PIXEL_VELOCITY_X = -20;
	const int16_t MAX_PIXEL_VELOCITY_X = 20;
	const int16_t MIN_PIXEL_VELOCITY_Y = -30;
	const int16_t MAX_PIXEL_VELOCITY_Y = -15;
	
	// seed the random generato with the posx, so that I always get the same series of random numbers
	randomSeed(mPosX);
	int16_t t = (mIsDirectionMirrored ? mCurrentAnimFrame + 8 : mCurrentAnimFrame) + (mIsAParachuter ? 16 : 0) + (mIsAClimber ? 32 : 0);
	int16_t minusHalfGTSquaredPlusPosY = (int)mPosY + ((5*t*t) >> (T_NORMALIZATION_SHIFT*2));
	for (int16_t i = 0; i < EXPLOSION_PIXEL_COUNT; ++i)
	{
		int16_t x = MapManager::ConvertToScreenCoord(mPosX + ((random(MIN_PIXEL_VELOCITY_X, MAX_PIXEL_VELOCITY_X) * t)  >> T_NORMALIZATION_SHIFT));
		int16_t y = minusHalfGTSquaredPlusPosY + ((random(MIN_PIXEL_VELOCITY_Y, MAX_PIXEL_VELOCITY_Y) * t) >> T_NORMALIZATION_SHIFT);
		arduboy.drawPixel(x, y, INVERT);
	}
}

int16_t Lem::GetRootPosition(StateId stateId)
{
	int16_t normalRoot = (pgm_read_byte_near(LemAnimRootPosition + (stateId >> 2)) >> ((stateId % 4) << 1)) & 0x0003;
	if (mIsDirectionMirrored)
		return pgm_read_byte_near(LemAnimWidthPerLemState + stateId) - 1 - normalRoot;
	return normalRoot;
}

void Lem::SetCurrentState(StateId stateId, int16_t shiftX, int16_t shiftY)
{
	// if the shiftX == 255, that means we need to compute the shift X 
	// by placing the root of the new anim at the place of the root of the old anim
	if (shiftX == 255)
		shiftX = GetRootPosition((StateId)mCurrentState) - GetRootPosition(stateId);
	
	// set the state id and reset the current frame
	mCurrentState = stateId;
	mCurrentAnimFrame = 0;
	
	// add the shift in x and y when transitionning
	mPosX += shiftX;
	mPosY += shiftY;

	// for stair builder only, set 2 pixel under itself at start
	if (stateId == STAIR)
	{
		int16_t y = mPosY+6;
		MapManager::SetPixel(mPosX+2, y,  true);
		MapManager::SetPixel(GetPosXPlus(1), y,  true);
	}

	// call update state again because we may need to change again
	UpdateState();
}

bool Lem::IsUnderCursorPosition()
{
	// check the easiest test first
	int16_t curY = HUD::GetCursorY();
	int16_t y = mPosY;
	if ((curY > y) && (curY < y + 6))
	{
		int16_t curX = HUD::GetCursorX();
		int16_t x = MapManager::ConvertToScreenCoord(mPosX);
		return (curX >= x) && (curX < x + GetFrameWidthForCurrentAnim());
	}
	return false;
}
