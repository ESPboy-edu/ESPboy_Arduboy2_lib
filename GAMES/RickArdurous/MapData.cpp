/*
 * This is a generated file, use the Editor to modify it.
 */

#include "MapData.h"
#include "SpriteData.h"
#include "MapManager.h"
#include "GameManager.h"
#include "ArrowLauncher.h"
#include "Spike.h"
#include "Statuette.h"
#include "Dynamite.h"
#include "DynamiteCrate.h"
#include "Graal.h"
#include "BulletCrate.h"
#include "Enemy.h"
#include "DestroyableBlock.h"
#include "Stalactite.h"
#include "Stalagmite.h"
#include "Boulder.h"
#include <avr/pgmspace.h>

#define ID(id1,id2) ((id1<< 4) | id2)

const unsigned char MapManager::Level[] PROGMEM = {
	ID(3,2),ID(1,2),ID(1,3),ID(1,3),ID(1,2),ID(1,2),ID(3,1),ID(2,2),ID(2,0),ID(11,11),ID(11,11),ID(0,9),ID(9,11),ID(11,11),ID(11,11),ID(11,11),ID(11,11),ID(11,11),ID(11,11),ID(11,11),ID(11,0),ID(1,0),ID(11,11),ID(11,1),
	ID(1,1),ID(11,0),ID(11,15),ID(1,0),ID(11,11),ID(11,15),ID(1,11),ID(0,0),ID(1,1),ID(8,15),ID(6,10),ID(10,15),ID(15,15),ID(2,0),ID(15,4),ID(8,0),
	ID(2,0),ID(15,12),ID(0,1),ID(8,15),ID(6,6),ID(5,15),ID(2,11),ID(11,11),ID(11,3),ID(6,4),ID(5,12),ID(12,12),ID(14,12),ID(15,3),ID(12,12),ID(15,2),ID(8,0),
	ID(8,15),ID(14,8),ID(1,15),ID(2,7),ID(5,15),ID(10,8),ID(4,5),ID(15,4),ID(13,15),ID(8,8),
	ID(8,15),ID(14,8),ID(1,15),ID(3,6),ID(5,3),ID(3,3),ID(3,3),ID(3,11),ID(15,2),ID(8,5),ID(15,4),ID(4,3),ID(0,11),ID(0,3),ID(3,7),ID(15,2),ID(8,0),
	ID(8,15),ID(6,9),ID(9,15),ID(4,7),ID(3,3),ID(0,11),ID(15,3),ID(11,11),ID(11,11),ID(11,11),ID(15,4),ID(8,15),ID(4,6),ID(11,11),ID(15,4),ID(11,0),ID(3,3),ID(1,0),
	ID(8,3),ID(7,15),ID(4,10),ID(10,15),ID(15,15),ID(6,7),ID(1,6),ID(15,3),ID(5,15),ID(8,8),ID(1,1),
	ID(1,1),ID(1,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(1,1),ID(5,14),ID(3,3),ID(5,14),ID(4,3),ID(3,3),ID(5,14),ID(3,1),ID(1,1),
	ID(1,0),ID(11,11),ID(11,11),ID(11,11),ID(11,11),ID(11,11),ID(11,2),ID(1,1),ID(1,2),ID(1,1),ID(1,2),ID(1,1),ID(1,1),ID(2,1),ID(1,1),ID(1,1),ID(8,13),ID(0,2),ID(0,13),ID(6,1),ID(0,11),ID(0,13),ID(0,2),ID(2,1),
	ID(8,15),ID(12,8),ID(0,0),ID(0,2),ID(1,2),ID(1,11),ID(15,3),ID(11,0),ID(11,15),ID(2,0),ID(1,8),ID(13,15),ID(3,13),ID(5,15),ID(4,13),ID(15,2),ID(11,2),
	ID(8,15),ID(2,11),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,0),ID(15,4),ID(0,2),ID(0,15),ID(10,8),ID(8,13),ID(15,3),ID(13,5),ID(15,4),ID(13,15),ID(3,1),
	ID(8,15),ID(4,11),ID(11,11),ID(0,1),ID(2,0),ID(11,15),ID(15,15),ID(1,7),ID(3,1),ID(6,4),ID(5,4),ID(9,9),ID(6,15),ID(2,12),ID(12,12),ID(12,15),ID(2,8),
	ID(1,7),ID(15,7),ID(0,11),ID(15,4),ID(7,3),ID(3,3),ID(3,7),ID(15,10),ID(1,4),ID(5,15),ID(2,10),ID(10,15),ID(8,7),ID(1,0),
	ID(1,1),ID(3,3),ID(3,0),ID(11,15),ID(8,8),ID(1,2),ID(2,1),ID(3,3),ID(7,15),ID(2,7),ID(3,7),ID(15,3),ID(8,6),ID(15,3),ID(5,5),ID(15,2),ID(12,12),ID(12,12),ID(15,1),ID(11,11),ID(1,0),
	ID(1,2),ID(1,2),ID(8,15),ID(10,8),ID(2,1),ID(1,2),ID(2,1),ID(8,15),ID(2,8),ID(2,1),ID(7,15),ID(2,2),ID(5,15),ID(14,8),
	ID(2,1),ID(1,1),ID(2,2),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,1),ID(2,2),ID(1,2),ID(1,2),ID(1,1),ID(1,1),ID(1,2),ID(8,15),ID(2,8),ID(1,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,14),ID(3,3),ID(3,1),
	ID(2,2),ID(2,0),ID(11,11),ID(0,2),ID(1,1),ID(1,1),ID(1,0),ID(11,11),ID(11,11),ID(11,11),ID(11,11),ID(11,11),ID(0,11),ID(0,11),ID(11,15),ID(2,1),ID(2,1),ID(11,11),ID(11,11),ID(11,0),ID(2,1),ID(1,13),ID(1,8),ID(2,1),
	ID(2,0),ID(11,15),ID(5,11),ID(4,6),ID(11,15),ID(15,15),ID(4,8),ID(2,0),ID(15,7),ID(11,11),ID(13,11),ID(11,0),ID(2,0),
	ID(8,15),ID(14,7),ID(1,12),ID(12,14),ID(12,12),ID(12,12),ID(15,1),ID(8,8),ID(15,2),ID(7,3),ID(1,8),ID(15,10),ID(13,15),ID(3,6),
	ID(8,15),ID(2,7),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,6),ID(4,5),ID(8,15),ID(2,13),ID(15,5),ID(1,8),ID(15,2),ID(8,1),ID(1,8),ID(15,10),ID(13,15),ID(3,5),
	ID(5,15),ID(2,0),ID(1,2),ID(0,11),ID(11,15),ID(1,11),ID(11,0),ID(1,6),ID(5,8),ID(15,2),ID(13,15),ID(5,2),ID(8,15),ID(2,8),ID(2,1),ID(8,15),ID(2,7),ID(3,7),ID(15,5),ID(13,15),ID(2,7),ID(2,0),
	ID(6,15),ID(3,11),ID(15,8),ID(11,11),ID(1,8),ID(15,2),ID(13,15),ID(3,7),ID(3,2),ID(1,15),ID(2,8),ID(1,2),ID(8,15),ID(3,11),ID(11,11),ID(11,11),ID(11,0),ID(3,3),ID(3,3),ID(1,0),
	ID(1,7),ID(15,13),ID(8,1),ID(7,15),ID(1,13),ID(15,9),ID(8,1),ID(1,8),ID(7,15),ID(9,0),ID(11,0),ID(2,2),
	ID(2,2),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,7),ID(15,2),ID(7,1),ID(1,1),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,1),ID(1,1),ID(1,1),ID(3,3),ID(3,3),ID(3,12),ID(14,15),ID(5,11),ID(5,0),
	ID(1,11),ID(11,11),ID(11,11),ID(11,11),ID(11,11),ID(11,11),ID(15,2),ID(4,5),ID(1,11),ID(11,0),ID(5,4),ID(5,0),ID(0,1),ID(1,0),ID(11,11),ID(11,0),ID(5,6),ID(0,11),ID(11,11),ID(11,15),ID(1,13),ID(15,6),ID(6,0),
	ID(6,15),ID(11,14),ID(12,5),ID(6,8),ID(15,5),ID(5,15),ID(2,0),ID(11,15),ID(11,5),ID(15,1),ID(13,15),ID(6,5),
	ID(5,15),ID(2,12),ID(12,12),ID(12,5),ID(15,4),ID(13,9),ID(9,5),ID(1,3),ID(14,2),ID(5,6),ID(5,15),ID(13,5),ID(9,9),ID(12,14),ID(12,12),ID(12,12),ID(12,12),ID(5,0),
	ID(5,6),ID(15,5),ID(5,15),ID(4,13),ID(10,10),ID(6,1),ID(1,13),ID(1,1),ID(4,5),ID(12,14),ID(12,12),ID(12,12),ID(12,12),ID(5,5),ID(15,3),ID(5,10),ID(10,15),ID(1,13),ID(15,6),ID(5,0),
	ID(4,5),ID(15,5),ID(5,12),ID(12,12),ID(12,12),ID(0,5),ID(5,0),ID(15,2),ID(11,0),ID(1,5),ID(15,1),ID(13,15),ID(6,1),ID(6,12),ID(14,12),ID(4,5),ID(6,15),ID(1,13),ID(15,2),ID(5,14),ID(4,5),ID(6,0),
	ID(6,9),ID(9,12),ID(12,14),ID(12,11),ID(15,7),ID(6,15),ID(5,0),ID(6,4),ID(5,12),ID(14,15),ID(4,8),ID(5,15),ID(1,13),ID(15,3),ID(11,11),ID(11,11),ID(11,11),ID(13,15),ID(1,9),ID(9,0),
	ID(5,10),ID(10,15),ID(2,13),ID(15,12),ID(5,15),ID(7,13),ID(15,2),ID(7,3),ID(1,5),ID(15,1),ID(13,15),ID(9,13),ID(15,1),ID(10,10),
	ID(4,5),ID(6,5),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(1,1),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),ID(3,3),
};

