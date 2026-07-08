#include "Engine.h"
#include "MathsFunctions.h"
#include "ArduboyTonesPitches.h"
//#include "Generated/Sounds.inc.h"

const uint16_t Sounds::kick[] PROGMEM = {
	0xb4,20,
	0x8f,20,
	0x22,20,
	0x8000
};

const uint16_t Sounds::largeKick[] PROGMEM = {
	0xa4,40,
	0x7f,40,
	0x12,40,
	0x8000
};

const uint16_t Sounds::bounce[] PROGMEM = {
	0x94,10,
	0x5f,10,
	0x12,10,
	0x8000
};

const uint16_t Sounds::slide[] PROGMEM = {
	0x100,	25,
	0x5f,	25,
	0x90,	25,
	0x5f,	25,
	0x80,	25,
	0x5f,	25,
	0x70,	25,
	0x5f,	25,
	0x60,	25,
	0x8000
};

const uint16_t Sounds::whistle[] PROGMEM = {
	2500,10,
	2650,10,
	2500,10,
	2650,10,
	2500,10,
	2650,10,
	2500,10,
	2650,10,
	2500,10,
	2650,10,
	2650,10,
	2500,10,
	2650,10,
	2500,10,
	2650,10,
	0x8000
};

const uint16_t Sounds::goal[] PROGMEM = {
  NOTE_C5, 120,   
  NOTE_F5, 120,   
  NOTE_A5, 120,   
  NOTE_C6, 120,   
  NOTE_REST, 30,  
  NOTE_F6, 400,   
  0x8000
};

const uint16_t Sounds::win[] PROGMEM = {
  NOTE_C5, 60, NOTE_C5, 60,
  NOTE_F5, 60, NOTE_F5, 60,
  NOTE_A5, 60, NOTE_A5, 60,
  NOTE_C6, 400,
  0x8000
};

const uint16_t Sounds::lose[] PROGMEM = {
  NOTE_DS4, 180,
  NOTE_D4,  180,
  NOTE_CS4, 180,
  NOTE_C4,  500, 
  0x8000
};


Engine engine;

void Engine::init()
{
	menu.init();
	gameState = GameState_Menu;

	settings.matchHalfLength = 2;
}

void Engine::startMultiplayer(bool isHost)
{
	teams[WHITE_TEAM].init(people, isHost ? Team::LocalPlayer : Team::RemotePlayer);
	teams[BLACK_TEAM].init(people + PLAYERS_PER_TEAM, isHost ? Team::RemotePlayer : Team::LocalPlayer);
	match.reset();
	gameState = GameState_Playing;
	frameCount = 0;
}

void Engine::startSinglePlayer()
{
	teams[WHITE_TEAM].init(people, Team::LocalPlayer); 
	teams[BLACK_TEAM].init(people + PLAYERS_PER_TEAM, Team::ComputerPlayer);
	match.reset();
	gameState = GameState_Playing;
}

void Engine::startDemo()
{
	teams[WHITE_TEAM].init(people, Team::ComputerPlayer);
	teams[BLACK_TEAM].init(people + PLAYERS_PER_TEAM, Team::ComputerPlayer);
	match.reset();
	gameState = GameState_Playing;
}

bool Engine::isDemo()
{
	return teams[WHITE_TEAM].controllerType == Team::ComputerPlayer;
}

void Engine::update()
{
	switch(gameState)
	{
	case GameState_Playing:
		{
			ball.update();

			for (uint8_t n = 0; n < NUM_PEOPLE; n++)
			{
				people[n].update();
			}

			match.update();
			teams[WHITE_TEAM].update();
			teams[BLACK_TEAM].update();

			updateCamera();

			// Check for exiting demo mode
			if (isDemo() && (Platform.readInput() & Input_Btn_A))
			{
				gameState = GameState_Menu;
			}
		}
		break;
	case GameState_Menu:
		{
			menu.update();
		}
		break;
	}

	frameCount++;
}

