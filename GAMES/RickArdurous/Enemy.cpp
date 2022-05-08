/*
 * Implement the logic and drawing of the Enemies
*/

#include "RickArdurous.h"
#include "Enemy.h"
#include "SpriteData.h"
#include "Rick.h"
#include "MapManager.h"
#include "Physics.h"

const signed char WALK_AND_WAIT_ANIM_SPEED[] = { 8, 13, 6, 8 };

Enemy::Enemy() :
PhysicsId(Physics::INVALID_FALL_ID)
{
}

bool Enemy::Update(UpdateStep step)
{
	switch (step)
	{
		case UpdateStep::CHECK_LETHAL:
		{
			if (IsPropertySet(Item::PropertyFlags::ALIVE))
			{
				if (MapManager::IsOnScreen(X, Y, MyWidth, MyHeight))
				{
					// set the visible flag if we are on screen and update the logic normally
					SetProperty(Item::PropertyFlags::IS_VISIBLE);
					
					// first update the logic of the AI to determine its state and animation,
					// so that we draw the same frame in black and white
					// update the frame counter (need to be done in any state)
					AnimFrameCount++;
					// then call the corect update according to the current state
					switch (AnimState)
					{
						case State::WALK:
							UpdateWalk();
							break;
						case State::HALF_TURN:
							UpdateHalfTurn();
							break;
						case State::WAIT:
						case State::WAIT_AGAIN:
							UpdateWait();
							break;
						case State::FALL:
							UpdateFall();
							break;
					}
					
					// if we have a collision, that means we hit a lethal pixel
					// Draw in black to delete the bullet in case we are hit by a bullet
					int collision = Draw(BLACK);
					if (collision != 0)
					{
						// compute the horizontal velocity for the death trajectory
						bool isCollisionOnLeftHalfOfSprite = collision < (1 << (MyWidth >> 1));
						signed char velocityX = (IsPropertySet(PropertyFlags::MIRROR_X) != isCollisionOnLeftHalfOfSprite) ? DEATH_VELOCITY_X : -DEATH_VELOCITY_X;
						// if we are falling stop the fall before reusing the physics id
						if (AnimState == State::FALL)
							Physics::StopFall(PhysicsId);
						PhysicsId = Physics::StartParabolicTrajectory(X, Y, velocityX);
						AnimState = State::DEATH;
						ClearProperty(Item::PropertyFlags::ALIVE);
					}
				}
				else
				{
					ClearProperty(Item::PropertyFlags::IS_VISIBLE);
				}
			}
			break;
		}
		
		case UpdateStep::DRAW_ENEMIES:
		{
			// draw the enemy in white if he's alive
			if (IsPropertySet(Item::PropertyFlags::ALIVE | Item::PropertyFlags::IS_VISIBLE))
				Draw(WHITE);
			break;
		}
		
		case UpdateStep::DRAW_IGNORED_BY_ENEMIES:
		{
			// SPECIAL_ case, the Enemy is in that update step only when he is dead
			if (!IsPropertySet(Item::PropertyFlags::ALIVE))
			{
				Draw(INVERT);
				return UpdateDeath();
			}
			break;
		}
		
		case UpdateStep::CHECK_STATIC_COLLISION:
		{
			if (IsPropertySet(Item::PropertyFlags::ALIVE | Item::PropertyFlags::IS_VISIBLE))
			{
				// check the ground collision
				int yUnderFeet = GetYUnderFeet();
				if (!IsThereAnyGroundCollisionAt(yUnderFeet))
				{
					// check if we are already falling or not
					if (AnimState == State::FALL)
					{
						// If we are falling, check the ground several pixel below, depending on my current
						// fall speed, to see if I will touch the ground during next frame.
						// If yes, reduce my falling speed to avoid penetrating in the ground during next frame
						unsigned char fallSpeed = Physics::GetCurrentFallSpeed(PhysicsId);
						for (unsigned char i = fallSpeed; i > 1; --i)
							if (IsThereAnyGroundCollisionAt(yUnderFeet + i))
								fallSpeed--;
						Physics::LimitFallSpeed(PhysicsId, fallSpeed);
					}
					else
					{
						// if we are not falling init the fall
						InitFall();
					}
				}
				else
				{
					// there is ground, check if I was falling
					if (AnimState == State::FALL)
					{
						InitWalk();
						Physics::StopFall(PhysicsId);
					}
				}
			}
			break;
		}
		case UpdateStep::RESPAWN:
		{
			// memorize my size based on the type of enemy
			MyWidth = IsSkeleton() ? SpriteData::SKELETON_SPRITE_WIDTH : 
						(IsScorpion() ? SpriteData::SCORPION_SPRITE_WIDTH : SpriteData::MUMMY_SPRITE_WIDTH);
			MyHeight = IsScorpion() ? 4 : 12;
			// and start to walk (if we are not alive, this update is not even called)
			InitWalk();
			break;
		}
	}
	return false;
}

