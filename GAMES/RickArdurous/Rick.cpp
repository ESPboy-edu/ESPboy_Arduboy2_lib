/*
* Rick file handle the main character.
*/

#include "RickArdurous.h"
#include "Rick.h"
#include "SpriteData.h"
#include "Input.h"
#include "Dynamite.h"
#include "ArrowBullet.h"
#include "MapData.h"
#include "PickUpItem.h"
#include "MapManager.h"
#include "Physics.h"
#include "FXManager.h"

namespace Rick
{
	static constexpr int WALK_ANIM_SPEED = 3;
	static constexpr int NO_HORIZONTAL_MOVE_AIR_CONTROL_ANIM_SPEED = 1;
	static constexpr int MIN_AIR_CONTROL_ANIM_SPEED = 3;
	static constexpr int MAX_AIR_CONTROL_ANIM_SPEED = 6;
	static constexpr int FIRE_ANIM_SPEED = 3;
	static constexpr int CROUCH_STAND_ANIM_SPEED = 3;
	static constexpr int CRAWL_ANIM_SPEED = 3;
	static constexpr int CLIMB_LADDER_ANIM_SPEED = 3;
	static constexpr int DEATH_ANIM_SPEED = 4;
	static constexpr int WIDTH_DIFF_BETWEEN_CRAWL_AND_STAND = SpriteData::RICK_CRAWL_SPRITE_WIDTH - SpriteData::RICK_SPRITE_WIDTH;
	static constexpr int DEATH_VELOCITY_X = 9;
	// The standing sprite are stored on 2 bytes vertically, whereas the crawl sprite only use one.
	// When switching from stand to crawl state, we don't change the Y, but just draw the sprite lower
	// This constant value, store the difference between those two sprites visually speaking
	static constexpr int VISUAL_HEIGHT_DIFF_BETWEEN_STAND_AND_CRAWL = 5;
	// The distance from the left edge of the sprite, from which we check the collision under the feet
	static constexpr int LEFT_X_SHIFT_FOR_COLLISION_UNDER_FEET = 2;
	static constexpr int RIGHT_X_SHIFT_FOR_COLLISION_UNDER_FEET_STAND = 6;
	static constexpr int RIGHT_X_SHIFT_FOR_COLLISION_UNDER_FEET_CRAWL = 9;
	
	// state of Rick
	enum AnimState : unsigned char
	{
		IDLE = 0,
		WALK,
		JUMP,
		FALL,
		FIRE,
		CROUCH_DOWN,
		STAND_UP,
		CRAWL,
		CLIMB_LADDER,
		DEATH,
		WAIT_FOR_RESPAWN,
	};
	
	// the current state of Rick
	unsigned char State = AnimState::WAIT_FOR_RESPAWN; // keep the initialization here for when the Arduboy is switch on
	unsigned char CurrentAnimFrame;
	signed char CurrentAnimDirection;
	bool IsAlive() { return (State < AnimState::DEATH);}
	
	// position of Rick
	int X = 200; // do not init X and Y it will be by the respawn of the first level (except that we don't want Rick in the Main Menu)
	unsigned char Y; // do not init X and Y it will be by the respawn of the first level
	int GetX() { return X; }
	unsigned char GetY() { return Y; }
	int GetCenterX() { return X + 4; }
	unsigned char GetCenterY() { return Y + 10; }
	unsigned char GetWidth() { return (State == AnimState::CRAWL) ? SpriteData::RICK_CRAWL_SPRITE_WIDTH : SpriteData::RICK_SPRITE_WIDTH; }
	unsigned char GetHeight() { return (State == AnimState::CRAWL) ? 8 : 12; }
	
	// orientation of Rick
	bool IsLookingLeft = true;

	// variable for different states
	unsigned char StateFrameCounter = 0; // a generic frame counter that can be used by different states
	
	// variable for the jump state
	unsigned char JumpAndFallAnimSpeedIndex = 0;
	unsigned char AirControlAnimSpeed = NO_HORIZONTAL_MOVE_AIR_CONTROL_ANIM_SPEED;
	unsigned char AirControlFrameCount = 0;
	
	// variable for crawl state
	bool IsThereAnyCeilingAboveHead = false;
	bool ShouldWePlaceADynamite = false;
	
	// variable for the climb ladder state
	bool IsInFrontOfLadder = false;
	bool IsAboveLadder = false;
	
	// variable for death state
	unsigned char DeathParabolicId = 0;
	
