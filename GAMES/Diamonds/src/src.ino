#include <Arduino.h>

#include "game.h"
#include <Arduboy2.h>
#include <ESPboyPlaytune.h>

Arduboy2 arduboy;
Sprites sprites;

boolean outEn() { return true; }
ESPboyPlaytune sound(outEn);

void setup()
{
//    Serial.begin(9600);
    arduboy.begin();
    arduboy.setFrameRate(60);
    arduboy.setTextSize(1);
    arduboy.clear();

    sound.initChannel(PIN_SPEAKER_1);
    sound.setSynth(EPT_SYNTH_PIN, 10);
    //sound.initChannel(PIN_SPEAKER_2);
}

void loop()
{   
    delay(0);
    if (!arduboy.nextFrame()) {
        return;
    }

    arduboy.pollButtons();

    tick();


#ifdef DEBUG
    {
        char line[32];
        sprintf(line, "%d%%", arduboy.cpuLoad());

        arduboy.setCursor(104, 2);
        arduboy.print(line);
    }
#endif


//    Serial.write(arduboy.getBuffer(), 128 * 64 / 8);
    arduboy.display();


} // loop
