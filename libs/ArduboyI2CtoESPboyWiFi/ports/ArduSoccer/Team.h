#ifndef TEAM_H_
#define TEAM_H_

#include <stdint.h>
#include "Defines.h"
class Person;

class Team
{
public:
	void init(Person* players, uint8_t inControllerType);
	void update();

	Person* getClosestPlayer(int16_t x, int16_t y);
	void calculateFormationPosition(uint8_t index, int16_t& outX, int16_t& outY);
	bool isTopHalf();
	void cycleSelectedPlayer() { shouldCycleSelectedPlayer = true; }
	void calculateCycleSelectedPlayer();

	enum Controller
	{
		LocalPlayer,
		RemotePlayer,
		ComputerPlayer
	};

	uint8_t controllerType;
	Person* players;

	bool shouldCycleSelectedPlayer;
	Person* selectedPlayer;
	int16_t formationOffsetX, formationOffsetY;
	const int16_t* formation;

	uint8_t score;
};

#endif
