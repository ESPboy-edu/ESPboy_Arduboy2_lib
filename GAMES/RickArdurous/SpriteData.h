#ifndef _SPRITE_DATA_H_
#define _SPRITE_DATA_H_

namespace SpriteData
{
	enum WallId : unsigned char
	{
		ROCK_CEILING = 0,
		ROCK,
		ROCK_2,
		ROCK_GROUND,
		BLOCK_16_8_RIGHT,
		BLOCK_8_8,
		BLOCK_8_8_SPLIT,
		STAIR,
		ROCK_LEFT_WALL,
		BIG_STATUE_TOP,
		BIG_STATUE_BOTTOM,
		ROCK_CEILING_THIN,
		PLATFORM,
		LADDER,
		PLATFORM_WITH_LADDER,
		NOTHING = 15,
	};
	
	// sprite of the walls
	// the sprite width and height in pixel
	static constexpr int LEVEL_SPRITE_WIDTH = 8;
	static constexpr int LEVEL_SPRITE_HEIGHT = 8;
	extern const unsigned char Walls[][LEVEL_SPRITE_WIDTH * (LEVEL_SPRITE_HEIGHT / 8)];
	
	// destroyable Blocks
	static constexpr int DESTROYABLE_BLOCK_SPRITE_WIDTH = 15;
	static constexpr int DESTROYABLE_BLOCK_SPRITE_HEIGHT = 8;
	extern const unsigned char DestroyableBlock[];
	
	enum RickAnimFrameId : unsigned char
	{
		IDLE = 0,
		WALK_START = 0,
		WALK_END = 3,
		FIRE_START = 4,
		FIRE_END = 5,
		CROUCH_START = 6,
		CROUCH_END = 7,
		STAND_UP_START = 7,
		STAND_UP_END = 6,
		JUMP = 8,
		CLIMB_START = 9,
		CLIMB_END = 11,
		DEATH_START = 12,
		DEATH_END = 14,
		// restart the index because these index are for the RickCrawl array
		CRAWL_START = 0,
		CRAWL_END = 2,
	};
	
	// sprite of the main character
	static constexpr int RICK_SPRITE_WIDTH = 9;
	static constexpr int RICK_SPRITE_HEIGHT = 16;
	extern const unsigned char Rick[][RICK_SPRITE_WIDTH * (RICK_SPRITE_HEIGHT / 8)];
	static constexpr int RICK_CRAWL_SPRITE_WIDTH = 12;
	static constexpr int RICK_CRAWL_SPRITE_HEIGHT = 8;
	extern const unsigned char RickCrawl[][RICK_CRAWL_SPRITE_WIDTH * (RICK_CRAWL_SPRITE_HEIGHT / 8)];
	
	// animation for the dynamite, with the fuse and the explosion
	static constexpr int DYNAMITE_SPRITE_FRAME_COUNT = 11;
	static constexpr int DYNAMITE_SPRITE_WIDTH = 8;
	static constexpr int DYNAMITE_SPRITE_HEIGHT = 8;
	extern const unsigned char Dynamite[][DYNAMITE_SPRITE_WIDTH * (DYNAMITE_SPRITE_HEIGHT / 8)];

	// animation for the sparks when the fuse of the dynamite is consuming. This anim should be drawn in inverted color.
	static constexpr int SPARKS_SPRITE_FRAME_COUNT = 5;
	static constexpr int SPARKS_SPRITE_WIDTH = 5;
	static constexpr int SPARKS_SPRITE_HEIGHT = 8;
	extern const unsigned char Sparks[][SPARKS_SPRITE_WIDTH * (SPARKS_SPRITE_HEIGHT / 8)];

	// animation for the shiny horizontal spikes
	static constexpr int SPIKE_SPRITE_FRAME_COUNT = 6;
	static constexpr int SPIKE_HORIZONTAL_SPRITE_WIDTH = 3;
	static constexpr int SPIKE_HORIZONTAL_SPRITE_HEIGHT = 8;
	extern const unsigned char SpikeHorizontalBottom[][SPIKE_HORIZONTAL_SPRITE_WIDTH * (SPIKE_HORIZONTAL_SPRITE_HEIGHT / 8)];
	extern const unsigned char SpikeHorizontalTop[][SPIKE_HORIZONTAL_SPRITE_WIDTH * (SPIKE_HORIZONTAL_SPRITE_HEIGHT / 8)];

	// animation for the shiny vertical spikes
	static constexpr int SPIKE_VERTICAL_SPRITE_WIDTH = 4;
	static constexpr int SPIKE_VERTICAL_SPRITE_HEIGHT = 8;
	extern const unsigned char SpikeVertical[][SPIKE_VERTICAL_SPRITE_WIDTH * (SPIKE_VERTICAL_SPRITE_HEIGHT / 8)];