const unsigned int MapManager::LevelLineIndex[] PROGMEM = {0,24,40,57,67,84,102,113,137,161,178,195,212,226,247,261,285,309,322,336,354,376,396,408,431,454,466,484,504,526,546,560,584,};

BulletCrate bulletCrate1;
DynamiteCrate dynamiteCrate1;
Statuette statuette1;
Graal graal1;
Spike spike1;
Spike spike2;
Spike spike3;
Spike spike4;
Spike spike5;
Enemy enemy1;
Enemy enemy2;
Enemy enemy3;
Enemy enemy4;
Stalagmite stalagmite1;
Stalagmite stalagmite2;
Stalactite stalactite1;
Stalactite stalactite2;
Stalactite stalactite3;
ArrowLauncher arrowLauncher1;
ArrowLauncher arrowLauncher2;
ArrowLauncher arrowLauncher3;
DestroyableBlock destBlock1;
Boulder boulder1;

void InitScreen0(bool shouldRespawn)
{
	// Add a checkpoint if we need to

	// init all the item of the current puzzle screen
	stalagmite1.Init(14, 40, Item::PropertyFlags::NONE, shouldRespawn);
	stalagmite2.Init(108, 32, Item::PropertyFlags::NONE, shouldRespawn);
	stalactite1.Init(81, 8, Item::PropertyFlags::NONE, shouldRespawn);
	stalactite2.Init(40, 8, Item::PropertyFlags::NONE, shouldRespawn);
	stalactite3.Init(17, 10, Item::PropertyFlags::NONE, shouldRespawn);

	// The following items must only be init for the game, and not in the main menu
	if (GameManager::CurrentGameState == GameManager::PLAYING)
	{
		statuette1.Init(8, 40, Item::PropertyFlags::NONE, shouldRespawn);
		graal1.Init(61, 32, Item::PropertyFlags::NONE, shouldRespawn);
	}
}

