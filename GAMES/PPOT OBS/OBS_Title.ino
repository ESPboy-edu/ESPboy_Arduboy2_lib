#include "src/utils/Arduboy2Ext.h"

const uint8_t textLengths[] = { 50, 51, 54, 61 };

const uint8_t PROGMEM IntroText_00[] = {    
    5,'Y','o','u','r',' ','s','h','i','p',' ','h','a','s',' ','b','e','e','n',
    11,'d','a','m','a','g','e','d',' ','b','y',' ','s','o','m','e',
    16,'s','p','a','c','e',' ','d','e','b','r','i','s','.','.',
    };

const uint8_t PROGMEM IntroText_01[] = {
    7,'.','.','t','h','e',' ','s','t','e','e','r','i','n','g',' ','i','s',
    5,'b','r','o','k','e','n',' ','a','n','d',' ','y','o','u',' ','c','a','n',
    17,'n','o','t',' ','n','a','v','i','g','a','t','e','.',
};

const uint8_t PROGMEM IntroText_02[] = {
    8,'Y','o','u',' ','d','i','s','c','o','v','e','r',' ','t','h','a','t',
    3,'b','y',' ','f','i','r','i','n','g',' ','t','h','e',' ','l','a','s','e','r',
    15,'y','o','u',' ','c','a','n',' ','s','t','e','e','r','.','.',
};

const uint8_t PROGMEM IntroText_03[] = {
    0,'.','.','s','o','r','t',' ','o','f','!',' ','A','v','o','i','d',' ','t','h','e',
    2,'a','s','t','e','r','o','i','d','s',' ','a','n','d',' ','e','n','e','m','y',
    4,'s','h','i','p','s',' ','a','n','d',' ','f','l','y',' ','h','o','m','e','.',
};

const uint8_t * const IntroTexts[] = { IntroText_00, IntroText_01, IntroText_02, IntroText_03 }; 

