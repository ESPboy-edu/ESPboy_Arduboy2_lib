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
 * File:   main.cpp
 * Author: Schuemi
 *
 * Created on 10. Dezember 2017, 14:06
 */


#include "Arduboy2.h"
#include "GameController.h"
#include "DashBoard.h"
#include "Highscore.h"
#include <ArduboyTones.h>

#include "constants.h"


Arduboy2 arduboy;
ArduboyTones m_tones(arduboy.audio.enabled);


DashBoard dBoard;
GameController game;
Highscore highscore;
StarField m_starField(0, 0, 128, 64, 4);
SpaceShip m_spaceShip(0);

enum MAINSTATE{
    MAINSTATE_DASHBOARD,
    MAINSTATE_GAME,
    MAINSTATE_HIGHSCORE
};
enum MAINSTATE mainState = MAINSTATE_DASHBOARD;


void setup() {
    
  Serial.begin(115200);
  arduboy.begin();
  arduboy.setFrameRate(30);
  
  arduboy.audio.begin();
  highscore.loadListEEPROM();
  if (game.haveSavedGame()) {
    mainState = MAINSTATE_GAME;
    game.loadGameData();
  }
}


void loop() {
     
    if (!(arduboy.nextFrame()) && ! game.isSpeedUp())
        return;
    
    
    
    if (mainState == MAINSTATE_DASHBOARD){
       
        dBoard.draw();
        
        if (dBoard.m_startGame){
            mainState = MAINSTATE_GAME;
          
            game.reset_(dBoard.m_startLevel, dBoard.m_gameMode);
        }
        if (dBoard.m_startHighScore){
            mainState = MAINSTATE_HIGHSCORE;
            dBoard.reset_();
        }
        
    }
    
    if (mainState == MAINSTATE_GAME){
       
       game.calcGameLogic();
       game.draw();
      
       
       
       if (game.isGameOver()) {
           if (game.getPoints() > 0){
               mainState = MAINSTATE_HIGHSCORE; 
               highscore.addNewRecord(game.getPoints());
           }else mainState = MAINSTATE_DASHBOARD;
           dBoard.reset_();
           
       }
    }
    if (mainState == MAINSTATE_HIGHSCORE){
       highscore.draw();
       
        if (highscore.m_closeHighScore){
            highscore.reset_();
            mainState = MAINSTATE_DASHBOARD;
        }
    }
        
    
}
