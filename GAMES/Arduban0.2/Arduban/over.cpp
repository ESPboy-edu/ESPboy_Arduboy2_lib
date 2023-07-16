#include "over.h"
#include "images.h"
#include "encourage.h"

void gameOver()
{
    sprites.drawSelfMasked(8, 35, ManAndBox, 0);

    if(arduboy.everyXFrames(60))
        setRandomEncouragement();

    font4x6.setCursor(8, 3);
    font4x6.print(getRandomEncouragment());

    font4x6.setCursor(8, 13);
    font4x6.println(F("All levels complete!"));

    font4x6.setCursor(8, 23);
    font4x6.println(F("Improve your scores?"));

    if(arduboy.justPressed(A_BUTTON))
        gameState = STATE_GAME_INTRO;
}