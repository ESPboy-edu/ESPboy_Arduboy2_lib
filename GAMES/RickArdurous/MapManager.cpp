/**
* The Map Manager handle the drawing of the map
*/
#include "RickArdurous.h"
#include "MapManager.h"
#include "MapData.h"
#include "SpriteData.h"
#include "Rick.h"
#include "PickUpItem.h"
#include "DestroyableBlock.h"
#include <avr/pgmspace.h>
#include "Input.h"
#include "Physics.h"
#include "GameManager.h"
#include "Graal.h"
#include "MapDataConstVariables.h"

namespace MapManager
{
	// the vertical shift value in pixel that should be aplied to the camera when drawing the level
	static constexpr int CAMERA_VERTICAL_SHIFT = 6;
	static constexpr int NB_HORIZONTAL_SPRITE_PER_SCREEN = 16;
	static constexpr int NB_VERTICAL_SPRITE_PER_SCREEN = 8;
	static constexpr int SHUTTER_SPEED = 5;

	class CameraCoord
	{
	public:
		unsigned char Current = 0;
		unsigned char Target = 0;
		signed char Transition = 0;
		signed char StartDrawSprite = 0;
		signed char EndDrawSprite = 0;
		
		void Animate(signed char spriteSize);
		
	private:
		signed char GetCameraSpeed(signed char step, signed char subStep);
	};

	// The current camera coordinate reference the top left corner of the screen portion of the level, in the big level array.
	CameraCoord CameraX;
	CameraCoord CameraY;
	
	// the items to currently update
	Item * ItemsToUpdate[MAX_UPDATABLE_ITEM_COUNT];
	unsigned char ItemsToUpdateCount;
	
	// variable for managing puzzle screen
	// the id of the puzzle screen where the last checkpoint is
	unsigned char LastCheckPointPuzzleScreenId;
	// the id of the puzzle screen currently player by the player
	unsigned char CurrentPuzzleScreenId;
	// the id of the farest puzzle screen reached by the player
	unsigned char FarestPuzzleScreenIdReached;
	// a variable to indicate in which direction the player goes when traversing the puzzle screens
	signed char PuzzleScreenMoveDirection;
	// coordinates of the edge between the current puzzle screen and the previous screen
	unsigned char PreviousPuzzleScreenCoordX;
	unsigned char PreviousPuzzleScreenCoordY;
	unsigned char LastCheckPointPreviousPuzzleScreenCoordX;
	unsigned char LastCheckPointPreviousPuzzleScreenCoordY;
	unsigned char LastCheckPointPuzzleScreenCoordX;
	unsigned char LastCheckPointPuzzleScreenCoordY;
	
	// variable use the draw the Shutter animation after a Respawn
	signed char ShutterHeight;
	signed char ShutterDirection;
	
	void Init(bool shouldRespawn);
	void RemoveItem(unsigned char index);
	void RemoveAllItemsOutsideOfTheScreen();
	void UpdateItems(Item::UpdateStep updateStep);
	void TeleportAndRespawnToLastCheckpoint();
	void AnimateCameraTransition();
	void AnimateShutterTransition();
	void BeginSwitchPuzzleScreen(unsigned char newTargetCameraX, unsigned char newTargetCameraY);
	void EndSwitchPuzzleScreen();
	void Draw(unsigned char minSpriteIndex, unsigned char maxSpriteIndex, signed char rickFeetOnScreen);
	unsigned char GetLevelSpriteAtWorldCoordinate(int xWorld, unsigned char yWorld);
	unsigned char GetLevelSpriteAt(unsigned char xMap, unsigned char yMap);
	bool IsDestroyableBlockAlive(unsigned char xMap, unsigned char yMap);
}

void MapManager::Reset(unsigned char startScreenId, unsigned char startScreenCameraX, unsigned char startScreenCameraY)
{
	// reset all internal variables
	ItemsToUpdateCount = 0;
	LastCheckPointPuzzleScreenId = startScreenId;
	CurrentPuzzleScreenId = startScreenId;
	FarestPuzzleScreenIdReached = startScreenId;
	PuzzleScreenMoveDirection = 1;
	LastCheckPointPuzzleScreenCoordX = startScreenCameraX;
	LastCheckPointPuzzleScreenCoordY = startScreenCameraY;
	PreviousPuzzleScreenCoordX = startScreenCameraX;
	LastCheckPointPreviousPuzzleScreenCoordX = startScreenCameraX;
	PreviousPuzzleScreenCoordY = startScreenCameraY;
	LastCheckPointPreviousPuzzleScreenCoordY = startScreenCameraY;
	
	// init the current camera
	CameraX.Current = startScreenCameraX;
	CameraX.Target =  startScreenCameraX;
	CameraY.Current = startScreenCameraY;
	CameraY.Target =  startScreenCameraY;
	
	// init the shutter variables in open mode
	ShutterHeight = (startScreenId == 0) ? 0 : HEIGHT / 2;
	ShutterDirection = -SHUTTER_SPEED;
	
	// init the first puzzle screen
	Init(true);
}

