#include "settings.h"
#include "memory.h"

uint8_t setting = 0;
bool confirm = false;

void toggleSound()
{
    arduboy.audio.toggle();
    arduboy.audio.saveOnOff();
}

void clearSettings()
{
    resetLevels();
    level = 1;
}

void settings()
{
    font4x6.setCursor(0, 10);
    font4x6.println(F("Settings"));

    font4x6.setCursor(5, 28);
    font4x6.print(setting == 0 ? '>' : ' ');
    font4x6.print(F("Turn sound "));
    if(arduboy.audio.enabled())
        font4x6.print(F("off"));
    else
        font4x6.print(F("on"));


    font4x6.setCursor(5, 38);
    font4x6.print(setting == 1 ? '>' : ' ');
    font4x6.print(F("Clear all scores"));

    if(confirm)
    {
        font4x6.setCursor(5, 56);
        font4x6.print(F("Are you sure?"));
    }

    if(arduboy.justPressed(A_BUTTON))
    {
        sound.tone(NOTE_E3, NOTE_LENGTH);
        if(confirm && setting == 1)
        {
            clearSettings();
            confirm = false;
        }
        else if(setting == 1)
        {
            confirm = true;
        }
        else if(setting == 0)
        {
            toggleSound();
        }
    }
    if(arduboy.justPressed(B_BUTTON))
    {
        sound.tone(NOTE_C1, NOTE_LENGTH);
        if(confirm)
            confirm = false;
        else
            gameState = STATE_GAME_INTRO;
    }
    else if(!confirm && arduboy.justPressed(UP_BUTTON))
    {
        sound.tone(NOTE_C3, NOTE_LENGTH);
        setting = setting == 0 ? 1 : 0;
    }
    else if(!confirm && arduboy.justPressed(DOWN_BUTTON))
    {
        sound.tone(NOTE_C3, NOTE_LENGTH);
        setting = setting == 1 ? 0 : 1;
    }
}