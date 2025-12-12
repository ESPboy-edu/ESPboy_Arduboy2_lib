#include <Arduboy2.h>

void renderBoard_Zoom() {


    int16_t worldX = 64 - (player.getX() * Constants::CellWidth_Zoom_PlusBorder) + player.getXOffset() - (Constants::CellWidth_Zoom_PlusBorder / 2) - player.getXOffset() + gameStats.xOffset;
    int16_t worldY = 32 - (player.getY() * Constants::CellHeight_Zoom_PlusBorder) + player.getYOffset() + (Constants::CellHeight_Zoom_PlusBorder / 2) - player.getYOffset();

    for (uint16_t y = 0; y < Constants::BoardHeight; y++){

        for (uint16_t x = 0; x < Constants::BoardWidth; x++){

            if (board[y][x] > static_cast<uint8_t>(Tiles::None)) {
                Sprites::drawExternalMask(worldX + (x * Constants::CellWidth_Zoom_PlusBorder), 
                                        worldY + (y * Constants::CellHeight_Zoom_PlusBorder), 
                                        Images::Tiles_Zoom, Images::Tile_Zoom_Mask, board[y][x] + (gameStats.tileSet * 7), 0);
            }

        }

    }


    // Render falling tiles ..
                
    for (FallingTile &fallingTile : fallingTiles) {

        if (fallingTile.isActive()) {

            Sprites::drawExternalMask(worldX + (fallingTile.getX() * Constants::CellWidth_Zoom_PlusBorder), 
                                      worldY + (fallingTile.getY() * Constants::CellHeight_Zoom_PlusBorder) + fallingTile.getYOffset(), 
                                      Images::Player_Zoom_Dying, Images::Player_Zoom_Dying_Mask, 0, 0);
            
        }

    }


    // Render arrows .. (if defined)

    for (Arrow &arrow : arrows) {

        if (arrow.getX() > 0 || arrow.getY() > 0) {
    
            Sprites::drawExternalMask(worldX + (arrow.getX() * Constants::CellWidth_Zoom_PlusBorder) - 2, 
                                      worldY + (arrow.getY() * Constants::CellHeight_Zoom_PlusBorder) + arrow.getYOffset() + 3, 
                                      Images::Arrow_Zoom, Images::Arrow_Zoom_Mask, 0, 0);


        }

    }

    // Render player ..

    if (gameStats.xOffset == 0) {

        uint8_t frame = arduboy.getFrameCount(32) / 8;

        if (player.isDying()) {

            if (player.isMoving()) {

                Sprites::drawExternalMask(worldX + (player.getX() * Constants::CellWidth_Zoom_PlusBorder) + player.getXOffset(), 
                                          worldY + (player.getY() * Constants::CellHeight_Zoom_PlusBorder) + player.getYOffset() - 1, 
                                          Images::Player_Zoom, Images::Player_Zoom_Mask, frame, frame);

            }
            else {

                Sprites::drawExternalMask(worldX + (player.getX() * Constants::CellWidth_Zoom_PlusBorder), 
                                          worldY + (player.getY() * Constants::CellHeight_Zoom_PlusBorder) + player.getYDyingOffset_1() - 1, 
                                          Images::Player_Zoom_Dying, Images::Player_Zoom_Dying_Mask, frame, frame);

            }
    
            Sprites::drawExternalMask(worldX + (player.getXOld() * Constants::CellWidth_Zoom_PlusBorder), 
                                      worldY + (player.getYOld() * Constants::CellHeight_Zoom_PlusBorder) + player.getYDyingOffset_2() - 1, 
                                      Images::Player_Zoom_Dying, Images::Player_Zoom_Dying_Mask, frame, frame);

        }
        else {

            Sprites::drawExternalMask(worldX + (player.getX() * Constants::CellWidth_Zoom_PlusBorder) + player.getXOffset(), 
                                      worldY + (player.getY() * Constants::CellHeight_Zoom_PlusBorder) + player.getYOffset() - 1, 
                                      Images::Player_Zoom, Images::Player_Zoom_Mask, frame, frame);

        }
        
    }

}


