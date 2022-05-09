#include "Ardulem.h"
#include "MapManager.h"
#include "MapData.h"
#include "HUD.h"
#include "Input.h"
#include "SpriteData.h"
#include "MainMenu.h"
#include "Music.h"
#include "MemoryPool.h"
#include "Lem.h"

// we need a compiler builtin function to count the number of bits set in a char (well, we'll use the int16_t func)
uint8_t bitcount(uint16_t dta){
   uint16_t val=dta;
   uint8_t cnt=0;
   for(uint8_t i=0; i<16; i++){
    if((val & 1) == 1) cnt++;
    val = val >> 1;
   }
   return (cnt);
};

//usefull macro for modif map manipulation
// get the modif map index from (ool, line) index. Normally (lineY * MODIF_MAP_COLUMN_COUNT) + colX   but  MODIF_MAP_COLUMN_COUNT=16
#define GET_MAP_INDEX(colX, lineY) (((int16_t)lineY << 4) + colX)

namespace MapManager
{
	const uint16_t MAP_SCREEN_WIDTH = WIDTH - HUD::HUD_WIDTH;
	const uint16_t MAP_MAX_WIDTH = 256; // do not increase that number
	
	/*
	 * The modification list will store all the modification made by the Lem to the original map.
	 * The modifications will be paint16_t with the color INVERT, which means if a bit is set, it is
	 * either a solid pixel of the map that was dig up by a lem, or an empty pixel that was fill
	 * with a stair of a lem. Therefore, painting with the INVERT color will bring the correct
	 * Modification.
	 * The modificaiton list will contains a list of char, each char represent 8 vertical pixel to
	 * paint16_t in invert mode. Now to know the localisation of each modif char, we use a bitfield map.
	 * The map is split in 8 row, and each row will contains 256 bits (i.e. 32 chars). Each bit set
	 * in the row indicates that a modification char should be applied on that particular x.
	 * So the map itself will take 8x32 = 256 chars. And each modification will take one char
	 */
	const uint16_t X_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT = 4; // x in [0..255] and map index in [0..15]
	const uint16_t Y_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT = 3; // y in [0..63] and map index in [0..8]
	const uint16_t NB_BIT_MODIF_MAP_CELL = sizeof(int16_t)*8;
	const uint16_t MODIF_MAP_LINE_COUNT = 8;
	const uint16_t MODIF_MAP_COLUMN_COUNT = MAP_MAX_WIDTH / NB_BIT_MODIF_MAP_CELL;
	const uint16_t MODIFICATION_MAP_SIZE = MODIF_MAP_LINE_COUNT * MODIF_MAP_COLUMN_COUNT;
	uint16_t ModificationMap[MODIFICATION_MAP_SIZE];
	
	unsigned char * ModificationList;
	uint16_t ModificationListCount;
	uint16_t ModificationListSize;
	
	// the current map Description we are playing
	unsigned char CurrentMapId = 0;
	MapData::MapDescription CurrentMapDescription;
	const uint16_t * CurrentMapMirroredSprites = 0;
	
	unsigned char RequiredLemPercentage = 0; // computed in the init function from the ratio required lem count by available lem count
	unsigned char IntroAnimFrameIndex = 0;
	
	// this variable store the current scrolling value of the map on the screen
	int16_t ScrollValue = 0;
	bool ScrollView(int16_t scrollMoveInPixel);
	
	// private functions
	void UpdateInput();
	void DrawMap();

