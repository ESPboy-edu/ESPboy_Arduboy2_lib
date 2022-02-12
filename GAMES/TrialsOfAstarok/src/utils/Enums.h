#pragma once

enum GameState {
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

enum TitleScreenMode {
	Play,
	HighScore
};

enum MapLevel {
	BelowGround,
	AboveGround,
};

enum Hearts {
	FilledIn,
	Outline,
};

enum SpriteDataLayout {
	Type,
	Flags,
	Speed,
	Intelligence,
};

enum Direction { Up, Down, Left, Right, None };

enum ObjectTypes {
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

enum EventType {
    Off = 0,
    Playing,
    Death_Init,
    Death,
    Flash,
    LevelExit,
    StartLevel
};