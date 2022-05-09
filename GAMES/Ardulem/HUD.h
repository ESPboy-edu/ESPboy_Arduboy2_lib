#ifndef HUD_H
#define HUD_H
/*
 * This file contains the code that draw the left HUD bar during gameplay
 */

namespace HUD
{
	// constants
	const uint16_t HUD_WIDTH = 30;

	// WARNING: Do not tune those values, because it won't work with the way the Lem are updated,
	// because the LEM are updated every 10 frame for the normal walk
	const int16_t NORMAL_SPEED_FPS = 60;
	const int16_t FAST_SPEED_FPS = 6;
	const int16_t RATIO_BETWEEN_FAST_AND_NORMAL_FPS = 10;

	enum Button
	{
		TIMER = 0,
		LEM_WALK,
		LEM_BLOCKER,
		LEM_BOMB,
		LEM_DIG_DIAG,
		LEM_DIG_HORIZ,
		LEM_DIG_VERT,
		LEM_STAIR,
		LEM_CLIMB,
		LEM_PARACHUTE,
		DROP_SPEED,
		COUNTER
	};
	
	// the button currently selected in the HUD
	extern Button GetSelectedButton();
	extern bool IsSelectedButtonValid();
	
	// the lem drop framerate currently set in HUD
	extern int16_t GetLemDropFrameRate();

	// tell if the player has accelerate the time of the game
	extern bool IsGameSpeedUp();

	// set the cursor position
	extern unsigned char GetCursorX();
	extern void SetCursorX(unsigned char x);
	
	extern unsigned char GetCursorY();
	extern void SetCursorY(unsigned char y);
	
	// cursor shape (square or cross)
	extern void SetCursorShape(bool isSquared, bool cursorDirection);
	
	extern void Init(int16_t timeInSecond);
	extern void Update();
	extern bool DrawBlinkingText(int16_t x, int16_t y, const __FlashStringHelper * text, bool isBlinking);
	extern int16_t PrintNumber(int16_t x, int16_t y, int16_t number, int16_t numDigits, bool shouldAddZerosInFront, signed char color, bool useTinyFont);
}

#endif
