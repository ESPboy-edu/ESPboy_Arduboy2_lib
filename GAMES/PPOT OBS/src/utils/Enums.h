#pragma once

enum class GameState : uint8_t {
	SplashScreen_Init,
	SplashScreen,
	Title_Init,
	Title,
	Game_Init,
	Game,
	Score,
};
    
enum class Direction : int8_t {
	Up = -1,
	None = 0,
	Down = 1,
};
     
enum class Motion : uint8_t {
	Slow,
	Fast
};
 
enum class Path : uint8_t {
    Small,
	Medium,
	Large,
};

enum class HitObject : int8_t {
    None,
	LargeAsteroid,
	Enemy,
};

enum class TitleMode : int8_t {
    OBS,
	Scroll_One,
	Scenario,
	Scroll_Two,
	Entrance,
};
