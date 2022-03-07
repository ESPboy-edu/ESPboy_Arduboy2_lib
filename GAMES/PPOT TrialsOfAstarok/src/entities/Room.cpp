#include "Room.h"

void Room::clearRoom() {
    for (int a = 0; a < Constants::RoomBytes; a++) data[a] = 0;
}

void Room::setTile(uint8_t x, uint8_t y) {

    if (x > Constants::RoomWidth || y > Constants::RoomHeight) return;
    data[x]     |= 1 << y;
}

void Room::clearTile(uint8_t x, uint8_t y) {

    if (x > Constants::RoomWidth || y > Constants::RoomHeight) return;
    data[x]     &= ~(1 << y);

}

bool Room::readTile(uint8_t x, uint8_t y) {

    if (x > Constants::RoomWidth || y > Constants::RoomHeight) return false;
    return (data[x] & (1 << y)) > 0;

}