void Engine::updateCamera()
{
	int targetCameraX, targetCameraY;

	targetCameraX = ball.x;
	targetCameraY = ball.y;

	if (match.state == Match::Scored && match.electedKicker && match.timeInState > 30)
	{
		targetCameraX = match.electedKicker->x;
		targetCameraY = match.electedKicker->y;
	}

	int targetCameraOffsetX = 0, targetCameraOffsetY = 0;

	if (!ball.owner)
	{
		targetCameraOffsetX = ball.velocityX >> 2;
		targetCameraOffsetY = ball.velocityY >> 2;

		/*if (targetCameraOffsetX == 0 && targetCameraOffsetY == 0 && 0)
		{
			// Try get the player controlled character in view instead
			Person& person = people[personPlayer1];
			targetCameraOffsetX = (person.x - ball.x) / 2;
			targetCameraOffsetY = (person.y - ball.y) / 2;

			int maxX = DISPLAYWIDTH / 3;
			int maxY = DISPLAYHEIGHT / 3;

			if (targetCameraOffsetX < -maxX)
			{
				targetCameraOffsetX = -maxX;
			}
			else if (targetCameraOffsetX > maxX)
			{
				targetCameraOffsetX = maxX;
			}
			if (targetCameraOffsetY < -maxY)
			{
				targetCameraOffsetY = -maxY;
			}
			else if (targetCameraOffsetY > maxY)
			{
				targetCameraOffsetY = maxY;
			}
		}*/
	}
	else
	{
		if (engine.match.state != Match::KickOff)
		{
			int8_t deltaX, deltaY;
			Person::getDirectionOffset(ball.owner->direction, deltaX, deltaY);
			targetCameraOffsetX = deltaX * 5;
			targetCameraOffsetY = deltaY * 12;
		}
	}

	if (camera.offsetX < targetCameraOffsetX)
		camera.offsetX++;
	else if (camera.offsetX > targetCameraOffsetX)
		camera.offsetX--;
	if (camera.offsetY < targetCameraOffsetY)
		camera.offsetY++;
	else if (camera.offsetY > targetCameraOffsetY)
		camera.offsetY--;

	targetCameraX += camera.offsetX;
	targetCameraY += camera.offsetY;

	targetCameraX -= HALF_DISPLAYWIDTH;
	targetCameraY -= HALF_DISPLAYHEIGHT;

	//camera.x = people[personPlayer1].x - HALF_DISPLAYWIDTH;
	//camera.y = people[personPlayer1].y - HALF_DISPLAYHEIGHT;
	//camera.x = targetCameraX - HALF_DISPLAYWIDTH;
	//camera.y = targetCameraY - HALF_DISPLAYHEIGHT;

	int cameraDeltaX = (targetCameraX - camera.x) >> 1;
	int cameraDeltaY = (targetCameraY - camera.y) >> 1;

	if (cameraDeltaX < -MAX_CAMERA_DELTA)
		cameraDeltaX = -MAX_CAMERA_DELTA;
	if (cameraDeltaX > MAX_CAMERA_DELTA)
		cameraDeltaX = MAX_CAMERA_DELTA;
	if (cameraDeltaY < -MAX_CAMERA_DELTA)
		cameraDeltaY = -MAX_CAMERA_DELTA;
	if (cameraDeltaY > MAX_CAMERA_DELTA)
		cameraDeltaY = MAX_CAMERA_DELTA;

	camera.x += cameraDeltaX;
	camera.y += cameraDeltaY;

	if (camera.x < 0)
		camera.x = 0;
	if (camera.x > BACKGROUND_WIDTH - DISPLAYWIDTH)
		camera.x = BACKGROUND_WIDTH - DISPLAYWIDTH;
	if (camera.y < 0)
		camera.y = 0;
	if (camera.y > BACKGROUND_HEIGHT - DISPLAYHEIGHT)
		camera.y = BACKGROUND_HEIGHT - DISPLAYHEIGHT;

}

void Engine::setCameraFocus(int focusX, int focusY)
{
	camera.x = focusX - HALF_DISPLAYWIDTH;
	camera.y = focusY - HALF_DISPLAYHEIGHT;

	if (camera.x < 0)
		camera.x = 0;
	if (camera.x > BACKGROUND_WIDTH - DISPLAYWIDTH)
		camera.x = BACKGROUND_WIDTH - DISPLAYWIDTH;
	if (camera.y < 0)
		camera.y = 0;
	if (camera.y > BACKGROUND_HEIGHT - DISPLAYHEIGHT)
		camera.y = BACKGROUND_HEIGHT - DISPLAYHEIGHT;
}

void Engine::draw()
{
	if (gameState == GameState_Menu)
	{
		menu.draw();
	}
	else
	{
		renderer.draw();
	}
}

void Engine::returnToMenu()
{
	Platform.disconnectMultiplayer();
	menu.init();
	gameState = GameState_Menu;
}