void MapManager::AddItem(Item * item)
{
	if (ItemsToUpdateCount < MAX_UPDATABLE_ITEM_COUNT)
	{
		// search if the item is not already inside the array
		for (unsigned char i = 0; i < ItemsToUpdateCount; ++i)
			if (ItemsToUpdate[i] == item)
				return;
		// add the item to the last position of the array
		ItemsToUpdate[ItemsToUpdateCount] = item;
		// increase the item count
		ItemsToUpdateCount++;
	}
}

void MapManager::RemoveItem(unsigned char index)
{
	// when this function is called, normally ItemsToUpdateCount is always > 0
	// decrease the item count
	ItemsToUpdateCount--;
	// if the array is not empty, move the last item to the empty place
	ItemsToUpdate[index] = ItemsToUpdate[ItemsToUpdateCount];
}

void MapManager::RemoveItem(Item * item)
{
	for (unsigned char i = 0; i < ItemsToUpdateCount; ++i)
		if (ItemsToUpdate[i] == item)
		{
			// remove the item found
			RemoveItem(i);
			// exit the loop when we have found and removed the item
			break;
		}
}

void MapManager::RemoveAllItemsOutsideOfTheScreen()
{
	// remove items outside of the screen
	for (unsigned char i = 0; i < ItemsToUpdateCount; ++i)
	{
		Item * currentItem = ItemsToUpdate[i];
		if (!IsOnScreen(currentItem->GetX(), currentItem->GetY(), 8, 8))
		{
			RemoveItem(i);
			i--;
		}
	}
}

void MapManager::CallMeBackForEachTrapTriggerer(Item* caller, ItemCallback callback)
{
	for (unsigned char i = 0; i < ItemsToUpdateCount; ++i)
		if (ItemsToUpdate[i]->IsPropertySet(Item::PropertyFlags::TRAP_TRIGERER))
			(*callback)(caller, ItemsToUpdate[i]);
}

void MapManager::UpdateItems(Item::UpdateStep updateStep)
{
	// iterate on all the items, and if one return true, we remove it from the update array
	for (unsigned char i = 0; i < ItemsToUpdateCount; ++i)
		if (ItemsToUpdate[i]->Update(updateStep))
		{
			RemoveItem(i);
			i--;
		}
}

void MapManager::Update()
{
	// update the input of the main character
	Rick::UpdateInput();

	// draw items that are lethal when other items are next to them, such ase the dynamite explosion
	// (which will stay in screen buffer as Lethal, so it is also lethal when overlapping)
	MapManager::UpdateItems(Item::UpdateStep::DRAW_LETHAL_BESIDE);
	
	// update the entities who wants to check if there's a lethal element next to them
	MapManager::UpdateItems(Item::UpdateStep::CHECK_LETHAL_BESIDE);
	
	// update the lethal entities
	MapManager::UpdateItems(Item::UpdateStep::DRAW_LETHAL);

	// Check the lethal collision for Rick after drawing the lethal items
	Rick::CheckLethalCollision();
	
	// Check lethal collision also for the ennemies (they should draw in black to erase the bullets) or destroyable blocks
	MapManager::UpdateItems(Item::UpdateStep::CHECK_LETHAL);

	// erase the bullet to avoid the bullet to be considered as static collision
	// also this will kill the bullet that hit Rick or an Enemy
	MapManager::UpdateItems(Item::UpdateStep::ERASE_BULLET);

	// Draw the ennemies
	MapManager::UpdateItems(Item::UpdateStep::DRAW_ENEMIES);

	// Check again the lethal collision for Rick because enemies are lethal to the player
	Rick::CheckLethalCollision();

	// get the position of the feet of rick in screen coordinate
	signed char rickFeetOnScreen = Rick::GetFeetYOnScreen();
	
	// Draw the static collision of the map
	Draw(0, SpriteData::PLATFORM, rickFeetOnScreen);

	// Draw static collision items
	MapManager::UpdateItems(Item::UpdateStep::DRAW_STATIC_COLLISION);

	// check the collision with the walls, floor and ceilling after the map has been drawn
	Rick::CheckStaticCollision();

	// draw the platforms above the player after checking the collision of the player
	Draw(SpriteData::PLATFORM, SpriteData::PLATFORM, rickFeetOnScreen);

	// call the function to check the static collision for the items that need it, including the Enemies
	MapManager::UpdateItems(Item::UpdateStep::CHECK_STATIC_COLLISION);

	// draw the pickup items or all the items ignores by the ennemies like a burning dynamite
	MapManager::UpdateItems(Item::UpdateStep::DRAW_IGNORED_BY_ENEMIES);

	// draw the ladders after checking the collision
	Draw(SpriteData::LADDER, SpriteData::LADDER, rickFeetOnScreen);

	// update the main character
	Rick::Draw();
	
	// animate the camera (in case the target as changed).
	// Should be done before or after all the drawing to have a consistent conversion from world coord to screen coord
	AnimateCameraTransition();
	
	// animate the shutter transition (at the beginning of the game, or after a checkpoint respawn)
	AnimateShutterTransition();
}

