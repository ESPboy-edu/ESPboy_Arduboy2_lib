#include "Map.h"
#include "../../AstarokGame.h"

void Map::generateRoom(uint8_t roomNum) {

    this->game->seed = this->game->seeds[(this->game->mapNumber + roomNum) % Constants::GameSeeds] * this->game->mapNumber + roomNum;

    rooms[roomNum % Constants::MapRooms].clearRoom();
    uint8_t floor = (hash(this->game->seed) % 3) + (Constants::RoomHeight - 3);

    uint8_t upperPlatform_X = Constants::NoPlatform;
    uint8_t upperPlatform_Row = 0;
    uint8_t upperPlatform_Floor = 0;

    int8_t gap = 0;
    bool largeGap = false;
    bool largeGapFinished = false;
    int tSpawnBarrier = roomNum * Constants::RoomWidth;
    uint8_t flatFloor = 0;


    // If this is the first room on a level, add the sign ..

    if (roomNum == 0) {
        addSign(1, floor - 1);
    }

    for (int x = 0; x < Constants::RoomWidth; x++) {


        // Should we launch an upper platform?

        if (upperPlatform_X == Constants::NoPlatform && !largeGapFinished && gap == 0) {

            if (x >= 3 && x < 9 && roomNum < this->lastRoom && hash(this->game->seed) % 7 == 0) {
                upperPlatform_X = 0;
                upperPlatform_Floor = floor;
                upperPlatform_Row = hash(this->game->seed) % 5;
            }

        }

        if (upperPlatform_X != Constants::NoPlatform) {

            if (Constants::UpperPlatform[(upperPlatform_Row * 4) + upperPlatform_X] > 0) {
                
                rooms[roomNum % Constants::MapRooms].setTile(x, upperPlatform_Floor - Constants::UpperPlatform[(upperPlatform_Row * 4) + upperPlatform_X]);

            }

        }


        if (gap == 0) {


            // Fill the map data from the current floor to the bottom of the map ..

            for (int b = floor; b < Constants::RoomHeight; b++) {
                rooms[roomNum % Constants::MapRooms].setTile(x, b);
            }

            if (roomNum && (roomNum < this->lastRoom)) {

                if (upperPlatform_X == Constants::NoPlatform) {

                    if (hash(this->game->seed) % 10 == 0) { 
                        gap = (hash(this->game->seed) % 3) + 2;
                        if (gap == 4) largeGap = true;
                        flatFloor = 0;
                    }
                    else if (hash(this->game->seed) % 5 == 0) {
                        if (floor < Constants::RoomHeight - 1) floor++;
                        else floor--;
                        flatFloor = 0;
                    }
                    else {
                        flatFloor++;
                    }

                }

                if (tSpawnBarrier > spawnBarrier) {

                    uint8_t yLocation = floor - 2;
                    uint8_t randomMax = 5;


                    // Place items on upper platform?

                    if (upperPlatform_X > 0 && upperPlatform_X < 3) {

                        yLocation = upperPlatform_Floor - Constants::UpperPlatform[(upperPlatform_Row * 4) + upperPlatform_X] - 1;
                        uint8_t randomNumber = hash(this->game->seed) % randomMax;

                        if (randomNumber == 1) {

                            this->game->addMob(Data::Spider, Images::SpriteImages[ObjectTypes::Spider], Images::SpriteMasks[ObjectTypes::Spider], tSpawnBarrier + x, yLocation);

                        }
                        else if ((hash(this->game->seed) % 4) == 0) {

                            this->addObject(ObjectTypes::Coin, tSpawnBarrier + x, yLocation);

                        }

                    }
                    

                    // Place items on lower platform?

                    if (!(hash(this->game->seed) % 3)) {

                        switch (this->game->mapNumber) {

                            case 0 ... 3:
                                randomMax = 40;
                                break;

                            case 4 ... 6:
                                randomMax = 35;
                                break;

                            default:
                                randomMax = 30;
                                break;

                        }

                        switch (hash(this->game->seed) % randomMax) {

                            case 0 ... 9:
                                this->game->addMob(Data::Spider, Images::SpriteImages[ObjectTypes::Spider], Images::SpriteMasks[ObjectTypes::Spider], tSpawnBarrier + x, yLocation);
                                break;

                            case 10 ... 15:
                                this->game->addMob(Data::Skull, Images::SpriteImages[ObjectTypes::Skull], Images::SpriteMasks[ObjectTypes::Skull], tSpawnBarrier + x, yLocation);
                                break;

                            case 16 ... 18:
                                if (roomNum > 6) {
                                    this->game->addMob(Data::Starman, Images::SpriteImages[ObjectTypes::Starman], Images::SpriteMasks[ObjectTypes::Starman], tSpawnBarrier + x, yLocation);
                                }
                                break;

                            case 19 ... 25:
                                this->addObject(ObjectTypes::Coin, tSpawnBarrier + x, floor - 1);
                                break;

                            case 26 ... 27:
                                if (flatFloor >= 2 && upperPlatform_X == Constants::NoPlatform) {

                                    uint8_t upperRange = 1;

                                    switch (this->game->mapNumber) {

                                        case 1 ... 4:
                                            upperRange = 1;
                                            break;

                                        case 5 ... 6:
                                            upperRange = 2;
                                            break;

                                        case 7 ... 8:
                                            upperRange = 3;
                                            break;

                                        case 9 ... 10:
                                            upperRange = 4;
                                            break;

                                        default:
                                            upperRange = 5;
                                            break;
                                            
                                    }

                                    if (hash(this->game->seed) % upperRange == 0) {
                                        this->addObject(ObjectTypes::Chest_Closed, tSpawnBarrier + x, floor - 1);
                                    }

                                }
                                break;

                                case 28 ... 29:
                                if (roomNum > 8) {
                                    this->game->addMob(Data::Bolt, Images::SpriteImages[ObjectTypes::Bolt], Images::SpriteMasks[ObjectTypes::Bolt], tSpawnBarrier + x, 2);
                                }
                                break;

                            default: break;

                        }

                    }

                }

            }

        }
        else {
            
            gap--;
            
            if (gap == 0 && largeGap) {
                largeGapFinished = true;
                largeGap = false;
            }
            else {
                largeGapFinished = false;
            }

            if (hash(this->game->seed) % 5 == 0) {

                this->game->addMob(Data::Fireball, Images::Fireball, Images::Fireball_Mask, tSpawnBarrier + x, 16, floor - (hash(this->game->seed) % 4) + 1);

            }

        }


        // Increase upper platform counter if a platform is being rendered ..

        if (upperPlatform_X != Constants::NoPlatform) {

            upperPlatform_X++;

            if (upperPlatform_X == 4) {
                upperPlatform_X = Constants::NoPlatform;
            }

        }

    }


    // If this is the last room in a map, then add a pipe so we can leave ..

    if (roomNum == this->lastRoom) {

        addExit(this->game->mapNumber % 2 == MapLevel::AboveGround ? ObjectTypes::AboveGroundExit : ObjectTypes::UnderGroundExit, maxXTile() - 2, floor - 1);

    }

    if (tSpawnBarrier > spawnBarrier) spawnBarrier = tSpawnBarrier;
    

}

