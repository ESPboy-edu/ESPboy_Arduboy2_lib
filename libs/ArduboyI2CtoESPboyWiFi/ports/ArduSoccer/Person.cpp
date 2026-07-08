#include "Engine.h"
#include "Person.h"
#include "MathsFunctions.h"

#define ENABLE_AI_PLAYER 1

enum FrameNames
{
	SOUTH_STAND = 0,
	SOUTH_WALK1,
	SOUTH_WALK2,
	EAST_STAND,
	EAST_WALK1,
	EAST_WALK2,
	WEST_STAND,
	WEST_WALK1,
	WEST_WALK2,
	NORTH_STAND,
	NORTH_WALK1,
	NORTH_WALK2,
	FALL_RIGHT,
	EAST_TACKLE,
	SOUTH_TACKLE,
	NORTH_TACKLE,
	WEST_TACKLE,
	SOUTH_DIVE_RIGHT1,
	SOUTH_DIVE_RIGHT2,
	SOUTH_DIVE_LEFT1,
	SOUTH_DIVE_LEFT2,
	NORTH_DIVE_RIGHT1,
	NORTH_DIVE_RIGHT2,
	NORTH_DIVE_LEFT1,
	NORTH_DIVE_LEFT2
};

const uint8_t walkAnimations[] PROGMEM =
{
	NORTH_STAND, NORTH_WALK1, NORTH_STAND, NORTH_WALK2,
	EAST_STAND, EAST_WALK1, EAST_STAND, EAST_WALK2,
	SOUTH_STAND, SOUTH_WALK1, SOUTH_STAND, SOUTH_WALK2,
	WEST_STAND, WEST_WALK1, WEST_STAND, WEST_WALK2,
};

const uint8_t inputToDirection[16] PROGMEM =
{
	NoDirection,	// 0000
	North,			// 0001
	East,			// 0010
	NorthEast,		// 0011
	South,			// 0100
	NoDirection,	// 0101
	SouthEast,		// 0110
	East,			// 0111
	West,			// 1000
	NorthWest,		// 1001
	NoDirection,	// 1010
	North,			// 1011
	SouthWest,		// 1100
	West,			// 1101
	South,			// 1110
	NoDirection,	// 1111
};

const uint8_t directionToWalkAnimation[] PROGMEM =
{
	0,
	0,
	1,
	2,
	2,
	2,
	3,
	0
};

const uint8_t directionToSlideTackleFrame[] PROGMEM =
{
	NORTH_TACKLE,
	EAST_TACKLE,
	EAST_TACKLE,
	EAST_TACKLE,
	SOUTH_TACKLE,
	WEST_TACKLE,
	WEST_TACKLE,
	WEST_TACKLE
};

const uint8_t directionToFallFrame[] PROGMEM =
{
	FALL_RIGHT,
	FALL_RIGHT, 
	FALL_RIGHT,
	FALL_RIGHT,
	FALL_RIGHT,
	FALL_RIGHT,
	FALL_RIGHT,
	FALL_RIGHT,
};

const int8_t directionToX[] PROGMEM =
{
	0,
	1,
	1,
	1,
	0,
	-1,
	-1,
	-1
};

const int8_t directionToY[] PROGMEM =
{
	-1,
	-1,
	0,
	1,
	1,
	1,
	0,
	-1,
};

