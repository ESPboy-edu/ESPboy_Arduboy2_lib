#ifndef PERSON_H_
#define PERSON_H_

#include "Defines.h"

class Team;

class Person
{
public:
	enum State
	{
		Standing,
		Walking,
		Stunned,
		Fallen,
		SlideTackle,
		DiveLeft,
		DiveRight
	};

	uint8_t index;
	int x, y;
	uint8_t z;
	uint8_t team;

	uint8_t direction : 8;
	State state : 8;

	uint8_t animation;
	uint8_t animationFrame;
	uint8_t displayFrame;

	void init(uint8_t index);
	void update();
	void stun(uint8_t frames, bool shouldFall = false);
	void kickBall(int velocityX, int velocityY, int velocityZ);
	void goalieDive();
	void tryPass(uint8_t direction);
	void tryShoot();
	Team* getTeam();
	void takeBall();

	bool isSelectedPlayer();
	bool isHoldingBall() { return isGoalie() && hasBall() && isInOwnPenaltyBox(); }
	bool hasBall();
	bool isOnScreen(int margin = 0);
	bool isGoalie() { return index == 0 || index == PLAYERS_PER_TEAM; }
	bool isInOwnPenaltyBox();

	static void getDirectionOffset(uint8_t direction, int8_t& dx, int8_t& dy);

	bool tryMove(int deltaX, int deltaY);
	bool isColliding();
	uint8_t getAvoidDirection(uint8_t dir);

	static int8_t ballDeltaX, ballDeltaY;
};

#endif

