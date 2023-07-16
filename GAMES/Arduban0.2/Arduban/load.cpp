#include "load.h"
#include "levels.h"
#include "memory.h"
#include "game.h"

// For uint8_t
#include <stdint.h>

// Finds the player in the new board
void findPlayer()
{
    for(uint8_t r = 0; r < ROWS; r++)
    {
        for(uint8_t c = 0; c < COLUMNS; c++)
        {
            if(board[r][c] == PLAYER || board[r][c] == PLAYER_ON_GOAL)
            {
                pr = r;
                pc = c;
                return;
            }
        }
    }
}

void loadLevel()
{
    const uint8_t * levelData = Levels;
    for(size_t i = level; i > 1; --i)
    {
        const uint8_t levelSize = pgm_read_byte(levelData);
        levelData += levelSize + 1;
    }

    uint8_t row = 0;
    uint8_t column = 0;

    for(uint8_t data = pgm_read_byte(++levelData); row < ROWS; data = pgm_read_byte(levelData))
    {
        if(column >= COLUMNS)
        {
            ++row;
            column = 0;
        }

        const uint8_t count = ((data >> 4) & 0x0F);
        const uint8_t tile  = ((data >> 0) & 0x0F);

        for(uint8_t i = 0; i <= count; ++i)
        {
            board[row][column] = tile;
            ++column;
        }

        ++levelData;
    }

    findPlayer();
    arduboy.setRGBled(0, 0, 0);
    gameState = STATE_GAME_PLAY;
    reset();

    setLevel(level);
    bestScore = getMoves(level);
}