#include "src/utils/Arduboy2Ext.h"
#include "src/utils/Constants.h"
#include "src/images/Images.h"


const uint8_t textLengths[] = { 90, 84, 87 };

const uint8_t PROGMEM IntroText_00[] = { 3,'E','a','c','h',' ','y','e','a','r',' ','t','h','e',' ','p','e','o','p','l','e',' ','o','f','~','t','h','e',' ','v','i','l','l','a','g','e', 
                            ' ','c','h','o','o','s','e',' ','a',' ','n','e','w','~',12,'c','h','a','m','p','i','o','n',' ','t','o',' ','p','r','o','t','e','c','t','~',15,'t',
                            'h','e','m',' ','f','r','o','m',' ','A','s','t','a','r','o','k','.' };

const uint8_t PROGMEM IntroText_01[] = { 7, 'C','o','m','p','e','t','e',' ','i','n',' ','t','h','e',' ','t','r','i','a','l','s','~',6,'t','o',' ','s','e','e',' ','h','o','w',' ','f','a',
                            'r',' ','y','o','u',' ','c','a','n','~',11,'g','e','t',' ','a','n','d',' ','y','o','u',' ','c','o','u','l','d',' ','b','e','~',25,'t','h','a','t',' ','c','h','a','m','p','i','o','n','!' };


const uint8_t PROGMEM IntroText_02[] = { 10,'Y','o','u','r',' ','c','h','o','i','c','e',' ','o','f',' ','r','u','n','e','s','~',7,'w','i','l','l',' ','s','e','l','e','c','t',' ',
                            'd','i','f','f','e','r','e','n','t','~',12,'c','o','u','r','s','e','s',' ','.','.',' ','s','o','m','e',' ','a','r','e','~',13,'e','a','s','i','e','r',
                            ' ','t','h','a','n',' ','o','t','h','e','r','s','!' };

const uint8_t * const IntroTexts[] = { IntroText_00, IntroText_01, IntroText_02 }; 

void introText_Init() {

    introTextVars.reset();
    gameState = GameState::IntroText;

}

void introText() {

    uint8_t line = 0;
    uint8_t x = 0;
    
    Sprites::drawSelfMasked(1, 1, Images::EnterRunes_00, 0);
    Sprites::drawSelfMasked(1, 54, Images::EnterRunes_02, 0);
    Sprites::drawSelfMasked(120, 1, Images::EnterRunes_01, 0);
    Sprites::drawSelfMasked(120, 54, Images::EnterRunes_03, 0);

    arduboy.drawFastHLine(1, 1, 125);
    arduboy.drawFastHLine(1, 61, 125);
    arduboy.drawFastHLine(3, 63, 122);


    for (uint8_t i = 0; i < introTextVars.index; i++) {

        uint8_t c = pgm_read_byte(&IntroTexts[introTextVars.panel][i]);

        switch (c) {

            case '~':
                line++;
                x = 0;
                break;

            case 0 ... 31:
                x = x + c;
                break;

            case ' ':
                x = x + 4;
                break;
            
            default:
                Sprites::drawSelfMasked(6 + x, 13 + (line * 10), Font::Images, Font::getIndex(c));
                x = x + 5;
                break;

        }
        
    }


    if (introTextVars.index < textLengths[introTextVars.panel] && arduboy.getFrameCount(2)) {

        introTextVars.index++;

    }


    uint8_t justPressed = arduboy.justPressedButtons();

    if (justPressed & A_BUTTON || justPressed & B_BUTTON) {
        
        if (introTextVars.index == textLengths[introTextVars.panel] && introTextVars.skipTypewriter == false) {
            introTextVars.panel++;
            introTextVars.index = 0;
        }
        else {
            if (introTextVars.skipTypewriter) introTextVars.panel++;
            introTextVars.skipTypewriter = true;
            introTextVars.index = textLengths[introTextVars.panel];
        }

        if (introTextVars.panel == 3) {
            tunes.stopScore();
            gameState = GameState::Seed_Init;
        }

    }

}