	// Inventory (is initialized in the Reset function)
	signed char LifeCount;
	signed char StatuetteCount;
	signed char BulletCount;
	signed char DynamiteCount;
	
	// all the dynamite instances
	Dynamite AllDynamites[MAX_DYNAMITE_COUNT];
	// all the bullet instances
	ArrowBullet AllBullets[MAX_BULLET_COUNT];
	
	// variable for checkpoint
	unsigned char StatuetteCountAtLastCheckpoint = 0;
	
	void InitIdle();
	void InitFall();
	void InitCrouch();
	void InitCrawl();
	void InitStandUp();
	void InitClimbLadder();
	void InitDeath(unsigned int collision);
	bool IsDynamitePlacementRequested();
	void PlaceDynamite();
	void HandleInput();
	void SetNextAnimFrame(unsigned char startFrameId, unsigned char endFrameId, bool isLooping);
	void UpdateAirControl(bool towardLeftDirection);
	bool IsThereAnyGroundOrCeilingCollisionAt(unsigned char y);
	bool IsThereAnyCeilingAboveCrawl();
	unsigned int Draw(unsigned char color);
}

/**
 * This function should be called when user starts a new game
 */
void Rick::Reset()
{
	// reset variable not reset in the respawn function
	LifeCount = MAX_LIFE_COUNT;
	// reset the statuette count, because that will initialize the current statuette count in the respawn function
	StatuetteCountAtLastCheckpoint = 0;
	// start with Death State, so that the init is properly done for the first respawn of the level
	State = AnimState::DEATH;
}

/**
 * This function can be called to move Rick character outside of the main menu,
 * so that he's not drawn while the main menu is active.
 */
void Rick::MoveOutsideMainMenu()
{
	X = 200;
}

int Rick::GetLeftForScreenTransition()
{
	// remove 2, one for being the first pixel of the next screen,
	// and one because the last pixel of decor sprite is black,
	// therefore no collision would be detected during the first frame of the transition
	if (State == AnimState::CRAWL)
		return X + (LEFT_X_SHIFT_FOR_COLLISION_UNDER_FEET + 2);
	else
		return X + LEFT_X_SHIFT_FOR_COLLISION_UNDER_FEET;
}

int Rick::GetRightForScreenTransition()
{
	if (State == AnimState::CRAWL)
		return X + (RIGHT_X_SHIFT_FOR_COLLISION_UNDER_FEET_CRAWL - 1);
	else
		return X + RIGHT_X_SHIFT_FOR_COLLISION_UNDER_FEET_STAND;
}

int Rick::GetTopForScreenTransition()
{
	if (State == AnimState::CRAWL)
		return Y + (VISUAL_HEIGHT_DIFF_BETWEEN_STAND_AND_CRAWL + 2);
	else
		return Y + 2;
}

int Rick::GetBottomForScreenTransition()
{
	return Y + 10;
}

signed char Rick::GetFeetYOnScreen()
{
	signed char feetOnScreen = MapManager::GetYOnScreen(Y) + 12;
	if (State == AnimState::FALL)
		feetOnScreen -= Physics::JUMP_AND_FALL_VERTICAL_MOVE[JumpAndFallAnimSpeedIndex] + 1;
	if (feetOnScreen >= HEIGHT)
		feetOnScreen = HEIGHT-1;
	return feetOnScreen;
}

/**
 * This function can be called in two different case, either the first time when Rick reach a checkpoint,
 * or later on when Rick die and he should respawn at the specified location.
 * Therefore this function will do nothing if the player is not dead, but will actually teleport and revive
 * the player at the specified location if he's dead.
 * @param respawnWorldX the X coord where we should respawn in world coordinate
 * @param respawnWorldY the Y coord where we should respawn in world coordinate
 */
void Rick::CheckPointRespawn(int respawnWorldX, unsigned char respawnWorldY)
{
	// if we are not alive we should respawn, otherwise this is a checkpoint, we should memorize some stuffs
	if (!IsAlive())
	{
		// teleport to the specified position
		X = respawnWorldX;
		Y = respawnWorldY;
		// reinit the statuette count with the one memorized during the last checkpoint, and reset the bullet and dynamite count
		StatuetteCount = StatuetteCountAtLastCheckpoint;
		BulletCount = MAX_BULLET_COUNT;
		DynamiteCount = MAX_DYNAMITE_COUNT;
		// kill all the dynamites
		for (unsigned char i = 0; i < MAX_DYNAMITE_COUNT; ++i)
			AllDynamites[i].Kill();
		// kill all the bullets
		for (unsigned char i = 0; i < MAX_BULLET_COUNT; ++i)
			AllBullets[i].KillBulletWithoutSparks();
		// start in idle state
		InitIdle();
	}
	else
	{
		// memorize the number of statuette collected so far
		StatuetteCountAtLastCheckpoint = StatuetteCount;
	}
}