const uint8_t goalieDiveZ[] PROGMEM =
{
	0,
	2,
	4,
	6,
	7,
	8,
	8,
	8,
	7,
	6,
	4,
	2,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

struct DifficultySettings
{
	uint8_t tackleDelay;
	uint8_t chaseBallDelay;
	uint8_t applyPressureDelay;
	uint8_t pressureDistance;
};

const DifficultySettings difficultySettings[] PROGMEM =
{
	// Easy
	{
		// Tackle delay
		60,
		// Chase ball delay
		45,
		// Apply pressure delay
		90,
		// Pressure distance
		50,
	},

	// Hard
	{
		// Tackle delay
		15,
		// Chase ball delay
		0,
		// Apply pressure delay
		30,
		// Pressure distance
		40,
	}
};


int8_t Person::ballDeltaX;
int8_t Person::ballDeltaY;

void Person::init(uint8_t startIndex)
{
	index = startIndex;
	animation = 0;
	state = Person::Standing;
	if (index < PLAYERS_PER_TEAM)
	{
		team = 0;
		direction = South;
		displayFrame = SOUTH_STAND;
	}
	else if (index < PLAYERS_PER_TEAM * 2)
	{
		team = 1;
		direction = North;
		displayFrame = NORTH_STAND;
	}
	else
	{
		team = 2;
		direction = East;
		displayFrame = EAST_STAND;
	}
}

void Person::stun(uint8_t frames, bool shouldFall)
{
	animationFrame = frames;
	state = shouldFall ? Person::Fallen : Person::Stunned;

	if (shouldFall)
	{
		displayFrame = pgm_read_byte(&directionToFallFrame[direction]);
	}
}

bool Person::isOnScreen(int margin)
{
	int displayX = x - engine.camera.x;
	int displayY = y - engine.camera.y;

	return displayX >= -4 - margin && displayY >= -margin && displayX < DISPLAYWIDTH + 4 + margin && displayY < DISPLAYHEIGHT + 16 + margin;
}

void Person::kickBall(int velocityX, int velocityY, int velocityZ)
{
	if (engine.match.state == Match::ThrowIn)
	{
		velocityZ *= 2;
	}

	engine.ball.setOwner(nullptr);
	engine.ball.velocityX = velocityX;
	engine.ball.velocityY = velocityY;
	engine.ball.velocityZ = velocityZ;

	stun(KICK_RECOVERY_FRAMES);
	animation = pgm_read_byte(&directionToWalkAnimation[direction]);
	displayFrame = pgm_read_byte(&walkAnimations[animation * 4 + 1]);

	engine.match.onKick();

	if (velocityZ > 60)
	{
		Platform.playSound(Sounds::largeKick);
	}
	else
	{
		Platform.playSound(Sounds::kick);
	}
}

void Person::update()
{
	bool movementAllowed = engine.match.shouldAllowFreeMovement();
	uint8_t teamDifficulty = team != REFEREE_TEAM && getTeam()->controllerType == Team::ComputerPlayer ? engine.settings.difficulty : 1;
	uint8_t input = 0;
	uint8_t inputDown = 0;

	if (isSelectedPlayer())
	{
		if (getTeam()->controllerType == Team::LocalPlayer)
		{
			input = Platform.readInput(LOCAL_PLAYER);
			inputDown = Platform.readInputDown(LOCAL_PLAYER);
		}
		else if (getTeam()->controllerType == Team::RemotePlayer)
		{
			input = Platform.readInput(REMOTE_PLAYER);
			inputDown = Platform.readInputDown(REMOTE_PLAYER);
		}

		if (!hasBall())
		{
			bool swapBecauseOffScreen = false;

			if (!isOnScreen(8))
			{
				// Check if any other players on the team are on screen instead
				for (int n = 0; n < NUM_PEOPLE; n++)
				{
					Person& other = engine.people[n];
					if (&other != this && other.team == team && other.isOnScreen() && !other.isGoalie())
					{
						swapBecauseOffScreen = true;
						break;
					}
				}
			}

			if (swapBecauseOffScreen || (inputDown & Input_Btn_A))
			{
				// Swap selected player if pressing A or off screen
				getTeam()->cycleSelectedPlayer();
			}
		}
	}

	bool canControl = state == Person::Standing || state == Person::Walking;

	if (canControl)
	{
		uint8_t inputDirection = pgm_read_byte(&inputToDirection[input & 0xf]);

		// Referee AI - follow ball around
		if (team == REFEREE_TEAM)
		{
			inputDirection = calculateFacingDirection(x, y, engine.ball.x, engine.ball.y);
			int distanceToBall = estimateDistance(x, y, engine.ball.x, engine.ball.y);
			
			if (distanceToBall < 32)
			{
				inputDirection = (inputDirection + 4) & 7;
			}
			else if (distanceToBall < 64)
			{
				inputDirection = NoDirection;
			}
		}
		else 
		{
			// General player AI

			// Celebration behaviour if scoring a goal
			if (engine.match.state == Match::Scored)
			{
				if (getTeam() == engine.match.electedTeam && engine.match.electedKicker)
				{
					movementAllowed = true;

					if (engine.match.electedKicker == this)
					{
						if (getTeam()->isTopHalf())
						{
							inputDirection = calculateFacingDirection(x, y, PITCH_RIGHT, PITCH_TOP);
						}
						else
						{
							inputDirection = calculateFacingDirection(x, y, PITCH_RIGHT, PITCH_BOTTOM);
						}
					}
					else
					{
						if ((engine.frameCount & 3) == (index & 3))	// Only change direction every 4 frames to avoid flip flop behaviour
						{
							inputDirection = calculateFacingDirection(x, y, engine.match.electedKicker->x, engine.match.electedKicker->y);
							inputDirection = getAvoidDirection(inputDirection);
						}
						else
						{
							inputDirection = direction;
						}

						if (estimateDistance(x, y, engine.match.electedKicker->x, engine.match.electedKicker->y) < 32)
						{
							inputDirection = NoDirection;
						}
					}
				}
				else
				{
					inputDirection = NoDirection;
				}
			}
			else
			{
				if (!isSelectedPlayer() && ENABLE_AI_PLAYER)
				{
					bool shouldChangeDirection = (engine.frameCount & 7) == 0;		// Only change direction every few frames to avoid flip flop behaviour

					// Follow formation
					{
						int16_t targetX, targetY;
						getTeam()->calculateFormationPosition(index, targetX, targetY);
						int formationLooseness = 16;

						if (isGoalie())
						{
							if (hasBall())
							{
								inputDirection = NoDirection;
								direction = getTeam()->isTopHalf() ? South : North;
							}
							else
							{
								inputDirection = calculateFacingDirection(x, y, targetX, targetY);
								if (inputDirection == NoDirection)
								{
									direction = calculateFacingDirection(x, y, engine.ball.x, engine.ball.y);
								}
							}
						}
						else
						{
							if (estimateDistance(x, y, targetX, targetY) > formationLooseness)
							{
								if (shouldChangeDirection)
								{
									inputDirection = calculateFacingDirection(x, y, targetX, targetY);
									inputDirection = getAvoidDirection(inputDirection);
								}
								else
								{
									inputDirection = direction;
								}
							}
						}
					}

					if (hasBall())
					{
						if (movementAllowed)
						{
							if (isGoalie())
							{
								uint8_t targetDirection = getTeam()->isTopHalf() ? South : North;
								if (direction == targetDirection && engine.ball.ownerTimer > 32)
								{
									inputDown |= Input_Btn_A;
								}

								if (direction != targetDirection)
								{
									inputDirection = targetDirection;
								}
							}
							else
							{
								// Run towards the goal
								int goalX = BACKGROUND_WIDTH / 2;
								int goalY = getTeam()->isTopHalf() ? PITCH_BOTTOM : PITCH_TOP;

								inputDirection = calculateFacingDirection(x, y, goalX, goalY);

								// Try shoot
								if (estimateDistance(x, y, goalX, goalY) < 48 && direction == inputDirection)
								{
									inputDown |= Input_Btn_B;
								}
								else
								{
									if (shouldChangeDirection)
									{
										// Run down a wing
										if (x > BACKGROUND_WIDTH / 4 && x < 3 * BACKGROUND_WIDTH / 4 && y > BACKGROUND_HEIGHT / 4 && y < 3 * BACKGROUND_HEIGHT / 4)
										{
											if (index & 1)
												inputDirection++;
											else
												inputDirection--;
											inputDirection &= 7;
										}

										inputDirection = getAvoidDirection(inputDirection);
									}
									else
									{
										inputDirection = direction;
									}

									// Pass the ball if we have had it too long
									if (engine.ball.ownerTimer > 60)
									{
										inputDown |= Input_Btn_A;
									}
								}
							}
						}
						else
						{
							if (engine.ball.ownerTimer > 60)
							{
								inputDown |= Input_Btn_A;
							}
						}
					}
					else
					{
						bool shouldChaseBall = engine.match.shouldAllowFreeMovement();

						if (shouldChaseBall && (!engine.ball.owner || !engine.ball.owner->isHoldingBall()))
						{
							int estimatedBallDistance = estimateDistance(x, y, engine.ball.x, engine.ball.y);
							bool otherTeamHasBall = engine.ball.owner && engine.ball.owner->team != team;
							bool isClosestPlayer = getTeam()->getClosestPlayer(engine.ball.x, engine.ball.y) == this;

							if (otherTeamHasBall)
							{
								uint8_t chaseDelay = pgm_read_byte(&difficultySettings[teamDifficulty].chaseBallDelay);

								if (engine.ball.ownerTimer < chaseDelay)
								{
									shouldChaseBall = false;
								}
								else if (!isClosestPlayer)
								{
									uint8_t applyPressureDelay = pgm_read_byte(&difficultySettings[teamDifficulty].applyPressureDelay);
									uint8_t pressureDistance = pgm_read_byte(&difficultySettings[teamDifficulty].pressureDistance);
									if (engine.ball.ownerTimer < applyPressureDelay || estimatedBallDistance > pressureDistance)
									{
										shouldChaseBall = false;
									}
								}
							}
							else
							{
								if (!isClosestPlayer)
								{
									shouldChaseBall = false;
								}
							}

							if (shouldChaseBall)
							{
								if (shouldChangeDirection)
								{
									inputDirection = calculateFacingDirection(x, y, engine.ball.x, engine.ball.y);
								}
								else
								{
									inputDirection = direction;
								}

								if (otherTeamHasBall)
								{
									uint8_t tackleDelay = pgm_read_byte(&difficultySettings[engine.settings.difficulty].tackleDelay);
									bool autoSlideTackle = engine.ball.ownerTimer > tackleDelay;

									if (estimatedBallDistance < 10 && (autoSlideTackle || isGoalie()))
									{
										// Slide tackle opponent
										inputDown |= Input_Btn_B;
									}
								}
							}
						}
					}
				}
			}
		}

		if (inputDirection != NoDirection && movementAllowed)
		{
			state = Person::Walking;

			// Slow turn if dribbling the ball
			if (hasBall())
			{
				int directionDelta = inputDirection - direction;
				if (directionDelta > 4 || directionDelta < -4)
				{
					if (inputDirection < direction)
					{
						if (direction == NorthWest)
							direction = North;
						else direction++;
					}
					else if (inputDirection > direction)
					{
						if (direction == North)
							direction = NorthWest;
						else direction--;
					}
				}
				else
				{
					if (inputDirection < direction)
						direction--;
					else if (inputDirection > direction)
						direction++;
				}
			}
			else
			{
				direction = inputDirection;
			}
		}
		else
		{
			state = Person::Standing;

			if (!isSelectedPlayer() && !hasBall())
			{
				// Face the ball
				//direction = calculateFacingDirection(x, y, engine.ball.x, engine.ball.y);
			}

			// Check if the goalie needs to dive
			if (isGoalie() && engine.ball.owner == nullptr)
			{
				goalieDive();
			}
		}

		if (engine.match.shouldAllowKicking())
		{
			if (inputDown & Input_Btn_A)
			{
				if (hasBall())
				{
					// Pass
					tryPass(inputDirection != NoDirection ? inputDirection : direction);
					//kickBall(deltaX * 50, deltaY * 50, 50);
					return;
				}
			}
			if (inputDown & Input_Btn_B)
			{
				if (hasBall())
				{
					// Shoot
					tryShoot();
					return;
				}
				else
				{
					// Slide tackle
					animationFrame = 0;
					state = Person::SlideTackle;
					displayFrame = pgm_read_byte(&directionToSlideTackleFrame[direction]);
					Platform.playSound(Sounds::slide);
				}
			}
		}
	}

	if (state == Person::Stunned || state == Person::Fallen)
	{
		if (animationFrame == 0)
		{
			if (!isColliding())
			{
				state = Person::Standing;
			}
		}
		else animationFrame--;
		return;
	}

	switch (state)
	{
	case Person::Standing:
		animationFrame = 0;
		break;
	case Person::Walking:
		//if ((engine.frameCount & 1) == 0)
		{
			int8_t deltaX, deltaY;
			getDirectionOffset(direction, deltaX, deltaY);

			if (tryMove(deltaX, deltaY))
			{
				if ((engine.frameCount & 3) == 0)
				{
					animationFrame++;
					if (animationFrame == 4)
					{
						animationFrame = 0;
					}
				}
			}
		}
		break;
	case Person::DiveLeft:
		{
			if (animationFrame < GOALIE_DIVE_FRAMES / 2)
			{
				displayFrame = direction == South ? SOUTH_DIVE_LEFT1 : NORTH_DIVE_LEFT1;
				tryMove(-2, 0);
			}
			else if(animationFrame < GOALIE_DIVE_FRAMES)
			{
				displayFrame = direction == South ? SOUTH_DIVE_LEFT2 : NORTH_DIVE_LEFT2;
				tryMove(-1, 0);
			}
			else
			{
				stun(0);
				z = 0;
				return;
			}
			
			z = pgm_read_byte(&goalieDiveZ[animationFrame]);
			animationFrame++;
		}
		break;
	case Person::DiveRight:
		{
			if (animationFrame < GOALIE_DIVE_FRAMES / 2)
			{
				displayFrame = direction == South ? SOUTH_DIVE_RIGHT1 : NORTH_DIVE_RIGHT1;
				tryMove(2, 0);
			}
			else if(animationFrame < GOALIE_DIVE_FRAMES)
			{
				displayFrame = direction == South ? SOUTH_DIVE_RIGHT2 : NORTH_DIVE_RIGHT2;
				tryMove(1, 0);
			}
			else
			{
				stun(0);
				z = 0;
				return;
			}
			
			z = pgm_read_byte(&goalieDiveZ[animationFrame]);
			animationFrame++;
		}
		break;
	case Person::SlideTackle:
		{
			int8_t deltaX, deltaY;
			getDirectionOffset(direction, deltaX, deltaY);
			if (animationFrame < SLIDE_TACKLE_FRAMES / 3)
			{
				tryMove(deltaX, deltaY);
			}
			if (animationFrame < 2 * SLIDE_TACKLE_FRAMES / 3)
			{
				tryMove(deltaX, deltaY);
			}

			// Check if we are fouling anyone
			if (!hasBall())
			{
				for (int n = 0; n < PLAYERS_PER_TEAM * 2; n++)
				{
					Person& other = engine.people[n];
					if (other.team != team && !other.hasBall() && other.state != Person::Stunned && other.state != Person::Fallen)
					{
						int diffX = other.x - x;
						int diffY = other.y - y;

						if (diffX >= -TACKLE_DISTANCE && diffX <= TACKLE_DISTANCE && diffY >= -TACKLE_DISTANCE && diffY <= TACKLE_DISTANCE)
						{
							other.stun(SLIDE_TACKLE_RECOVERY_FRAMES, true);
						}
					}
				}
			}

			animationFrame++;
			if (animationFrame >= SLIDE_TACKLE_FRAMES || (hasBall() && engine.ball.ownerTimer > SLIDE_TACKLE_FRAMES / 2))
			{
				state = Person::Standing;
				animationFrame = 0;
			}
		}
		break;
	}

	// Animate standing / walking
	if (state == Person::Standing || state == Person::Walking)
	{
		animation = pgm_read_byte(&directionToWalkAnimation[direction]);
		displayFrame = pgm_read_byte(&walkAnimations[animation * 4 + animationFrame]);
	}

	// Check for tackling / gaining control of the ball
	if (team != REFEREE_TEAM && engine.ball.z < PERSON_HEIGHT && engine.match.shouldAllowFreeMovement())
	{
		int diffX = engine.ball.x - x;
		int diffY = engine.ball.y - y;

		if (engine.ball.owner == NULL && diffX >= -GET_BALL_DISTANCE && diffX <= GET_BALL_DISTANCE && diffY >= -GET_BALL_DISTANCE && diffY <= GET_BALL_DISTANCE)
		{
			takeBall();
		}
		else if (engine.ball.owner && engine.ball.owner->team != team && diffX >= -TACKLE_DISTANCE && diffX <= TACKLE_DISTANCE && diffY >= -TACKLE_DISTANCE && diffY <= TACKLE_DISTANCE)
		{
#if 1
			if (state == Person::SlideTackle && !engine.ball.owner->isHoldingBall())
			{
				engine.ball.owner->stun(SLIDE_TACKLE_RECOVERY_FRAMES, true);
				takeBall();
			}
#else
			if (state == Person::SlideTackle)
			{
				engine.ball.owner->stun(SLIDE_TACKLE_RECOVERY_FRAMES, true);
  			}
			else
			{
				engine.ball.owner->stun(TACKLE_RECOVERY_FRAMES);
			}
			takeBall();
#endif
		}
	}

	if (hasBall())
	{
		if (getTeam()->controllerType != Team::ComputerPlayer)
		{
			getTeam()->selectedPlayer = this;
		}

		if (engine.match.state == Match::ThrowIn)
		{
			engine.ball.setPosition(engine.ball.x, engine.ball.y, 6);
			if (engine.ball.x < BACKGROUND_WIDTH / 2)
			{
				direction = Direction::East;
			}
			else
			{
				direction = Direction::West;
			}
		}

		if (engine.match.shouldAllowFreeMovement())
		{
			int8_t deltaX, deltaY;
			getDirectionOffset(direction, deltaX, deltaY);

			if (state == Person::DiveRight)
			{
				// Saving during a dive
				engine.ball.setPosition(x + 2, y + deltaY, z + 3);
			}
			else if (state == Person::DiveLeft)
			{
				// Saving during a dive
				engine.ball.setPosition(x - 2, y + deltaY, z + 3);
			}
			else
			{
				// Dribbling the ball
				int ballZ = engine.ball.z;

				if (isHoldingBall())
				{
					// Goalie Holding ball in hands
					ballZ = z + 4;
					deltaX *= 5;
					deltaY *= 2;
					engine.ball.velocityZ = 0;
				}
				else
				{
					// Dribbling on floor
					deltaX *= 6;
					deltaY *= 4;

					if (deltaX == 0 && deltaY < 0)
					{
						if (x < BACKGROUND_WIDTH / 2)
						{
							deltaX = 5;
						}
						else
						{
							deltaX = -5;
						}
					}
				}

				if (ballDeltaX < deltaX)
					ballDeltaX++;
				else if (ballDeltaX > deltaX)
					ballDeltaX--;
				if (ballDeltaY < deltaY)
					ballDeltaY++;
				else if (ballDeltaY > deltaY)
					ballDeltaY--;

				engine.ball.setPosition(x + ballDeltaX, y + ballDeltaY, ballZ);
			}
		}
	}
}

void Person::getDirectionOffset(uint8_t direction, int8_t& dx, int8_t& dy)
{
	dx = ((int8_t)pgm_read_byte(&directionToX[direction]));
	dy = ((int8_t)pgm_read_byte(&directionToY[direction]));
}

bool Person::tryMove(int deltaX, int deltaY)
{
	bool moved = false;
	bool isRestrictedToPenaltyBox = isHoldingBall();

	if (deltaX != 0)
	{
		x += deltaX;
		if (isColliding() || (isRestrictedToPenaltyBox && !isInOwnPenaltyBox()))
		{
			x -= deltaX;
		}
		else moved = true;
	}
	if (deltaY != 0)
	{
		y += deltaY;
		if (isColliding() || (isRestrictedToPenaltyBox && !isInOwnPenaltyBox()))
		{
			y -= deltaY;
		}
		else moved = true;
	}

	return moved;
}

bool Person::isColliding()
{
	if (x < PERSON_HALF_WIDTH)
		return true;
	if (y < 16)
		return true;
	if (x >= BACKGROUND_WIDTH - PERSON_HALF_WIDTH)
		return true;
	if (y >= BACKGROUND_HEIGHT)
		return true;

	// Left post
	if (x >= LEFT_POST_X1 && x <= LEFT_POST_X2)
	{
		// Top goal
		if (y >= TOP_GOAL_POST_Y1 && y <= TOP_GOAL_POST_Y2)
		{
			return true;
		}
		// Bottom goal
		if (y >= BOTTOM_GOAL_POST_Y1 && y <= BOTTOM_GOAL_POST_Y2)
		{
			return true;
		}
	}

	// Right post
	if (x >= RIGHT_POST_X1 && x <= RIGHT_POST_X2)
	{
		// Top goal
		if (y >= TOP_GOAL_POST_Y1 && y <= TOP_GOAL_POST_Y2)
		{
			return true;
		}
		// Bottom goal
		if (y >= BOTTOM_GOAL_POST_Y1 && y <= BOTTOM_GOAL_POST_Y2)
		{
			return true;
		}
	}

	if (x >= GOAL_NET_X1 && x <= GOAL_NET_X2 && y >= TOP_GOAL_NET_Y1 && y <= TOP_GOAL_NET_Y2)
		return true;

	if (x >= GOAL_NET_X1 && x <= GOAL_NET_X2 && y >= BOTTOM_GOAL_NET_Y1 && y <= BOTTOM_GOAL_NET_Y2)
		return true;

	// Person can collide with another person if one of them has the ball
	/*for (int n = 0; n < NUM_PEOPLE; n++)
	{
		Person& other = engine.people[n];
		if (&other != this && other.state != Person::Fallen)
		{
			if (hasBall() || other.hasBall())
			{
				if (y >= other.y - 1 && y <= other.y + 1 && x >= other.x - PERSON_HALF_WIDTH && x <= other.x + PERSON_HALF_WIDTH)
				{
					return true;
				}
			}
		}
	}*/

	return false;
}

void Person::goalieDive()
{
	int goalLine;

	if (state == Person::DiveLeft || state == Person::DiveRight)
	{
		// Already diving
		return;
	}

	if (getTeam()->isTopHalf())
	{
		direction = South;
		goalLine = (PITCH_TOP << FIXED_SHIFT);

		if (engine.ball.y > PITCH_TOP + 32 || engine.ball.velocityY > -16)
		{
			// No need to dive
			return;
		}
	}
	else
	{
		direction = North;
		goalLine = (PITCH_BOTTOM << FIXED_SHIFT);

		if (engine.ball.y < PITCH_BOTTOM - 32 || engine.ball.velocityY < 16)
		{
			// No need to dive
			return;
		}
	}

	int time = (goalLine - engine.ball.fixedY) / engine.ball.velocityY;
	int predictedX = engine.ball.x + ((time * engine.ball.velocityX) >> FIXED_SHIFT);

	if (predictedX < LEFT_POST_X1 - PERSON_HALF_WIDTH || predictedX > RIGHT_POST_X2 + PERSON_HALF_WIDTH)
	{
		// No need to dive
		return;
	}

	if (predictedX < x - PERSON_HALF_WIDTH)
	{
		state = Person::DiveLeft;
	}
	else if (predictedX > x + PERSON_HALF_WIDTH)
	{
		state = Person::DiveRight;
	}
	else if (predictedX < x)
	{
		direction = West;
		state = Person::Walking;
	}
	else if (predictedX > x)
	{
		direction = East;
		state = Person::Walking;
	}

	animationFrame = 0;
}

void Person::tryShoot()
{
	int goalX = BACKGROUND_WIDTH / 2;
	int goalY = 0;

	if (getTeam()->isTopHalf())
	{
		goalY = PITCH_BOTTOM;
	}
	else
	{
		goalY = PITCH_TOP;
	}

	// Check if we are facing the right direction
	uint8_t goalDirection = calculateFacingDirection(x, y, goalX, goalY);

	if (direction == goalDirection || 1)
	{
		// Just kick in the facing direction
		int8_t deltaX = ((int8_t)pgm_read_byte(&directionToX[direction]));
		int8_t deltaY = ((int8_t)pgm_read_byte(&directionToY[direction]));
		kickBall(deltaX * 60, deltaY * 60, 75);
	}
	else
	{
		// Aim toward the goal;
		int dirX = goalX - x;
		int dirY = goalY - y;
		int16_t magnitude = estimateMagnitude(dirX, dirY);

		const int MASS_PASS_MAGNITUDE = 128;

		while (magnitude > MASS_PASS_MAGNITUDE)
		{
			magnitude >>= 1;
			dirX /= 2;
			dirY /= 2;
		}

		int16_t dirZ = magnitude >> 1;

		if (dirZ < 30)
		{
			dirZ = 30;
		}

		kickBall(dirX, dirY, dirZ);
	}
}

void Person::tryPass(uint8_t passDirection)
{
	Team* team = getTeam();
	Person* target = nullptr;
	int targetDistance = -1;

	for (int pass = 0; pass < 3; pass++)
	{
		for (int n = 1; n < PLAYERS_PER_TEAM; n++)
		{
			Person* other = &team->players[n];
			if (other != this)
			{
				if (pass == 0)
				{
					// Check for the correct direction
					uint8_t targetDirection = calculateFacingDirection(x, y, other->x, other->y);
					if (passDirection != targetDirection)
					{
						continue;
					}
				}
				if (pass == 1)
				{
					// Check for the almost the correct direction
					uint8_t targetDirection = calculateFacingDirection(x, y, other->x, other->y);
					int diff = targetDirection - passDirection;
					if (diff < 0)
						diff = -diff;
					if (diff > 4)
						diff = 8 - diff;

					if (diff > 2)
					{
						continue;
					}
				}

				int distance = estimateDistance(x, y, other->x, other->y);

				if (!target || distance < targetDistance)
				{
					target = other;
					targetDistance = distance;
				}
			}
		}

		if (target)
			break;
	}

	int16_t dirX = target->x - x;
	int16_t dirY = target->y - y;
	int16_t magnitude = estimateMagnitude(dirX, dirY);

	const int MIN_PASS_MAGNITUDE = 32;
	const int MAX_PASS_MAGNITUDE = 128;

	while (magnitude > MAX_PASS_MAGNITUDE)
	{
		magnitude >>= 1;
		dirX /= 2;
		dirY /= 2;
	}

	while (magnitude && magnitude < MIN_PASS_MAGNITUDE)
	{
		magnitude <<= 1;
		dirX *= 2;
		dirY *= 2;
	}
	
	int16_t dirZ = magnitude >> 1;

	if (dirZ < 30)
	{
		dirZ = 30;
	}
	//dirX *= 2;
	//dirY *= 2;

	kickBall(dirX, dirY, dirZ);
}

Team* Person::getTeam()
{
	if (team == REFEREE_TEAM)
		return nullptr;
	return &engine.teams[team];
}

void Person::takeBall()
{
	int diffX = engine.ball.x - x;
	int diffY = engine.ball.y - y;
	engine.ball.setOwner(this);
	ballDeltaX = diffX;
	ballDeltaY = diffY;
}

bool Person::isInOwnPenaltyBox()
{
	if (getTeam()->isTopHalf())
	{
		return x > (BACKGROUND_WIDTH / 2) - (PENALTY_BOX_WIDTH / 2)
			&& x < (BACKGROUND_WIDTH / 2) + (PENALTY_BOX_WIDTH / 2)
			&& y > PITCH_TOP && y < PITCH_TOP + PENALTY_BOX_HEIGHT;
	}
	else
	{
		return x > (BACKGROUND_WIDTH / 2) - (PENALTY_BOX_WIDTH / 2)
			&& x < (BACKGROUND_WIDTH / 2) + (PENALTY_BOX_WIDTH / 2)
			&& y < PITCH_BOTTOM && y > PITCH_BOTTOM - PENALTY_BOX_HEIGHT;
	}
}

bool Person::hasBall()
{
	return engine.ball.owner == this;
}

bool Person::isSelectedPlayer()
{
	return team != REFEREE_TEAM && getTeam()->selectedPlayer == this;
}

uint8_t Person::getAvoidDirection(uint8_t dir)
{
	int8_t offsetX, offsetY;
	int checkX, checkY;

	/*for (int n = 0; n < PLAYERS_PER_TEAM * 2; n++)
	{
		Person& other = engine.people[n];

		if (this != &other)
		{
			if (estimateDistance(x, y, other.x, other.y) < 16)
			{
				uint8_t awayDirection = calculateFacingDirection(other.x, other.y, x, y);
				getDirectionOffset(awayDirection, offsetX, offsetY);
				checkX = x + offsetX * 16;
				checkY = y + offsetY * 16;

				if (checkX > PITCH_LEFT + 32 && checkX < PITCH_RIGHT - 32 && checkY > PITCH_TOP + 32 && checkY < PITCH_BOTTOM - 32)
				{
					return awayDirection;
				}
			}
		}
	}
	*/
	bool needAvoid = false;

	getDirectionOffset(dir, offsetX, offsetY);
	checkX = x + offsetX * 16;
	checkY = y + offsetY * 16;

	for (int n = 0; n < PLAYERS_PER_TEAM * 2; n++)
	{
		Person& other = engine.people[n];

		if (this != &other)
		{
			if (estimateDistance(checkX, checkY, other.x, other.y) < 16)
			{
				needAvoid = true;
				break;
			}
		}
	}

	if (!needAvoid)
	{
		return dir;
	}

	if (index & 1)
	{
		dir = (dir + 1) & 7;
	}
	else
	{
		dir = (dir - 1) & 7;
	}

	return dir;
}