signed char MapManager::GetXOnScreen(int xWorld)
{
	return xWorld - ((int)CameraX.Current * SpriteData::LEVEL_SPRITE_WIDTH) - CameraX.Transition;
}

signed char MapManager::GetYOnScreen(unsigned char yWorld)
{
	return (signed char)(yWorld - (CameraY.Current * SpriteData::LEVEL_SPRITE_HEIGHT)) - CameraY.Transition + CAMERA_VERTICAL_SHIFT;
}

bool MapManager::IsOnScreen(int xWorld, unsigned char yWorld, unsigned char spriteWidth, unsigned char spriteHeight)
{
	// translate global coord to coord local to the screen
	signed char xOnScreen = MapManager::GetXOnScreen(xWorld);
	signed char yOnScreen = MapManager::GetYOnScreen(yWorld);
	// check that the part of the main character (including the sprite width and height) is inside the screen dimension
	return (xOnScreen + spriteWidth >= 0) /*&& (xOnScreen < WIDTH)*/ && (yOnScreen + spriteHeight >= 0) && (yOnScreen < HEIGHT);
}

bool MapManager::IsThereStaticCollisionAt(int xWorld, unsigned char yWorld, bool ignoreCeilingSprites)
{
	unsigned char spriteId = GetLevelSpriteAtWorldCoordinate(xWorld, yWorld);
	// if the sprite is a destroyable block, check if it is destroyed
	if (spriteId == SpriteData::NOTHING)
		return IsDestroyableBlockAlive(xWorld / SpriteData::LEVEL_SPRITE_WIDTH,
									yWorld / SpriteData::LEVEL_SPRITE_HEIGHT);
	// otherwise it depends on the type of sprite
	return (!ignoreCeilingSprites && (spriteId != SpriteData::WallId::LADDER)) ||
			(ignoreCeilingSprites && (spriteId < SpriteData::WallId::ROCK_CEILING_THIN));
}

unsigned char MapManager::GetCeillingScreenPositionAbove(int xWorld, unsigned char yWorld)
{
	// convert the world coordinate into index for the sprite map
	unsigned char xMap = xWorld / SpriteData::LEVEL_SPRITE_WIDTH;
	unsigned char yMap = yWorld / SpriteData::LEVEL_SPRITE_HEIGHT;
	while (yMap % NB_VERTICAL_SPRITE_PER_SCREEN)
	{
		if (GetLevelSpriteAt(xMap, yMap) != SpriteData::NOTHING)
			return (((yMap % NB_VERTICAL_SPRITE_PER_SCREEN) + 1) * SpriteData::LEVEL_SPRITE_HEIGHT) + CAMERA_VERTICAL_SHIFT;
		// look at the level sprite above
		yMap--;
	}
	return SpriteData::LEVEL_SPRITE_HEIGHT + CAMERA_VERTICAL_SHIFT;
}

bool MapManager::IsThereLadderAt(int xWorld, unsigned char yWorld)
{
	unsigned char spriteId = GetLevelSpriteAtWorldCoordinate(xWorld, yWorld);
	return (spriteId == SpriteData::WallId::LADDER) || (spriteId == SpriteData::WallId::PLATFORM_WITH_LADDER);
}

unsigned char MapManager::GetLevelSpriteAtWorldCoordinate(int xWorld, unsigned char yWorld)
{
	// convert the world coordinate into index for the sprite map
	int xMap = xWorld / SpriteData::LEVEL_SPRITE_WIDTH;
	unsigned char yMap = yWorld / SpriteData::LEVEL_SPRITE_HEIGHT;
	// check if we are inside the map. If not, consider that there is collision
	// to avoid the main character to exit the map and navigate into random memory
	if ((xMap < 0) || (xMap >= MapManager::LEVEL_WIDTH) || /*(yMap < 0) ||*/ (yMap >= MapManager::LEVEL_HEIGHT))
		return SpriteData::BLOCK_16_8_RIGHT;
	// call the function to get the sprite inside the map
	return GetLevelSpriteAt(xMap, yMap);
}