	// map config
	unsigned char GetStartX()				{ return CurrentMapDescription.StartX; }
	unsigned char GetStartY()				{ return CurrentMapDescription.StartY; }
	unsigned char GetHomeX()				{ return CurrentMapDescription.HomeX; }
	unsigned char GetHomeY()				{ return CurrentMapDescription.HomeY; }
	unsigned char GetMinDropSpeed()			{ return CurrentMapDescription.MinDropSpeed; }
	unsigned char GetAvailableLemCount()	{ return CurrentMapDescription.AvailableLemCount * 5; }
	unsigned char GetRequiredLemCount()		{ return CurrentMapDescription.RequiredLemCount * 5; }
	unsigned char GetRequiredLemPercentage(){ return RequiredLemPercentage; }
	unsigned char GetWalkerCount()			{ return CurrentMapDescription.LemWalkCount; }
	unsigned char GetBlockerCount()			{ return CurrentMapDescription.LemBlockCount; }
	unsigned char GetBomberCount()			{ return CurrentMapDescription.LemBombCount; }
	unsigned char GetDiggerDiagonalCount()	{ return CurrentMapDescription.LemDigDiagCount; }
	unsigned char GetDiggerHorizontalCount(){ return CurrentMapDescription.LemDigHorizCount; }
	unsigned char GetDiggerVerticalCount()	{ return CurrentMapDescription.LemDigVertCount; }
	unsigned char GetStairerCount()			{ return CurrentMapDescription.LemStairCount; }
	unsigned char GetClimberCount()			{ return CurrentMapDescription.LemClimbCount; }
	unsigned char GetParachuterCount()		{ return CurrentMapDescription.LemParaCount; }
	void DecreaseWalkerCount()				{ CurrentMapDescription.LemWalkCount--; }
	void DecreaseBlockerCount()				{ CurrentMapDescription.LemBlockCount--; }
	void DecreaseBomberCount()				{ CurrentMapDescription.LemBombCount--; }
	void DecreaseDiggerDiagonalCount()		{ CurrentMapDescription.LemDigDiagCount--; }
	void DecreaseDiggerHorizontalCount()	{ CurrentMapDescription.LemDigHorizCount--; }
	void DecreaseDiggerVerticalCount()		{ CurrentMapDescription.LemDigVertCount--; }
	void DecreaseStairerCount()				{ CurrentMapDescription.LemStairCount--; }
	void DecreaseClimberCount()				{ CurrentMapDescription.LemClimbCount--; }
	void DecreaseParachuterCount()			{ CurrentMapDescription.LemParaCount--; }
	int16_t GetMaxScrollValue()					{ return (CurrentMapDescription.SpriteColumnCount << 3) - MAP_SCREEN_WIDTH; }

	// internal functions
	int16_t GetSpriteCountBeforeColumn(const unsigned char * mapLocalization, int16_t col);
	int16_t GetSpriteGlobalId(int16_t spriteIdIndex, signed char spriteIdSubIndex);
	bool IsSpriteMirrored(int16_t spriteIndex);
	unsigned char Get8PixelsOutsideScreen(int16_t worldX, int16_t lineY, bool considerAddedPixels);
	unsigned char GetPixelsColumnAligned(int16_t worldX, int16_t lineY, bool considerAddedPixels);

	void ClearModificationList();
	void DrawModifications();
	int16_t GetModificationIndex(int16_t colX, int16_t bitX, int16_t lineY);
	int16_t GetModificationIndex(int16_t mapIndex, int16_t bitX);
	void Modify8Pixels(int16_t worldX, int16_t lineY, unsigned char pixels);
	void Delete8AlignedPixels(int16_t worldX, int16_t LineY, unsigned char pixelToDelete);
}

bool MapManager::ScrollView(int16_t scrollMoveInPixel)
{
	// memorize current value and scroll
	int16_t previousValue = ScrollValue;
	ScrollValue += scrollMoveInPixel;
	
	// clamp the value
	if (ScrollValue < 0)
		ScrollValue = 0;
	else if (ScrollValue > GetMaxScrollValue())
		ScrollValue = GetMaxScrollValue();
	
	// return true if the view has changed
	return (previousValue != ScrollValue);
}

