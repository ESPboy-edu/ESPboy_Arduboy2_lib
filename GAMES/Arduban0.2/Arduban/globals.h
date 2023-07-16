#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Arduboy2.h>
#include <ArduboyTones.h>
#include "Font4x6.h"

// TODO: Set this to the number of levels
#define MAX_LEVELS 330

// For serial port debugging
#define DEBUG               0

// The default tone length
#define NOTE_LENGTH       150

// Game State
#define STATE_GAME_INTRO    0
#define STATE_LEVEL_INIT    1
#define STATE_GAME_PLAY     2
#define STATE_LEVEL_SOLVED  3
#define STATE_GAME_OVER     4
#define STATE_SETTINGS      5

// Board definition
#define COLUMNS            16
#define ROWS                8

#define WALL            0x01    // #
#define PLAYER          0x02    // @
#define PLAYER_ON_GOAL  0x03    // +
#define BOX             0x04    // $
#define BOX_ON_GOAL     0x05    // *
#define GOAL            0x06    // .
#define FLOOR           0x07    //

// The current board/level we are playing
extern byte board[ROWS][COLUMNS];

extern Arduboy2Base arduboy;
extern ArduboyTones sound;
extern Sprites sprites;
extern Font4x6 font4x6;

extern byte gameState;
extern uint16_t level;
extern uint8_t frame;
extern uint16_t moves;
extern uint16_t bestScore;

// Player column and row
extern int8_t pr;
extern int8_t pc;

// Helper function to dump a byte as a hex value to Serial
void printHex(byte b);

// Helper routine to dump a byte array as hex values to Serial.
void printHex(byte *buffer, byte bufferSize);

#endif