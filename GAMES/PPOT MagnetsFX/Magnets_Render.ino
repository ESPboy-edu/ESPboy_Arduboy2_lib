#include <ArduboyFX.h>  



void renderGrid(uint8_t currentPlane, bool renderGuides) {

    bool flash = false;

    uint8_t xOffset_Cols[] = { 11, 10 };
    uint8_t yOffset_Rows[] = { 10, 9 };
    uint8_t xOffset_Numbers = 0;
    uint8_t yOffset_Numbers = 0;
    uint8_t yOffset_Overall = 0;

    if (frameCount % 64 < 32 && (gameState == GameState::Play || gameState == GameState::Play_Hint0 || gameState == GameState::Play_Hint1)) {

        flash = true;

    }


    // Determine overall Y offset ..

    switch (game.getRenderSize()) {

        case RenderSize::Small:

            switch (game.getGameSize()) {
                
                case GameSize::Small:
                case GameSize::Medium:

                    yOffset_Overall = 0;
                    break;
                
                case GameSize::Large:

                    switch (game.getY()) {
                        
                        case 0 ... 5:       
                            yOffset_Overall = 0;
                            break;

                        case 6:             
                            yOffset_Overall = 6;
                            break;

                        default:            
                            yOffset_Overall = 12;
                            break;

                    }

                    break;
                    
            } 

            break;

        case RenderSize::Large:

            switch (game.getGameSize()) {
                
                case GameSize::Small:

                    yOffset_Overall = 0;
                    break;

                case GameSize::Medium:

                    switch (game.getY()) {
                        
                        case 0 ... 5:       
                            yOffset_Overall = 0;
                            break;

                        case 6:             
                            yOffset_Overall = 9;
                            break;

                        default:            
                            yOffset_Overall = 18;
                            break;

                    }

                    break;
                
                case GameSize::Large:

                    switch (game.getY()) {
                        
                        case 0 ... 3:       
                            yOffset_Overall = 0;
                            break;

                        case 4 ...7:             
                            yOffset_Overall = (game.getY() - 4) * 9;
                            break;

                        default:            
                            yOffset_Overall = 36;
                            break;

                    }

                    break;
                    
            } 

            break;

    }

    xOffset_Numbers = xOffset_Cols[static_cast<uint8_t>(game.getRenderSize())];
    yOffset_Numbers = yOffset_Rows[static_cast<uint8_t>(game.getRenderSize())];


    // + Cols

    for (uint8_t i = 1; i <= game.getWidth(); i++) {

        uint8_t val = game.puzzle[0][i];
        uint8_t sol = game.solution[0][i];

        if (sol != Constants::NoNumber) {
                
            if (val == sol) {

                SpritesU::drawOverwrite(xOffset_Numbers + ((i - 1) * game.getSpacing()), -yOffset_Overall, Images::Numbers_5x3_1D_W, (sol * 3) + currentPlane);

            }
            else {

                SpritesU::drawOverwrite(xOffset_Numbers + ((i - 1) * game.getSpacing()), -yOffset_Overall, Images::Numbers_5x3_1D_G, (sol * 3) + currentPlane);

            }

        }

    }


    // + Rows

    for (uint8_t i = 1; i <= game.getHeight(); i++) {

        uint8_t val = game.puzzle[i][0];
        uint8_t sol = game.solution[i][0];

        if (sol != Constants::NoNumber) {

            if (val == sol) {

                SpritesU::drawOverwrite(2, -yOffset_Overall + yOffset_Numbers + ((i - 1) * game.getSpacing()), Images::Numbers_5x3_1D_W, (sol * 3) + currentPlane);

            }
            else {

                SpritesU::drawOverwrite(2, -yOffset_Overall + yOffset_Numbers + ((i - 1) * game.getSpacing()), Images::Numbers_5x3_1D_G, (sol * 3) + currentPlane);

            }

        }

    }


    // - Cols
        
    for (uint8_t i = 1; i <= game.getWidth(); i++) {

        uint8_t val = game.puzzle[game.getHeight() + 1][i];
        uint8_t sol = game.solution[game.getHeight() + 1][i];

        if (sol != Constants::NoNumber) {

            if (val == sol) {

                SpritesU::drawOverwrite(xOffset_Numbers + ((i - 1) * game.getSpacing()), -yOffset_Overall + 11 + (game.getHeight() * game.getSpacing()), Images::Numbers_5x3_1D_W, (sol * 3) + currentPlane);

            }
            else {

                SpritesU::drawOverwrite(xOffset_Numbers + ((i - 1) * game.getSpacing()), -yOffset_Overall + 11 + (game.getHeight() * game.getSpacing()), Images::Numbers_5x3_1D_G, (sol * 3) + currentPlane);

            }

        }

    }


    // - Rows

    for (uint8_t i = 1; i <= game.getHeight(); i++) {

        uint8_t val = game.puzzle[i][game.getWidth() + 1];
        uint8_t sol = game.solution[i][game.getWidth() + 1];
        
        if (sol != Constants::NoNumber) {

            if (val == sol) {

                SpritesU::drawOverwrite(12 + (game.getWidth() * game.getSpacing()), -yOffset_Overall + yOffset_Numbers + ((i - 1) * game.getSpacing()), Images::Numbers_5x3_1D_W, (sol * 3) + currentPlane);

            }
            else {

                SpritesU::drawOverwrite(12 + (game.getWidth() * game.getSpacing()), -yOffset_Overall + yOffset_Numbers + ((i - 1) * game.getSpacing()), Images::Numbers_5x3_1D_G, (sol * 3) + currentPlane);

            }

        }

    }

    SpritesU::drawOverwriteFX(0, -yOffset_Overall, Images::Plus, currentPlane);
    SpritesU::drawOverwriteFX(9 + (game.getWidth() * game.getSpacing()), -yOffset_Overall + 9 + (game.getHeight() * game.getSpacing()), Images::Minus, currentPlane);


    
    for (uint8_t y = 1; y < game.getHeight() + 1; y++) {

        for (uint8_t x = 1; x < game.getWidth() + 1; x++) {

            uint8_t i = game.puzzle[y][x];
 

            // Tileset to use?

            uint8_t tileSet = 0;

            switch (game.getRenderSize()) {

                case RenderSize::Small:
                    tileSet = 6;
                    break;

                case RenderSize::Large:
                    tileSet = 0;
                    break;

            }


            // High contrast ..

            tileSet = tileSet + 12;

            switch (i) {

                case 1:
                    if (flash && ((game.getX() == x && game.getY() == y) || (game.getX() - 1 == x && game.getY() == y))) { tileSet = tileSet + 3; }
                    SpritesU::drawPlusMaskFX(9 + ((x - 1) * game.getSpacing()), -yOffset_Overall + 9 + ((y - 1) * game.getSpacing()), Images::Tile_0102, tileSet + currentPlane);
                    x++;
                    break;

                case 4:
                    if (flash && ((game.getX() == x && game.getY() == y) || (game.getX() - 1 == x && game.getY() == y))) { tileSet = tileSet + 3; }
                    SpritesU::drawPlusMaskFX(9 + ((x - 1) * game.getSpacing()), -yOffset_Overall + 9 + ((y - 1) * game.getSpacing()), Images::Tile_0403, tileSet + currentPlane);
                    x++;
                    break;

                case 11:
                    if (flash && ((game.getX() == x && game.getY() == y) || (game.getX() == x && game.getY() - 1 == y))) { tileSet = tileSet + 3; }
                    SpritesU::drawPlusMaskFX(9 + ((x - 1) * game.getSpacing()), -yOffset_Overall + 9 + ((y - 1) * game.getSpacing()), Images::Tile_1112, tileSet + currentPlane);
                    break;

                case 22:
                    if (flash && ((game.getX() == x && game.getY() == y) || (game.getX() == x && game.getY() - 1 == y))) { tileSet = tileSet + 3; }
                    SpritesU::drawPlusMaskFX(9 + ((x - 1) * game.getSpacing()), -yOffset_Overall + 9 + ((y - 1) * game.getSpacing()), Images::Tile_2221, tileSet + currentPlane);
                    break;

                case 31:
                    if (flash && ((game.getX() == x && game.getY() == y) || (game.getX() - 1 == x && game.getY() == y))) { tileSet = tileSet + 3; }
                    SpritesU::drawPlusMaskFX(9 + ((x - 1) * game.getSpacing()), -yOffset_Overall + 9 + ((y - 1) * game.getSpacing()), Images::Tile_3132, tileSet + currentPlane);
                    x++;
                    break;

                case 41:
                    if (flash && ((game.getX() == x && game.getY() == y) || (game.getX() == x && game.getY() - 1 == y))) { tileSet = tileSet + 3; }
                    SpritesU::drawPlusMaskFX(9 + ((x - 1) * game.getSpacing()), -yOffset_Overall + 9 + ((y - 1) * game.getSpacing()), Images::Tile_4142, tileSet + currentPlane);
                    break;

                case 51:
                    if (flash && ((game.getX() == x && game.getY() == y) || (game.getX() - 1 == x && game.getY() == y))) { tileSet = tileSet + 3; }
                    SpritesU::drawPlusMaskFX(9 + ((x - 1) * game.getSpacing()), -yOffset_Overall + 9 + ((y - 1) * game.getSpacing()), Images::Tile_5152, tileSet + currentPlane);
                    x++;
                    break;

                case 61:
                    if (flash && ((game.getX() == x && game.getY() == y) || (game.getX() == x && game.getY() - 1 == y))) { tileSet = tileSet + 3; }
                    SpritesU::drawPlusMaskFX(9 + ((x - 1) * game.getSpacing()), -yOffset_Overall + 9 + ((y - 1) * game.getSpacing()), Images::Tile_6162, tileSet + currentPlane);
                    break;

            }

        }

    }

    if (renderGuides) {

        uint8_t tileSet = game.getRenderSize() == RenderSize::Large ? 0 : 3;

        SpritesU::drawPlusMaskFX(7, -yOffset_Overall + game.getY() * game.getSpacing(), Images::Cursor_Y, tileSet + currentPlane);
        SpritesU::drawPlusMaskFX(9 + (game.getWidth() * game.getSpacing()), -yOffset_Overall + game.getY() * game.getSpacing(), Images::Cursor_Y, tileSet + currentPlane);

        SpritesU::drawPlusMaskFX(game.getX() * game.getSpacing(), -yOffset_Overall + 7, Images::Cursor_X, tileSet + currentPlane);
        SpritesU::drawPlusMaskFX(game.getX() * game.getSpacing(), -yOffset_Overall + 9 + (game.getHeight() * game.getSpacing()), Images::Cursor_X, tileSet + currentPlane);

    }

}