void MapManager::InitMap()
{
	// copy the map description of the specified map id into my RAM instance of Map description
	CurrentMapDescription.StartX = pgm_read_byte(&(MapData::AllMaps[CurrentMapId].StartX));
	CurrentMapDescription.StartY = pgm_read_byte(&(MapData::AllMaps[CurrentMapId].StartY));
	CurrentMapDescription.HomeX = pgm_read_byte(&(MapData::AllMaps[CurrentMapId].HomeX));
	CurrentMapDescription.HomeY = pgm_read_byte(&(MapData::AllMaps[CurrentMapId].HomeY));
	// get the pack lem count number
	unsigned char packedChar = pgm_read_byte(&(MapData::AllMaps[CurrentMapId].TimeInMultipleOf10s) + 1);
	CurrentMapDescription.AvailableLemCount = packedChar & 0x0F;
	CurrentMapDescription.RequiredLemCount = packedChar >> 4;
	// get the pack drop speed and walker count
	packedChar = pgm_read_byte(&(MapData::AllMaps[CurrentMapId].TimeInMultipleOf10s) + 2);
	CurrentMapDescription.MinDropSpeed = packedChar & 0x0F;
	CurrentMapDescription.LemWalkCount = packedChar >> 4;
	// for the bitfield, we need to read the whole int16_t and unpack, 
	// and we need to use the adress of the previous variable to work around a compiler error
	int16_t lemCount = pgm_read_word(&(MapData::AllMaps[CurrentMapId].TimeInMultipleOf10s) + 3);
	CurrentMapDescription.LemBlockCount = lemCount & 0x000F;
	CurrentMapDescription.LemBombCount = (lemCount >> 4) & 0x000F;
	CurrentMapDescription.LemDigDiagCount = (lemCount >> 8) & 0x000F;
	CurrentMapDescription.LemDigHorizCount = lemCount >> 12;
	// read the second bitfield
	lemCount = pgm_read_word(&(MapData::AllMaps[CurrentMapId].TimeInMultipleOf10s) + 5);
	CurrentMapDescription.LemDigVertCount = lemCount & 0x000F;
	CurrentMapDescription.LemStairCount = (lemCount >> 4) & 0x000F;
	CurrentMapDescription.LemClimbCount = (lemCount >> 8) & 0x000F;
	CurrentMapDescription.LemParaCount = lemCount >> 12;
	// now read the pointer tables
	CurrentMapDescription.SpriteLocalization = (const unsigned char *)pgm_read_ptr(&(MapData::AllMaps[CurrentMapId].SpriteLocalization));
	CurrentMapDescription.SpriteColumnCount = pgm_read_byte(&(MapData::AllMaps[CurrentMapId].SpriteColumnCount));
	CurrentMapDescription.SpriteIdList = (const uint16_t *)pgm_read_ptr(&(MapData::AllMaps[CurrentMapId].SpriteIdList));
	// count how many sprites in the level to find where is the beginning of the mirror list
	// for that we ask the total number of sprite (all the sprite after the last column)
	// then divided by 3, because each int16_t in the SpriteIdList store 3 sprites
	int16_t packedSpritesCount = GetSpriteCountBeforeColumn(CurrentMapDescription.SpriteLocalization, CurrentMapDescription.SpriteColumnCount);
	int16_t spritesCount = packedSpritesCount / 3;
	if ((packedSpritesCount % 3) != 0)
		spritesCount++;
	CurrentMapMirroredSprites = (const uint16_t *)(pgm_read_dword(&(MapData::AllMaps[CurrentMapId].SpriteIdList)) + spritesCount);

	// compute the required lem percentage
	RequiredLemPercentage = (unsigned char)(((int16_t)CurrentMapDescription.RequiredLemCount * 100) / CurrentMapDescription.AvailableLemCount);

	// init the map scroll view so that the player can see the start in the middle of the screen
	ScrollValue = 0;
	ScrollView(CurrentMapDescription.StartX - (MAP_SCREEN_WIDTH/2));

	// init the start intro frame
	IntroAnimFrameIndex = 0;
	
	// compute the size of the modification list and its position
	int16_t LemSize = sizeof(Lem) * GetAvailableLemCount();
	ModificationList = &(MemoryPool::Pool[LemSize]);
	ModificationListSize = MemoryPool::MEMORY_POOL_SIZE - LemSize;

	// clear the modification list
	memset(ModificationMap, 0, sizeof(ModificationMap));
	memset(ModificationList, 0, ModificationListSize);
	ModificationListCount = 0;
	
	// init the HUD with the data of the map
	HUD::Init(pgm_read_byte(&(MapData::AllMaps[CurrentMapId].TimeInMultipleOf10s)) * 10);
}

void MapManager::Update()
{
	// update input before drawing, because we may update the ScrollValue and this value is used in DrawMap,
	// and of course, we want the value to match with the map drawn this frame, otherwise there will be bugs with the lem logic
	UpdateInput();
	DrawMap();
	DrawModifications();
}

