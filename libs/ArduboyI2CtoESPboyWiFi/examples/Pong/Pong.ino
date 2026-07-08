/*
MIT License

Copyright (c) 2024-2026 sub1inear

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including but not limited to the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom this Software is
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

#define I2C_IMPLEMENTATION
// --- CHANGED: Include our custom ESPboy Wi-Fi bridge library ---
#include <ArduboyI2CtoESPboyWiFi.h> 

Arduboy2 arduboy;

// -------------------------------------------------------------
// Constants
// -------------------------------------------------------------
constexpr uint8_t fieldLeft = 0;
constexpr uint8_t fieldRight = WIDTH;
constexpr uint8_t fieldTop = 0;
constexpr uint8_t fieldBottom = HEIGHT;
constexpr uint8_t fieldCenterX = (fieldLeft + fieldRight) / 2;
constexpr uint8_t fieldCenterY = (fieldTop + fieldBottom) / 2;
constexpr uint8_t fieldWidth = fieldRight - fieldLeft;
constexpr uint8_t fieldHeight = fieldBottom - fieldTop;

constexpr uint8_t paddleWidth = 2;
constexpr uint8_t paddleHeight = 10;
constexpr int8_t paddleSpeed = 2;
constexpr int8_t paddleStartY = fieldCenterY - paddleHeight / 2;

constexpr uint8_t ballSize = 2;
constexpr uint8_t ballSpeedX = 1;
constexpr uint8_t ballSpeedY = 1;
constexpr uint8_t ballStartX = fieldCenterX - ballSize / 2;
constexpr uint8_t ballStartY = fieldCenterY - ballSize / 2;

// 5x7 font + 1 pixel spacing
constexpr uint8_t charWidth = 6;

constexpr uint8_t leftScoreX = fieldCenterX - (fieldCenterX / 2) - (charWidth / 2);
constexpr uint8_t leftScoreY = 0;
constexpr uint8_t rightScoreX = fieldCenterX + (fieldCenterX / 2) - (charWidth / 2);
constexpr uint8_t rightScoreY = 0;

constexpr uint8_t nullInput = 0xFF;

// -------------------------------------------------------------
// Structs
// -------------------------------------------------------------

struct Ball {
    int8_t x;
    int8_t y;
    int8_t dx;
    int8_t dy;
};

struct Player {
    int8_t paddleY;
    uint8_t score;
};

// -------------------------------------------------------------
// Data
// -------------------------------------------------------------

Ball ball;

Player leftPlayer;
Player rightPlayer;
bool isRightPlayer = false;

bool serveRight = true;
bool serveDown = false;

volatile bool controllerReceived = false;
volatile uint8_t controllerInput = 0;

volatile bool controllerRequested = false;
volatile uint8_t targetInput = 0;

// --- ADDED: Frame counter for network-safe delay ---
// 120 frames at 60FPS = 2 seconds of pause after a goal.
// We use frames instead of delay() to keep the Wi-Fi connection alive 
// and maintain perfect lockstep synchronization between devices.
uint8_t pauseFrames = 0;
// ---------------------------------------------------

// -------------------------------------------------------------
// Target Functions
// -------------------------------------------------------------

void onReceive() {
    // --- CHANGED: Use ArduboyI2C namespace ---
    const uint8_t *buffer = ArduboyI2C::getBuffer(); 
    // the first byte of the buffer is the input from the remote player
    controllerInput = buffer[0];
    // tell the main loop that we have new input
    controllerReceived = true;
}

void onRequest() {
    // --- CHANGED: Use ArduboyI2C namespace ---
    ArduboyI2C::reply(targetInput); 
    controllerRequested = true;
}

// -------------------------------------------------------------
// Reset Functions
// -------------------------------------------------------------

void resetBall() {
    // reset ball position
    ball.x = ballStartX;
    ball.y = ballStartY;

    // alternate the direction of the ball each time it is reset
    ball.dx = serveRight ? ballSpeedX : -ballSpeedX;
    serveRight = !serveRight;

    ball.dy = serveDown ? ballSpeedY : -ballSpeedY;
    serveDown = !serveDown;
}


void resetPlayers() {
    // reset player positions and scores
    leftPlayer = { paddleStartY, 0 };
    rightPlayer = { paddleStartY, 0 };
}

void reset() {
    resetPlayers();
    resetBall();
}

// -------------------------------------------------------------
// Update Functions
// -------------------------------------------------------------

void updatePlayer(Player &player, uint8_t input) {
    // update player position based on input
    if (input & UP_BUTTON) {
        player.paddleY -= paddleSpeed;
    }
    if (input & DOWN_BUTTON) {
        player.paddleY += paddleSpeed;
    }
    // constrain the player position to the field boundaries
    player.paddleY = constrain(player.paddleY, fieldTop, fieldBottom - paddleHeight);
}

void updateBall() {
    // update ball position based on its velocity
    ball.x += ball.dx;
    ball.y += ball.dy;

    // if the ball leaves the field boundaries, ...
    if (ball.y <= fieldTop || ball.y >= fieldBottom - ballSize) {
        // reverse its vertical direction
        ball.dy = -ball.dy;
        // constrain its position to the field boundaries
        ball.y  = constrain(ball.y, fieldTop, fieldBottom - ballSize);
    }

    Rect ballRect(ball.x, ball.y, ballSize, ballSize);

    Rect leftPaddleRect(fieldLeft, leftPlayer.paddleY, paddleWidth, paddleHeight);

    // if the ball is moving left and collides with the left paddle, ...
    if (ball.dx < 0 && arduboy.collide(leftPaddleRect, ballRect)) {
        ball.dx = -ball.dx;
        ball.x = fieldLeft + paddleWidth;
    }
    
    Rect rightPaddleRect(fieldRight - paddleWidth, rightPlayer.paddleY, paddleWidth, paddleHeight);

    // if the ball is moving right and collides with the right paddle, ...
    if (ball.dx > 0 && arduboy.collide(rightPaddleRect, ballRect)) {
        ball.dx = -ball.dx;
        ball.x = fieldRight - ballSize - paddleWidth;
    }

    // if the ball touches the left side, ...
    if (ball.x < fieldLeft) {
        if (rightPlayer.score < UINT8_MAX) {
            rightPlayer.score++;
        }
        // --- CHANGED: Trigger 2-second pause (120 frames at 60fps) instead of instant reset ---
        pauseFrames = 120;
        // -------------------------------------------------------------------------------------

    // if the ball touches the right side, ...
    } else if (ball.x > fieldRight - ballSize) {
        if (leftPlayer.score < UINT8_MAX) {
            leftPlayer.score++;
        }
        // --- CHANGED: Trigger 2-second pause (120 frames at 60fps) instead of instant reset ---
        pauseFrames = 120;
        // -------------------------------------------------------------------------------------
    }
}

void update(uint8_t leftInput, uint8_t rightInput) {
    // Players can always move their paddles, even during the pause
    updatePlayer(leftPlayer, leftInput);
    updatePlayer(rightPlayer, rightInput);
    
    // --- ADDED: Network-safe frame delay logic ---
    // Skip moving the ball for 'pauseFrames' amount of frames after a goal.
    if (pauseFrames > 0) {
        pauseFrames--; 
        if (pauseFrames == 0) {
            resetBall(); // When the timer reaches 0, drop the ball in the center
        }
    } else {
        updateBall(); // Move the ball normally if there's no pause
    }
    // ---------------------------------------------
}

// -------------------------------------------------------------
// Draw Functions
// -------------------------------------------------------------

void drawScore(uint8_t score, int8_t x, int8_t y) {
    arduboy.setCursor(x, y);
    arduboy.print(score);
}

void drawScores() {
    drawScore(leftPlayer.score, leftScoreX, 0);
    drawScore(rightPlayer.score, rightScoreX, 0);
}

void drawPlayer(const Player &player, int8_t x) {
    arduboy.fillRect(x, player.paddleY, paddleWidth, paddleHeight);
}

void drawPlayers() {
    drawPlayer(leftPlayer, fieldLeft);
    drawPlayer(rightPlayer, fieldRight - paddleWidth);
}

void drawBall() {
    arduboy.fillRect(ball.x, ball.y, ballSize, ballSize);
}

void drawCenterLine() {
    for (uint8_t y = fieldTop; y < fieldBottom; y += 4) {
        arduboy.drawFastVLine(fieldCenterX, y, 2);
    }
}

void draw() {
    arduboy.clear();
    drawScores();
    drawCenterLine();
    drawPlayers();
    drawBall();
    arduboy.display();
}

void drawMessage(const __FlashStringHelper *message) {
    arduboy.clear();
    arduboy.print(message);
    arduboy.display();
}

// -------------------------------------------------------------
// Main Functions
// -------------------------------------------------------------

void setup() {
    arduboy.begin();
    
    // --- CHANGED: Initialize the ESP-NOW Wi-Fi bridge ---
    ArduboyI2C::begin(); 

    // check if the cable is flipped
    // --- CHANGED: Use ArduboyI2C namespace ---
    ArduboyI2C::checkCableFlipped([]() {
        drawMessage(F("Please flip the cable\non this device."));
    });

    drawMessage(F("Waiting for other\nplayer..."));

    // --- CHANGED: Strict type comparison for v3.0.0 ---
    // ArduboyI2C::handshake() returns a Role enum instead of a boolean.
    isRightPlayer = (ArduboyI2C::handshake() == ArduboyI2C::Role::Controller);
    
    if (!isRightPlayer) {
        // --- CHANGED: Use ArduboyI2C namespace and v3.0.0 constant ---
        ArduboyI2C::onReceive(onReceive);
        ArduboyI2C::onRequest(onRequest);
        ArduboyI2C::setAddress(ArduboyI2C::nullAddress);
    }

    reset();
}

void loop() {
    if (!arduboy.nextFrame()) {
        return;
    }
    
    uint8_t localInput = arduboy.buttonsState();
    uint8_t leftInput, rightInput;

    if (isRightPlayer) {
        rightInput = localInput;
        do {
            // --- CHANGED: Use ArduboyI2C namespace, targetAddress, and Mode::Sync (v3.0.0) ---
            ArduboyI2C::write(ArduboyI2C::targetAddress, rightInput, ArduboyI2C::Mode::Sync);
        } while (ArduboyI2C::getError() != ArduboyI2C::Error::None); // --- CHANGED: Error::None ---
        
        do {
            // --- CHANGED: Use ArduboyI2C namespace and constants ---
            ArduboyI2C::read(ArduboyI2C::targetAddress, leftInput);
        } while (ArduboyI2C::getError() != ArduboyI2C::Error::None);
        
    } else {
        targetInput = leftInput = localInput;
        
        // --- CHANGED: Use ArduboyI2C namespace and constants ---
        ArduboyI2C::setAddress(ArduboyI2C::targetAddress);
        
        // --- ADDED: CRITICAL FIX ---
        // Call ArduboyI2C::update() inside empty while loops to prevent the ESP8266
        // Watchdog from resetting the device, and to monitor the "Sync lost" state.
        while (!controllerReceived) { ArduboyI2C::update(); }
        // ---------------------------
        
        rightInput = controllerInput;
        controllerReceived = false;

        // --- ADDED: CRITICAL FIX ---
        while (!controllerRequested) { ArduboyI2C::update(); }
        // ---------------------------
        
        controllerRequested = false;
        
        // --- CHANGED: Use ArduboyI2C namespace and v3.0.0 constant ---
        ArduboyI2C::setAddress(ArduboyI2C::nullAddress);
    }

    update(leftInput, rightInput);
    draw();
}