/**
 * Init function of the Idle State
 */
void Rick::InitIdle()
{
	State = AnimState::IDLE;
	CurrentAnimFrame = SpriteData::RickAnimFrameId::IDLE;
	CurrentAnimDirection = 1;
	AirControlAnimSpeed = NO_HORIZONTAL_MOVE_AIR_CONTROL_ANIM_SPEED;
}

/**
 * Init function of the Wait for Respawn (after death of after finishining the game)
 */
void Rick::InitWaitForRespawn()
{
	// change my state to wait for respawn
	State = WAIT_FOR_RESPAWN;
	// ask the MapManager to start the shutter animation, and that will respawn me to the last checkpoint after that
	MapManager::StartShutterAnimation();
}

/**
 * Init function of the Fall State
 */
void Rick::InitFall()
{
	// SPECIAL_ case for when I fall from the crawl state and there's ceiling above,
	// then we should teleport Rick below to avoid his head to collide in the ceiling
	if ((State == AnimState::CRAWL) && IsThereAnyCeilingAboveHead)
		Y += VISUAL_HEIGHT_DIFF_BETWEEN_STAND_AND_CRAWL;
	// then change the state and the other parameters
	State = AnimState::FALL;
	CurrentAnimFrame = SpriteData::RickAnimFrameId::JUMP;
	CurrentAnimDirection = 1;
	StateFrameCounter = 0;
	JumpAndFallAnimSpeedIndex = Physics::JUMP_AND_FALL_VERTICAL_ANIM_SPEED_COUNT - 1;
	AirControlFrameCount = 0;
}

/**
 * Init function of the Crouch State
 */
void Rick::InitCrouch()
{
	State = AnimState::CROUCH_DOWN;
	CurrentAnimFrame = SpriteData::RickAnimFrameId::CROUCH_START;
	CurrentAnimDirection = 1;
}

/**
 * Init function of the Crawl State
 */
void Rick::InitCrawl()
{
	// if the down button is still pressed, go to crawl state
	State = AnimState::CRAWL;
	CurrentAnimFrame = SpriteData::RickAnimFrameId::CRAWL_START;
	CurrentAnimDirection = 1;
	// if the rick is looking right, move x 3 pixel to the left for keeping the hat aligned with the idle animation
	if (!IsLookingLeft)
		X -= WIDTH_DIFF_BETWEEN_CRAWL_AND_STAND;
}

/**
 * Init function of the stand up State
 */
void Rick::InitStandUp()
{
	State = AnimState::STAND_UP;
	CurrentAnimFrame = SpriteData::RickAnimFrameId::STAND_UP_START;
	CurrentAnimDirection = -1;
	// if the rick is looking right, move x 3 pixel to the right for keeping the hat aligned with the idle animation
	if (!IsLookingLeft)
		X += WIDTH_DIFF_BETWEEN_CRAWL_AND_STAND;
}

/**
 * Init function of the Climb ladder State
 */
void Rick::InitClimbLadder()
{
	State = AnimState::CLIMB_LADDER;
	CurrentAnimFrame = SpriteData::RickAnimFrameId::CLIMB_START;
	CurrentAnimDirection = 1;
	AirControlAnimSpeed = NO_HORIZONTAL_MOVE_AIR_CONTROL_ANIM_SPEED;
}

/**
 * Init function of the Death State
 */
void Rick::InitDeath(unsigned int collision)
{
	State = AnimState::DEATH;
	CurrentAnimFrame = SpriteData::RickAnimFrameId::DEATH_START;
	CurrentAnimDirection = -1;

	// compute the horizontal velocity
	bool isCollisionOnLeftHalfOfSprite = collision < (1 << (GetWidth() >> 1));
	signed char velocityX = (IsLookingLeft != isCollisionOnLeftHalfOfSprite) ? DEATH_VELOCITY_X : -DEATH_VELOCITY_X;
	DeathParabolicId = Physics::StartParabolicTrajectory(X, Y, velocityX);

	// decrease the life counter
	if (LifeCount > 0)
		LifeCount--;
	
	// play the death sound
	FXManager::StartFXCommand(FXManager::BufferId::GAME, {10, 0, 0, 0, 4, 68, -4, 4});
}

