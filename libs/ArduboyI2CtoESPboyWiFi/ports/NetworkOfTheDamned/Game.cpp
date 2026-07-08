#include "Defines.h"
#include "Game.h"
#include "FixedMath.h"
#include "Draw.h"
#include "Map.h"
#include "Projectile.h"
#include "Particle.h"
#include "MapGenerator.h"
#include "Platform.h"
#include "Entity.h"
#include "Enemy.h"
#include "Menu.h"
#include "Font.h"

Player Game::players[2];
const char* Game::displayMessage = nullptr;
uint8_t Game::displayMessageTime = 0;
Game::State Game::state = Game::State::Menu;
uint8_t Game::floor = 1;
uint8_t Game::globalTickFrame = 0;
Stats Game::stats;
Menu Game::menu;
uint8_t Game::localPlayerId = 0;

void Game::Init()
{
	state = State::Menu;

	menu.Init();
	ParticleSystemManager::Init();
	ProjectileManager::Init();
	EnemyManager::Init();

}

void Game::StartGame()
{
  	PlatformNet::Init();
	globalTickFrame = 0;
	SeedRandom(0);
	floor = 1;
	stats.Reset();
	players[0].Init();
	players[1].Init();
	SwitchState(State::EnteringLevel);
}

void Game::SwitchState(State newState)
{
	if(state != newState)
	{
		state = newState;
		menu.ResetTimer();
	}
}

void Game::ShowMessage(const char* message)
{
	constexpr uint8_t messageDisplayTime = 90;

	displayMessage = message;
	displayMessageTime = messageDisplayTime;
}

void Game::NextLevel()
{
	if (floor == 10)
	{
		GameOver();
	}
	else
	{
		floor++;
		SwitchState(State::EnteringLevel);
	}
}

void Game::Respawn()
{

}

void Game::StartLevel()
{
	ParticleSystemManager::Init();
	ProjectileManager::Init();
	EnemyManager::Init();
	MapGenerator::Generate();
	EnemyManager::SpawnEnemies();

	players[0].NextLevel();
	players[1].NextLevel();

	Platform::ExpectLoadDelay();
	SwitchState(State::InGame);
}

void Game::Draw()
{
	switch(state)
	{
		case State::Menu:
			menu.Draw();
			break;
		case State::EnteringLevel:
			menu.DrawEnteringLevel();
			break;
		case State::InGame:
		{
			Player& player = GetLocalPlayer();
			Renderer::camera.x = player.x;
			Renderer::camera.y = player.y;
			Renderer::camera.angle = player.angle;

			Renderer::Render();
		}
			break;
		case State::GameOver:
			menu.DrawGameOver();
			break;
		case State::FadeOut:
			menu.FadeOut();
			break;
	}
}

void Game::TickInGame()
{
	uint8_t localInput = Platform::GetInput();
	uint8_t remoteInput;
	if (localPlayerId == 1)
	{
		PlatformNet::RunController(localInput, remoteInput);
	}
	else
	{
		PlatformNet::RunTarget(localInput, remoteInput);
	}

	if (displayMessageTime > 0)
	{
		displayMessageTime--;
		if (displayMessageTime == 0)
			displayMessage = nullptr;
	}

	ProjectileManager::Update();
	ParticleSystemManager::Update();
	EnemyManager::Update();

	for (uint8_t n = 0; n < 2; n++)
	{
		Player& player = players[n];
		player.Tick(localPlayerId == n ? localInput : remoteInput);

		if (player.hp == 0)
		{
			GameOver();
			break;
		}

		if (Map::GetCell(player.x / CELL_SIZE, player.y / CELL_SIZE) == CellType::Exit)
		{
			NextLevel();
			break;
		}
	}
}

void Game::Tick()
{
	switch(state)
	{
		case State::InGame:
		case State::FadeOut:
			TickInGame();
			break;
		case State::EnteringLevel:
			menu.TickEnteringLevel();
			break;
		case State::Menu:
			menu.Tick();
			break;
		case State::GameOver:
			menu.TickGameOver();
			break;
	}

	globalTickFrame++;
}

void Game::GameOver()
{
	SwitchState(State::FadeOut);
}

Player& Game::GetLocalPlayer()
{
	return players[localPlayerId];
}

Player& Game::GetRemotePlayer()
{
	return players[!localPlayerId];
}

void Stats::Reset()
{
	killedBy = EnemyType::None;
	chestsOpened = 0;
	coinsCollected = 0;
	crownsCollected = 0;
	scrollsCollected = 0;

	for (uint8_t& killCounter : enemyKills)
	{
		killCounter = 0;
	}
}