unsigned char MapManager::GetLevelSpriteAt(unsigned char xMap, unsigned char yMap)
{
	// compute start and end index in the array of sprite ids
	int startLineIndex = pgm_read_word(&(LevelLineIndex[yMap]));
	int endLineIndex = pgm_read_word(&(LevelLineIndex[yMap + 1]));
	
	// get the index of the sprite in the one dimentionnal array
	unsigned char targetSpriteIndex = xMap + 1;
	// iterate through the array to find the correct index
	unsigned char spriteIndex = 0;
	bool shouldReadEmptySpaceCount = false;
	for (int i = startLineIndex; i < endLineIndex; ++i)
	{
		// get the char that contains two ids (or one id and one count of empty space)
		unsigned char packedId = pgm_read_byte(&(Level[i]));
		// get the first id (or space count)
		unsigned char id = packedId / 16;
		// loop two times to check the 2 ids
		for (unsigned char j = 0; j < 2; ++j)
		{
			// first check if we have to read the count of empty space
			if (shouldReadEmptySpaceCount)
			{
				// the id should be interprated as a count, so sum it to the sprite index and reset the flag
				spriteIndex += id;
				shouldReadEmptySpaceCount = false;
				// if we reach the target, return NOTHING since we were in the middle of a compressed line of NOTHING
				if (spriteIndex >= targetSpriteIndex)
					return SpriteData::NOTHING;
			}
			else if (id == SpriteData::NOTHING)
			{
				// if the id is NOTHING, set the flag to increase the sprite index with the next value
				shouldReadEmptySpaceCount = true;
			}
			else
			{
				// otherwise this is a normal sprite, so just increase the index by one
				spriteIndex++;
				// check if we reach the target
				if (spriteIndex == targetSpriteIndex)
					return id;
			}
			// get the second id to check
			id = packedId & 0x0F;
		}
	}
	// by default return a collision
	return SpriteData::BLOCK_16_8_RIGHT;
}

bool MapManager::IsDestroyableBlockAlive(unsigned char xMap, unsigned char yMap)
{
	for (unsigned char i = 0; i < ItemsToUpdateCount; i++)
		if (ItemsToUpdate[i]->IsPropertySet(Item::PropertyFlags::DESTROYABLE_BLOCK))
		{
			DestroyableBlock * block = (DestroyableBlock*)ItemsToUpdate[i];
			if (block->IsLocatedAt(xMap, yMap))
				return block->IsAlive();
		}
	return false;
}

/**
 * Tell if there is any horizontal collision (i.e. ground or ceiling) at the specified world vertical coordinate,
 * and in between specified x world coordinates.
 * This function will test two pixels on the left and the right as specified in parameter.
 * If the specified y coordinate is outside the screen, then this function will ask the MapManager to check if there is
 * a sprite at that place in the level, to still have accurate collision event if the ground or ceiling is outside the
 * the screen.
 */
bool MapManager::IsThereAnyHorizontalCollisionAt(int xWorld, unsigned char yWorld, unsigned char width)
{
	// get the y coordinate first on screen (and check if it is on screen)
	signed char yOnScreen = MapManager::GetYOnScreen(yWorld);
	int rightWorld = xWorld + width;
	// check if the Y coordinate is out of the screen, if yes ask the map manager if there is a sprite below the scrren
	if ((yOnScreen < 0) || (yOnScreen >= HEIGHT))
		return MapManager::IsThereStaticCollisionAt(xWorld, yWorld) || 
				MapManager::IsThereStaticCollisionAt(rightWorld, yWorld);
	// get the left coordinates to check if it is outside the screen in that case, check the sprite
	// and if we find a collision stop here, otherwise continue to check
	signed char leftOnScreen = MapManager::GetXOnScreen(xWorld);
	if (((leftOnScreen < 0) /*|| (leftOnScreen >= WIDTH)*/) && MapManager::IsThereStaticCollisionAt(xWorld, yWorld))
		return true;
	// get the right coordinates to check if it is outside the screen in that case, check the sprite
	// and if we find a collision stop here, otherwise continue to check
	signed char rightOnScreen = MapManager::GetXOnScreen(rightWorld);
	if (((rightOnScreen < 0) /*|| (rightOnScreen >= WIDTH)*/) && MapManager::IsThereStaticCollisionAt(rightWorld, yWorld))
		return true;
	// if no collision found so far, and at least it is partially (or totally) on screen
	if (rightOnScreen >= 0)
	{
		if (WIDTH - leftOnScreen < width)
			width = WIDTH - leftOnScreen;
		else if (leftOnScreen < 0)
		{
			width += leftOnScreen;
			leftOnScreen = 0;
		}
		return arduboy.CheckWhitePixelsInRow(leftOnScreen, yOnScreen, width);
	}
	// finally no collision found
	return false;
}

