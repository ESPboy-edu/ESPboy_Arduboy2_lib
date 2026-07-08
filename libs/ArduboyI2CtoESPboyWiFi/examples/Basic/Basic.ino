/*
MIT License

Copyright (c) 2024-2026 sub1inear

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <Arduboy2.h>

// define in one file before including
#define I2C_IMPLEMENTATION
// --- CHANGED: Include our custom ESPboy Wi-Fi bridge library instead of the standard ArduboyI2C ---
#include <ArduboyI2CtoESPboyWiFi.h> 

Arduboy2 arduboy;

struct Player {
    int8_t x;
    int8_t y;
};

Player localPlayer = { 0, 0 };
Player remotePlayer = { 0, 0 };

bool isController = false;

void drawMessage(const __FlashStringHelper *message) {
    // __FlashStringHelper ensures message is stored in flash memory (with the F() macro)
    // otherwise the exactly the same as `char`
    arduboy.clear();
    arduboy.print(message);
    arduboy.display();
}

void onReceive() {
    // --- CHANGED: Use ArduboyI2C namespace instead of I2C ---
    remotePlayer = *reinterpret_cast<const Player *>(ArduboyI2C::getBuffer());
}

void onRequest() {
    // send our player data to the other device
    // --- CHANGED: Use ArduboyI2C namespace ---
    ArduboyI2C::reply(localPlayer);
}

void setup() {
    // initialize arduboy hardware
    arduboy.begin();

    // initialize I2C (twi) hardware
    // --- CHANGED: Initialize the ESP-NOW Wi-Fi bridge ---
    ArduboyI2C::begin();

    // check if the cable is flipped
    // calls function to display message if it is flipped
    // waits for it to be flipped back
    // --- CHANGED: Use ArduboyI2C namespace ---
    ArduboyI2C::checkCableFlipped([]() {
        // cable is flipped, display message
        drawMessage(F("Please flip the cable\non this device."));
    });

    // display handshaking message, I2C::handshake blocks
    drawMessage(F("Waiting for other\nplayer..."));

    // handshake with other devices and get a unique id for this device
    // note: I2C::handshake enables general calls by default
    
    // --- CHANGED: Strict type comparison for v3.0.0 ---
    // ArduboyI2C::handshake() now returns a Role enum instead of a boolean.
    isController = (ArduboyI2C::handshake() == ArduboyI2C::Role::Controller);

    if (!isController) {
        // --- CHANGED: Use ArduboyI2C namespace ---
        ArduboyI2C::onReceive(onReceive);
        ArduboyI2C::onRequest(onRequest);
    }
}

void loop() {
    // wait for next frame
    if (!arduboy.nextFrame()) {
        return;
    }
    
    // --- ADDED: CRITICAL FIX FOR PASSIVE TARGET/SLAVE DEVICES ---
    // Since Target devices only react to interrupts/callbacks and don't actively 
    // call read/write functions in the loop, we must manually call update() here.
    // This feeds the ESP8266 Watchdog Timer and processes "Synch lost" network timeouts.
    ArduboyI2C::update();
    // -------------------------------------------------------------

    arduboy.clear();

    // move our player around with the D-Pad
    if (arduboy.pressed(RIGHT_BUTTON)) { localPlayer.x++; }
    if (arduboy.pressed(LEFT_BUTTON))  { localPlayer.x--; }
    if (arduboy.pressed(DOWN_BUTTON))  { localPlayer.y++; }
    if (arduboy.pressed(UP_BUTTON))    { localPlayer.y--; }

    localPlayer.x = constrain(localPlayer.x, 0, WIDTH - 8);
    localPlayer.y = constrain(localPlayer.y, 0, HEIGHT - 8);

    if (isController) {
        // send our player data to the other device
        // --- CHANGED: Use ArduboyI2C namespace, v3.0.0 constants, and Mode::Sync ---
        ArduboyI2C::read(ArduboyI2C::targetAddress, remotePlayer);
        ArduboyI2C::write(ArduboyI2C::targetAddress, localPlayer, ArduboyI2C::Mode::Sync);
    }

    // draw the players
    // isController -> filled, !isController -> outlined
    Player &filledPlayer = (isController) ? localPlayer : remotePlayer;
    Player &outlinedPlayer = (isController) ? remotePlayer : localPlayer;

    arduboy.fillRect(filledPlayer.x, filledPlayer.y, 8, 8, WHITE);
    arduboy.drawRect(outlinedPlayer.x, outlinedPlayer.y, 8, 8, WHITE);

    // display
    arduboy.display();
}