#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "LivingItem.h"

class Enemy : public LivingItem
{
public:
	Enemy();
	virtual bool Update(UpdateStep step);
	
private:
	static constexpr int WALL_COLLISION_DETECTION_DISTANCE = 1;
	static constexpr int MUMMY_HALF_TURN_ANIM_SPEED = 14;
	static constexpr int SKELETON_HALF_TURN_ANIM_SPEED = 5;
	static constexpr int SKELETON_WALK_ANIM_SPEED = 3;
	static constexpr int SCORPION_WALK_ANIM_SPEED = 1;
	static constexpr int DEATH_VELOCITY_X = 9;
	static constexpr int SKELETON_SENSOR = 4;

	enum State : unsigned char
	{
		WALK = 0,
		HALF_TURN,
		WAIT,
		FALL,
		DEATH,
		WAIT_AGAIN,
	};
	
	unsigned char AnimFrameId;
	unsigned char AnimFrameCount;
	State AnimState;
	unsigned char PhysicsId;
	unsigned char MyWidth;
	unsigned char MyHeight;
	
	inline bool IsScorpion() { return IsPropertySet(Item::PropertyFlags::SPECIAL_); }
	inline bool IsSkeleton() { return IsPropertySet(Item::PropertyFlags::SPECIAL__2); }
	int GetYUnderFeet();
	bool IsThereAnyGroundCollisionAt(unsigned char yWorld);
	bool IsThereWallCollisionOrGap(bool shouldCheckGap);
	void MoveAccordingToOrientation();
	
	void InitWait();
	void InitWalk();
	void InitFall();
	void InitHalfTurn();
	void UpdateWalk();
	void UpdateHalfTurn();
	void UpdateWait();
	void UpdateFall();
	bool UpdateDeath();
	void UpdateSkeletonBehavior();
	int Draw(unsigned char color);
};

#endif