/**
 * Init the current puzzle screen id, by adding all the items of this screen into the array of items to update.
 * This function will call the pointer on the specific Init function for the correct screen.
 * You can specify if you want the items to be initialized or not. If the player goes back in previous screen, you
 * don't want to respawn all the items, just add them to the list to update.
 * @param shouldInitItem if true all the items in the current screen will be initialized (so respwaned)
 */
void MapManager::Init(bool shouldInitItem)
{
	if ((CurrentPuzzleScreenId >= 0) && (CurrentPuzzleScreenId < PUZZLE_SCREEN_COUNT))
		(*MapManager::ItemInitFunctions[CurrentPuzzleScreenId])(shouldInitItem);
}

/**
 * This function is called from the Map Data init functions, to indicated that the current screen initialized
 * should become the new checkpoint. This function specifies the coordinates of where Rick should respawn if he dies.
 * @param rickX the X location where Rick should respawn in world coordinate
 * @param rickY the Y location where Rick should respawn in world coordinate
 */
void MapManager::MemorizeCheckPoint(int rickX, unsigned char rickY)
{
	// check if we reach a NEW checkpoint (farer than the last one memorized)
	if (CurrentPuzzleScreenId > LastCheckPointPuzzleScreenId)
	{
		// memorize the current screen id
		LastCheckPointPuzzleScreenId = CurrentPuzzleScreenId;
		// memorise the current screen coordinates
		LastCheckPointPuzzleScreenCoordX = CameraX.Target;
		LastCheckPointPuzzleScreenCoordY = CameraY.Target;
		// memorize also the previous screen coordinates (to restore them after a respawn)
		LastCheckPointPreviousPuzzleScreenCoordX = PreviousPuzzleScreenCoordX;
		LastCheckPointPreviousPuzzleScreenCoordY = PreviousPuzzleScreenCoordY;
	}
	
	// respawn the player at the correct location
	Rick::CheckPointRespawn(rickX, rickY);
}

void MapManager::TeleportAndRespawnToLastCheckpoint()
{
	// check if Rick still has some life
	if (Rick::LifeCount == 0)
	{
		GameManager::CurrentGameState = GameManager::GameState::GAME_OVER;
	}
	else
	{
		// set the current puzzle id to the last checkpoint puzzle id
		CurrentPuzzleScreenId = LastCheckPointPuzzleScreenId;
		FarestPuzzleScreenIdReached = LastCheckPointPuzzleScreenId;
		PuzzleScreenMoveDirection = 1;
		
		// reset the previous screen coordinates before the last checkpoint
		PreviousPuzzleScreenCoordX = LastCheckPointPreviousPuzzleScreenCoordX;
		PreviousPuzzleScreenCoordY = LastCheckPointPreviousPuzzleScreenCoordY;

		// teleport the camera to avoid a transition when restarting to last checkpoint
		CameraX.Current = LastCheckPointPuzzleScreenCoordX;
		CameraX.Target = LastCheckPointPuzzleScreenCoordX;
		CameraY.Current = LastCheckPointPuzzleScreenCoordY;
		CameraY.Target = LastCheckPointPuzzleScreenCoordY;

		// remove all the items, by clearing the whole array
		ItemsToUpdateCount = 0;
		
		// reset all the physics
		Physics::ResetAll();
		
		// call the init
		Init(true);
	}
}

/**
 * This function is called when the player as finished his death animation, and that the game should restart
 * from the last checkpoint. This function start the shutter animation, and when the shutter will be closed
 * in the AnimateShutterTransition() function, it will respawn the player to last checkpoint.
 */
void MapManager::StartShutterAnimation()
{
	// start the shutter animation by giving a non zero value
	ShutterHeight = SHUTTER_SPEED;
	ShutterDirection = SHUTTER_SPEED;
}

