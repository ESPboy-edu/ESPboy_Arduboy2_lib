#include <avr/pgmspace.h>
#include "MapData.h"

// The ID macro is used to pack two local id of the SpriteIdList
#define ID(id1, id2, id3) (id1 | (id2<<5) | (id3<<10))
// The MIRROR macro is uselse in the game code, but is here to help the Editor to parse the data.
// Use it if you plan to use the Editor.
#define MIRROR(bitfield) (bitfield)

/*
 * All the sprite that can be used to create the maps (up to 32, because the sprite ids are stored on 5 bits)
 */
const unsigned char MapData::MapSprite[][8] PROGMEM = {
	{0x3, 0x7, 0xfd, 0xaf, 0x57, 0xfd, 0x7, 0x3, }, {0x00, 0x00, 0xff, 0xaa, 0x55, 0xff, 0x00, 0x00, },
	{0xc0, 0xe0, 0xbf, 0xea, 0xf5, 0xbf, 0xe0, 0xc0, }, {0x00, 0x00, 0xe0, 0xa0, 0x50, 0xf8, 0x00, 0x00, },
	{0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, }, {0x1c, 0x3e, 0x73, 0x6b, 0x33, 0x1b, 0xb, 0xb, },
	{0x3b, 0x7b, 0xdb, 0xfb, 0x7b, 0xdb, 0x7b, 0x3b, }, {0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, },
	{0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, }, {0x55, 0x11, 0x55, 0x44, 0x55, 0x11, 0x55, 0x44, },
	{0x00, 0x00, 0x00, 0x00, 0xff, 0x55, 0x11, 0x44, }, {0x33, 0x3b, 0x3d, 0xff, 0xff, 0x3b, 0x37, 0x37, },
	{0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81, }, {0x80, 0x40, 0x20, 0x10, 0x18, 0x24, 0x42, 0x81, },
	{0x1, 0x2, 0x4, 0x8, 0x18, 0x24, 0x42, 0x81, }, {0x00, 0x00, 0x40, 0x68, 0x64, 0x34, 0x36, 0x3b, },
	{0x80, 0x80, 0xc0, 0xc0, 0xc0, 0x80, 0xc0, 0xc0, }, {0x00, 0x30, 0x78, 0xfc, 0xf4, 0x68, 0x30, 0x00, },
	{0xc, 0x3e, 0x2f, 0xfd, 0xff, 0x3b, 0x1e, 0xc, }, {0x00, 0x60, 0x38, 0xfe, 0xff, 0x3c, 0x70, 0x60, },
	{0x1, 0x3, 0x7, 0xd, 0x7, 0x5, 0xf, 0xb, }, {0xb, 0xb, 0xd, 0xf, 0x1b, 0x1b, 0x3f, 0x6f, },
	{0xfb, 0x6e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, }, {0x00, 0x00, 0x80, 0xc0, 0x40, 0xe0, 0xb8, 0xfe, },
	{0x1f, 0x1b, 0xf, 0x5, 0x7, 0x7, 0xb, 0xf, }, {0x7, 0x5, 0x3, 0x3, 0x3, 0x3, 0x5, 0xf, },
	{0xb, 0x1f, 0x3b, 0x6f, 0x3d, 0x1b, 0x1f, 0xb, }, {0x80, 0xc0, 0xe0, 0xb0, 0xf8, 0xfc, 0x6e, 0xff, },
	{0x80, 0x80, 0xc0, 0xc0, 0xc0, 0x40, 0xc0, 0x80, }, {0xbf, 0xf6, 0xfc, 0xfc, 0xd8, 0xf8, 0xec, 0xfe, },
	{0xff, 0xb6, 0xfe, 0xdc, 0xfc, 0xfc, 0xda, 0xff, }, {0xff, 0xbd, 0xef, 0xff, 0xf7, 0x7f, 0xff, 0xfb, }, 
};

/*
 * The various map localization and id list
 */
