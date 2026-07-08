#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <stdint.h>

#include "Defines.h"

#define INPUT_BIT(x) (1 << (x))

enum
{
	Input_Dpad_Up		= INPUT_BIT(0),
	Input_Dpad_Right	= INPUT_BIT(1),
	Input_Dpad_Down		= INPUT_BIT(2),
	Input_Dpad_Left		= INPUT_BIT(3),
	Input_Btn_A			= INPUT_BIT(4),
	Input_Btn_B			= INPUT_BIT(5),
	Input_Btn_C			= INPUT_BIT(6),
};

class PlatformBase
{
public:
	uint8_t readInput(uint8_t playerId = 0)		{ return inputState[playerId]; }
	uint8_t readInputDown(uint8_t playerId = 0) { return inputState[playerId] & (~lastInputState[playerId]); }
	uint8_t readInputUp(uint8_t playerId = 0)	{ return lastInputState[playerId] & (~inputState[playerId]); }

	bool isMuted() { return m_isMuted; }
	void setMuted(bool muted) { m_isMuted = muted; }

	uint8_t lastInputState[2];
	uint8_t inputState[2];
	bool m_isMuted : 1;
};

#endif
