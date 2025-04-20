#include <Arduboy2.h>

uint8_t highScore_Entry = 5;
uint8_t highScore_Char = 0;


void highScore_Init() {

world.setBananas(12);
    world.setGameState(GameState::HighScore_Edit);

    highScore_Entry = 0;
    highScore_Char = 0;

    for (highScore_Entry = 0; highScore_Entry < 5; highScore_Entry++) {

        if (world.getBananas() >= cookie.highScore_Values[highScore_Entry]) {

            break;

        }

    }


    if (highScore_Entry < 5) {

        for (uint8_t y = 4; y >= highScore_Entry + 1; y--) {

            cookie.highScore_Chars[y][0] = cookie.highScore_Chars[y - 1][0];
            cookie.highScore_Chars[y][1] = cookie.highScore_Chars[y - 1][1];
            cookie.highScore_Chars[y][2] = cookie.highScore_Chars[y - 1][2];
            cookie.highScore_Values[y] = cookie.highScore_Values[y - 1];

        }

        cookie.highScore_Chars[highScore_Entry][0] = 0;
        cookie.highScore_Chars[highScore_Entry][1] = 0;
        cookie.highScore_Chars[highScore_Entry][2] = 0;
        cookie.highScore_Values[highScore_Entry] = world.getBananas();

    }

}

void highScore_Update() { 


    const uint8_t justPressed = getJustPressedButtons();
    const uint8_t pressed = getPressedButtons();
    Player &player = world.getPlayer();

    frameCount = frameCount + 1;

    switch (world.getGameState()) {
        
        case GameState::HighScore_Edit:

            if (justPressed & A_BUTTON) {
            
                highScore_Entry = 255;
                world.setGameState(GameState::HighScore_View);

                #ifndef DEBUG
                saveCookie();
                #endif

            }

            if (justPressed & LEFT_BUTTON && highScore_Char > 0) {
            
                highScore_Char--;

            }

            if (justPressed & RIGHT_BUTTON && highScore_Char < 2) {
            
                highScore_Char++;

            }

            if (justPressed & UP_BUTTON) {
            
                cookie.highScore_Chars[highScore_Entry][highScore_Char]++;

                if (cookie.highScore_Chars[highScore_Entry][highScore_Char] == 26) {
                    cookie.highScore_Chars[highScore_Entry][highScore_Char] = 0;
                }

            }

            if (justPressed & DOWN_BUTTON) {

                cookie.highScore_Chars[highScore_Entry][highScore_Char]--;

                if (cookie.highScore_Chars[highScore_Entry][highScore_Char] == 255) {
                    cookie.highScore_Chars[highScore_Entry][highScore_Char] = 25;
                }
            
            }

            break;

        case GameState::HighScore_View:

            if (justPressed & A_BUTTON) {
            
               world.setGameState(world.getPrevGameState());

            }

            break;

    }

}

void highScore_RenderScore(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a, uint8_t y) {

    uint16_t score = cookie.highScore_Values[y];
    SpritesU::drawOverwriteFX(107, 19 + (y * 9), Images::Font_Numbers, ((score / 100) * 3) + currentPlane);
    score = score % 100;
    SpritesU::drawOverwriteFX(113, 19 + (y * 9), Images::Font_Numbers, ((score / 10) * 3) + currentPlane);
    SpritesU::drawOverwriteFX(119, 19 + (y * 9), Images::Font_Numbers, ((score % 10) * 3) + currentPlane);

}

void highScore_RenderScore_Grey(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a, uint8_t y) {

    uint16_t score = cookie.highScore_Values[y];
    SpritesU::drawOverwriteFX(107, 19 + (y * 9), Images::Font_Numbers, (((score / 100) + 10) * 3) + currentPlane);
    score = score % 100;
    SpritesU::drawOverwriteFX(113, 19 + (y * 9), Images::Font_Numbers, (((score / 10) + 10) * 3) + currentPlane);
    SpritesU::drawOverwriteFX(119, 19 + (y * 9), Images::Font_Numbers, (((score % 10) + 10) * 3) + currentPlane);

}

void highScore(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a) {

    if (a.needsUpdate()) highScore_Update();

    Player &player = world.getPlayer();
    uint8_t currentPlane = a.currentPlane();
    
    SpritesU::drawOverwriteFX(0, 0, Images::Title, (6 * 3) + currentPlane);

    for (uint8_t y = 0; y < 5; y++) {


        uint8_t xOffset = 0;

        if (y != highScore_Entry && highScore_Entry <= 4) {

            SpritesU::drawPlusMaskFX(81 + xOffset, 19 + (y * 9), Images::Font_Alpha, ((cookie.highScore_Chars[y][0] + 27) * 3) + currentPlane);

        }
        else {

            if (highScore_Char != 0 || highScore_Entry > 4 || (frameCount % 64) < 32) {
                SpritesU::drawPlusMaskFX(81 + xOffset, 19 + (y * 9), Images::Font_Alpha, (cookie.highScore_Chars[y][0] * 3) + currentPlane);
            }

        }

        xOffset = xOffset + FX::readIndexedUInt8(Constants::Font_Width, cookie.highScore_Chars[y][0]) - 1;

        if (y != highScore_Entry && highScore_Entry <= 4) {

            SpritesU::drawPlusMaskFX(81 + xOffset, 19 + (y * 9), Images::Font_Alpha, ((cookie.highScore_Chars[y][1] + 27) * 3) + currentPlane);

        }
        else {

            if (highScore_Char != 1 || highScore_Entry > 4 || (frameCount % 64) < 32) {
                SpritesU::drawPlusMaskFX(81 + xOffset, 19 + (y * 9), Images::Font_Alpha, (cookie.highScore_Chars[y][1] * 3) + currentPlane);
            }

        }
        
        xOffset = xOffset + FX::readIndexedUInt8(Constants::Font_Width, cookie.highScore_Chars[y][1]) - 1;

        if (y != highScore_Entry && highScore_Entry <= 4) {

            SpritesU::drawPlusMaskFX(81 + xOffset, 19 + (y * 9), Images::Font_Alpha, ((cookie.highScore_Chars[y][2] + 27) * 3) + currentPlane);

        }
        else {

            if (highScore_Char != 2 || highScore_Entry > 4 || (frameCount % 64) < 32) {
                SpritesU::drawPlusMaskFX(81 + xOffset, 19 + (y * 9), Images::Font_Alpha, (cookie.highScore_Chars[y][2] * 3) + currentPlane);
            }

        }

        if (y != highScore_Entry && highScore_Entry <= 4) {

            highScore_RenderScore_Grey(a, y);

        }
        else {

            highScore_RenderScore(a, y);

        }

    }

}