void MapManager::UpdateInput()
{
	const int16_t INPUT_FRAME_COUNT_FIRST_MODULO = 5;
	const int16_t INPUT_FRAME_COUNT_SECOND_MODULO = 1;
	const int16_t SCROLL_VIEW_SPEED = 2;

	// early exit if button A is down, because that means we want to control the HUD
	if (Input::IsDown(A_BUTTON))
		return;

	// get the current cursor position
	unsigned char cursorX = HUD::GetCursorX();
	unsigned char cursorY = HUD::GetCursorY();
	
	// Move the cursor and scroll the view if the cursor reach one border of the view
	if (Input::IsDownModulo(LEFT_BUTTON, INPUT_FRAME_COUNT_FIRST_MODULO, INPUT_FRAME_COUNT_SECOND_MODULO))
	{
		if (cursorX == HUD::HUD_WIDTH)
			MapManager::ScrollView(-SCROLL_VIEW_SPEED);
		else
			HUD::SetCursorX(cursorX - 1);
	}
	else if (Input::IsDownModulo(RIGHT_BUTTON, INPUT_FRAME_COUNT_FIRST_MODULO, INPUT_FRAME_COUNT_SECOND_MODULO))
	{
		if (cursorX == WIDTH-1)
			MapManager::ScrollView(SCROLL_VIEW_SPEED);
		else
			HUD::SetCursorX(cursorX + 1);
	}
	else if (Input::IsDownModulo(DOWN_BUTTON, INPUT_FRAME_COUNT_FIRST_MODULO, INPUT_FRAME_COUNT_SECOND_MODULO))
	{
		if (cursorY < HEIGHT-1)
			HUD::SetCursorY(cursorY + 1);
	}
	else if (Input::IsDownModulo(UP_BUTTON, INPUT_FRAME_COUNT_FIRST_MODULO, INPUT_FRAME_COUNT_SECOND_MODULO))
	{
		if (cursorY > 0)
			HUD::SetCursorY(cursorY - 1);
	}
}

void MapManager::DrawStartAndHome()
{
	// compute the current frame index
	if (MainMenu::IsCurrentGameStatePlaying() && (IntroAnimFrameIndex < ANIM_START_FRAME_COUNT-1) && arduboy.everyXFrames(5))
		IntroAnimFrameIndex++;

	// draw start
	int16_t startX = CurrentMapDescription.StartX;
	if (IsOnScreen(startX) || IsOnScreen(startX + 16))
	{
		// draw the start and its mirror
		int16_t screenX = ConvertToScreenCoord(startX);
		int16_t screenY = CurrentMapDescription.StartY;
		arduboy.drawBitmapExt(screenX, screenY, anim_Start[IntroAnimFrameIndex], sizeof(anim_Start[0]), WHITE);
		arduboy.drawBitmapExt(screenX+8, screenY, anim_Start[IntroAnimFrameIndex], sizeof(anim_Start[0]), WHITE, true);
	}
	
	// draw home
	int16_t homeX = CurrentMapDescription.HomeX-7;
	if (IsOnScreen(homeX) || IsOnScreen(homeX + 15))
	{
		int16_t screenX = ConvertToScreenCoord(homeX);
		int16_t screenY = CurrentMapDescription.HomeY - 7;
		arduboy.drawBitmapExt(screenX,   screenY,   sprite_HomeBottom, 15, WHITE);
		arduboy.drawBitmapExt(screenX+5, screenY-4, sprite_HomeTop, 5, WHITE);
		arduboy.drawBitmapExt(screenX+6, screenY-11, anim_HUDFlag[(arduboy.frameCount >> 2)%ANIM_HUD_FLAG_FRAME_COUNT], sizeof(anim_HUDFlag[0]), WHITE);
	}
}

inline int16_t MapManager::GetSpriteCountBeforeColumn(const unsigned char * mapLocalization, int16_t col)
{
	int16_t result = 0;
	for (int16_t i = 0; i < col; ++i)
		result += bitcount(pgm_read_byte(mapLocalization + i));
	return result;
}

inline int16_t MapManager::GetSpriteGlobalId(int16_t spriteIdIndex, signed char spriteIdSubIndex)
{
	// get the local id of that sprite
	int16_t ThreePackedLocIds = pgm_read_word(CurrentMapDescription.SpriteIdList + spriteIdIndex);
	// now return the good part
	return (ThreePackedLocIds >> (5*spriteIdSubIndex)) & 0x001F;
}