/**
 * This function check if the user pressed the correct button to place a dynamite
 * and also check if the player has at least one dynamite available
 */
bool Rick::IsDynamitePlacementRequested()
{
	if ((Input::IsJustPressed(A_BUTTON)) && (DynamiteCount > 0))
		for (signed char i = DynamiteCount-1; i >= 0; --i)
			if (!AllDynamites[i].IsPropertySet(Item::PropertyFlags::ALIVE))
				return true;
	return false;
}

/**
 * This function search for an available dynamite and spawn it at the player's location
 */
void Rick::PlaceDynamite()
{
	// find an available dynamite and light it up
	for (signed char i = DynamiteCount-1; i >= 0; --i)
		if (!AllDynamites[i].IsPropertySet(Item::PropertyFlags::ALIVE))
		{
			// determines where to place the dynamite depending on our witdh and orientation
			int dynamiteX = IsLookingLeft ? X - 1: (X + GetWidth() - SpriteData::DYNAMITE_SPRITE_WIDTH - 1);
			AllDynamites[i].LightUp(dynamiteX, Y + 5);
			DynamiteCount--;
			// start FX
			FXManager::StartFXCommand(FXManager::BufferId::RICK, {4, 0, 0, 1, 26, 60, 0, 5});
			break;
		}
	// reset the dynamite placement flag 
	ShouldWePlaceADynamite = false;
}

