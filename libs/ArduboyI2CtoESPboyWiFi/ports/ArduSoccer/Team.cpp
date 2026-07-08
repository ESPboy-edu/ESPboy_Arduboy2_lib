#include "Defines.h"
#include "Team.h"
#include "Person.h"
#include "MathsFunctions.h"
#include "Engine.h"

static const int16_t formationPositions[] PROGMEM =
{
	0, -64,
	64, -32,
	-64, -32,
	0, 32
};

void Team::init(Person* inPlayers, uint8_t inControllerType)
{
	players = inPlayers;
	score = 0;
	selectedPlayer = nullptr;
	controllerType = inControllerType;
}

Person* Team::getClosestPlayer(int16_t x, int16_t y)
{
	Person* closest = nullptr;
	int closestDistance = -1;

	for (int n = 0; n < PLAYERS_PER_TEAM; n++)
	{
		Person& player = players[n];
		int distance = estimateDistance(player.x, player.y, x, y);

		if (closest == nullptr || distance < closestDistance)
		{
			closest = &player;
			closestDistance = distance;
		}
	}

	return closest;
}

#define FORMATION_EDGE_SPACING 32
#define MAX_FORMATION_OFFSET_X 32
#define MAX_FORMATION_OFFSET_Y 64
#define ATTACK_Y_OFFSET 20
#define DEFEND_Y_OFFSET -64
#define GOALIE_ATTACK_Y_OFFSET 128
#define GOALIE_DEFEND_Y_OFFSET -128

void Team::calculateFormationPosition(uint8_t index, int16_t& outX, int16_t& outY)
{
	int multiplier = isTopHalf() ? 1 : -1;

	if (engine.match.state == Match::KickOff)
	{
		formationOffsetX = 0;

		if (isTopHalf())
		{
			formationOffsetY = -64;
		}
		else
		{
			formationOffsetY = 64;
		}
	}
	else if (engine.match.state == Match::GoalKick)
	{
		formationOffsetX = 0;

		if (engine.match.electedTeam && engine.match.electedTeam->isTopHalf())
		{
			formationOffsetY = 64;
		}
		else
		{
			formationOffsetY = -64;
		}
	}
	else if (engine.match.state == Match::ThrowIn)
	{
		if (engine.ball.x < BACKGROUND_WIDTH / 2)
		{
			formationOffsetX = -MAX_FORMATION_OFFSET_X;
		}
		else
		{
			formationOffsetX = MAX_FORMATION_OFFSET_X;
		}
		formationOffsetY = engine.ball.y - CENTER_MARK_Y;

		if (engine.match.electedKicker == &engine.people[index])
		{
			if (engine.ball.x < BACKGROUND_WIDTH / 2)
			{
				outX = engine.ball.x - 5;
			}
			else
			{
				outX = engine.ball.x + 5;
			}

			outY = engine.ball.y - 1;
			return;
		}
	}
	else if (engine.match.state == Match::Corner)
	{
		formationOffsetX = 0;

		if (engine.match.electedTeam && engine.match.electedTeam->isTopHalf())
		{
			formationOffsetY = 64;
		}
		else
		{
			formationOffsetY = -64;
		}

		if (engine.match.electedKicker == &engine.people[index])
		{
			if (engine.ball.x < BACKGROUND_WIDTH / 2)
			{
				outX = engine.ball.x - 6;
			}
			else
			{
				outX = engine.ball.x + 6;
			}

			outY = engine.ball.y;
			return;
		}
	}

	if (index >= PLAYERS_PER_TEAM)
		index -= PLAYERS_PER_TEAM;

	if (index == 0)
	{
		// Goalie

		if (engine.match.state == Match::GoalKick && engine.match.electedTeam == this)
		{
			// Goal kick position
			outX = engine.ball.x - 6;
			outY = isTopHalf() ? engine.ball.y - 4 : engine.ball.y + 4;
		}
		else
		{
			// Standing in goal
			outX = BACKGROUND_WIDTH / 2;
			outY = CENTER_MARK_Y - 125 * multiplier;
		}
		return;
	}

	if (engine.match.state == Match::KickOff && engine.match.electedTeam == this)
	{
		if (engine.match.electedKicker == &players[index])
		{
			if (isTopHalf())
			{
				outX = CENTER_MARK_X - 4;
				outY = CENTER_MARK_Y - 4;
			}
			else
			{
				outX = CENTER_MARK_X - 4;
				outY = CENTER_MARK_Y + 4;
			}
			return;
		}
		else if (index == PLAYERS_PER_TEAM - 2)
		{
			if (isTopHalf())
			{
				outX = CENTER_MARK_X + 24;
				outY = CENTER_MARK_Y - 4;
			}
			else
			{
				outX = CENTER_MARK_X + 24;
				outY = CENTER_MARK_Y + 4;
			}
			return;
		}
	}

	index--;

	outX = (int16_t)pgm_read_word(&formationPositions[index * 2]);
	outY = (int16_t)pgm_read_word(&formationPositions[index * 2] + 1);
	outY *= multiplier;

	outX += CENTER_MARK_X + formationOffsetX;
	outY += CENTER_MARK_Y + formationOffsetY;

	if (outX < PITCH_LEFT + FORMATION_EDGE_SPACING)
	{
		outX = PITCH_LEFT + FORMATION_EDGE_SPACING;
	}
	if (outY < PITCH_TOP + FORMATION_EDGE_SPACING)
	{
		outY = PITCH_TOP + FORMATION_EDGE_SPACING;
	}
	if (outX > PITCH_RIGHT - FORMATION_EDGE_SPACING)
	{
		outX = PITCH_RIGHT - FORMATION_EDGE_SPACING;
	}
	if (outY > PITCH_BOTTOM - FORMATION_EDGE_SPACING)
	{
		outY = PITCH_BOTTOM - FORMATION_EDGE_SPACING;
	}
}