void InitScreen1(bool shouldRespawn)
{
	// Add a checkpoint if we need to
	MapManager::MemorizeCheckPoint(52, 67);

	// init all the item of the current puzzle screen
	boulder1.Init(91, 68, Item::PropertyFlags::TRAP_TRIGERER | Item::PropertyFlags::ALIVE  | Item::PropertyFlags::MIRROR_X);
}

void InitScreen2(bool shouldRespawn)
{
	// Add a checkpoint if we need to
	MapManager::MemorizeCheckPoint(132, 83);

	// init all the item of the current puzzle screen
	stalagmite1.Init(193, 112, Item::PropertyFlags::NONE, shouldRespawn);
	stalagmite2.Init(185, 112, Item::PropertyFlags::NONE, shouldRespawn);
	statuette1.Init(235, 80, Item::PropertyFlags::NONE, shouldRespawn);
}

void InitScreen3(bool shouldRespawn)
{
	// Add a checkpoint if we need to
	MapManager::MemorizeCheckPoint(236, 131);

	// init all the item of the current puzzle screen
	statuette1.Init(190, 160, Item::PropertyFlags::NONE, shouldRespawn);
	spike1.Init(216, 165, Item::PropertyFlags::MIRROR_X);
	spike2.Init(216, 169, Item::PropertyFlags::MIRROR_X);
	spike3.Init(196, 159, Item::PropertyFlags::NONE);
	spike4.Init(196, 153, Item::PropertyFlags::NONE);
	spike5.Init(207, 136, Item::PropertyFlags::SPECIAL_);
	enemy1.Init(161, 130, Item::PropertyFlags::TRAP_TRIGERER | Item::PropertyFlags::SPECIAL__2, shouldRespawn);
}