void renderBoard_Normal() {

    for (uint16_t y = 0; y < Constants::BoardHeight; y++){

        for (uint16_t x = 0; x < Constants::BoardWidth; x++){

            if (board[y][x] > static_cast<uint8_t>(Tiles::None)) {
                Sprites::drawExternalMask(Constants::Board_XOffset + gameStats.xOffset + (x * Constants::CellWidth_PlusBorder), 
                                        Constants::Board_YOffset + (y * Constants::CellHeight_PlusBorder) + gameStats.yOffset, 
                                        Images::Tiles, Images::Tile_Mask, board[y][x] + (gameStats.tileSet * 7), 0);
            }

        }

    }


    // Render falling tiles ..
                
    for (FallingTile &fallingTile : fallingTiles) {

        if (fallingTile.isActive()) {

            Sprites::drawExternalMask(Constants::Board_XOffset + (fallingTile.getX() * Constants::CellWidth_PlusBorder), 
                                      Constants::Board_YOffset + (fallingTile.getY() * Constants::CellHeight_PlusBorder) + fallingTile.getYOffset() + gameStats.yOffset, 
                                      Images::Player_Dying, Images::Player_Mask, 0, 0);
            
        }

    }


    // Render arrows .. (if defined)

    for (Arrow &arrow : arrows) {

        if (arrow.getX() > 0 || arrow.getY() > 0) {
    
            Sprites::drawExternalMask(Constants::Board_XOffset + gameStats.xOffset + (arrow.getX() * Constants::CellWidth_PlusBorder) - 1, 
                                    Constants::Board_YOffset + (arrow.getY() * Constants::CellHeight_PlusBorder) + arrow.getYOffset() + gameStats.yOffset - 1, 
                                    Images::Arrow, Images::Arrow_Mask, 0, 0);


        }

    }


    // Render player ..

    if (gameStats.xOffset == 0) {

        uint8_t frame = arduboy.getFrameCount(32) / 8;

        if (player.isDying()) {

            if (player.isMoving()) {

                Sprites::drawExternalMask(Constants::Board_XOffset + (player.getX() * Constants::CellWidth_PlusBorder) + player.getXOffset(), 
                                          Constants::Board_YOffset + (player.getY() * Constants::CellHeight_PlusBorder) + player.getYOffset() + gameStats.yOffset - 1, 
                                          Images::Player, Images::Player_Mask, frame, frame);

            }
            else {

                Sprites::drawExternalMask(Constants::Board_XOffset + (player.getX() * Constants::CellWidth_PlusBorder), 
                                          Constants::Board_YOffset + (player.getY() * Constants::CellHeight_PlusBorder) + player.getYDyingOffset_1() + gameStats.yOffset - 1, 
                                          Images::Player_Dying, Images::Player_Mask, frame, frame);

            }
    
            Sprites::drawExternalMask(Constants::Board_XOffset + (player.getXOld() * Constants::CellWidth_PlusBorder), 
                                      Constants::Board_YOffset + (player.getYOld() * Constants::CellHeight_PlusBorder) + player.getYDyingOffset_2() + gameStats.yOffset - 1, 
                                      Images::Player_Dying, Images::Player_Mask, frame, frame);

        }
        else {

            Sprites::drawExternalMask(Constants::Board_XOffset + (player.getX() * Constants::CellWidth_PlusBorder) + player.getXOffset(), 
                                      Constants::Board_YOffset + (player.getY() * Constants::CellHeight_PlusBorder) + player.getYOffset() + gameStats.yOffset - 1, 
                                      Images::Player, Images::Player_Mask, frame, frame);

        }
        
    }

}


void renderHUD() {

    arduboy.fillRect(0, 0, 128, 10, BLACK);

    Sprites::drawOverwrite(0, 0, Images::Level, 0);

    {
        uint8_t digits[3] = {};
        uint8_t x = 45;
        
        extractDigits(digits, gameStats.level);

        for (uint8_t i = 0; i < 3; ++i) {

            if (digits[i] == 1) x = x - 3;

        }

        for (uint8_t i = 0; i < 3; ++i) {
            Sprites::drawOverwrite(x, 0, Images::Numbers, digits[i]);
            x = x - (digits[i] == 1 ? 5 : 8);
        }

    }


    Sprites::drawOverwrite(71, 0, Images::Moves, 0);

    {
        uint8_t digits[3] = {};
        uint8_t x = 121;

        extractDigits(digits, gameStats.moves < 999 ? gameStats.moves : 999);

        for (uint8_t i = 0; i < 3; ++i) {

            if (digits[i] == 1) x = x - 3;

        }
        
        for (uint8_t i = 0; i < 3; ++i) {
            Sprites::drawOverwrite(x, 0, Images::Numbers, digits[i]);
            x = x - (digits[i] == 1 ? 5 : 8);
        }

    }

    arduboy.drawFastHLine(0, 8, 128, WHITE);


}
