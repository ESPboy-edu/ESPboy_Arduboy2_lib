#include "Engine.h"
#include "Match.h"

void Match::reset()
{
	for (uint8_t n = 0; n < NUM_PEOPLE; n++)
	{
		engine.people[n].init(n);
	}

	engine.teams[WHITE_TEAM].score = 0;
	engine.teams[BLACK_TEAM].score = 0;
	matchTimer = 0;
	matchHalf = 0;
	setupKickOff(&engine.teams[WHITE_TEAM]);
	//setupCorner(&engine.teams[WHITE_TEAM]);

	engine.renderer.showLargeMessage(PSTR("KICK OFF!"));
	engine.setCameraFocus(CENTER_MARK_X, CENTER_MARK_Y);
	regenerateScoreText();
}

void Match::onGoalScored(Team* team)
{
	Platform.playSound(Sounds::goal);
	team->score++;
	setState(Match::Scored);
	electedTeam = team;
	
	if (engine.ball.lastOwner && engine.ball.lastOwner->getTeam() == team)
	{
		electedKicker = engine.ball.lastOwner;
	}

	regenerateScoreText();
	engine.renderer.showLargeMessage(PSTR("GOAL!"));
}

void Match::onHalfTime()
{
	setState(Match::HalfTime);
	engine.renderer.showLargeMessage(PSTR("HALF TIME"));
	matchHalf = SECOND_HALF;
}

void Match::onMatchEnd()
{
	setState(Match::MatchEnd);

	if (engine.teams[0].score == engine.teams[1].score)
	{
		engine.renderer.showLargeMessage(PSTR("DRAW!"));
	}
	else
	{
		int winningTeam = engine.teams[0].score < engine.teams[1].score ? 1 : 0;

		if (engine.teams[winningTeam].controllerType == Team::LocalPlayer)
		{
			engine.renderer.showLargeMessage(PSTR("YOU WIN!"));
			Platform.playSound(Sounds::win);
		}
		else
		{
			engine.renderer.showLargeMessage(PSTR("YOU LOSE!"));
			Platform.playSound(Sounds::lose);
		}
	}
}

void Match::update()
{
	switch(state)
	{
	case Match::Playing:
		if (!engine.ball.owner || !engine.ball.owner->isHoldingBall())
		{
			if (engine.ball.isInsideTopNet())
			{
				if (engine.teams[WHITE_TEAM].isTopHalf())
				{
					onGoalScored(&engine.teams[BLACK_TEAM]);
				}
				else
				{
					onGoalScored(&engine.teams[WHITE_TEAM]);
				}
			}
			else if (engine.ball.isInsideBottomNet())
			{
				if (engine.teams[WHITE_TEAM].isTopHalf())
				{
					onGoalScored(&engine.teams[WHITE_TEAM]);
				}
				else
				{
					onGoalScored(&engine.teams[BLACK_TEAM]);
				}
			}
			else if (engine.ball.y < PITCH_TOP)
			{
				Platform.playSound(Sounds::whistle);

				if (engine.ball.lastOwner && engine.ball.lastOwner->getTeam() == getTeamAtTopHalf())
				{
					queueState(Match::Corner, getTeamAtBottomHalf());
				}
				else
				{
					queueState(Match::GoalKick, getTeamAtTopHalf());
				}
			}
			else if (engine.ball.y > PITCH_BOTTOM)
			{
				Platform.playSound(Sounds::whistle);

				if (engine.ball.lastOwner && engine.ball.lastOwner->getTeam() == getTeamAtBottomHalf())
				{
					queueState(Match::Corner, getTeamAtTopHalf());
				}
				else
				{
					queueState(Match::GoalKick, getTeamAtBottomHalf());
				}
			}
			else if (engine.ball.x < PITCH_LEFT || engine.ball.x > PITCH_RIGHT)
			{
				Platform.playSound(Sounds::whistle);

				Team* team = engine.ball.lastOwner != nullptr && engine.ball.lastOwner->team == WHITE_TEAM ? &engine.teams[BLACK_TEAM] : &engine.teams[WHITE_TEAM];
				queueState(Match::ThrowIn, team);
				throwInY = engine.ball.y;
			}
		}
		matchTimer++;

		if (matchHalf == FIRST_HALF && matchTimer > engine.settings.matchHalfLength * 60 * TARGET_FRAMERATE)
		{
			Platform.playSound(Sounds::whistle);
			onHalfTime();
		}
		if (matchHalf == SECOND_HALF && matchTimer > engine.settings.matchHalfLength * 60 * TARGET_FRAMERATE * 2)
		{
			Platform.playSound(Sounds::whistle);
			onMatchEnd();
		}

		break;

	case Match::Scored:
		if (timeInState > 30 * 5)
		{
			if (electedTeam == &engine.teams[WHITE_TEAM])
			{
				setupKickOff(&engine.teams[BLACK_TEAM]);
			}
			else
			{
				setupKickOff(&engine.teams[WHITE_TEAM]);
			}
			
			engine.renderer.showLargeMessage(nullptr);
		}
		break;

	case Match::HalfTime:
		if (timeInState > 30 * 5)
		{
			Platform.playSound(Sounds::whistle);
			setupKickOff(&engine.teams[BLACK_TEAM]);
			engine.renderer.showLargeMessage(PSTR("2ND HALF"));
		}
		break;

	case Match::MatchEnd:
		if (timeInState > 30 * 5)
		{
			engine.returnToMenu();
		}
		break;

	case Match::KickOff:
		if (timeInState == 60)
		{
			Platform.playSound(Sounds::whistle);
		}
		break;

	case Match::Queued:
		if (timeInState > 30 * 1)
		{
			switch (queuedState)
			{
			case Match::GoalKick:
				setupGoalKick(electedTeam);
				break;
			case Match::Corner:
				setupCorner(electedTeam);
				break;
			case Match::ThrowIn:
				setupThrowIn(electedTeam);
				break;
			default:
				setState(queuedState);
				break;
			}
		}
		break;
	}

	timeInState++;
}

