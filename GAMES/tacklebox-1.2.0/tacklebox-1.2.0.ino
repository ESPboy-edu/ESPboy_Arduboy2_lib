//#include <EEPROM.h>
#include <Arduboy2.h>
#include <math.h>

#include "src/game.h"
#include "src/renderer.h"
#include "src/util.h"
#include "src/sfx.h"

Arduboy2Base arduboy;

Renderer renderer(arduboy);
Game game;

void setup() {
    //Serial.begin(9600);
    arduboy.begin();
    arduboy.setFrameRate(40);
    arduboy.audio.begin();
    BeepPin1::begin();
    randomSeed(analogRead(0));
    State::load();
}


uint8_t loopCounter = 1;

void loop() {
    delay(0);
    if (!arduboy.nextFrame()) {
        return;
    }

    loopCounter += 1;

    if (loopCounter == 61) {
        loopCounter = 1;
    }

    arduboy.clear();
    arduboy.pollButtons();
    BeepPin1::timer();
    
    game.update(loopCounter);
    game.render(loopCounter);
    arduboy.display();
}