void InitScreen4(bool shouldRespawn)
{
	// Add a checkpoint if we need to
	MapManager::MemorizeCheckPoint(110, 139);

	// init all the item of the current puzzle screen
	stalagmite1.Init(34, 176, Item::PropertyFlags::NONE, shouldRespawn);
	stalagmite2.Init(48, 176, Item::PropertyFlags::NONE, shouldRespawn);
	statuette1.Init(112, 176, Item::PropertyFlags::NONE, shouldRespawn);
	destBlock1.Init(72, 144, Item::PropertyFlags::NONE, shouldRespawn);
	dynamiteCrate1.Init(90, 144, Item::PropertyFlags::NONE, shouldRespawn);
}

void InitScreen5(bool shouldRespawn)
{
	// Add a checkpoint if we need to
	MapManager::MemorizeCheckPoint(102, 195);

	// init all the item of the current puzzle screen
	statuette1.Init(25, 240, Item::PropertyFlags::NONE, shouldRespawn);
	enemy1.Init(37, 194, Item::PropertyFlags::TRAP_TRIGERER, shouldRespawn);
	enemy2.Init(70, 210, Item::PropertyFlags::TRAP_TRIGERER, shouldRespawn);
	enemy3.Init(80, 234, Item::PropertyFlags::TRAP_TRIGERER, shouldRespawn);
	enemy4.Init(32, 218, Item::PropertyFlags::TRAP_TRIGERER | Item::PropertyFlags::MIRROR_X, shouldRespawn);
	bulletCrate1.Init(110, 240, Item::PropertyFlags::NONE, shouldRespawn);
}

void InitScreen6(bool shouldRespawn)
{
	// Add a checkpoint if we need to
	MapManager::MemorizeCheckPoint(132, 235);

	// init all the item of the current puzzle screen
	stalactite1.Init(237, 195, Item::PropertyFlags::NONE, shouldRespawn);
	statuette1.Init(166, 200, Item::PropertyFlags::NONE, shouldRespawn);
	enemy1.Init(219, 202, Item::PropertyFlags::TRAP_TRIGERER | Item::PropertyFlags::SPECIAL__2 | Item::PropertyFlags::MIRROR_X, shouldRespawn);
	destBlock1.Init(184, 240, Item::PropertyFlags::NONE, shouldRespawn);
	enemy2.Init(213, 244, Item::PropertyFlags::SPECIAL_, shouldRespawn);
}