void Map::addExit(ObjectTypes exitType, int x, int y) {

    addObject(exitType, x, y);

}

void Map::addSign(int x, int y) {

    addObject(ObjectTypes::Sign,  x,   y);

}

void Map::addObject(ObjectTypes type, int tX, int tY) {

    if (checkObject(tX, tY)) {
        return;
    }
    else {

        objects[this->objectIndex].x = tX;
        objects[this->objectIndex].y = tY;
        objects[this->objectIndex].type = type;
        this->objectIndex++;

        if (this->objectIndex == Constants::MapObjects) this->objectIndex = 0;

    }

}

void Map::newMap() {

    // Reset Variables

    this->objectIndex = 0; 
    this->firstRoom = 0; 
    this->spawnBarrier = 0;

    for (uint8_t a = 0; a < Constants::MapObjects; a++) objects[a].deactivate();
    for (uint8_t a = 0; a < Constants::SpriteCap; a++) this->game->mobs[a].deactivate(false);

    // Seed for level length

    this->game->seed = this->game->seeds[ this->game->mapNumber % Constants::GameSeeds ] * this->game->mapNumber;
    this->lastRoom = Constants::MinLevelWidth + (hash(this->game->seed) % this->game->mapNumber); 



    for (uint8_t a = 0; a < Constants::MapRooms; a++) {
        generateRoom(a);
    }

}

void Map::loadMap() {

    int roomBeforePlayerIsIn = (this->game->player.x / (Constants::RoomWidth*Constants::TileSize)) - 1;

    if (roomBeforePlayerIsIn < 0) roomBeforePlayerIsIn = 0;

    if (roomBeforePlayerIsIn > this->firstRoom) {
        this->firstRoom = roomBeforePlayerIsIn;
        generateRoom(roomBeforePlayerIsIn + Constants::MapRooms - 1);
    }

}

bool Map::isTile(int x, int y) {

    int room = (x / Constants::RoomWidth) % Constants::MapRooms;
    return rooms[room].readTile(x % Constants::RoomWidth, y);

}


// Return first object found at coordinates x, y ..

uint8_t Map::checkObject(int x, int y) {

    if (y < 0) return 0;

    for (InteractiveObject &obj : this->objects) {
        if ((obj.x == x) && (obj.y == y)) return obj.type;
    }

    return 0;
}

int16_t Map::minXPixel() { return this->firstRoom * Constants::RoomWidth * Constants::TileSize; }
int16_t Map::maxXPixel() { return ((this->lastRoom + 1) * Constants::RoomWidth * Constants::TileSize) - 1; }
int16_t Map::maxYPixel() { return ((this->mapHeight + 1) * Constants::RoomHeight * Constants::TileSize) - 1; }
int16_t Map::minXTile() { return this->firstRoom * Constants::RoomWidth; }
int16_t Map::maxXTile() { return ((this->lastRoom + 1) * Constants::RoomWidth) - 1; }
int16_t Map::maxYTile() { return ((this->mapHeight + 1) * Constants::RoomHeight) - 1; }