void Rick::UpdateInput()
{
	// handle the input differently according to the current state
	if ((State == AnimState::JUMP) || (State == AnimState::FALL))
	{
		// increase the frame counter for the jump, and switch to the next frame when needed
		StateFrameCounter++;
		if (StateFrameCounter >= Physics::JUMP_AND_FALL_VERTICAL_ANIM_SPEED[JumpAndFallAnimSpeedIndex])
		{
			StateFrameCounter = 0;
			if (State == AnimState::JUMP)
			{
				// move up
				Y -= Physics::JUMP_AND_FALL_VERTICAL_MOVE[JumpAndFallAnimSpeedIndex];
				// increase the jump frame counter and check if we need to change state to fall
				JumpAndFallAnimSpeedIndex++;
				if (JumpAndFallAnimSpeedIndex >= Physics::JUMP_AND_FALL_VERTICAL_ANIM_SPEED_COUNT)
				{
					State = AnimState::FALL;
					JumpAndFallAnimSpeedIndex--;
				}
			}
			else
			{
				// in fall state, move down
				Y += Physics::JUMP_AND_FALL_VERTICAL_MOVE[JumpAndFallAnimSpeedIndex];
				// decrease the jump counter
				if (JumpAndFallAnimSpeedIndex > Physics::FALL_VERTICAL_MIN_INDEX)
					JumpAndFallAnimSpeedIndex--;
			}
		}
		
		// move left or right while jumping or falling
		AirControlFrameCount++;
		if (AirControlFrameCount >= AirControlAnimSpeed)
		{
			AirControlFrameCount = 0;
			// In jump or Fall state, we can do air control
			if (Input::IsDown(LEFT_BUTTON))
				UpdateAirControl(true);
			else if (Input::IsDown(RIGHT_BUTTON))
				UpdateAirControl(false);
			
			// move the X according to the orientation Rick, and if he didn't jump vertically
			if (AirControlAnimSpeed != NO_HORIZONTAL_MOVE_AIR_CONTROL_ANIM_SPEED)
			{
				if (IsLookingLeft)
					X--;
				else
					X++;
			}
		}
		
		// grab the ladder if we are in front of it
		if (IsInFrontOfLadder)
			InitClimbLadder();
	}
	else if (State == AnimState::FIRE)
	{
		// change the anim frame id if it is the time to
		if (arduboy.everyXFrames(FIRE_ANIM_SPEED))
			CurrentAnimFrame += CurrentAnimDirection;
		
		// check if we reach the end of the animation
		if (CurrentAnimFrame > SpriteData::RickAnimFrameId::FIRE_END)
		{
			// reset the anim frame to a correct value and reverse the anim direction
			CurrentAnimFrame = SpriteData::RickAnimFrameId::FIRE_END;
			CurrentAnimDirection = -1;
			
			// fire the bullet
			for (signed char i = BulletCount-1; i >= 0; --i)
				if (!AllBullets[i].IsPropertySet(Item::PropertyFlags::ALIVE))
				{
					AllBullets[i].Fire(IsLookingLeft ? X : X + SpriteData::RICK_SPRITE_WIDTH, Y + 8, IsLookingLeft);
					BulletCount--;
					FXManager::StartFXCommand(FXManager::BufferId::RICK, {1, 0, 0, 0, 0, 64, -1, 12});
					break;
				}
		}
		// check if we finished the ping pong loop
		else if (CurrentAnimFrame < SpriteData::RickAnimFrameId::FIRE_START)
		{
			// go back to idle
			InitIdle();
		}
	}
	else if (State == AnimState::CROUCH_DOWN)
	{
		// check if the player wants to place a dynamite
		if (!ShouldWePlaceADynamite)
			ShouldWePlaceADynamite = IsDynamitePlacementRequested();
		
		// change the anim frame id if it is the time to
		if (arduboy.everyXFrames(CROUCH_STAND_ANIM_SPEED))
			CurrentAnimFrame += CurrentAnimDirection;
		
		// check if we reach the end of the animation
		if (CurrentAnimFrame > SpriteData::RickAnimFrameId::CROUCH_END)
		{
			// now we can place the dynamite, because we reach the end of the animation
			if (ShouldWePlaceADynamite)
				PlaceDynamite();

			// check if the down button is still pressed or not
			if (Input::IsDown(DOWN_BUTTON))
			{
				InitCrawl();
			}
			else
			{
				// the player release the down button, so we need to stand up
				// reset the anim frame to a correct value and reverse the anim direction
				InitStandUp();
			}
		}
	}
	else if (State == AnimState::STAND_UP)
	{
		// change the anim frame id if it is the time to
		if (arduboy.everyXFrames(CROUCH_STAND_ANIM_SPEED))
			CurrentAnimFrame += CurrentAnimDirection;

		// check if we finished the ping pon+g loop
		if (CurrentAnimFrame < SpriteData::RickAnimFrameId::STAND_UP_END)
		{
			// go back to idle
			InitIdle();
		}
	}
	else if (State == AnimState::CRAWL)
	{
		// light up the dynamite if the playe press A
		if (IsDynamitePlacementRequested())
			PlaceDynamite();
		
		// if player release the down button and there's no ceiling above, then the main character goes up
		if (!Input::IsDown(DOWN_BUTTON) && !IsThereAnyCeilingAboveHead)
		{
			InitStandUp();
			return;
		}
		
		// handle the crawling on left and right
		if (Input::IsDown(LEFT_BUTTON))
		{
			IsLookingLeft = true;
			// check if we should move to the next frame
			if (arduboy.everyXFrames(CRAWL_ANIM_SPEED))
			{
				X--;
				SetNextAnimFrame(SpriteData::RickAnimFrameId::CRAWL_START, SpriteData::RickAnimFrameId::CRAWL_END, true);
			}
		}
		else if (Input::IsDown(RIGHT_BUTTON))
		{
			IsLookingLeft = false;
			// check if we should move to the next frame
			if (arduboy.everyXFrames(CRAWL_ANIM_SPEED))
			{
				X++;
				SetNextAnimFrame(SpriteData::RickAnimFrameId::CRAWL_START, SpriteData::RickAnimFrameId::CRAWL_END, true);
			}
		}
	}
	else if (State == AnimState::CLIMB_LADDER)
	{
		if (arduboy.everyXFrames(CLIMB_LADDER_ANIM_SPEED))
		{
			bool isMoving = false;
			if (Input::IsDown(UP_BUTTON))
			{
				isMoving = true;
				Y--;
			}
			else if (Input::IsDown(DOWN_BUTTON))
			{
				isMoving = true;
				Y++;
			}
			else if (Input::IsDown(LEFT_BUTTON))
			{
				isMoving = true;
				X--;
			}
			else if (Input::IsDown(RIGHT_BUTTON))
			{
				isMoving = true;
				X++;
			}
		
			// update the animation if moving
			if (isMoving)
				SetNextAnimFrame(SpriteData::RickAnimFrameId::CLIMB_START, SpriteData::RickAnimFrameId::CLIMB_END, false);
		}
	}
	else if (State == AnimState::DEATH)
	{
		if (arduboy.everyXFrames(DEATH_ANIM_SPEED))
		{
			// reverse direction if we reach the end of the loop
			if ((CurrentAnimFrame == SpriteData::RickAnimFrameId::DEATH_START) ||
				(CurrentAnimFrame == SpriteData::RickAnimFrameId::DEATH_END))
				CurrentAnimDirection = -CurrentAnimDirection;
			// increase the anim frame
			CurrentAnimFrame += CurrentAnimDirection;
		}
		
		// update the trajectory
		Physics::UpdateParabolicTrajectory(DeathParabolicId, X, Y);
		
		// check if the X and Y are outside of the screen
		if (!MapManager::IsOnScreen(X, Y, GetWidth(), GetHeight()))
		{
			// stop the parabolic trajectory
			Physics::StopParabolicTrajectory(DeathParabolicId);
			// init Rick in wait for respawn state
			InitWaitForRespawn();
		}
	}
	else if (IsAlive())
	{
		// check if we start a the jump
		if (Input::IsJustPressed(B_BUTTON))
		{
			State = AnimState::JUMP;
			CurrentAnimFrame = SpriteData::RickAnimFrameId::JUMP;
			StateFrameCounter = 0;
			JumpAndFallAnimSpeedIndex = 0;
			AirControlFrameCount = 0;
			FXManager::StartFXCommand(FXManager::BufferId::RICK, {1, 0, 0, 0, 0, 52, 1, 6});
		}
		else if (Input::IsDown(DOWN_BUTTON))
		{
			if ((IsInFrontOfLadder || IsAboveLadder) && Input::IsJustPressed(DOWN_BUTTON))
				InitClimbLadder();
			else
				InitCrouch();
		}
		else if (Input::IsDown(UP_BUTTON) && IsInFrontOfLadder)
		{
			InitClimbLadder();
		}
		else if (Input::IsDown(LEFT_BUTTON))
		{
			IsLookingLeft = true;
			AirControlAnimSpeed = MIN_AIR_CONTROL_ANIM_SPEED;

			// reset anim frame to the first frame of the walk, and set the state
			if (Input::IsJustPressed(LEFT_BUTTON))
			{
				CurrentAnimFrame = SpriteData::RickAnimFrameId::WALK_START;
				State = AnimState::WALK;
			}
			
			if (arduboy.everyXFrames(WALK_ANIM_SPEED))
			{
				X--;
				SetNextAnimFrame(SpriteData::RickAnimFrameId::WALK_START, SpriteData::RickAnimFrameId::WALK_END, true);
			}
		}
		else if (Input::IsDown(RIGHT_BUTTON))
		{
			IsLookingLeft = false;
			AirControlAnimSpeed = MIN_AIR_CONTROL_ANIM_SPEED;
			
			// reset anim frame to the first frame of the walk, and set the state
			if (Input::IsJustPressed(RIGHT_BUTTON))
			{
				CurrentAnimFrame = SpriteData::RickAnimFrameId::WALK_START;
				State = AnimState::WALK;
			}
			if (arduboy.everyXFrames(WALK_ANIM_SPEED))
			{
				X++;
				SetNextAnimFrame(SpriteData::RickAnimFrameId::WALK_START, SpriteData::RickAnimFrameId::WALK_END, true);
			}
		}
		else
		{
			// reset the state to idle by default
			InitIdle();
		}
		
		// fire when pressing the correct button
		if ((Input::IsJustPressed(A_BUTTON)) && (BulletCount > 0))
		{
			for (signed char i = BulletCount-1; i >= 0; --i)
				if (!AllBullets[i].IsPropertySet(Item::PropertyFlags::ALIVE))
				{
					CurrentAnimFrame = SpriteData::RickAnimFrameId::FIRE_START;
					CurrentAnimDirection = 1;
					State = AnimState::FIRE;
					break;
				}
		}
	}
}

