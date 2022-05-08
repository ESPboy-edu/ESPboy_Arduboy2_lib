#ifndef _MAP_MANAGER_H_
#define _MAP_MANAGER_H_

class Item;

// define the type of pointer on member function of Item, for callback purpose
typedef void (*ItemCallback)(Item* caller, Item* trigerer);

namespace MapManager
{
	void Reset(unsigned char startScreenId, unsigned char startScreenCameraX, unsigned char startScreenCameraY);
	void Update();
	
	// function to add or remove the items currently updated
	void AddItem(Item * item);
	void RemoveItem(Item * item);
	
	// function to get local coordinates
	signed char GetXOnScreen(int xWorld);
	signed char GetYOnScreen(unsigned char yWorld);
	bool IsOnScreen(int xWorld, unsigned char yWorld, unsigned char spriteWidth, unsigned char spriteHeight);
	
	// Check if there's a solid sprite (static collision) or ladder at the specified world coodinates
	bool IsThereStaticCollisionAt(int xWorld, unsigned char yWorld, bool ignoreCeilingSprites = false);
	bool IsThereAnyHorizontalCollisionAt(int xWorld, unsigned char yWorld, unsigned char width);
	bool IsThereLadderAt(int xWorld, unsigned char yWorld);
	unsigned char GetCeillingScreenPositionAbove(int xWorld, unsigned char yWorld);

	// function to iterate on items
	void CallMeBackForEachTrapTriggerer(Item* caller, ItemCallback callback);
	
	// checkpoint functions
	void MemorizeCheckPoint(int rickX, unsigned char rickY);
	void StartShutterAnimation();
}

#endif
