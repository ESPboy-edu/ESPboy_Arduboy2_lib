#ifndef RENDERER_H_
#define RENDERER_H_

#include "Defines.h"

enum
{
	BALL_DRAWABLE = NUM_PEOPLE,
	BALL_SHADOW_DRAWABLE,
	UPPER_GOAL_DRAWABLE,
	LOWER_GOAL_DRAWABLE,
	NUM_DRAWABLES
};

struct Font
{
	const uint8_t* fontData;
	const uint8_t width, height, spacing, spaceWidth;
};

class Renderer
{
public:
	Renderer();

	void draw();

	void drawPerson(int index);
	void drawBall();
	void drawBallShadow();
	void drawLowerGoal();
	void drawUpperGoal();
	void drawOffScreenArrow(class Team* team);

	void showLargeMessage(const char* message);
	bool isShowingLargeMessage() { return largeMessageCounter > 0; }

	void drawText(const Font& font, const char* text, int16_t x, int16_t y, uint8_t colour, bool isRAMString = false);

	uint8_t drawOrder[NUM_DRAWABLES];

	int getSortPosition(uint8_t drawable);

private:
	const char* largeMessage;
	int largeMessageCounter;
	uint8_t largeMessageX;
};

extern const Font smallFont PROGMEM;
extern const Font largeFont PROGMEM;


#endif