/**
 * This function will animate the shutter by closing it or opening it depending on the shutter speed sign.
 * If the shutter is closing, then when the shutter as finished to close it will either call the victory
 * screen if the graal has been collected, or it will teleport the camera to skip the transition, and will call
 * a Init with full initialization to respawn the dead enemies.
*/
void MapManager::AnimateShutterTransition()
{
	if (ShutterHeight > 0)
	{
		// draw the two shutter parts
		arduboy.fillRect(0, 0, WIDTH, ShutterHeight, BLACK);
		arduboy.fillRect(0, HEIGHT - ShutterHeight, WIDTH, ShutterHeight, BLACK);
		
		// increase the shutter height
		ShutterHeight += ShutterDirection;
		
		// check if the shutter is closed
		if (ShutterHeight >= (HEIGHT / 2))
		{
			// when the shutter is closed, check if we collected the graal or not
			if (graal1.IsPropertySet(Item::PropertyFlags::ALIVE))
			{
				// if the shutter is closed, call the teleport
				TeleportAndRespawnToLastCheckpoint();
				// reverse the shutter direction to reopen it
				ShutterDirection = -SHUTTER_SPEED;
			}
			else
			{
				GameManager::CurrentGameState = GameManager::GameState::VICTORY;
			}
		}
	}
}

/**
 * Get the speed that the camera should use according to the specified step parameters
 * If the step is greater than 1, then the returned speed correspond to the number of pixel to move,
 * otherwise, the speed returned is the number of frame to wait.
 * @param step the current step in the scrolling between 0 and 8
 * @param subStep the current subStep bewteen 0 and 7
 */
signed char MapManager::CameraCoord::GetCameraSpeed(signed char step, signed char subStep)
{
	// check if we need to move every frame or not
	if (step > 1)
	{
		if (step > 8)
			step = 8;
		// this array return the number of pixel the camera should move this frame
		signed char speed[] = { 1, 1, 1, 2, 2, 3, 4, 4, 4 };
		return speed[step];
	}
	else
	{
		// use an array to declare the number of frame to wait
		signed char frame[] = { 1, 2, 3, 4 };
		// check if we need to wait or if we can advance the camera transition
		if (arduboy.everyXFrames(frame[subStep >> 1]))
			return 1;
		else
			return 0;
	}
}

void MapManager::CameraCoord::Animate(signed char spriteSize)
{
	// Now check if the target is different from the current position of the camera
	signed char diff = Target - Current;
	if (diff > 0)
	{
		Transition += GetCameraSpeed(diff, Transition);
		StartDrawSprite = 0;
		EndDrawSprite = 1;
		if (Transition >= spriteSize)
		{
			Transition -= spriteSize;
			Current++;
			// check if we reach the end of the transition
			if (Target == Current)
				MapManager::EndSwitchPuzzleScreen();
		}
	}
	else if (diff < 0)
	{
		Transition -= GetCameraSpeed(-diff, -Transition);
		StartDrawSprite = -1;
		EndDrawSprite = 0;
		if (Transition <= -spriteSize)
		{
			Transition += spriteSize;
			Current--;
			// check if we reach the end of the transition
			if (Target == Current)
			{
				StartDrawSprite = 0;
				MapManager::EndSwitchPuzzleScreen();
			}
		}
	}
	else
	{
		StartDrawSprite = 0;
		EndDrawSprite = 0;
	}
}

void MapManager::BeginSwitchPuzzleScreen(unsigned char newTargetCameraX, unsigned char newTargetCameraY)
{
	bool isHorizontalTransition = (newTargetCameraX != CameraX.Target);
	bool isVerticalTransition = (newTargetCameraY != CameraY.Target);
	if (isHorizontalTransition || isVerticalTransition)
	{
		// if Rick just crossed the same edge as before, he is returning back
		bool isRickReturning = 	(newTargetCameraX == PreviousPuzzleScreenCoordX) &&
								(newTargetCameraY == PreviousPuzzleScreenCoordY);

		// put the current camera in the previous before setting the current with the new
		PreviousPuzzleScreenCoordX = CameraX.Target;
		PreviousPuzzleScreenCoordY = CameraY.Target;
		
		// set the new target camera
		CameraX.Target = newTargetCameraX;
		CameraY.Target = newTargetCameraY;

		// if the player return to the previous screen, inverse the direction
		if (isRickReturning)
			PuzzleScreenMoveDirection = -PuzzleScreenMoveDirection;
		
		// dump the progress in eeprom
		SaveAndLoadAliveStatusForAllItems(CurrentPuzzleScreenId, CurrentPuzzleScreenId + PuzzleScreenMoveDirection);

		// increase the puzzle screen according to the direction
		CurrentPuzzleScreenId += PuzzleScreenMoveDirection;
		
		// check if the player reach a new puzzle, and if yes, memorize it
		bool isRickReachedANewPuzzle = CurrentPuzzleScreenId > FarestPuzzleScreenIdReached;
		if (isRickReachedANewPuzzle)
			FarestPuzzleScreenIdReached = CurrentPuzzleScreenId;
		
		// init the items of the new puzzle screen
		Init(isRickReachedANewPuzzle);
	}
}