	// animation of a shiny star sparks that should be drawn on top of the gold statue. This anim should be drawn in inverted color.
	static constexpr int SHINE_STAR_SPRITE_FRAME_COUNT = 4;
	static constexpr int SHINE_STAR_SPRITE_WIDTH = 5;
	static constexpr int SHINE_STAR_SPRITE_HEIGHT = 8;
	extern const unsigned char ShineStar[][SHINE_STAR_SPRITE_WIDTH * (SHINE_STAR_SPRITE_HEIGHT / 8)];
	
	// arrow launcher
	static constexpr int ARROW_LAUNCHER_FACE_SPRITE_WIDTH = 4;
	static constexpr int ARROW_LAUNCHER_FACE_SPRITE_HEIGHT = 8;
	extern const unsigned char ArrowLauncherFace[];
	
	// statue sprite
	static constexpr int STATUETTE_SPRITE_WIDTH = 5;
	static constexpr int STATUETTE_SPRITE_HEIGHT = 8;
	extern const unsigned char Statuette[];

	// statue sprite
	static constexpr int GRAAL_SPRITE_WIDTH = 5;
	static constexpr int GRAAL_SPRITE_HEIGHT = 8;
	extern const unsigned char Graal[];
	
	// dynamite and bullet bonus sprite
	static constexpr int CRATE_SPRITE_WIDTH = 11;
	static constexpr int CRATE_SPRITE_HEIGHT = 8;
	extern const unsigned char BulletCrate[];
	extern const unsigned char DynamiteCrate[];
	
	// Mummies
	enum EnemyAnimFrameId : unsigned char
	{
		ENEMY_WALK_START = 0,
		ENEMY_WALK_END = 3,
		ENEMY_HALF_TURN,
		ENEMY_WAIT_START,
		ENEMY_WAIT_END = 8,
		ENEMY_FALL,
	};
	static constexpr int MUMMY_SPRITE_WIDTH = 9;
	static constexpr int MUMMY_SPRITE_HEIGHT = 16;
	extern const unsigned char Mummy[][MUMMY_SPRITE_WIDTH * (MUMMY_SPRITE_HEIGHT / 8)];

	// Skeletons
	static constexpr int SKELETON_SPRITE_WIDTH = 6;
	static constexpr int SKELETON_SPRITE_HEIGHT = 16;
	extern const unsigned char Skeleton[][SKELETON_SPRITE_WIDTH * (SKELETON_SPRITE_HEIGHT / 8)];

	// Scorpions
	static constexpr int SCORPION_SPRITE_WIDTH = 8;
	static constexpr int SCORPION_SPRITE_HEIGHT = 8;
	extern const unsigned char Scorpion[][SCORPION_SPRITE_WIDTH * (SCORPION_SPRITE_HEIGHT / 8)];

	// Destroyable block fragment sprite
	static constexpr int BLOCK_FRAGMENT_SPRITE_WIDTH = 5;
	static constexpr int BLOCK_FRAGMENT_SPRITE_HEIGHT = 8;
	extern const unsigned char BlockFragment[];

	// Stalactite and Stalagmite
	static constexpr int STALACTITE_SPRITE_WIDTH = 3;
	static constexpr int STALACTITE_SPRITE_HEIGHT = 8;
	extern const unsigned char Stalactite[];
	static constexpr int STALAGMITE_SPRITE_WIDTH = 3;
	static constexpr int STALAGMITE_SPRITE_HEIGHT = 8;
	extern const unsigned char Stalagmite[];

	// big rolling boulder
	static constexpr int BOULDER_SPRITE_WIDTH = 6;
	static constexpr int BOULDER_SPRITE_HEIGHT = 16;
	extern const unsigned char Boulder[];
	static constexpr int BOULDER_SHADOW_SPRITE_WIDTH = 4;
	static constexpr int BOULDER_SHADOW_SPRITE_HEIGHT = 8;
	extern const unsigned char BoulderShadow[][BOULDER_SHADOW_SPRITE_WIDTH * (BOULDER_SHADOW_SPRITE_HEIGHT / 8)];

	// HUD elements
	static constexpr int HUD_SPRITE_WIDTH = 5;
	static constexpr int HUD_BULLET_SPRITE_WIDTH = 7;
	static constexpr int HUD_SPRITE_HEIGHT = 8;
	extern const unsigned char HUDHeart[];
	extern const unsigned char HUDStatuette[];
	extern const unsigned char HUDDynamite[];
	extern const unsigned char HUDBullet[];

	static constexpr int HUD_SLASH_SPRITE_WIDTH = 3;
	extern const unsigned char HUDSlash[];

	// tiny numbers for HUD
	static constexpr int NUMBERS_SPRITE_WIDTH = 4;
	static constexpr int NUMBERS_SPRITE_HEIGHT = 8;
	extern const unsigned char Numbers[][NUMBERS_SPRITE_WIDTH * (NUMBERS_SPRITE_HEIGHT / 8)];
}

#endif