/*
 * Tell if the sprite specified with it's global index (i.e. not its index in the array of sprite)
 * is mirrored according to the map data.
 */
bool MapManager::IsSpriteMirrored(int16_t spriteIndex)
{
	int16_t mirrorIntIndex = spriteIndex >> 4; // divide by 16 the id to get the int16_t index
	int16_t mirrorBitIndex = spriteIndex % 16; // get the rest to get the bit
	return pgm_read_word(CurrentMapMirroredSprites + mirrorIntIndex) & (1<<mirrorBitIndex);
}

void MapManager::DrawMap()
{
	// get the various pointer on the current map data
	const unsigned char * mapLocalization = CurrentMapDescription.SpriteLocalization;
	
	// find the first map localization pixel from the scrolling value (by dividing by 8)
	int16_t firstSpriteColumn = ScrollValue >> 3;
	
	// get the number of sprites that should be ignored for the not drawn sprite columns
	int16_t currentSpriteDrawn = GetSpriteCountBeforeColumn(mapLocalization, firstSpriteColumn);
	int16_t currentSpriteIdIndex = currentSpriteDrawn / 3;
	signed char currentSpriteIdSubIndex = currentSpriteDrawn % 3;
	
	// compute the last column drawn by adding the number of sprite column visible on the screen
	int16_t lastSpriteColumn = firstSpriteColumn + ((WIDTH - HUD::HUD_WIDTH)/8);
	// clamp the last column if the map is less wide than the screen
	if (lastSpriteColumn >= CurrentMapDescription.SpriteColumnCount)
		lastSpriteColumn = CurrentMapDescription.SpriteColumnCount -1;

	// now iterate from the first to last column to draw the sprites
	for (int16_t col = firstSpriteColumn; col <= lastSpriteColumn; ++col)
	{
		// get the column from the map localization
		signed char currentSpriteColumnLocalization = pgm_read_byte(mapLocalization + col);
		
		// compute the current x of this sprite column
		int16_t spriteColX = HUD::HUD_WIDTH + (col<<3) - ScrollValue;
		
		// now iterate on every bit to see if there's a sprite to draw
		for (int16_t i = 0; i < 8; ++i)
			if (currentSpriteColumnLocalization & (1<<i))
			{
				// we found a bit set, there's a sprite to draw, get it's global id
				int16_t currentSpriteGlobalId = GetSpriteGlobalId(currentSpriteIdIndex, currentSpriteIdSubIndex);
				
				// now check it's mirrored status (compute which char in the array, then which bit that char)
				bool isMirrored = IsSpriteMirrored(currentSpriteDrawn);

				// now draw the sprite, at the correct position
				arduboy.drawBitmapExt(spriteColX, i<<3, MapData::MapSprite[currentSpriteGlobalId], 8, WHITE, isMirrored);
				
				// increase the sprite counter
				currentSpriteDrawn++;
				currentSpriteIdSubIndex++;
				if (currentSpriteIdSubIndex == 3)
				{
					currentSpriteIdIndex++;
					currentSpriteIdSubIndex = 0;
				}
			}
	}
}

/*
 * Tells if the specific x position (in map coordinate) is currently visible on screen
 */
bool MapManager::IsOnScreen(int16_t worldX)
{
	return (worldX >= ScrollValue) && (worldX < ScrollValue + MAP_SCREEN_WIDTH);
}

/*
 * Tells if the specified world coordinate is inside the world
 */
bool MapManager::IsInMapBoundary(int16_t worldX, int16_t worldY)
{
	// for the y, the map is 64 pixel
	return (worldY < 64) && (worldX >= 0) && (worldX < (CurrentMapDescription.SpriteColumnCount << 3));
}

unsigned char MapManager::ConvertToScreenCoord(int16_t worldX)
{
	return worldX - ScrollValue + HUD::HUD_WIDTH;
}

/*
 * Get the column of 8 pixels at the specified worldX and that contains the worldY.
 */