int Enemy::GetYUnderFeet()
{
	return IsScorpion() ? Y + 4 : Y + 14;
}

void Enemy::MoveAccordingToOrientation()
{
	// move the X depending on the direction
	if (IsPropertySet(PropertyFlags::MIRROR_X))
		X--;
	else
		X++;
}

bool Enemy::IsThereWallCollisionOrGap(bool shouldCheckGap)
{
	int wallX = IsPropertySet(PropertyFlags::MIRROR_X) ? X - WALL_COLLISION_DETECTION_DISTANCE : X + MyWidth + WALL_COLLISION_DETECTION_DISTANCE;
	unsigned char gapHeightMultiplier = IsScorpion() ? 1 : 2;
	return (MapManager::IsThereStaticCollisionAt(wallX, Y, true) || 
		(!IsScorpion() && MapManager::IsThereStaticCollisionAt(wallX, Y + SpriteData::LEVEL_SPRITE_HEIGHT)) ||
		(shouldCheckGap && !MapManager::IsThereStaticCollisionAt(wallX, Y + (SpriteData::LEVEL_SPRITE_HEIGHT * gapHeightMultiplier))));
}

bool Enemy::IsThereAnyGroundCollisionAt(unsigned char yWorld)
{
	// ask the MapManager to check for the collisions (using one pixel less on the left and on the right)
	return MapManager::IsThereAnyHorizontalCollisionAt(X + 1, yWorld, MyWidth - 2);
}

void Enemy::UpdateSkeletonBehavior()
{
	int rickX = Rick::GetCenterX();
	int myCenterX = X + (SpriteData::SKELETON_SPRITE_WIDTH / 2);
	bool isRickOnMyLeft = rickX < (myCenterX - SKELETON_SENSOR);
	bool isRickOnMyRight = rickX > (myCenterX + SKELETON_SENSOR);
	bool amILookingLeft = IsPropertySet(MIRROR_X);
	if ((isRickOnMyRight && amILookingLeft) || (isRickOnMyLeft && !amILookingLeft))
	{
		InitHalfTurn();
	}
	else 
	{
		// check if rick is aligned with me, or if I'm blocked by collision
		if ((!isRickOnMyLeft && !isRickOnMyRight) || IsThereWallCollisionOrGap(false))
		{
			if (AnimState != State::WAIT)
				InitWait();
		}
		else
		{
			if (AnimState != State::WALK)
				InitWalk();
		}
	}
}

void Enemy::InitFall()
{
	AnimState = State::FALL;
	PhysicsId = Physics::StartFall();
	if (IsSkeleton())
		AnimFrameId = SpriteData::EnemyAnimFrameId::ENEMY_FALL;
}

void Enemy::InitWait()
{
	AnimState = WAIT;
	AnimFrameId = SpriteData::EnemyAnimFrameId::ENEMY_WAIT_START;
	AnimFrameCount = 0;
}

void Enemy::InitWalk()
{
	AnimState = State::WALK;
	AnimFrameId = SpriteData::EnemyAnimFrameId::ENEMY_WALK_START;
	AnimFrameCount = 0;
}

void Enemy::InitHalfTurn()
{
	// reverse the walking direction imediately for the next frame to test the collision at the right place
	InverseProperty(PropertyFlags::MIRROR_X);
	// init the half turn state
	AnimState = State::HALF_TURN;
	AnimFrameId = SpriteData::EnemyAnimFrameId::ENEMY_HALF_TURN;
	AnimFrameCount = 0;
}

