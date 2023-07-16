#include "intro.h"
#include "images.h"
#include "memory.h"

bool playedIntro = false;

#define TUNE_NOTE_LENGTH 250

const uint16_t tune[] PROGMEM = {
    NOTE_C4, TUNE_NOTE_LENGTH,
    NOTE_E4, TUNE_NOTE_LENGTH,
    NOTE_G4, TUNE_NOTE_LENGTH,
    NOTE_F4, TUNE_NOTE_LENGTH,
    NOTE_E4, TUNE_NOTE_LENGTH,
    NOTE_F4, TUNE_NOTE_LENGTH,
    NOTE_C4, TUNE_NOTE_LENGTH * 2,
    TONES_END
};

void gameIntro()
{
    if(!playedIntro)
    {
        playedIntro = true;
        sound.tones(tune);
    }

    sprites.drawSelfMasked(8, 6, Arduban, 0);
    sprites.drawSelfMasked(8, 35, ManAndBox, 0);

    font4x6.setCursor(77, 40);
    font4x6.print(F("Level "));
    font4x6.print(level);

    font4x6.setCursor(83, 48);
    uint16_t moves = getMoves(level);
    if(moves == 0xFFFF)
    {
        font4x6.print(F("Unsolved"));
    }
    else
    {
        font4x6.print(moves);
        font4x6.print(F(" moves"));
    }

    if(arduboy.justPressed(A_BUTTON))
    {
        sound.tone(NOTE_C3, NOTE_LENGTH);
        gameState = STATE_LEVEL_INIT;
    }
    else if(arduboy.justPressed(B_BUTTON))
    {
        sound.tone(NOTE_C3, NOTE_LENGTH);
        gameState = STATE_SETTINGS;
    }
    else if(arduboy.justPressed(UP_BUTTON) && level > 1)
    {
        sound.tone(NOTE_C3, NOTE_LENGTH);
        level--;
    }
    else if(arduboy.justPressed(DOWN_BUTTON) && level < MAX_LEVELS)
    {
        sound.tone(NOTE_C3, NOTE_LENGTH);
        level++;
    }
    else if(arduboy.justPressed(LEFT_BUTTON))
    {
        sound.tone(NOTE_C3, NOTE_LENGTH);
        while(level > 1 && isLevelSolved(level))
            level--;
    }
    else if(arduboy.justPressed(RIGHT_BUTTON))
    {
        sound.tone(NOTE_C3, NOTE_LENGTH);
        while(level < MAX_LEVELS && isLevelSolved(level))
            level++;
    }
}