unsigned char MapManager::Get8PixelsOutsideScreen(int16_t worldX, int16_t lineY, bool considerAddedPixels)
{
	// compute the sprite column from the x and we already know the row (i.e. lineY)
	unsigned char col = worldX >> 3;
	
	// start with a default black pixel
	unsigned char eightPixelsColumn = 0;
	
	// now find the pixel inside the const map data
	unsigned char currentSpriteColumnLocalization = pgm_read_byte(CurrentMapDescription.SpriteLocalization + col);
	
	// check if there's a sprite at the specified lineY and col
	if (currentSpriteColumnLocalization & (0x01 << lineY))
	{
		// there's a sprite at the specified location, so count the number of sprite to ignore before that one
		int16_t spriteIndex = GetSpriteCountBeforeColumn(CurrentMapDescription.SpriteLocalization, col);
		// and add all the sprite if any before that one (shift the loca char before counting the remaining bits)
		spriteIndex += bitcount((uint16_t)((currentSpriteColumnLocalization << (8 - lineY)) & 0xFF));

		// get the global id of the sprite we need
		int16_t currentSpriteGlobalId = GetSpriteGlobalId(spriteIndex/3, spriteIndex%3);

		// we have to choose the right column of the sprite depending if the sprite is mirrored or not
		int16_t spriteColumnIndex = (worldX % 8);
		if (IsSpriteMirrored(spriteIndex))
			spriteColumnIndex = 7-spriteColumnIndex;

		// now get the correct column of the sprite
		eightPixelsColumn = pgm_read_byte(MapData::MapSprite[currentSpriteGlobalId] + spriteColumnIndex);
	}
	
	// now apply the modification of that column of 8 pixels if needed
	// get the col and lineY in the modif map
	col = worldX >> X_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT;
	int16_t bitX = worldX % NB_BIT_MODIF_MAP_CELL;
	int16_t mapIndex = GET_MAP_INDEX(col, lineY);
	
	// check if there's a modif at the specified coordinate
	if (ModificationMap[mapIndex] & (1 << bitX))
	{
		// we found a modif, get it from the list
		int16_t modifIndex = GetModificationIndex(mapIndex, bitX);
		if (modifIndex < ModificationListSize)
		{
			// if there's a modif, apply it to the original map data depending if we need the added pixels
			// If we want the added pixels (wich means we also want the deletd ones), we can use a XOR
			// | original pixel | modif | result pixel
			// |        0       |   1   |   1   (this was an added pixel -> take the modif, now it is set)
			// |        0       |   0   |   0   (pixel not modified -> it stays empty)
			// |        1       |   1   |   0   (this was a deleted pixel -> take the modif, now it is empty)
			// |        1       |   0   |   1   (pixel not modified -> it stays set)
			//
			// but if we only want the deleted pixels, we can use an AND + NOT modif:
			// | original pixel | modif | ~modif | result pixel
			// |        0       |   1   |    0   |      0   (this was an added pixel -> ignore the modif, leave it empty)
			// |        0       |   0   |    1   |      0   (pixel not modified -> it stays empty)
			// |        1       |   1   |    0   |      0   (this was a deleted pixel -> take the modif, now it's empty)
			// |        1       |   0   |    1   |      1   (pixel not modified -> it stays set)
			if (considerAddedPixels)
				eightPixelsColumn ^= ModificationList[modifIndex];
			else
				eightPixelsColumn &= ~ModificationList[modifIndex];
		}
	}

	// and return the final 8 pixels column
	return eightPixelsColumn;
}

/*
 * Get a column of pixel at the specified position downward up to a specified number of pixel (no more that 8), 
 * no matter if the specified position is inside the screen or outside the screen.
 * You can specify a flag to tell if you want to consider (true) or ignore (false) the added pixel.
 * This function will always consider the deleted pixels (no parameter for now, cause no need for gameplay)
 * The added consideration is usefull for the stairs. You want the lem to go through the stair, but walk on them
 * therefore you will consider added pixel for checking the ground under lem feet, but ignore them
 * while checking the wall in front of the lem.
 * If the requested column is on the screen and we can consider added pixels, then we return it directly from the screen buffer 
 * assume that the map is drawn first before anything else, so only the map data cover the screen
 * Otherwise, if the pixel is outside the screen or we should not consider added pixels, we will recompute the column.
 */