void Rick::SetNextAnimFrame(unsigned char startFrameId, unsigned char endFrameId, bool isLooping)
{
	// increase the current frame to the next one
	CurrentAnimFrame += CurrentAnimDirection;
	// check if we need to loop the animation
	if (CurrentAnimFrame > endFrameId)
	{
		if (isLooping)
			CurrentAnimFrame = startFrameId;
		else
		{
			CurrentAnimFrame = endFrameId -1;
			CurrentAnimDirection = -1;
		}
	}
	else if (CurrentAnimFrame < startFrameId)
	{
		CurrentAnimFrame = startFrameId + 1;
		CurrentAnimDirection = 1;
	}
}

void Rick::UpdateAirControl(bool towardLeftDirection)
{
	// first check if we jump without any horizontal movement (just vertically)
	if (AirControlAnimSpeed == NO_HORIZONTAL_MOVE_AIR_CONTROL_ANIM_SPEED)
	{
		// the horizontal speed was null, so set the looking orientation and set the anim speed to max
		IsLookingLeft = towardLeftDirection;
		AirControlAnimSpeed = MAX_AIR_CONTROL_ANIM_SPEED;
	}
	else if (IsLookingLeft == towardLeftDirection)
	{
		if (AirControlAnimSpeed > MIN_AIR_CONTROL_ANIM_SPEED)
			AirControlAnimSpeed--;
	}
	else
	{
		if (AirControlAnimSpeed < MAX_AIR_CONTROL_ANIM_SPEED)
			AirControlAnimSpeed++;
		if (AirControlAnimSpeed == MAX_AIR_CONTROL_ANIM_SPEED)
			IsLookingLeft = towardLeftDirection;
	}
}

