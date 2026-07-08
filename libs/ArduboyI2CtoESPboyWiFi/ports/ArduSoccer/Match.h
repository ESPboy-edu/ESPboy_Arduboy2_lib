#ifndef MATCH_H_
#define MATCH_H_

#include "Defines.h"

#define MAX_SCORE_TEXT_SIZE 8

enum
{
	FIRST_HALF = 0,
	SECOND_HALF = 1
};

class Match
{
public:
	enum State
	{
		KickOff,
		Playing,
		Corner,
		ThrowIn,
		GoalKick,
		Penalty,
		FreeKick,
		Scored,
		HalfTime,
		MatchEnd,
		Queued
	};

	void reset();
	void update();

	void setState(Match::State newState);
	void queueState(Match::State newState, Team* electedTeam = nullptr, Person* electedKicker = nullptr);
	bool shouldAllowFreeMovement();
	bool shouldAllowKicking();

	void setupKickOff(Team* team);	
	void setupGoalKick(Team* team);
	void setupCorner(Team* team);
	void setupThrowIn(Team* team);
	void teleportPlayersToFormationPositions();
	void onKick();
	void onGoalScored(Team* team);
	void onHalfTime();
	void onMatchEnd();	

	class Team* getTeamAtTopHalf();
	class Team* getTeamAtBottomHalf();

	void regenerateScoreText();
	char* getMatchTimeString();

	char scoreText[MAX_SCORE_TEXT_SIZE];

	Person* electedKicker;
	Team* electedTeam;

	State state;
	State queuedState;
	uint32_t timeInState;

	uint32_t matchTimer;
	uint8_t matchHalf;
	int16_t throwInY;
	
	char* printInt(char* buffer, uint8_t number, bool leadingZeroes = false);
};

#endif
