#include "solved.h"
#include "images.h"
#include "encourage.h"

void levelSolved()
{
    sprites.drawSelfMasked(8, 35, ManAndBox, 0);

    font4x6.setCursor(8, 3);
    font4x6.print(getRandomEncouragment());

    font4x6.setCursor(8, 13);
    font4x6.print(moves);
    font4x6.print(F(" moves for level "));
    font4x6.print(level);

    if(bestScore != 0xFFFF)
    {
        font4x6.setCursor(8, 23);
        font4x6.print(F("Previous best was "));
        font4x6.print(bestScore);
    }

    if (arduboy.justPressed(A_BUTTON))
    {
        if(level < MAX_LEVELS)
        {
            level++;
            gameState = STATE_LEVEL_INIT;
        }
        else
        {
            gameState = STATE_GAME_OVER;
        }
        arduboy.setRGBled(0, 0, 0);
    }
    else if (arduboy.justPressed(B_BUTTON))
    {
        level++;
        gameState = STATE_GAME_INTRO;
        arduboy.setRGBled(0, 0, 0);
    }
}