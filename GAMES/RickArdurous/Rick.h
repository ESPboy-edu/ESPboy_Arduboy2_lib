#ifndef _RICK_H_
#define _RICK_H_

namespace Rick
{
	static constexpr int MAX_LIFE_COUNT = 6;
	static constexpr int MAX_BULLET_COUNT = 5;
	static constexpr int MAX_DYNAMITE_COUNT = 5;
	
	void Reset();
	void UpdateInput();
	void CheckLethalCollision();
	void CheckStaticCollision();
	void CheckLadderCollision();
	signed char GetFeetYOnScreen();
	void Draw();
	bool IsAlive();
	void CheckPointRespawn(int respawnWorldX, unsigned char respawnWorldY);
	void InitWaitForRespawn();

	// function to get the coordinate of the sprite of the main character
	int GetX();
	unsigned char GetY();
	int GetCenterX();
	unsigned char GetCenterY();
	void MoveOutsideMainMenu();
	// function to get the coordinate when the screen should do a transition
	int GetLeftForScreenTransition();
	int GetRightForScreenTransition();
	int GetTopForScreenTransition();
	int GetBottomForScreenTransition();
	
	// Rick Inventory
	extern signed char LifeCount;
	extern signed char StatuetteCount;
	extern signed char BulletCount;
	extern signed char DynamiteCount;
}

#endif
