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
 * File:   StarField.cpp
 * Author: Schuemi
 * 
 * Created on 12. Dezember 2017, 11:28
 */

#include "StarField.h"
#include "Arduboy2.h"

extern Arduboy2 arduboy;

StarField::StarField(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t direction) {
    m_x = x;
    m_width = width;
    m_y = y;
    m_height = height;
    m_direction = direction;
    for (int i = 0; i < MAX_STARS; i++) {
        m_starsX[i] = m_x + random(m_width);
        m_starsY_Speed[i] = m_y + random(m_height);
        m_starsY_Speed[i] += ((random(3)) << 6);
    }
    
}

void StarField::move() {
   for (int i = 0; i < MAX_STARS; i++) {
       uint8_t y = m_starsY_Speed[i] & 0x3f;
       uint8_t speed = (m_starsY_Speed[i] >> 6) + 1;
       if (m_direction == 1) {
           y += speed;
            if (y > m_y + m_height){
                m_starsX[i] = m_x + random(m_width);
                y = m_y;
                speed = random(3);
            }
       }
      /* if (m_direction == 2) {
            if (m_starsY[i] < 2){
                m_starsX[i] = m_x + random(m_width);
                m_starsY[i] = m_y + m_height;
                m_starsSpeed[i] = random(3) + 1;
            } else m_starsY[i] -= m_starsSpeed[i];
       }*/
       /*if (m_direction == 3) {
            m_starsX[i] += m_starsSpeed[i];
            if (m_starsX[i] > m_x + m_width){
                m_starsY[i] = m_y+ random(m_height);
                m_starsX[i] = m_x;
                m_starsSpeed[i] = random(3) + 1;
            }
       }*/
       if (m_direction == 4) {
            if (m_starsX[i] < 2){
                y = m_y + random(m_height);
                m_starsX[i] = m_x + m_width;
                speed = random(3);
            } else m_starsX[i] -=speed;
       }
       m_starsY_Speed[i] = y;
       m_starsY_Speed[i] += ((speed - 1) << 6);
   } 
}
void StarField::draw(bool warp) {
    for (int i = 0; i < MAX_STARS; i++) {
        uint8_t y = m_starsY_Speed[i] & 0x3f;
        uint8_t speed = (m_starsY_Speed[i] >> 6) + 1;
         arduboy.drawPixel(m_starsX[i], y,1);
         if (warp || speed > 2){
            if (m_direction == 1) arduboy.drawPixel(m_starsX[i], y + 1,1); //  || m_direction == 2
            if (m_direction == 4) arduboy.drawPixel(m_starsX[i] + 1, y,1); // m_direction == 3 || 
            
         }
    }
}

//StarField::~StarField() {
//}