const unsigned char TutoLoca[] PROGMEM = { 0xE0, 0xC0, 0x80, 0x80, 0x80, 0xF0, 0x80, 0x80, 0xE0, 0x80, 0x80, 0x80, 0x80, 0x0, 0x40, 0x60, 0xE0, 0x60, 0x60, 0x60, 0xE0, 0x60, 0x40, };
const uint16_t TutoIds[] PROGMEM = {
	ID(0,2,31), ID(2,31,30), ID(31,31,0), ID(1,2,31), ID(29,31,3), ID(2,31,31), ID(30,31,27), ID(5,27,4), ID(31,6,1), ID(31,4,31),
	ID(4,31,4), ID(31,6,1), ID(27,4,5), MIRROR(10304), MIRROR(8464), MIRROR(80)};

const unsigned char TempleLoca[] PROGMEM = { 0x0, 0x40, 0xC4, 0x7C, 0x40, 0x40, 0x7C, 0xC4, 0x54, 0x14, 0x1C, 0x3C, 0xEC, 0x28, 0xA8, 0xE8, 0xA8, 0x88, 0x80, 0x80, 0x80, 0x84, 0xFC, 0x84, 0x24, 0xE4, 0x3C, 0x24, 0xE0, 0x20, };
const uint16_t TempleIds[] PROGMEM = {
	ID(5,5,6), ID(1,6,1), ID(1,2,4), ID(4,4,6), ID(1,1,2), ID(4,8,6), ID(1,8,8), ID(5,8,8), ID(8,8,8), ID(8,8,8),
	ID(5,8,8), ID(6,1,1), ID(8,4,8), ID(4,5,8), ID(6,2,4), ID(8,5,6), ID(8,4,4), ID(4,4,5), ID(6,6,1), ID(1,1,2),
	ID(4,4,5), ID(4,5,4), ID(6,1,1), ID(6,1,2), ID(4,5,4), ID(6,1,1), ID(5,0,0), MIRROR(0), MIRROR(32), MIRROR(16384), MIRROR(16384), MIRROR(16896)};

const unsigned char ForestLoca[] PROGMEM = { 0x90, 0x90, 0x98, 0x9C, 0xB8, 0x30, 0x70, 0x78, 0x7C, 0x68, 0xF0, 0xF8, 0xD0, 0xE0, 0xE0, 0xE0, 0x60, 0xE0, 0xF0, 0xF8, 0xF0, 0xE0, 0xC0, 0xE0, 0xC0, 0xE0, 0x60, 0x70, 0x38, 0x38, 0x18, 0x8, };
const uint16_t ForestIds[] PROGMEM = {
	ID(22,16,21), ID(28,15,24), ID(28,19,11), ID(21,28,15), ID(31,21,16), ID(23,31,28), ID(31,20,22), ID(19,31,24), ID(28,31,7), ID(31,26,22),
	ID(27,21,22), ID(28,31,20), ID(28,31,7), ID(31,26,22), ID(30,25,18), ID(31,24,23), ID(31,25,31), ID(31,20,23), ID(31,28,31), ID(20,15,27),
	ID(31,25,19), ID(11,31,31), ID(26,15,29), ID(31,25,23), ID(31,24,30), ID(24,18,31), ID(26,31,25), ID(23,31,20), ID(31,21,23), ID(31,20,28),
	ID(31,31,27), ID(31,21,31), ID(21,22,0), MIRROR(51373), MIRROR(17106), MIRROR(24599), MIRROR(60), MIRROR(24714), MIRROR(18744), MIRROR(1)};

