#ifndef ENGINE_H_
#define ENGINE_H_

#ifdef _WIN32
#include "../Windows/SDLPlatform.h"
#else
#include "ArduboyPlatform.h"
#endif

#include "Platform.h"
#include "Renderer.h"
#include "Person.h"
#include "Ball.h"
#include "Match.h"
#include "Team.h"
#include "Menu.h"

enum
{
	GameState_Menu,
	GameState_Playing,
};

struct Camera
{
	int x, y;
	int offsetX, offsetY;
};

struct GameSettings
{
	uint8_t matchHalfLength;
	uint8_t difficulty;
};

class Sounds
{
public:
	static const uint16_t kick[];
	static const uint16_t largeKick[];
	static const uint16_t bounce[];
	static const uint16_t slide[];
	static const uint16_t whistle[];
	static const uint16_t goal[];
	static const uint16_t win[];
	static const uint16_t lose[];
};

class Engine
{
public:
	void init();
	void update();
	void draw();

	void returnToMenu();

	void startSinglePlayer();
	void startMultiplayer(bool isHost);
	void startDemo();

	void setCameraFocus(int focusX, int focusY);

	bool isDemo();

	int16_t frameCount;
	uint8_t gameState;

	Renderer renderer;
	Camera camera;
	Menu menu;
	Person people[NUM_PEOPLE];
	Ball ball;
	Match match;
	Team teams[2];
	GameSettings settings;

private:
	void updateCamera();
};

extern Engine engine;

#endif