/**
 * Tell if there is any horizontal collision (i.e. ground or ceiling) at the specified world vertical coordinate.
 * This function will test two pixels on the left and the right of the current sprite position, a little inside the sprite.
 * If the specified y coordinate is outside the screen, then this function will ask the MapManager to check if there is
 * a sprite at that place in the level, to still have accurate collision event if the ground or ceiling is outside the
 * the screen.
 */
bool Rick::IsThereAnyGroundOrCeilingCollisionAt(unsigned char yWorld)
{
	// compute the width that should be used for checking the collisions
	unsigned char width;
	if (State == AnimState::CRAWL)
		width = RIGHT_X_SHIFT_FOR_COLLISION_UNDER_FEET_CRAWL - LEFT_X_SHIFT_FOR_COLLISION_UNDER_FEET;
	else
		width = RIGHT_X_SHIFT_FOR_COLLISION_UNDER_FEET_STAND - LEFT_X_SHIFT_FOR_COLLISION_UNDER_FEET;
	// ask the MapManager to check for the collisions
	return MapManager::IsThereAnyHorizontalCollisionAt(X + LEFT_X_SHIFT_FOR_COLLISION_UNDER_FEET, yWorld, width);
}

bool Rick::IsThereAnyCeilingAboveCrawl()
{
	int xWorld = X;
	if (!IsLookingLeft)
		xWorld += WIDTH_DIFF_BETWEEN_CRAWL_AND_STAND;
	// do not add VISUAL_HEIGHT_DIFF_BETWEEN_STAND_AND_CRAWL to Y, it is too low
	// add +2 or +3 otherwise the check is done on the separated line of the separated stone
	return MapManager::IsThereAnyHorizontalCollisionAt(xWorld, Y+2, SpriteData::RICK_SPRITE_WIDTH);
}

/**
 * Check if Rick is colliding with a static wall, floor, and ceiling and prevent him to move.
 */