const unsigned char PyramidLoca[] PROGMEM = { 0x3F, 0x77, 0x73, 0x79, 0x39, 0x73, 0x63, 0x73, 0xFF, 0xFF, 0xF2, 0xF0, 0xF0, 0xE1, 0xC3, 0x83, 0x81, 0xC0, 0xE0, 0xF0, 0xB8, 0x9C, 0x9C, 0xB9, 0xF1, 0xE0, 0xC0, 0x80, };
const uint16_t PyramidIds[] PROGMEM = {
	ID(31,31,31), ID(31,31,21), ID(31,21,22), ID(23,31,20), ID(31,20,13), ID(30,25,31), ID(13,12,29), ID(20,31,13), ID(12,30,31), ID(20,13,30),
	ID(20,31,24), ID(30,24,31), ID(21,13,29), ID(21,31,31), ID(14,13,12), ID(29,31,20), ID(27,31,14), ID(13,12,30), ID(31,4,22), ID(13,30,31),
	ID(6,13,12), ID(30,4,13), ID(12,12,4), ID(14,13,12), ID(4,12,14), ID(13,4,12), ID(14,4,14), ID(4,13,4), ID(13,12,6), ID(13,12,14),
	ID(4,13,12), ID(14,4,13), ID(12,14,4), ID(13,12,14), ID(4,14,13), ID(12,14,4), ID(14,13,12), ID(14,4,13), ID(12,6,13), ID(4,5,0), MIRROR(8841), MIRROR(37810), MIRROR(576), MIRROR(557), MIRROR(18498), MIRROR(8705), MIRROR(12554), MIRROR(82)};

const unsigned char BrickLoca[] PROGMEM = { 0xFC, 0xF8, 0xA2, 0xA3, 0xBA, 0xBB, 0xA2, 0xB8, 0xA0, 0xB0, 0xBC, 0xB0, 0x90, 0x90, 0x90, 0x90, 0x80, 0xC0, 0xC4, 0xC7, 0xC4, 0xC4, 0xC0, 0xF8, 0xC0, };
const uint16_t BrickIds[] PROGMEM = {
	ID(3,1,2), ID(9,9,9), ID(3,2,9), ID(10,9,9), ID(10,9,2), ID(9,10,9), ID(9,9,9), ID(9,9,3), ID(9,10,10), ID(9,9,9),
	ID(9,9,3), ID(2,9,9), ID(9,9,10), ID(9,9,3), ID(2,9,9), ID(9,9,9), ID(9,9,9), ID(9,9,9), ID(9,10,9), ID(9,10,9),
	ID(10,9,9), ID(3,2,9), ID(9,9,9), ID(9,9,9), ID(9,9,9), ID(9,0,1), ID(2,9,9), ID(10,10,0), MIRROR(4609), MIRROR(1536), MIRROR(1), MIRROR(128), MIRROR(0), MIRROR(6)};

/*
 * The various maps
 */
