/*
 * The MIT License
 *
 * Copyright 2017 Schuemi.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


/* 
 * File:   GameController.h
 * Author: Schuemi
 *
 * Created on 10. Dezember 2017, 18:16
 */

#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H
#include <Arduboy2.h>
#include "SpaceShip.h"
#include "StarField.h"
#include "Levels.h"
#include "ArduboyTones.h"


class GameController {
public:
    GameController();
    //virtual ~GameController();

struct gameData {
        bool m_gameOver                        : 1;
        bool m_speedUp                         : 1;
        bool m_bricksAreDisappearing           : 1;
        bool m_stopScrolling                   : 1;
        bool m_bricksOnField                   : 1;
        bool m_showLevelCleared                : 1;
        bool m_sezialDrawn                     : 1;
        
        uint32_t m_points;
        uint8_t m_shipXPosition; // the Position 0 - 15
        uint8_t m_shotPositionX[MAX_SHOOTS_AT_ONCE];
        uint8_t m_shotPositionY[MAX_SHOOTS_AT_ONCE];
        uint8_t m_shotActive;
        uint8_t m_highestBrick;
        uint8_t m_newObjectNumber;
        uint32_t m_lastFoundObjectsCount;
        uint8_t m_keyHoldCounter;
        uint8_t gameFieldBricks[GAMEFIELD_BRICKS_WIDTH*GAMEFIELD_BRICKS_HEIGHT];
        uint16_t m_scrollPosition;
        uint8_t m_scrollMinPosition;
        
        uint8_t m_exploding;
        uint8_t m_destroyBrickWaitingCounter;
        uint8_t m_drawingObject;
        uint8_t m_drawingObjectCount;
    
        uint8_t m_currentLevel;
        uint8_t m_currentLevelStage;
        uint8_t m_currentLevelMircoStage;
        
        uint8_t m_gameMode;
        
        uint16_t m_dataMagic;
    };

    void loadGameData();
    void calcGameLogic();
    void draw();
    void reset_(uint8_t startLevel, uint8_t gameMode);
   
    bool haveSavedGame();
    void deleteSavedGame();
    uint32_t getPoints();
    bool isGameOver();
    bool isSpeedUp();
    gameData m_gameData;

private:
     
    static const uint8_t spriteShot[];
    static const uint8_t brick[];
    static const uint8_t crosshair[];
    static const uint8_t brickSpezial[];
    
    //Arduboy2* g_arduboy;
    
    
   
    
    
    uint8_t m_noCollision;
    bool m_gameLoaded;
   
    
    
    void drawCrosshair();
    void drawShots();
    void drawShot(uint8_t shotNumber);
    void moveShots(bool collisionCheckOnly);
    
    void drawGameField();
   
    void drawBricks();
    bool insertObject(uint8_t x, uint8_t y, uint8_t opjectType, bool spezial);
    uint8_t getBrick(uint8_t x, uint8_t y);
    bool isBrickDestroying(uint8_t x, uint8_t y);
    void setBrick(uint8_t x, uint8_t y, uint8_t val);
    void setBrickDestroy(uint8_t x, uint8_t y, uint8_t val);
    
    
    void checkForQuarth(uint8_t x, uint8_t y);
    uint16_t checkOneQuarth(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t* foundSpezial);
    
    bool isValueInArray(uint8_t* array, uint8_t count, uint8_t searchValue);
    
    void scrollDown();
    
    
    bool m_gamePaused;
    
    levelCharacteristics m_currentLevelCharacteristics;
    void readLevelObject(levelObject* lo, uint8_t number);
    void createLine();

};

#endif /* GAMECONTROLLER_H */