void MapManager::EndSwitchPuzzleScreen()
{
	RemoveAllItemsOutsideOfTheScreen();
}

/**
 * this function first check if the main charactert has exited the screen.
 * If yes, that means we need to start an animation of the camera to switch to the next puzzle screen.
 * Then the function will calculate the difference between the current position of the camera
 * and the target position, and animate the camera position to reach the target.
 */
void MapManager::AnimateCameraTransition()
{
	if (Rick::IsAlive())
	{
		// Check if we should start a camera transition (if the main character exit the screen)
		// because NB_VERTICAL_SPRITE_PER_SCREEN=8 then the masking is equivalent to the computation
		int screenPuzzleY = CameraY.Current & 0xF8; // i.e. (CameraY.Current / NB_VERTICAL_SPRITE_PER_SCREEN) * NB_VERTICAL_SPRITE_PER_SCREEN
		if (Rick::GetBottomForScreenTransition() <= (screenPuzzleY * SpriteData::LEVEL_SPRITE_HEIGHT))
		{
			BeginSwitchPuzzleScreen(CameraX.Target, screenPuzzleY - NB_VERTICAL_SPRITE_PER_SCREEN);
		}
		else
		{
			int nextScreenPuzzleY = screenPuzzleY + NB_VERTICAL_SPRITE_PER_SCREEN;
			if (Rick::GetTopForScreenTransition() >= (nextScreenPuzzleY * SpriteData::LEVEL_SPRITE_HEIGHT) - CAMERA_VERTICAL_SHIFT)
			{
				BeginSwitchPuzzleScreen(CameraX.Target, nextScreenPuzzleY);
			}
			else
			{
				int screenPuzzleX = CameraX.Current & 0xF0; // i.e. (CameraX.Current / NB_HORIZONTAL_SPRITE_PER_SCREEN) * NB_HORIZONTAL_SPRITE_PER_SCREEN
				if (Rick::GetRightForScreenTransition() <= (screenPuzzleX * SpriteData::LEVEL_SPRITE_WIDTH))
				{
					BeginSwitchPuzzleScreen(screenPuzzleX - NB_HORIZONTAL_SPRITE_PER_SCREEN, CameraY.Target);
				}
				else
				{
					int nextScreenPuzzleX = screenPuzzleX + NB_HORIZONTAL_SPRITE_PER_SCREEN;
					if (Rick::GetLeftForScreenTransition() >= (nextScreenPuzzleX * SpriteData::LEVEL_SPRITE_WIDTH))
					{
						BeginSwitchPuzzleScreen(nextScreenPuzzleX, CameraY.Target);
					}
				}
			}
		}
	}
	
	// animate the two coordinates of the camera
	CameraX.Animate(SpriteData::LEVEL_SPRITE_WIDTH);
	CameraY.Animate(SpriteData::LEVEL_SPRITE_HEIGHT);
}

/**
 * This function draw the map based on the current position of the camera.
 */