void Match::setState(Match::State newState)
{
	state = newState;
	timeInState = 0;
	electedKicker = nullptr;
	electedTeam = nullptr;
	engine.ball.setOwner(nullptr);
}

void Match::queueState(Match::State newState, Team* team, Person* kicker)
{
	if (state != Match::Queued)
	{
		setState(Match::Queued);
		queuedState = newState;
		electedTeam = team;
		electedKicker = kicker;
	}
}


bool Match::shouldAllowFreeMovement()
{
	return state == Match::Playing;
}

bool Match::shouldAllowKicking()
{
	if (engine.renderer.isShowingLargeMessage())
	{
		return false;
	}
	switch (state)
	{
	case Match::Scored:
		return false;
	case Match::Playing:
		return true;
	case Match::Queued:
		return false;
	default:
		return timeInState > 60;
	}
}

void Match::onKick()
{
	switch (state)
	{
	default:
		setState(Match::Playing);
		break;
	case Match::Playing:
		break;
	}
}

void Match::teleportPlayersToFormationPositions()
{
	for (int n = 0; n < PLAYERS_PER_TEAM * 2; n++)
	{
		Person& person = engine.people[n];
		int16_t formationX, formationY;
		person.getTeam()->calculateFormationPosition(person.index, formationX, formationY);
		person.x = formationX;
		person.y = formationY;
		person.direction = person.getTeam()->isTopHalf() ? South : North;
	}
}

void Match::setupKickOff(Team* team)
{
	engine.ball.setPosition(CENTER_MARK_X, CENTER_MARK_Y);
	setState(Match::KickOff);

	electedTeam = team;
	electedKicker = &team->players[PLAYERS_PER_TEAM - 1];
	teleportPlayersToFormationPositions();

	electedKicker->takeBall();
}

void Match::setupCorner(Team* team)
{
	engine.renderer.showLargeMessage(PSTR("CORNER"));

	int cornerX = engine.ball.x < BACKGROUND_WIDTH / 2 ? PITCH_LEFT : PITCH_RIGHT;

	if (team->isTopHalf())
	{
		engine.ball.setPosition(cornerX, PITCH_BOTTOM);
	}
	else
	{
		engine.ball.setPosition(cornerX, PITCH_TOP);
	}
	setState(Match::Corner);

	electedTeam = team;
	electedKicker = &team->players[PLAYERS_PER_TEAM - 1];
	teleportPlayersToFormationPositions();

	electedKicker->takeBall();
}

void Match::setupThrowIn(Team* team)
{
	engine.renderer.showLargeMessage(PSTR("THROW IN"));

	int throwInX = engine.ball.x < BACKGROUND_WIDTH / 2 ? PITCH_LEFT : PITCH_RIGHT;

	engine.ball.setPosition(throwInX, throwInY);
	setState(Match::ThrowIn);

	electedTeam = team;

	electedKicker = team->getClosestPlayer(throwInX, throwInY);
	if (electedKicker->isGoalie())
	{
		electedKicker = &team->players[1];
	}
	teleportPlayersToFormationPositions();

	electedKicker->takeBall();
}

void Match::setupGoalKick(Team* team)
{
	engine.renderer.showLargeMessage(PSTR("GOAL KICK"));

	if (team->isTopHalf())
	{
		engine.ball.setPosition(CENTER_MARK_X + 32, PITCH_TOP + 16);
	}
	else
	{
		engine.ball.setPosition(CENTER_MARK_X + 32, PITCH_BOTTOM - 16);
	}
	
	setState(Match::GoalKick);

	electedTeam = team;
	electedKicker = &team->players[0];
	teleportPlayersToFormationPositions();

	electedKicker->takeBall();
}

void Match::regenerateScoreText()
{
	char* ptr = scoreText;

	ptr = printInt(ptr, engine.teams[0].score);
	*ptr++ = '-';
	ptr = printInt(ptr, engine.teams[1].score);
}

char* Match::printInt(char* buffer, uint8_t number, bool leadingZeroes)
{
	if (number > 99)
	{
		number = 99;
	}
	if (number >= 10 || leadingZeroes)
	{
		int tens = number / 10;
		*buffer++ = '0' + tens;
		number -= tens * 10;
	}
	*buffer++ = '0' + number;

	return buffer;
}

char* Match::getMatchTimeString()
{
	int totalSeconds = matchTimer / TARGET_FRAMERATE;
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds - minutes * 60;

	static char buffer[6];
	char* ptr = buffer;
	ptr = printInt(ptr, minutes);
	*ptr++ = ':';
	ptr = printInt(ptr, seconds, true);

	return buffer;
}

Team* Match::getTeamAtTopHalf()
{
	return &engine.teams[!matchHalf];
}

Team* Match::getTeamAtBottomHalf()
{
	return &engine.teams[matchHalf];
}
