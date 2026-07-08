#include "Engine.h"
#include "Ball.h"

#define ITOFIX(x) ((x) << FIXED_SHIFT)

void Ball::update()
{
	// If nobody is controlling the ball, let physics control it
	if (!owner)
	{
		bool wasInsideTopNet = isInsideTopNet();
		bool wasInsideBottomNet = isInsideBottomNet();

		fixedZ += velocityZ;
		if (wasInsideTopNet != isInsideTopNet() || wasInsideBottomNet != isInsideBottomNet())
		{
			fixedZ -= velocityZ;
			velocityZ = 0;
		}

		fixedY += velocityY;
		if (isColliding())
		{
			fixedY -= velocityY;
			velocityY = -velocityY / 2;
			Platform.playSound(Sounds::bounce);
		}
		if (wasInsideTopNet != isInsideTopNet())
		{
			if ((!wasInsideTopNet && velocityY > 0) || (wasInsideTopNet && velocityY < 0))
			{
				fixedY -= velocityY;
				velocityY = 0;
				wasInsideTopNet = !wasInsideTopNet;
			}
		}
		if (wasInsideBottomNet != isInsideBottomNet())
		{
			if ((!wasInsideBottomNet && velocityY < 0) || (wasInsideBottomNet && velocityY > 0))
			{
				fixedY -= velocityY;
				velocityY = 0;
				wasInsideBottomNet = !wasInsideBottomNet;
			}
		}

		fixedX += velocityX;
		if (isColliding())
		{
			fixedX -= velocityX;
			velocityX = -velocityX / 2;
			Platform.playSound(Sounds::bounce);
		}
		if (wasInsideTopNet != isInsideTopNet() || wasInsideBottomNet != isInsideBottomNet())
		{
			fixedX -= velocityX;
			velocityX = 0;
		}

		x = fixedX >> FIXED_SHIFT;
		y = fixedY >> FIXED_SHIFT;

		if (fixedZ == 0)
		{
			if (velocityX < -BALL_FRICTION)
				velocityX += BALL_FRICTION;
			else if (velocityX > BALL_FRICTION)
				velocityX -= BALL_FRICTION;
			else
				velocityX = 0;

			if (velocityY < -BALL_FRICTION)
				velocityY += BALL_FRICTION;
			else if (velocityY > BALL_FRICTION)
				velocityY -= BALL_FRICTION;
			else
				velocityY = 0;
		}
	}
	else
	{
		// Still apply Z physics if has a ball owner
		fixedZ += velocityZ;

		ownerTimer++;
	}

	if (fixedZ < 0)
	{
		// Bounce off the ground
		fixedZ = 0;
		if (velocityZ < -(2 << FIXED_SHIFT))
		{
			velocityZ = -velocityZ / 2;

			if (velocityZ > 20 && engine.match.state == Match::Playing)
			{
				Platform.playSound(Sounds::bounce);
			}
		}
		else
		{
			velocityZ = 0;
		}
	}
	else if (fixedZ > 0)
	{
		velocityZ -= BALL_GRAVITY;
	}

	z = fixedZ >> FIXED_SHIFT;

	// Clamp to play field
	if (x < 0)
	{
		x = 0;
		fixedX = x << FIXED_SHIFT;
		if (velocityX < 0)
			velocityX = -(velocityX >> 1);
	}
	if (x >= BACKGROUND_WIDTH)
	{
		x = BACKGROUND_WIDTH - 1;
		fixedX = x << FIXED_SHIFT;
		if (velocityX > 0)
			velocityX = -(velocityX >> 1);
	}
	if (y < 0)
	{
		y = 0;
		fixedY = y << FIXED_SHIFT;
		if (velocityY < 0)
			velocityY = -(velocityY >> 1);
	}
	if (y >= BACKGROUND_HEIGHT)
	{
		y = BACKGROUND_HEIGHT - 1;
		fixedY = y << FIXED_SHIFT;
		if (velocityY > 0)
			velocityY = -(velocityY >> 1);
	}

}

