#ifndef _MAP_DATA_H_
#define _MAP_DATA_H_

class Graal;

// some mandatory instance of Items
extern Graal graal1;

// define the type of pointer on member function of Item, for callback purpose
typedef void (*ItemInitFunction)(bool init);

namespace MapManager
{
	// all the map sprite ids of the full level
	extern const unsigned int LevelLineIndex[];
	extern const unsigned char Level[];
	
	// all the function to init the items in the level split per puzzle screen
	extern ItemInitFunction ItemInitFunctions[];
	
	// init the alive status of all the living items
	void SaveAndLoadAliveStatusForAllItems(unsigned char currentScreenIdToSave, unsigned char newScreenIdToLoad);
};

#endif