unsigned char MapManager::GetPixelsColumn(int16_t worldX, int16_t worldY, int16_t height_, bool considerAddedPixels)
{ 
	uint16_t lineY = worldY >> Y_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT;
	uint16_t yNormalized = (worldY % 8);
  int16_t leftShift = (16 - yNormalized - height_);

   
	// get one or two column of 8 pixels, depending where is the worldY and the required height
	uint16_t height16PixelsColumn = GetPixelsColumnAligned(worldX, lineY, considerAddedPixels);
	if ((yNormalized > height_-1) && (lineY < 7))
		height16PixelsColumn |= (GetPixelsColumnAligned(worldX, lineY + 1, considerAddedPixels) << 8);
	// return the value, shifted and masked according to the height required (shift in both direction)
  
	return (unsigned char)(((height16PixelsColumn << leftShift)&0xFFFF) >> (leftShift + yNormalized));
}

unsigned char MapManager::GetPixelsColumnAligned(int16_t worldX, int16_t lineY, bool considerAddedPixels)
{
		return Get8PixelsOutsideScreen(worldX, lineY, considerAddedPixels);
}

void MapManager::SetPixel(int16_t worldX, int16_t worldY, bool isAdded)
{
	// compute the line
	int16_t lineY = worldY >> Y_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT;
	signed char pixelShift = (worldY % 8);
	unsigned char pixelMask = (unsigned char)(1 << pixelShift);

	// get the current pixel column at the position of the modification.
	signed char currentPixel = (GetPixelsColumnAligned(worldX, lineY, true) & pixelMask) >> pixelShift;
	
	// if we want to add a pixel, but the pixel is already set, or, if we want to remove a pixel, but there's nothing,
	// then we have nothing to do. So call the modification only if the SetPixel will actually modify the map
	if (isAdded != currentPixel)
		Modify8Pixels(worldX, lineY, pixelMask);
}

/*
 * Modify the Map by deleting a column of 8 veritcal pixels. The column must be aligned on Y sprite line.
 * This function only delete pixels, it doesn't add a column of pixels.
 */
void MapManager::Delete8AlignedPixels(int16_t worldX, int16_t lineY, unsigned char pixelToDelete)
{
	// get the current column of pixels at the position of the modification.
	// make an AND operation, as we want both the current pixel set, and a modification for that pixel
	pixelToDelete &= GetPixelsColumnAligned(worldX, lineY, true);
	
	// check if there's any remaining pixel to modify
	if (bitcount((uint16_t)pixelToDelete) != 0)
		Modify8Pixels(worldX, lineY, pixelToDelete);
}

/*
 * Modify the Map by deleting a column of 16 veritcal pixels. The column must be aligned on Y sprite line.
 * You can use that function to delete 8 unaligned pixels, by shifting them inside the int.
 * This function only delete pixels, it doesn't add a column of pixels.
 */
void MapManager::Delete16AlignedPixels(int16_t worldX, int16_t lineY, uint16_t pixelToDelete)
{
	// compute the top and bottom pixels to delete after applying the specified shift
	unsigned char topPixelToDelete = (unsigned char)(pixelToDelete & 0xFF);
	unsigned char bottomPixelToDelete = (unsigned char)(pixelToDelete >> 8);
	
	// delete the pixels if there's any remaining
	if (topPixelToDelete != 0)
		Delete8AlignedPixels(worldX, lineY, topPixelToDelete);
	if ((bottomPixelToDelete != 0) && (lineY < 7))
		Delete8AlignedPixels(worldX, lineY+1, bottomPixelToDelete);
}

/*
 * Return the number of modification before the specified col x and bit x inside that col,
 * and the specified line y (between 0 and 8). If there's a modification at the specified coord, this modification
 * is not counted. Therefore this function returns the number of modification BEFORE the specified coord,
 * which can safely be used as an index for the ModificationList array.
 * So: if there's a modification at the specified position the returned value is the index of that modification,
 * and if there's no modification at the specified position, the returned value is the insertion point.
 */
int16_t MapManager::GetModificationIndex(int16_t colX, int16_t bitX, int16_t lineY)
{
	// compute the map index and call the other function with different signature
	return GetModificationIndex(GET_MAP_INDEX(colX, lineY), bitX);
}

