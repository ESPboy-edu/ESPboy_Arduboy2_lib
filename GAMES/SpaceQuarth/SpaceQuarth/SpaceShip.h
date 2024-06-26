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
 * File:   SpaceShip.h
 * Author: Schuemi
 *
 * Created on 10. Dezember 2017, 17:47
 */

#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "Arduboy2.h"

class SpaceShip {
public:
    SpaceShip(uint8_t x);
    void draw(uint8_t x);
    void setyPos(int8_t y);
    void doExplode();
    void reset_();
private:
    static const uint8_t m_sprite[];
    int8_t m_ypos;
    uint8_t m_exploding;
    uint8_t m_redLed;
};

#endif /* SPACESHIP_H */
