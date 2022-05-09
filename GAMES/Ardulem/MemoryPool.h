#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H
#include <Arduino.h>
 
/*
 * This simple file is used to declare a big huge array that is both used by the LemManager
 * to store all the lems, and by the MapManager to store the map modifications.
 * Each manager will store a pointer on one particular place inside the pool.
 * The shared pool allow to have more modification or more lem depending on the map.
 * The LemManager store the lem at the begining of the Pool, and doesn't need to know
 * how many, because the never more lem than what the map as planned to spawn will be spawn.
 * The MapManager knows how many lems will be spawns, so know the beginning of the map modification.
 */
namespace MemoryPool
{
	// Size of the memory pool that will both be used by the lem and the modification of the map
	// Each Lem weight 3 chars, and each 8 pixel column modification take one char
	// so 75 lems (which is the maximum you can set in the map data), will take 225 chars,
	// so it's not advised to go below 300.
	static const uint16_t MEMORY_POOL_SIZE = 1328;
	
	extern unsigned char Pool[MEMORY_POOL_SIZE];
}

#endif