void InitScreen7(bool shouldRespawn)
{
	// Add a checkpoint if we need to
	MapManager::MemorizeCheckPoint(256, 203);

	// init all the item of the current puzzle screen
	statuette1.Init(294, 200, Item::PropertyFlags::NONE, shouldRespawn);
	dynamiteCrate1.Init(363, 216, Item::PropertyFlags::NONE, shouldRespawn);
	arrowLauncher1.Init(363, 232, Item::PropertyFlags::MIRROR_X, 104);
	arrowLauncher2.Init(312, 223, Item::PropertyFlags::NONE, 48);
	arrowLauncher3.Init(371, 192, Item::PropertyFlags::MIRROR_X, 87);
}

void InitScreen8(bool shouldRespawn)
{
	// Add a checkpoint if we need to
	MapManager::MemorizeCheckPoint(311, 171);

	// init all the item of the current puzzle screen
	stalactite1.Init(328, 138, Item::PropertyFlags::NONE, shouldRespawn);
	stalactite2.Init(273, 131, Item::PropertyFlags::NONE, shouldRespawn);
	boulder1.Init(290, 148, Item::PropertyFlags::TRAP_TRIGERER | Item::PropertyFlags::ALIVE  | Item::PropertyFlags::SPECIAL_);
	spike1.Init(263, 157, Item::PropertyFlags::MIRROR_X);
	spike2.Init(320, 136, Item::PropertyFlags::SPECIAL_ | Item::PropertyFlags::MIRROR_X);
	spike3.Init(302, 130, Item::PropertyFlags::SPECIAL_ | Item::PropertyFlags::MIRROR_X);
	spike4.Init(340, 139, Item::PropertyFlags::SPECIAL_ | Item::PropertyFlags::MIRROR_X);
	spike5.Init(359, 138, Item::PropertyFlags::SPECIAL_ | Item::PropertyFlags::MIRROR_X);
	enemy1.Init(343, 164, Item::PropertyFlags::SPECIAL_, shouldRespawn);
	enemy2.Init(321, 164, Item::PropertyFlags::SPECIAL_ | Item::PropertyFlags::MIRROR_X, shouldRespawn);
	arrowLauncher1.Init(371, 151, Item::PropertyFlags::MIRROR_X, 71);
}

void InitScreen9(bool shouldRespawn)
{
	// Add a checkpoint if we need to
	MapManager::MemorizeCheckPoint(331, 107);

	// init all the item of the current puzzle screen
	stalactite1.Init(312, 72, Item::PropertyFlags::NONE, shouldRespawn);
	statuette1.Init(269, 112, Item::PropertyFlags::NONE, shouldRespawn);
	spike1.Init(263, 109, Item::PropertyFlags::MIRROR_X);
	spike2.Init(271, 95, Item::PropertyFlags::MIRROR_X);
	spike3.Init(268, 103, Item::PropertyFlags::SPECIAL_ | Item::PropertyFlags::MIRROR_X);
	spike4.Init(367, 106, Item::PropertyFlags::SPECIAL_ | Item::PropertyFlags::MIRROR_X);
	destBlock1.Init(288, 112, Item::PropertyFlags::NONE, shouldRespawn);
	enemy1.Init(277, 74, Item::PropertyFlags::TRAP_TRIGERER, shouldRespawn);
	enemy2.Init(343, 90, Item::PropertyFlags::TRAP_TRIGERER | Item::PropertyFlags::MIRROR_X, shouldRespawn);
	enemy3.Init(329, 74, Item::PropertyFlags::TRAP_TRIGERER, shouldRespawn);
	bulletCrate1.Init(363, 112, Item::PropertyFlags::NONE, shouldRespawn);
	enemy4.Init(277, 116, Item::PropertyFlags::SPECIAL_, shouldRespawn);
}

