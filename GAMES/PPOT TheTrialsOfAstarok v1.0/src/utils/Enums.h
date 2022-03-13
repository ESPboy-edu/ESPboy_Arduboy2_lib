#pragma once

enum GameState : uint8_t {
	SplashScreen_Init,
	SplashScreen,
	Title_Init,
	Title,
	IntroText_Init,
	IntroText,
	Seed_Init,
	Seed,
	Game_Init,
	Game_Play,
	Game_Mini,
	HighScore_Check,
	HighScore_Flash,
	HighScore_NoFlash,
};

enum TitleScreenMode : uint8_t {
	Play,
	HighScore
};

enum MapLevel : uint8_t {
	BelowGround,
	AboveGround,
};

enum Hearts : uint8_t {
	FilledIn,
	Outline,
};

enum SpriteDataLayout : uint8_t {
	Type,
	Flags,
	Speed,
	Intelligence,
};

enum Direction : uint8_t { Up, Down, Left, Right, None };

enum ObjectTypes : uint8_t {
	Player = 0,  // Sprites
	Spider,
	Starman,
	Skull,
	Bolt,
	Health = 5,
	QBlock,  // Interactive Tiles
	BQBlock,
	MushBlock,
	Bricks = 9,
	AboveGroundExit,
	UnderGroundExit,
	Sign,
	Fireball,
	Coin,
	Chest_Closed,
	Chest_Open,
};

enum EventType : uint8_t {
	Off = 0,
	Playing,
	Death_Init,
	Death,
	Flash,
	LevelExit,
	StartLevel
};