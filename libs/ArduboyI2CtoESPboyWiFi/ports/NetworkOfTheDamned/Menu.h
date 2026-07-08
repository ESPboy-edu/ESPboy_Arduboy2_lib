#pragma once

#include <stdint.h>

class Menu
{
public:
	void Init();
	void Draw();
	void Tick();

	void TickEnteringLevel();
	void DrawEnteringLevel();

	void TickGameOver();
	void DrawGameOver();

	void DrawHandshaking(const char *str, uint8_t line, uint8_t x);

	void ResetTimer();

	void FadeOut();

private:
	union
	{
		uint8_t selection;
		uint16_t timer;
		uint16_t fizzleFade;
	};

};