/*
 * Return the number of modification before the specified map index and bit x
 * If there's a modification at the specified coord, this modification
 * is not counted. Therefore this function returns the number of modification BEFORE the specified coord,
 * which can safely be used as an index for the ModificationList array.
 * So: if there's a modification at the specified position the returned value is the index of that modification,
 * and if there's no modification at the specified position, the returned value is the insertion point.
 */
int16_t MapManager::GetModificationIndex(int16_t mapIndex, int16_t bitX)
{
	// get the modif index in the modif list by counting the bit set to the coordinate
	int16_t modifIndex = 0;
	for (int16_t i = 0; i < mapIndex; ++i)
		modifIndex += bitcount(ModificationMap[i]);
	// add the last bit of the current mapBitfield
	modifIndex += bitcount(ModificationMap[mapIndex] << (NB_BIT_MODIF_MAP_CELL - bitX));
	
	// return the index in the modif list
	return modifIndex;
}

void MapManager::Modify8Pixels(int16_t worldX, int16_t lineY, unsigned char pixels)
{
	int16_t colX = worldX >> X_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT;
	int16_t bitX = worldX % NB_BIT_MODIF_MAP_CELL;
	int16_t mapIndex = GET_MAP_INDEX(colX, lineY);
	int16_t modifIndex = GetModificationIndex(mapIndex, bitX);
	
	// check if the modif index is inside the list
	if (modifIndex < ModificationListSize)
	{
		int16_t mapMaskForX = (1 << bitX);
		
		// check if there's already a modif of if we need to add one
		if (ModificationMap[mapIndex] & mapMaskForX)
		{
			// if there's already a modif, merge the new modif with the existing one with a XOR
			// (if both not modified: it stay not modified, if one is modified, we take the modif,
			// if a modif is modified again, it cancel the modif)
			ModificationList[modifIndex] ^= pixels;
			// if the modification becomes empty, then we should remove it from the list
			if (ModificationList[modifIndex] == 0)
			{
				// unset the bit in the modif map
				ModificationMap[mapIndex] &= ~mapMaskForX;
				
				// and delete the modification in the modif list
				for (int16_t i = modifIndex; i < ModificationListCount ; ++i)
					ModificationList[i] = ModificationList[i+1];
				ModificationListCount--;
			}
		}
		// otherwise we need to add a new modif, but only if the list is not full
		else if (ModificationListCount < ModificationListSize)
		{
			// otherwise we need to set the bit in the modif map
			ModificationMap[mapIndex] |= mapMaskForX;
			
			// and insert the modification in the modif list
			for (int16_t i = ModificationListCount-1; i >= modifIndex; --i)
				ModificationList[i+1] = ModificationList[i];
			ModificationList[modifIndex] = pixels;
			ModificationListCount++;
		}
	}
}

void MapManager::DrawModifications()
{
	int16_t startCol = ScrollValue >> X_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT;
	int16_t endCol = (ScrollValue + MAP_SCREEN_WIDTH) >> X_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT;
	int16_t startBitX = ScrollValue % NB_BIT_MODIF_MAP_CELL;
	
	// iterate on the line
	for (int16_t lineY = 0; lineY < MODIF_MAP_LINE_COUNT; ++lineY)
	{
		// get the modif index for the first x of the current line
		int16_t modifIndex = GetModificationIndex(startCol, startBitX, lineY);
		
		// now iterate on the column
		int16_t startB = startBitX;
		for (int16_t colX = startCol; colX <= endCol; ++colX)
		{
			// compute the current screen x for the current column
			int16_t screenX = MapManager::ConvertToScreenCoord(colX << X_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT);
			
			// and iterate on the bit of the map bitfield
			for (int16_t b = startB; b < NB_BIT_MODIF_MAP_CELL; ++b)
				if (ModificationMap[GET_MAP_INDEX(colX, lineY)] & (1<<b))
				{
					// draw the modif on screen
					arduboy.invertBufferCharWithSpecifiedChar(screenX + b, lineY, ModificationList[modifIndex++]);
					// stop drawing if we reach the end of the modification list
					if (modifIndex >= ModificationListSize)
						return;
				}
			// reset start b to 0 for the next column
			startB = 0;
		}
	}
}