void title_Init() {

    gameState = GameState::Title;

    for (Asteroid &smallAsteroid : smallAsteroids) {

        smallAsteroid.x = random(0, 256);
        smallAsteroid.y = random(0, 56);
        
    }

    for (Asteroid &largeAsteroid : largeAsteroids) {

        launchLargeAsteroid(largeAsteroid);
        
    }

    for (Point &star : starfield) {

        star.x = random(0, 128);
        star.y = random(0, 64);
        
    }

    titleScreenVars.reset();
    player.reset();

    #ifdef SOUNDS
        tunes.playScore(Sounds::Title);
    #endif

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void title() { 

    uint8_t justPressed = arduboy.justPressedButtons();

    if (justPressed > 0) {

        switch (titleScreenVars.mode) {

            case TitleMode::OBS:

                #ifdef SOUNDS
                    tunes.stopScore();
                #endif

                titleScreenVars.mode = TitleMode::Scroll_One;
                titleScreenVars.counter = 64;
                break;

            case TitleMode::Scroll_One:
                titleScreenVars.mode = TitleMode::Scenario;
                titleScreenVars.counter = 64;
                break;

            case TitleMode::Scroll_Two:
                gameState = GameState::Game_Init; 
                break;

            default:
                break;

        }

    }

      
    // Move and render starfield ..

    moveRenderStarfield();

    switch (titleScreenVars.mode) {

        case TitleMode::OBS:


            // Move and render asteroids ..

            moveRenderSmallAsteroids(true);
            moveRenderLargeAsteroids(true);
            

            Sprites::drawExternalMask(28, 11, Images::Title, Images::Title_Mask, 0, 0);

            for (uint8_t i = 0; i < Constants::SmallAsteroid_Size; i++) {

                Asteroid &smallAsteroid = smallAsteroids[i];

                if (i % 2 == 1) {
                    Sprites::drawExternalMask(smallAsteroid.x + xOffset, smallAsteroid.y + yOffset, Images::SmallAsteroid, Images::SmallAsteroid_Mask, 0, 0);
                }
                
            }

            for (uint8_t i = 0; i < Constants::LargeAsteroid_Size; i++) {
            
                Asteroid &largeAsteroid = largeAsteroids[i];
                            
                if (i % 2 == 1) {
                    Sprites::drawExternalMask(largeAsteroid.x + xOffset, largeAsteroid.y + yOffset, Images::BigAsteroid[largeAsteroid.type], Images::BigAsteroid_Mask[largeAsteroid.type], 0, 0);
                }
                
            }            
            break;

        case TitleMode::Scroll_One:

            titleScreenVars.counter--;
            Sprites::drawExternalMask(28, 11 - (64 - titleScreenVars.counter), Images::Title, Images::Title_Mask, 0, 0);
            Sprites::drawExternalMask(10, 75 - (64 - titleScreenVars.counter), Images::Scenario, Images::Scenario_Mask, 0, 0);

            if (titleScreenVars.counter == 0) {

                titleScreenVars.mode = TitleMode::Scenario;

            }

            break;

        case TitleMode::Scenario:

            Sprites::drawExternalMask(10, 11, Images::Scenario, Images::Scenario_Mask, 0, 0);

            if (titleScreenVars.index < textLengths[titleScreenVars.panel] && arduboy.getFrameCount(6) == 0) {
                
                #ifdef SOUNDS                
                    tunes.playScore(Sounds::Keypress);
                #endif

            }

            introText();

            break;


        case TitleMode::Scroll_Two:

            titleScreenVars.counter--;
            Sprites::drawExternalMask(10, 11 - (64 - titleScreenVars.counter), Images::Scenario, Images::Scenario_Mask, 0, 0);

            font4x6.setCursor(17, 19 - (64 - titleScreenVars.counter));
            font4x6.print(F("..sort of! Avoid the"));
            font4x6.setCursor(19, 29 - (64 - titleScreenVars.counter));
            font4x6.print(F("asteroids and enemy"));
            font4x6.setCursor(21, 39 - (64 - titleScreenVars.counter));
            font4x6.print(F("ships and fly home."));

            if (titleScreenVars.counter == 0) {

                titleScreenVars.mode = TitleMode::Entrance;
                titleScreenVars.counter = 0;

            }

            break;

        case TitleMode::Entrance:

            titleScreenVars.counter++;
            Sprites::drawExternalMask(-16 + titleScreenVars.counter, player.y, Images::PlayerShip, Images::PlayerShip_Mask, 0, 0);
            Sprites::drawExternalMask(-25 + titleScreenVars.counter, player.y + 3, Images::ShipParticle, Images::ShipParticle_Mask, arduboy.getFrameCountHalf(8), arduboy.getFrameCountHalf(8));

            if (titleScreenVars.counter == 25) {

                gameState = GameState::Game_Init;

            }

            break;

    }

}


void introText() {

    uint8_t line = 0;
    uint8_t x = 0;
    
    for (uint8_t i = 0; i < titleScreenVars.index; i++) {

        uint8_t c = pgm_read_byte(&IntroTexts[titleScreenVars.panel][i]);

        switch (c) {

            case 0 ... 31:
                line++;
                x = 0 + c;
                break;

            case ' ':
                x = x + 3;
                break;
            
            default:
                font4x6.setCursor(17 + x, 9 + (line * 10));
                font4x6.print(static_cast<char>(c));
                x = x + 5;
                break;

        }
        
    }


    if (titleScreenVars.index < textLengths[titleScreenVars.panel] && arduboy.getFrameCount(2)) {

        titleScreenVars.index++;

    }


    uint8_t justPressed = arduboy.justPressedButtons();

    if (justPressed & A_BUTTON || justPressed & B_BUTTON) {
        
        if (titleScreenVars.index == textLengths[titleScreenVars.panel] && titleScreenVars.skipTypewriter == false) {
            titleScreenVars.panel++;
            titleScreenVars.index = 0;
        }
        else {
            if (titleScreenVars.skipTypewriter) titleScreenVars.panel++;
            titleScreenVars.skipTypewriter = true;
            titleScreenVars.index = textLengths[titleScreenVars.panel];
        }

        if (titleScreenVars.panel == 4) {
            titleScreenVars.mode = TitleMode::Scroll_Two;
            titleScreenVars.counter = 64;
        }

    }

}