void InitScreen10(bool shouldRespawn)
{
	// Add a checkpoint if we need to

	// init all the item of the current puzzle screen
	statuette1.Init(352, 8, Item::PropertyFlags::NONE, shouldRespawn);
	spike1.Init(271, 23, Item::PropertyFlags::MIRROR_X);
	spike2.Init(263, 31, Item::PropertyFlags::MIRROR_X);
	spike3.Init(362, 32, Item::PropertyFlags::SPECIAL_);
	spike4.Init(278, 2, Item::PropertyFlags::SPECIAL_ | Item::PropertyFlags::MIRROR_X);
	enemy1.Init(342, 18, Item::PropertyFlags::TRAP_TRIGERER | Item::PropertyFlags::SPECIAL__2 | Item::PropertyFlags::MIRROR_X, shouldRespawn);
	enemy2.Init(334, 52, Item::PropertyFlags::SPECIAL_ | Item::PropertyFlags::MIRROR_X, shouldRespawn);
	arrowLauncher1.Init(257, 39, Item::PropertyFlags::NONE, 31);
	arrowLauncher2.Init(372, 8, Item::PropertyFlags::MIRROR_X, 32);
}

void InitScreen11(bool shouldRespawn)
{
	// Add a checkpoint if we need to
	MapManager::MemorizeCheckPoint(246, 3);

	// init all the item of the current puzzle screen
	statuette1.Init(156, 48, Item::PropertyFlags::NONE, shouldRespawn);
	boulder1.Init(207, 20, Item::PropertyFlags::TRAP_TRIGERER | Item::PropertyFlags::ALIVE  | Item::PropertyFlags::MIRROR_X | Item::PropertyFlags::SPECIAL_);
	spike1.Init(167, 22, Item::PropertyFlags::MIRROR_X);
	spike2.Init(180, 6, Item::PropertyFlags::NONE);
	spike3.Init(136, 9, Item::PropertyFlags::MIRROR_X);
	spike4.Init(136, 43, Item::PropertyFlags::SPECIAL_ | Item::PropertyFlags::MIRROR_X);
	spike5.Init(137, 32, Item::PropertyFlags::SPECIAL_);
	destBlock1.Init(184, 24, Item::PropertyFlags::NONE, shouldRespawn);
}

// The array that contains all the items
ItemInitFunction MapManager::ItemInitFunctions[] = {
	&InitScreen0, &InitScreen1, &InitScreen2, &InitScreen3, &InitScreen4, &InitScreen5, &InitScreen6, &InitScreen7, &InitScreen8, &InitScreen9, &InitScreen10, &InitScreen11, &InitScreen0, 
};


// this function is to save and load the living status of the items
void MapManager::SaveAndLoadAliveStatusForAllItems(unsigned char currentScreenIdToSave, unsigned char newScreenIdToLoad)
{
	bulletCrate1.SaveAndLoadAliveStatus(currentScreenIdToSave, newScreenIdToLoad);
	dynamiteCrate1.SaveAndLoadAliveStatus(currentScreenIdToSave, newScreenIdToLoad);
	statuette1.SaveAndLoadAliveStatus(currentScreenIdToSave, newScreenIdToLoad);
	graal1.SaveAndLoadAliveStatus(currentScreenIdToSave, newScreenIdToLoad);
	enemy1.SaveAndLoadAliveStatus(currentScreenIdToSave, newScreenIdToLoad);
	enemy2.SaveAndLoadAliveStatus(currentScreenIdToSave, newScreenIdToLoad);
	enemy3.SaveAndLoadAliveStatus(currentScreenIdToSave, newScreenIdToLoad);
	enemy4.SaveAndLoadAliveStatus(currentScreenIdToSave, newScreenIdToLoad);
	stalagmite1.SaveAndLoadAliveStatus(currentScreenIdToSave, newScreenIdToLoad);
	stalagmite2.SaveAndLoadAliveStatus(currentScreenIdToSave, newScreenIdToLoad);
	stalactite1.SaveAndLoadAliveStatus(currentScreenIdToSave, newScreenIdToLoad);
	stalactite2.SaveAndLoadAliveStatus(currentScreenIdToSave, newScreenIdToLoad);
	stalactite3.SaveAndLoadAliveStatus(currentScreenIdToSave, newScreenIdToLoad);
	destBlock1.SaveAndLoadAliveStatus(currentScreenIdToSave, newScreenIdToLoad);
}

// The following items are not used in the game, but they are written here,
// in order for the Editor to reload them.
/*
*/