void Rick::CheckStaticCollision()
{
	// early exit when the main character is dead, we don't need to check the static collision
	if (!IsAlive())
		return;
	
	// if we jump, check the ceiling, otherwise, check the floor
	if (State == AnimState::JUMP)
	{
		int yHead = Y;
		if (IsThereAnyGroundOrCeilingCollisionAt(yHead))
		{
			// move down Rick, until it is outside the collision, this can happen if Rick entered deeply in the collision
			Y++;
			while (IsThereAnyGroundOrCeilingCollisionAt(++yHead))
				Y++;
			// cancel the jump state and enter directly into the fall state, because Rick as collided with the ceilling
			InitFall();
		}
	}
	else if (State == AnimState::FALL)
	{
		// check the floor collisions but inside the feet to avoid walking on arrow or Enemies
		int yFeet = Y + 12;
		if (IsThereAnyGroundOrCeilingCollisionAt(yFeet))
		{
			// move up if Rick entered deeply in the ground (at least one pixel because we check collision on the feet)
			// Also this can happen if Rick moves more than 1 pixel per frame
			Y--;
			while (IsThereAnyGroundOrCeilingCollisionAt(--yFeet))
				Y--;
			// We found a collision under the feet, so if we are falling, stop falling
			InitIdle();
			// play a landing sound effect
			FXManager::StartFXCommand(FXManager::BufferId::RICK, {1, 0, 0, 0, 0, 56, -1, 4});
		}
	}
	else
	{
		// here we are not jumping or falling, so check the collision under the feet
		// There's no collision under the feet, and not jumping, then we fall
		int yUnderFeet = Y + 13;
		if (!IsThereAnyGroundOrCeilingCollisionAt(yUnderFeet))
		{
			if (!IsInFrontOfLadder)
				InitFall();
		}
		else if (((State == AnimState::CLIMB_LADDER) &&
				((!Input::IsDown(DOWN_BUTTON) && !IsInFrontOfLadder) ||
				 (Input::IsDown(DOWN_BUTTON) && !IsAboveLadder))))
				InitIdle();
	}
	
	// draw rick sprite to check the wall collisions
	unsigned int wallCollision = Draw(TRANSPARENT);
	if (wallCollision)
	{
		// cancel any speed in the air if colliding with a wall
		AirControlAnimSpeed = NO_HORIZONTAL_MOVE_AIR_CONTROL_ANIM_SPEED;
		// declare a variable to know in which direction we should push the main character to exit the collision
		bool pushToRight = IsLookingLeft;
		// if this is the left part of the sprite that has collision then we should push in the same direction as his look
		if ((wallCollision & 0x07) != 0)
			pushToRight = !IsLookingLeft;
		// push rick in the right direction
		if (pushToRight)
			X++;
		else
			X--;
	}
	
	// check above the head in craw state
	if (State == AnimState::CRAWL)
		IsThereAnyCeilingAboveHead = IsThereAnyCeilingAboveCrawl();
	
	// Now check the collision with the ladder (only use the map level for that, because we don't need a pixel precision)
	// get the x coord on scren of the middle of the character
	int middleWorldX = X + 4;
	// check if we are in front of a ladder
	IsInFrontOfLadder = MapManager::IsThereLadderAt(middleWorldX, Y + 5) ||
						MapManager::IsThereLadderAt(middleWorldX, Y + 12);
	// check if there's a ladder under the platform
	IsAboveLadder = MapManager::IsThereLadderAt(middleWorldX, Y + 13);
}

/**
 * Chekc if Rick collides with any pixel that will kill him.
 */
void Rick::CheckLethalCollision()
{
	if (IsAlive())
	{
		unsigned int collision = Draw(BLACK);
		if (collision != 0)
		{
			InitDeath(collision);
		}
		else if (State == AnimState::FALL)
		{
			// SPECIAL_ case when falling, because Rick falling anim has split legs which may not
			// touch the lethal pixel in the Draw function, but then later the ground collision check
			// may detect the lethal pixels and consider them as ground (then Rick can walk on enemies)
			int yFeet = Y + 12;
			if (arduboy.CheckWhitePixelsInRow(MapManager::GetXOnScreen(X) + LEFT_X_SHIFT_FOR_COLLISION_UNDER_FEET, MapManager::GetYOnScreen(yFeet), RIGHT_X_SHIFT_FOR_COLLISION_UNDER_FEET_STAND - LEFT_X_SHIFT_FOR_COLLISION_UNDER_FEET))
				InitDeath(32); // any number to indicate a collision pixel
		}
	}
}

/**
 * Default draw function for the main character that choose the right color in which to draw
 * depending on the state of the character.
 */
void Rick::Draw()
{
	// for the death animation, draw the main character in invert color
	Draw((State == AnimState::DEATH) ? INVERT : WHITE);
}

/**
 * Draw the main character with the specified color
 */
unsigned int Rick::Draw(unsigned char color)
{
	signed char xOnScreen = MapManager::GetXOnScreen(X);
	signed char yOnScreen = MapManager::GetYOnScreen(Y);
	if (State == AnimState::CRAWL)
		return arduboy.drawBitmapExtended(xOnScreen, yOnScreen + VISUAL_HEIGHT_DIFF_BETWEEN_STAND_AND_CRAWL, SpriteData::RickCrawl[CurrentAnimFrame], SpriteData::RICK_CRAWL_SPRITE_WIDTH, SpriteData::RICK_CRAWL_SPRITE_HEIGHT, color, IsLookingLeft);
	else
		return arduboy.drawBitmapExtended(xOnScreen, yOnScreen, SpriteData::Rick[CurrentAnimFrame], SpriteData::RICK_SPRITE_WIDTH, SpriteData::RICK_SPRITE_HEIGHT, color, IsLookingLeft);
}
