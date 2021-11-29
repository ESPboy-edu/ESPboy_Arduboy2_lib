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
 * File:   SpaceShip.cpp
 * Author: Schuemi
 * 
 * Created on 10. Dezember 2017, 17:47
 */

#include "SpaceShip.h"
#include "constants.h"
//#include <avr/pgmspace.h>
#include "Arduboy2.h"

extern Arduboy2 arduboy;


// spaceShip1.png  width: 12 height: 11

const uint8_t  SpaceShip::m_sprite[] = {
0x30,0x4c,0x42,0xe1,0x91,0x0f,0x0f,0x91,0xe3,0x44,0x7c,0x30,0x00,0x06,0x05,0x05,0x02,0x05,0x05,0x03,0x05,0x05,0x06,0x00,0x00,0x00,0x00
};


// bytes:63 ratio: 1.091



SpaceShip::SpaceShip(uint8_t x) {
    reset_();

}


void SpaceShip::doExplode() {
   
    if (! m_exploding) m_exploding = 1;
}

void SpaceShip::reset_() {
    m_exploding = 0;
    m_ypos = 0;
    m_redLed = 0;
    
}

void SpaceShip::setyPos(int8_t y) {
    m_ypos = y;
}
void SpaceShip::draw(uint8_t x) {
    int16_t xd = GAMEFIELD_X + (x*3);
    int16_t yd = 53 + m_ypos;
    
    if (m_exploding) {
        yd = 53;
        if (m_exploding < 25){ m_exploding++; m_redLed+=3; arduboy.drawBitmap(xd,yd,m_sprite, 12, 11); }
        if (m_exploding == 25 && m_redLed > 0) m_redLed--;
        
        for (int i = 0; i < m_redLed / 10; i++){
            arduboy.drawCircle(xd + 6, yd + 5, i);
        }
        
        arduboy.setRGBled(m_redLed ,0,0);
        
        for (int xx = xd; xx < xd + 12; xx++){
            for (int yy = yd; yy < yd + 11; yy++){
                if (!random(3))
                    arduboy.drawPixel(xx,yy, BLACK);
                
            }
        }
        return;
    }
    
    arduboy.drawBitmap(xd,yd,m_sprite, 12, 11);
   
}
