#include "game.h"
#include "levels.h"
#include "images.h"
#include "memory.h"
#include "encourage.h"

#define FRAMES_TO_RESET      90
#define EXPLODE_FRAMES        6

// Undo buffer defines
#define UP      0x01
#define DOWN    0x02
#define LEFT    0x04
#define RIGHT   0x08
#define PUSH    0x10
#define MOVE    0x00

#if DEBUG
#define MAX_UNDO             10
#else
#define MAX_UNDO            128
#endif

byte undoBuffer[MAX_UNDO];
uint8_t undoCount = 0;

uint8_t reset_count = 0;
bool exploded = false;

bool offScreen(int8_t r, int8_t c)
{
    return (r == -1 || r == ROWS || c == -1 || c == COLUMNS);
}

void move(int8_t x, int8_t y)
{
    int8_t r = pr + y;
    int8_t c = pc + x;

    if(offScreen(r,c))
    {
        sound.tone(NOTE_C1, NOTE_LENGTH);
        return;
    }

    switch (board[r][c])
    {
    case FLOOR:
        board[r][c] = PLAYER;
        sound.tone(NOTE_C3, NOTE_LENGTH);
        moves++;
        undoBuffer[moves % MAX_UNDO] = MOVE;
        break;
    case GOAL:
        sound.tone(NOTE_E3, NOTE_LENGTH);
        board[r][c] = PLAYER_ON_GOAL;
        moves++;
        undoBuffer[moves % MAX_UNDO] = MOVE;
        break;
    case BOX:
    case BOX_ON_GOAL:
        {
            // Need to look one square further then push box
            uint8_t r2 = r + y;
            uint8_t c2 = c + x;

            if(offScreen(r2,c2) ||
            board[r2][c2] == WALL ||
            board[r2][c2] == BOX ||
            board[r2][c2] == BOX_ON_GOAL)
            {
                sound.tone(NOTE_C1, NOTE_LENGTH);
                return;
            }

            if(board[r2][c2] == GOAL)
                sound.tone(NOTE_C3, NOTE_LENGTH, NOTE_E3, NOTE_LENGTH);
            else
                sound.tone(NOTE_C3, NOTE_LENGTH);

            // Push the box
            board[r2][c2] = board[r2][c2] == GOAL ? BOX_ON_GOAL : BOX;
            board[r][c] = board[r][c] == BOX_ON_GOAL ? PLAYER_ON_GOAL : PLAYER;
            moves++;
            undoBuffer[moves % MAX_UNDO] = PUSH;
        }
        break;
    case WALL:
    default:
        return;
    }

    // Set the board based on where the player WAS
    board[pr][pc] = board[pr][pc] == PLAYER_ON_GOAL ? GOAL : FLOOR;
    pr = r;
    pc = c;

    if(x == 1)
        undoBuffer[moves % MAX_UNDO] |= LEFT;
    else if(x == -1)
        undoBuffer[moves % MAX_UNDO] |= RIGHT;
    else if(y == 1)
        undoBuffer[moves % MAX_UNDO] |= DOWN;
    else if(y == -1)
        undoBuffer[moves % MAX_UNDO] |= UP;

    if(undoCount < MAX_UNDO)
        undoCount++;
}

// This will reset the level after FRAMES_TO_RESET of the A button being held down
void explode()
{
    if (exploded) return;

    reset_count++;
    if(reset_count > FRAMES_TO_RESET)
    {
        gameState = STATE_LEVEL_INIT;
        exploded = true;
        reset_count = 0;
        return;
    }
    if(reset_count < FRAMES_TO_RESET * 0.9)
        arduboy.setRGBled(reset_count * 255 / FRAMES_TO_RESET, 0, 0);
    else
        arduboy.setRGBled(255, 255, 255);

    sound.tone(NOTE_C3 + reset_count, NOTE_LENGTH);
}