void Enemy::UpdateWalk()
{
	// get the anim speed
	unsigned char walkAnimSpeed = IsSkeleton() ? SKELETON_WALK_ANIM_SPEED : 
				(IsScorpion() ? SCORPION_WALK_ANIM_SPEED : WALK_AND_WAIT_ANIM_SPEED[AnimFrameId]);
	// check if it is the time to change the anim frame
	if (AnimFrameCount == walkAnimSpeed)
	{
		// reset the frame counter
		AnimFrameCount = 0;
		// go to the next frame id
		if (AnimFrameId == SpriteData::EnemyAnimFrameId::ENEMY_WALK_END)
			AnimFrameId = SpriteData::EnemyAnimFrameId::ENEMY_WALK_START;
		else
			AnimFrameId++;

		// move the X according to the orientation for a ping pong trajectory
		MoveAccordingToOrientation();
	
		// Update the SPECIAL_ behavior of the skeleton or by default make half turn
		if (IsSkeleton())
		{
			UpdateSkeletonBehavior();
		}
		else if (IsThereWallCollisionOrGap(true))
		{
			if (IsScorpion())
				InitWait();
			else
				InitHalfTurn();
		}
	}
}

void Enemy::UpdateHalfTurn()
{
	// get the anim speed
	unsigned char halfTurnAnimSpeed = IsSkeleton() ? SKELETON_HALF_TURN_ANIM_SPEED : MUMMY_HALF_TURN_ANIM_SPEED;
	if (AnimFrameCount == halfTurnAnimSpeed)
		InitWalk();
}

void Enemy::UpdateWait()
{
	// get the wait anim speed
	unsigned char waitAnimSpeed = WALK_AND_WAIT_ANIM_SPEED[AnimFrameId - SpriteData::EnemyAnimFrameId::ENEMY_WAIT_START];
	// check if we need to change the frame id
	if (AnimFrameCount == waitAnimSpeed)
	{
		// reset the frame counter
		AnimFrameCount = 0;
		// change the anim id (only two ids in wait, but will make 3 frame with the mirror)
		if (AnimFrameId == SpriteData::EnemyAnimFrameId::ENEMY_WAIT_END)
		{
			AnimFrameId = SpriteData::EnemyAnimFrameId::ENEMY_WAIT_START;
			if (IsScorpion())
			{
				if (AnimState == State::WAIT)
					AnimState = State::WAIT_AGAIN;
				else
					InitHalfTurn();
			}
		}
		else
			AnimFrameId++;
		
		// check if we need to stop waiting and go back to walk
		if (IsSkeleton())
			UpdateSkeletonBehavior();
	}
}

void Enemy::UpdateFall()
{
	// update the fall
	unsigned char fallMoveCount = Physics::UpdateFall(PhysicsId, Y);
	// move a little bit on X during the first frame of the animation
	if ((fallMoveCount > 0) && (fallMoveCount < 3))
		MoveAccordingToOrientation();
}

bool Enemy::UpdateDeath()
{
	// update the trajectory
	Physics::UpdateParabolicTrajectory(PhysicsId, X, Y);
	
	// check if I'm still on screen
	if (!MapManager::IsOnScreen(X, Y, MyWidth, MyHeight))
	{
		// stop the parabolic trajectory
		Physics::StopParabolicTrajectory(PhysicsId);
		// clear alive property
		ClearProperty(Item::PropertyFlags::ALIVE);
		// we are now out of the screen we can be removed from the MapManager
		return true;
	}
	// not exited from the screen yet, so we should stayed updated
	return false;
}

int Enemy::Draw(unsigned char color)
{
	signed char xOnScreen = MapManager::GetXOnScreen(X);
	signed char yOnScreen = MapManager::GetYOnScreen(Y);
	bool isMirror = IsPropertySet(PropertyFlags::MIRROR_X);
	if (IsSkeleton())
		return arduboy.drawBitmapExtended(xOnScreen, yOnScreen,
									SpriteData::Skeleton[AnimFrameId],
									SpriteData::SKELETON_SPRITE_WIDTH, SpriteData::SKELETON_SPRITE_HEIGHT,
									color, isMirror);
	else if (IsScorpion())
		return arduboy.drawBitmapExtended(xOnScreen, yOnScreen,
									SpriteData::Scorpion[AnimFrameId],
									SpriteData::SCORPION_SPRITE_WIDTH, SpriteData::SCORPION_SPRITE_HEIGHT,
									color, isMirror);
	else
		return arduboy.drawBitmapExtended(xOnScreen, yOnScreen,
									SpriteData::Mummy[AnimFrameId],
									SpriteData::MUMMY_SPRITE_WIDTH, SpriteData::MUMMY_SPRITE_HEIGHT,
									color, isMirror);
}