void MapManager::Draw(unsigned char minSpriteIndex, unsigned char maxSpriteIndex, signed char rickFeetOnScreen)
{
	// compute the start and end coordinate of the sprites to draw, in the level array coordinates
	unsigned char startMapX = CameraX.Current + CameraX.StartDrawSprite;
	unsigned char endMapX = CameraX.Current + NB_HORIZONTAL_SPRITE_PER_SCREEN + CameraX.EndDrawSprite;
	unsigned char startMapY = CameraY.Current + CameraY.StartDrawSprite;
	unsigned char endMapY = CameraY.Current + NB_VERTICAL_SPRITE_PER_SCREEN;
	unsigned int endLineIndex = pgm_read_word(&(LevelLineIndex[startMapY]));
	// iterate on the line first before iterating on the columns
	for (unsigned char mapY = startMapY; mapY < endMapY; ++mapY)
	{
		// compute start and end index in the array of sprite ids, since we start a new line, the start is the end of the previous loop
		unsigned int startLineIndex = endLineIndex;
		endLineIndex = pgm_read_word(&(LevelLineIndex[mapY + 1]));
		// compute the screen y coordinate for the sprite
		signed char screenY = (SpriteData::LEVEL_SPRITE_HEIGHT * (mapY - CameraY.Current)) + CAMERA_VERTICAL_SHIFT - CameraY.Transition;
		// determines if we need to draw the platforms.
		// If we are not specifically drawing the platforms (the min sprite index is not platforms),
		// then we should only draw the platforms below the feet
		// Otherwise, if we are in the drawing platform step, we only draw those above the feet,
		// since the below ones as already been drawn
		bool shouldDrawPlatforms = ((minSpriteIndex == 0) && (screenY > rickFeetOnScreen)) ||
									((minSpriteIndex == SpriteData::PLATFORM) && (screenY <= rickFeetOnScreen));
		// init the mapX coord with -1 because we will uncompress the whole line, and we want the first sprite to have the coordinate 0
		unsigned char mapX = 255; // this is -1 but for unsigned char
		// declare and init variable necessary for iterating on the columns
		unsigned char previousSpriteId = SpriteData::BLOCK_8_8;
		unsigned char currentSpriteId = SpriteData::NOTHING;
		bool isReadingHighBit = true;
		bool shouldReadEmptySpaceCount = false;
		unsigned char packedId;
		for (unsigned int i = startLineIndex; i < endLineIndex; isReadingHighBit = !isReadingHighBit, i += isReadingHighBit)
		{
			// read the current sprite id which is stored on 4 bits, either the high bits or the low bits
			if (isReadingHighBit)
			{
				// get the char that contains two ids (or one id and one count of empty space)
				packedId = pgm_read_byte(&(Level[i]));
				// get the first id (or space count)
				currentSpriteId = packedId / 16;
			}
			else
			{
				currentSpriteId = packedId & 0x0F;
			}
			// first check if we have to read the count of empty space
			if (shouldReadEmptySpaceCount)
			{
				// the id should be interprated as a count, so sum it to the sprite index and reset the flag
				mapX += currentSpriteId;
				previousSpriteId = SpriteData::NOTHING;
				shouldReadEmptySpaceCount = false;
				continue;
			}
			else if (currentSpriteId == SpriteData::NOTHING)
			{
				// if the id is NOTHING, set the flag to increase the sprite index with the next value
				shouldReadEmptySpaceCount = true;
				continue;
			}
			else
			{
				// otherwise this is a normal sprite, so just increase the index by one
				mapX++;
			}
			
			// check if we reached the begining of the screen or the end
			if (mapX < startMapX)
				continue;
			else if (mapX >= endMapX)
				break;
			
			// SPECIAL_ case for the mix of ladder and platform, draw either a platform or a ladder depending on the drawing state
			if (currentSpriteId == SpriteData::PLATFORM_WITH_LADDER)
			{
				if (minSpriteIndex == SpriteData::LADDER)
					currentSpriteId = SpriteData::LADDER;
				else
					currentSpriteId = SpriteData::PLATFORM;
			}

			// draw the sprite if we need to
			if ((currentSpriteId >= minSpriteIndex) && (currentSpriteId <= maxSpriteIndex) &&
				((currentSpriteId != SpriteData::PLATFORM) || shouldDrawPlatforms))
			{
				// choose a random mirror flag for some sprite that can be mirrored, for display variation
				// and for the big statue, they are mirror if they are the right one (i.e. if there is the same sprite on their left)
				bool isMirror = false;
				if ((currentSpriteId <= SpriteData::BLOCK_16_8_RIGHT) || (currentSpriteId == SpriteData::ROCK_CEILING_THIN))
				{
					unsigned char randomValue = (mapY * 16) | mapX;
					randomValue ^= randomValue * 4;
					randomValue ^= randomValue / 8;
					isMirror = randomValue % 2;
				}
				else if ((currentSpriteId == SpriteData::STAIR) || (currentSpriteId == SpriteData::ROCK_LEFT_WALL))
					isMirror = (previousSpriteId == SpriteData::NOTHING);
				else if ((currentSpriteId == SpriteData::BIG_STATUE_TOP) || (currentSpriteId == SpriteData::BIG_STATUE_BOTTOM))
					isMirror = (previousSpriteId == currentSpriteId);

				// call the draw function
				arduboy.drawBitmapExtended((SpriteData::LEVEL_SPRITE_WIDTH * (mapX - CameraX.Current)) - CameraX.Transition,
					screenY,
					SpriteData::Walls[currentSpriteId],
					SpriteData::LEVEL_SPRITE_WIDTH, SpriteData::LEVEL_SPRITE_HEIGHT, WHITE, isMirror);
			}
			
			// memorise the previous sprite id with the current one for the next loop
			previousSpriteId = currentSpriteId;
		}
	}
}
