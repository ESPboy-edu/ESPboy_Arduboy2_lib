#include <Arduboy2.h>

// ----------------------------------------------------------------------------
//  Initialise state ..
//
void instructions_Init() {
    
    gameState = GameState::Instructions;
    gameStats.start = 0;
    
}   


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void instructions() {

    // Delay for 'good luck' ..

    if (gameStats.start < 56) { gameStats.start++; }


    // Handle player actions ..

    if (arduboy.justPressed(LEFT_BUTTON) && gameStats.instruction > 0) { 

        gameStats.instruction--;

    }         

    if (arduboy.justPressed(RIGHT_BUTTON) || arduboy.justPressed(A_BUTTON)) {

        if (gameStats.instruction < 2) { 

            gameStats.instruction++;
            gameStats.start = 0;

        }
        else {

            gameState = GameState::Game_Init;

        }

    }         


    // Render page ..

    Sprites::drawOverwrite(0, 0, Images::Instructions, 0);

    switch (gameStats.instruction) {

        case 0:
            Sprites::drawOverwrite(123, 0, Images::Arrow_Right, 0);

            font3x5.setCursor(0, 15);
            font3x5.print("Move~between~the~floor~tiles~to");

            font3x5.setCursor(0, 24);
            font3x5.print("turn~on~all~the~button~switches.");

            font3x5.setCursor(0, 38);
            font3x5.print("But~be~careful~though~as~some");

            font3x5.setCursor(0, 47);
            font3x5.print("floor~tiles~will~fall~when~crossed");

            font3x5.setCursor(0, 56);
            font3x5.print("preventing~you~from~back-tracking!");

            break;

        case 1:
            Sprites::drawOverwrite(116, 0, Images::Arrow_Left, 0);
            Sprites::drawOverwrite(123, 0, Images::Arrow_Right, 0);

            font3x5.setCursor(0, 13);
            font3x5.print("Floor Tiles");

            Sprites::drawOverwrite(0, 27, Images::Tiles, 1 + (gameStats.tileSet * 7));
            font3x5.setCursor(10, 27);
            font3x5.print("Normal:~falls~away~once~crossed.");

            Sprites::drawOverwrite(0, 39, Images::Tiles, 4 + (gameStats.tileSet * 7));
            font3x5.setCursor(10, 39);
            font3x5.print("Double:~can~be~crossed~twice.");

            Sprites::drawOverwrite(0, 51, Images::Tiles, 3 + (gameStats.tileSet * 7));
            font3x5.setCursor(10, 51);
            font3x5.print("Solid:~never~falls~when~crossed.");
            break;

        case 2:
            Sprites::drawOverwrite(116, 0, Images::Arrow_Left, 0);
            Sprites::drawOverwrite(123, 0, Images::Arrow_Right, 0);

            font3x5.setCursor(0, 13);
            font3x5.print("Button Switches");

            Sprites::drawOverwrite(0, 27, Images::Tiles, 5 + (gameStats.tileSet * 7));
            font3x5.setCursor(10, 27);
            font3x5.print("Off:~switch~is~in~OFF~position.");

            Sprites::drawOverwrite(0, 39, Images::Tiles, 6 + (gameStats.tileSet * 7));
            font3x5.setCursor(10, 39);
            font3x5.print("On:~switch~is~in~ON~position.");

            if (gameStats.start >= 56) {
            
                font3x5.setCursor(33, 56);
                font3x5.print("Press~A~to~start~!");

            }
            else {
            
                font3x5.setCursor(13, 56);
                font3x5.print("Hold~A~to~zoom~and~B~to~quit.");

            }

            break;

    }

    arduboy.drawFastHLine(0, 8, 128, WHITE);

}
