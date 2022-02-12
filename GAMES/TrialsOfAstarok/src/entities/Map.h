#pragma once

#include "../utils/Arduboy2Ext.h"
#include "../utils/Constants.h"

#include "Room.h"
#include "InteractiveObject.h"

class AstarokGame;

class Map {

    public:
        void newMap();
        void loadMap();
        void generateRoom(uint8_t roomNum);
        void addObject(ObjectTypes type, int x, int y);
        uint8_t checkObject(int x, int y);
        bool isTile(int x, int y);
        void addSign(int x, int y);
        void addExit(ObjectTypes exitType, int x, int y);

        int16_t minXPixel();
        int16_t maxXPixel();
        int16_t maxYPixel();
        int16_t minXTile();
        int16_t maxXTile();
        int16_t maxYTile();

        AstarokGame           * game;
        Room                  rooms[Constants::MapRooms];
        InteractiveObject     objects[Constants::MapObjects];
        uint8_t               objectIndex;
        uint8_t               firstRoom, lastRoom, mapHeight, spawnBarrier;

};