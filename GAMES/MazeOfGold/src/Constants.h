#pragma once

namespace Constants { 

	static constexpr uint8_t DELAY = 12;

	static constexpr uint8_t TileSize = 12;
	static constexpr uint8_t ScreenTileWidth = (128 / TileSize) + 1;
	static constexpr uint8_t ScreenTileHeight = (64 / TileSize) + 1;

	static constexpr int8_t DirectionOffsetX[4] = {0, 1, 0, -1};
	static constexpr int8_t DirectionOffsetY[4] = {-1, 0, 1, 0};
	static constexpr uint8_t MazeHeight = 23;
	static constexpr uint8_t MazeWidth = 23;
	static constexpr uint8_t MaxChests = 10;
	static constexpr uint8_t MaxEnemys = 12;
	static constexpr uint8_t MaxItems = 7; // Keep odd so there is a spare one at all times.
	static constexpr uint8_t NoItem = 255;
	static constexpr uint8_t InventoryCount = 10;

	//                                           0  1  2  3  4  5  6  7  8  9  0  1  2
	static constexpr uint8_t BombX[] PROGMEM = { 7, 6, 5, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0 };
	static constexpr uint8_t BombY[] PROGMEM = { 1, 0, 0, 0, 1, 2, 2, 2, 1, 1, 1, 1, 1 };

}