void undo(int8_t x, int8_t y, bool push)
{
#if DEBUG
    Serial.print(F("Undo x=");
    Serial.print(x);
    Serial.print(F(", y=");
    Serial.print(y);
    Serial.print(F(", p=");
    Serial.println(push);
#endif

    int8_t r = pr + y;
    int8_t c = pc + x;

    switch (board[r][c])
    {
    case FLOOR:
        board[r][c] = PLAYER;
        sound.tone(NOTE_C3, NOTE_LENGTH);
        break;
    case GOAL:
        sound.tone(NOTE_E3, NOTE_LENGTH);
        board[r][c] = PLAYER_ON_GOAL;
        break;
    case WALL:
    case BOX:
    case BOX_ON_GOAL:
    default:
        // This should never happen!
#if DEBUG
        Serial.print(F("Invalid square ");
        Serial.println(board[r][c]);
#endif
        return;
    }

    // Set the board based on where the player WAS
    board[pr][pc] = board[pr][pc] == PLAYER_ON_GOAL ? GOAL : FLOOR;

    // Are we pulling a box?
    int8_t br = pr - y;
    int8_t bc = pc - x;
    if(push && board[br][bc] == BOX || board[br][bc] == BOX_ON_GOAL)
    {
        if(board[pr][pc] == GOAL)
            sound.tone(NOTE_C3, NOTE_LENGTH, NOTE_E2, NOTE_LENGTH);
        else
            sound.tone(NOTE_C3, NOTE_LENGTH);

        // Pull the box
        board[pr][pc] = board[pr][pc] == GOAL ? BOX_ON_GOAL : BOX;
        board[br][bc] = board[br][bc] == BOX_ON_GOAL ? GOAL : FLOOR;
    }

    pr = r;
    pc = c;
    moves--;
    undoCount--;
}

#if DEBUG
void printUndo(byte move)
{
    if(move == 0x00)
    {
        Serial.print(F("--");
        return;
    }
    Serial.print((move & PUSH) == PUSH ? 'P' : 'M');
    if((move & LEFT) == LEFT)
        Serial.print('L');
    else if((move & RIGHT) == RIGHT)
        Serial.print('R');
    else if((move & DOWN) == DOWN)
        Serial.print('D');
    else if((move & UP) == UP)
        Serial.print('U');
}
#endif

void undo()
{
#if DEBUG
    Serial.println(F("Undo buffer;");
    for(int i = 0; i < MAX_UNDO; i++)
    {
        printUndo(undoBuffer[i]);
        Serial.print(F(", ");
    }
    Serial.println();

    Serial.print(F("Undo move ");
    Serial.print(moves % MAX_UNDO);
    Serial.print(F(", ");
    printUndo(undoBuffer[moves % MAX_UNDO]);
    Serial.println();
#endif

    if(undoCount == 0)
        return; // No more undos left

    byte move = undoBuffer[moves % MAX_UNDO];
    bool push = (move & PUSH) == PUSH;
    if((move & LEFT) == LEFT)
        undo(-1, 0, push);
    else if((move & RIGHT) == RIGHT)
        undo(1, 0, push);
    else if((move & DOWN) == DOWN)
        undo(0, -1, push);
    else if((move & UP) == UP)
        undo(0, 1, push);
}

void move()
{
    if(arduboy.justPressed(A_BUTTON))
        undo();
    else if(arduboy.pressed(A_BUTTON))
        explode();
    else if(arduboy.justReleased(A_BUTTON))
    {
        arduboy.setRGBled(0, 0, 0);
        exploded = false;
        reset_count = 0;
    }
    else if(arduboy.justPressed(UP_BUTTON))
        move(0, -1);
    else if(arduboy.justPressed(DOWN_BUTTON))
        move(0, 1);
    else if(arduboy.justPressed(RIGHT_BUTTON))
        move(1, 0);
    else if(arduboy.justPressed(LEFT_BUTTON))
        move(-1, 0);
    else if(arduboy.justPressed(B_BUTTON))
        gameState = STATE_GAME_INTRO;
}

bool isSolved()
{
    // Make sure all boxes are on goals
    for(int8_t r = 0; r < ROWS; r++)
    {
        for(int8_t c = 0; c < COLUMNS; c++)
        {
            if(board[r][c] == BOX)
                return false;
        }
    }
    setMoves(level, moves);
    arduboy.setRGBled(0, 128, 0);
    setRandomEncouragement();
    return true;
}

void drawBoard()
{
    for(int8_t r = 0; r < ROWS; r++)
    {
        for(int8_t c = 0; c < COLUMNS; c++)
        {
            uint8_t x = c * WIDTH / COLUMNS;
            uint8_t y = r * HEIGHT / ROWS;
            switch (board[r][c])
            {
            case FLOOR:
                break;
            case PLAYER:
            case PLAYER_ON_GOAL:
                if(reset_count > 0)
                {
                    sprites.drawSelfMasked(x, y, Explode, reset_count / (FRAMES_TO_RESET / EXPLODE_FRAMES));
                }
                else
                {
                    sprites.drawSelfMasked(x, y, Player, frame / 20 % 4);
                }
                break;
            case WALL:
                sprites.drawSelfMasked(x, y, Wall, 0);
                break;
            case BOX:
                sprites.drawSelfMasked(x, y, Box, 0);
                break;
            case BOX_ON_GOAL:
                sprites.drawSelfMasked(x, y, BoxOnGoal, frame / 30 % 2);
                break;
            case GOAL:
                sprites.drawSelfMasked(x, y, Goal, frame / 15 % 4);
                break;
            }
        }
    }
}

void gamePlay()
{
    move();
    if(isSolved())
    {
        sound.tone(NOTE_C4, NOTE_LENGTH, NOTE_E4, NOTE_LENGTH);
        gameState = STATE_LEVEL_SOLVED;
    }
    drawBoard();
}

void reset()
{
    moves = 0;
    undoCount = 0;
    reset_count = 0;
}