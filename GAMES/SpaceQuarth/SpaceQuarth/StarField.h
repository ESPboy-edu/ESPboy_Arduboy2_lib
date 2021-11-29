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
 * File:   StarField.h
 * Author: Schuemi
 *
 * Created on 12. Dezember 2017, 11:28
 */

#ifndef STARFIELD_H
#define STARFIELD_H

#include "Arduboy2.h"
#include "constants.h"


class StarField {
public:
    StarField(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t direction);
    
    void move();
    void draw(bool warp);
    //virtual ~StarField();
private:
   
    uint8_t m_starsX[MAX_STARS];
    uint8_t m_starsY_Speed[MAX_STARS];
    uint8_t m_x;
    uint8_t m_width;
    uint8_t m_y;
    uint8_t m_height;
    uint8_t m_direction;


};

#endif /* STARFIELD_H */
