#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "src/entities/Entities.h"
#include "src/utils/SpritesU.hpp"


void launchPuff(PuffType puffType, Item &item) {

    Item &puff = world.getItem(0);

    puff.setX(item.getX());
    puff.setY(item.getY() - (puffType == PuffType::Heart ? 3 : 0));
    puff.setFrame(0);
    puff.setData(puffType == PuffType::Heart ? 7 : 0);
    item.setCounter(3);

}


bool isAlignedWithTile() {

    return (world.getForeground() % 16) == 0;

}

bool isAlignedWithLevel() {

    return ((world.getPlayer().getY() - 46) % 16) == 0;

}

uint8_t getTile_ByIdx(int16_t i) {

    if (i == -1) return 11;
    if (i < 0) return 255;

    if (i == (Constants::World_Repeat * Constants::Tile_Count)) return 254;
    if (i == (Constants::World_Repeat * Constants::Tile_Count) + 1) return 11;
    if (i >= (Constants::World_Repeat * Constants::Tile_Count)) return 255;

    uint8_t tile = Level::Level[i % Constants::Tile_Count];    
    return tile;

}

uint8_t getTile(int8_t offset) {

    int8_t xOffset = world.getPlayer().getDirection() == Direction::Left ? -4 : 0;
    int16_t i = (-(world.getForeground() - 8 - xOffset) / 16) + offset;

    if (i < 0) return 255;
    if (i == (Constants::World_Repeat * Constants::Tile_Count)) return 254;
    if (i == (Constants::World_Repeat * Constants::Tile_Count) + 1) return 11;
    if (i >= (Constants::World_Repeat * Constants::Tile_Count)) return 255;

    uint8_t tile = Level::Level[i % Constants::Tile_Count];    
    return tile;

}


uint8_t getEnemyTile(int16_t enemyX, int8_t offset) {

    int16_t i = (enemyX / 16) + offset;
    if (i < 0) return 255;
    if (i == (Constants::World_Repeat * Constants::Tile_Count)) return 254;
    if (i == (Constants::World_Repeat * Constants::Tile_Count) + 1) return 11;
    if (i >= (Constants::World_Repeat * Constants::Tile_Count)) return 255;

    uint8_t tile = Level::Level[i % Constants::Tile_Count];    
    return tile;

}

uint8_t getJustPressedButtons() {

    a.pollButtons();
    return a.justPressedButtons();

}

uint8_t getPressedButtons() {

    return a.pressedButtons();

}

template<typename T> T absT(const T & v) {

	return (v < 0) ? -v : v;

}


void saveCookie() {

    FX::saveGameState(cookie);

}

bool collide(Rect rect1, Rect rect2) {

   return !(rect2.x                >= rect1.x + rect1.width  ||
            rect2.x + rect2.width  <= rect1.x                ||
            rect2.y                >= rect1.y + rect1.height ||
            rect2.y + rect2.height <= rect1.y);

}


bool collide(Point point, Rect rect) {

   return ((point.x >= rect.x) && (point.x < rect.x + rect.width) &&
           (point.y >= rect.y) && (point.y < rect.y + rect.height));

 }

inline void debug_break() {

    asm volatile("break\n");

}