bool Team::isTopHalf()
{
	return this != &engine.teams[engine.match.matchHalf];
}

void Team::update()
{
	int multiplier = isTopHalf() ? 1 : -1;

	if (engine.match.state == Match::KickOff)
	{
		return;
	}

	formationOffsetX = engine.ball.x - CENTER_MARK_X;
	formationOffsetY = engine.ball.y - CENTER_MARK_Y;

	if (engine.ball.owner)
	{
		int offset = 0;
		if (engine.ball.owner->isGoalie())
		{
			if (engine.ball.owner->getTeam() == this)
			{
				offset = GOALIE_ATTACK_Y_OFFSET;
			}
			else
			{
				offset = GOALIE_DEFEND_Y_OFFSET;
			}
		}
		else
		{
			if (engine.ball.owner->getTeam() == this)
			{
				offset = ATTACK_Y_OFFSET;
			}
			else
			{
				offset = DEFEND_Y_OFFSET;
			}
		}

		formationOffsetY += offset * multiplier;
	}

	if (formationOffsetX < -MAX_FORMATION_OFFSET_X)
		formationOffsetX = -MAX_FORMATION_OFFSET_X;
	if (formationOffsetX > MAX_FORMATION_OFFSET_X)
		formationOffsetX = MAX_FORMATION_OFFSET_X;
	if (formationOffsetY < -MAX_FORMATION_OFFSET_Y)
		formationOffsetY = -MAX_FORMATION_OFFSET_Y;
	if (formationOffsetY > MAX_FORMATION_OFFSET_Y)
		formationOffsetY = MAX_FORMATION_OFFSET_Y;

	if (controllerType != Team::ComputerPlayer)
	{
		if (shouldCycleSelectedPlayer || selectedPlayer == nullptr)
		{
			calculateCycleSelectedPlayer();
		}
	}
}

void Team::calculateCycleSelectedPlayer()
{
	// Find player closest to the ball that isn't the current one
	Person* closest = nullptr;
	int closestDistance = 0;

	for (int pass = 0; pass < 2; pass++)
	{
		for (int n = 0; n < PLAYERS_PER_TEAM; n++)
		{
			Person& person = players[n];

			if (pass == 0)
			{
				if (selectedPlayer == &person)
					continue;
				if (!person.isOnScreen())
					continue;
			}

			if (!person.isGoalie())
			{
				int distance = estimateDistance(engine.ball.x, engine.ball.y, person.x, person.y);

				if (closest == nullptr || distance < closestDistance)
				{
					closest = &person;
					closestDistance = distance;
				}
			}
		}
	}

	selectedPlayer = closest;
	shouldCycleSelectedPlayer = false;
}
