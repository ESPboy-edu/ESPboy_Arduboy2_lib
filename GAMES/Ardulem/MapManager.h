#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H
/*
 * This file is in charge of instantiating the current map from the Map Sprite and Map descriptions
 */
namespace MapManager
{
	extern void InitMap();
	extern void Update();
	extern void DrawStartAndHome();

	// map config
	extern unsigned char CurrentMapId;
	extern unsigned char GetStartX();
	extern unsigned char GetStartY();
	extern unsigned char GetHomeX();
	extern unsigned char GetHomeY();
	extern unsigned char GetMinDropSpeed();
	extern unsigned char GetAvailableLemCount();
	extern unsigned char GetRequiredLemCount();
	extern unsigned char GetRequiredLemPercentage();
	extern unsigned char GetWalkerCount();
	extern unsigned char GetBlockerCount();
	extern unsigned char GetBomberCount();
	extern unsigned char GetDiggerDiagonalCount();
	extern unsigned char GetDiggerHorizontalCount();
	extern unsigned char GetDiggerVerticalCount();
	extern unsigned char GetStairerCount();
	extern unsigned char GetClimberCount();
	extern unsigned char GetParachuterCount();
	
	// decrease lem count
	extern void DecreaseWalkerCount();
	extern void DecreaseBlockerCount();
	extern void DecreaseBomberCount();
	extern void DecreaseDiggerDiagonalCount();
	extern void DecreaseDiggerHorizontalCount();
	extern void DecreaseDiggerVerticalCount();
	extern void DecreaseStairerCount();
	extern void DecreaseClimberCount();
	extern void DecreaseParachuterCount();

	// coordinate helper
	extern bool IsOnScreen(int16_t worldX);
	extern bool IsInMapBoundary(int16_t worldX, int16_t worldY);
	unsigned char ConvertToScreenCoord(int16_t worldX);
	
	// pixel helper
	extern unsigned char GetPixelsColumn(int16_t worldX, int16_t worldY, int16_t height, bool considerAddedPixels);
	extern void SetPixel(int16_t worldX, int16_t worldY, bool isAdded);
	extern void Delete16AlignedPixels(int16_t worldX, int16_t lineY, uint16_t pixelToDelete);
}

#endif
