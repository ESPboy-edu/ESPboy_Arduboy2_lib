#ifndef MAP_DATA_H
#define MAP_DATA_H
/*
 * This file contains the data of all the map. This include the sprites from which the map is build
 * but also the each map definition. 
 */
namespace MapData
{
	// the map sprites are 8x8 pixels
	extern const unsigned char MapSprite[][8];

	struct MapDescription
	{
		// the start position where the lems will be spawned
		unsigned char StartX;
		unsigned char StartY;
		
		// the position of the home, where the lems will be saved
		unsigned char HomeX;
		unsigned char HomeY;
		
		// The time in multiple of 10 seconds
		unsigned char TimeInMultipleOf10s;
		
		// the lem count in the level
		unsigned char AvailableLemCount		: 4; // The number of Lem that will be spawned in the Map
		unsigned char RequiredLemCount		: 4; // The required number of Lem to save, in order to win

		// the minimum drop speed allowed
		unsigned char MinDropSpeed		: 4;
		unsigned char LemWalkCount		: 4;
		
		// Lems button configuration (how many lem per type, can you use)
		uint16_t LemBlockCount		: 4;
		uint16_t LemBombCount		: 4;
		uint16_t LemDigDiagCount	: 4;
		uint16_t LemDigHorizCount	: 4;
		uint16_t LemDigVertCount	: 4;
		uint16_t LemStairCount		: 4;
		uint16_t LemClimbCount		: 4;
		uint16_t LemParaCount		: 4;

		// a localization description of the Map. Each char is represent one column of sprite,
		// each bit indicates if there's a sprite or not. For example, if you want a map of 64x200 pixels
		// you will need 25 chars
		const unsigned char * SpriteLocalization;
		
		// The number of column of sprite that this map has. Basically it's the size of SpriteLocalization
		unsigned char SpriteColumnCount;
		
		// the list of all the id (from 0 to 31) of sprite as the set bit appear in the SpriteLocalization
		// list. Three ids are stored in each int. You can use the Macro ID() to pack the three ids
		// ---
		// Then after that list (but still in the same array), you should add a list of int representing 
		// A large bitfield (spread over several ints in that int array) telling for each sprite in the SpriteIdList
		// if the sprite should be drawn mirrored or not. Each bit correspond to each sprite. If the bit is set,
		// then the sprite is mirrored, if not set, the sprite should be drawn normally.
		// The ints are ordered like in the sprites order, but then inside a int, the bits are ordered from low to high
		const uint16_t * SpriteIdList;
	};
	
	// the instance of all the maps
	extern const MapDescription AllMaps[];
	
	// get the number of map in the all map array
	extern unsigned char GetMapCount();
}
 #endif