bool Ball::setPosition(int newX, int newY, int newZ)
{
	int16_t oldFixedX = fixedX;
	int16_t oldFixedY = fixedY;
	int16_t oldFixedZ = fixedZ;

	fixedX = newX << FIXED_SHIFT;
	fixedY = newY << FIXED_SHIFT;
	fixedZ = newZ << FIXED_SHIFT;

	if (isColliding())
	{
		fixedX = oldFixedX;
		fixedY = oldFixedY;
		fixedZ = oldFixedZ;
		return false;
	}
	else
	{
		x = newX;
		y = newY;
		z = newZ;
		return true;
	}

}

bool Ball::isColliding()
{
	if (fixedZ > ITOFIX(GOAL_BAR_Z2))
	{
		return false;
	}

	// Left post
	if (fixedX >= ITOFIX(LEFT_POST_X1) && fixedX <= ITOFIX(LEFT_POST_X2))
	{
		// Top goal
		if (fixedY >= ITOFIX(TOP_GOAL_POST_Y1) && fixedY <= ITOFIX(TOP_GOAL_POST_Y2))
		{
			return true;
		}
		// Bottom goal
		if (fixedY >= ITOFIX(BOTTOM_GOAL_POST_Y1) && fixedY <= ITOFIX(BOTTOM_GOAL_POST_Y2))
		{
			return true;
		}
	}

	// Right post
	if (fixedX >= ITOFIX(RIGHT_POST_X1) && fixedX <= ITOFIX(RIGHT_POST_X2))
	{
		// Top goal
		if (fixedY >= ITOFIX(TOP_GOAL_POST_Y1) && fixedY <= ITOFIX(TOP_GOAL_POST_Y2))
		{
			return true;
		}
		// Bottom goal
		if (fixedY >= ITOFIX(BOTTOM_GOAL_POST_Y1) && fixedY <= ITOFIX(BOTTOM_GOAL_POST_Y2))
		{
			return true;
		}
	}

	// Bar
	if (fixedX >= ITOFIX(LEFT_POST_X1) && fixedX <= ITOFIX(RIGHT_POST_X2))
	{
		if (fixedZ >= ITOFIX(GOAL_BAR_Z1))
		{
			// Top goal
			if (fixedY >= ITOFIX(TOP_GOAL_POST_Y1) && fixedY <= ITOFIX(TOP_GOAL_POST_Y2))
			{
				return true;
			}
			// Bottom goal
			if (fixedY >= ITOFIX(BOTTOM_GOAL_POST_Y1) && fixedY <= ITOFIX(BOTTOM_GOAL_POST_Y2))
			{
				return true;
			}
		}
	}

	return false;
}

bool Ball::isInsideTopNet()
{
	return fixedX >= ITOFIX(GOAL_NET_X1) && fixedX <= ITOFIX(GOAL_NET_X2) && fixedY >= ITOFIX(TOP_GOAL_NET_Y1) && fixedY <= ITOFIX(TOP_GOAL_NET_Y2) && fixedZ <= ITOFIX(GOAL_BAR_Z1);
}

bool Ball::isInsideBottomNet()
{
	return fixedX >= ITOFIX(GOAL_NET_X1) && fixedX <= ITOFIX(GOAL_NET_X2) && fixedY >= ITOFIX(BOTTOM_GOAL_NET_Y1) && fixedY <= ITOFIX(BOTTOM_GOAL_NET_Y2) && fixedZ <= ITOFIX(GOAL_BAR_Z1);
}

void Ball::setOwner(class Person* newOwner)
{
	if (newOwner)
	{
		owner = newOwner;
		lastOwner = newOwner;
	}
	else
	{
		if (owner)
		{
			lastOwner = owner;
			owner = nullptr;
		}
	}

	ownerTimer = 0;
}