const MapData::MapDescription MapData::AllMaps[] PROGMEM = {
	{ // -------- LEVEL -----------
		18, //StartX
		37, //StartY
		147, //HomeX
		39, //HomeY
		18, //Time In Multiple Of 10 seconds
		2, //Available Lem Count in multiple of 5 (from 0 to 15 max!!)
		2, //RequiredLemCount in multiple of 5 (obviously less or equals than the previous number)
		5, //Min Drop Speed in rough seconds (more precisely in multiple of 27 frames) (from 0 to 15)
		0, //Nb of available Walker
		1, //Nb of available Blocker
		0, //Nb of available Bomber
		0, //Nb of available Diagonal Digger
		2, //Nb of available Horizontal Digger
		0, //Nb of available Vertical Digger
		2, //Nb of available Stair Builder
		1, //Nb of available Climber
		1, //Nb of available Parachuter
		(const unsigned char *)TutoLoca, //SpriteLocalization
		sizeof(TutoLoca), //size of the Map in number of sprite columns
		(const uint16_t *)TutoIds //SpriteIdList
	},
	{ // -------- LEVEL -----------
		32, //StartX
		5, //StartY
		162, //HomeX
		55, //HomeY
		18, //Time In Multiple Of 10 seconds
		10, //Available Lem Count in multiple of 5 (from 0 to 15 max!!)
		8, //RequiredLemCount in multiple of 5 (obviously less or equals than the previous number)
		15, //Min Drop Speed in rough seconds (more precisely in multiple of 27 frames) (from 0 to 15) (high value = slow)
		0, //Nb of available Walker
		5, //Nb of available Blocker
		0, //Nb of available Bomber
		0, //Nb of available Diagonal Digger
		0, //Nb of available Horizontal Digger
		0, //Nb of available Vertical Digger
		9, //Nb of available Stair Builder
		0, //Nb of available Climber
		9, //Nb of available Parachuter
		(const unsigned char *)TempleLoca, //SpriteLocalization
		sizeof(TempleLoca), //size of the Map in number of sprite columns
		(const uint16_t *)TempleIds //SpriteIdList
	},
	{ // -------- LEVEL -----------
		222, //StartX
		10, //StartY
		20, //HomeX
		61, //HomeY
		20, //Time In Multiple Of 10 seconds
		3, //Available Lem Count in multiple of 5 (from 0 to 15 max!!)
		2, //RequiredLemCount in multiple of 5 (obviously less or equals than the previous number)
		6, //Min Drop Speed in rough seconds (more precisely in multiple of 27 frames) (from 1 to 15) (high value = slow)
		4, //Nb of available Walker
		4, //Nb of available Blocker
		2, //Nb of available Bomber
		2, //Nb of available Diagonal Digger
		1, //Nb of available Horizontal Digger
		2, //Nb of available Vertical Digger
		9, //Nb of available Stair Builder
		0, //Nb of available Climber
		1, //Nb of available Parachuter
		(const unsigned char *)ForestLoca, //SpriteLocalization
		sizeof(ForestLoca), //size of the Map in number of sprite columns
		(const uint16_t *)ForestIds //SpriteIdList
	},
	{ // -------- LEVEL -----------
		24, //StartX
		8, //StartY
		175, //HomeX
		55, //HomeY
		12, //Time In Multiple Of 10 seconds
		6, //Available Lem Count in multiple of 5 (from 0 to 15 max!!)
		6, //RequiredLemCount in multiple of 5 (obviously less or equals than the previous number)
		10, //Min Drop Speed in rough seconds (more precisely in multiple of 27 frames) (from 1 to 15) (high value = slow)
		0, //Nb of available Walker
		0, //Nb of available Blocker
		9, //Nb of available Bomber
		2, //Nb of available Diagonal Digger
		2, //Nb of available Horizontal Digger
		2, //Nb of available Vertical Digger
		0, //Nb of available Stair Builder
		0, //Nb of available Climber
		0, //Nb of available Parachuter
		(const unsigned char *)PyramidLoca, //SpriteLocalization
		sizeof(PyramidLoca), //size of the Map in number of sprite columns
		(const uint16_t *)PyramidIds //SpriteIdList
	},
	{ // -------- LEVEL -----------
		98, //StartX
		15, //StartY
		171, //HomeX
		47, //HomeY
		24, //Time In Multiple Of 10 seconds
		9, //Available Lem Count in multiple of 5 (from 0 to 15 max!!)
		9, //RequiredLemCount in multiple of 5 (obviously less or equals than the previous number)
		10, //Min Drop Speed in rough seconds (more precisely in multiple of 27 frames) (from 0 to 15) (high value = slow)
		2, //Nb of available Walker
		1, //Nb of available Blocker
		0, //Nb of available Bomber
		0, //Nb of available Diagonal Digger
		0, //Nb of available Horizontal Digger
		2, //Nb of available Vertical Digger
		9, //Nb of available Stair Builder
		1, //Nb of available Climber
		0, //Nb of available Parachuter
		(const unsigned char *)BrickLoca, //SpriteLocalization
		sizeof(BrickLoca), //size of the Map in number of sprite columns
		(const uint16_t *)BrickIds //SpriteIdList
	},
};

unsigned char MapData::GetMapCount()
{
	return sizeof(MapData::AllMaps) / sizeof(MapData::AllMaps[0]);
}
