#include "globals.h"
#include "levels.h"

Arduboy2Base arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Sprites sprites;
Font4x6 font4x6 = Font4x6();
uint16_t moves = 0;
uint16_t bestScore = 0xFFFF;

byte gameState = STATE_GAME_INTRO;

uint16_t level = 1;
uint8_t frame = 0;

// Player column and row
int8_t pr = 0;
int8_t pc = 0;

// The current board/level we are playing
byte board[ROWS][COLUMNS];

#if DEBUG
// Helper function to dump a byte as a hex value to Serial
void printHex(byte b)
{
    Serial.print(b < 0x10 ? F("0x0") : F("0x"));
    Serial.print(b, HEX);
}

// Helper routine to dump a byte array as hex values to Serial.
void printHex(byte *buffer, byte bufferSize)
{
  for (byte i = 0; i < bufferSize; i++)
  {
    printHex(buffer[i]);
  }
}
#endif