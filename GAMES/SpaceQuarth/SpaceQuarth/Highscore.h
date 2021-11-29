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
 * File:   Highscore.h
 * Author: Schuemi
 *
 * Created on 12. Dezember 2017, 21:05
 */

#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include <Arduboy2.h>

#define HIGHSCORE_LIST_SIZE 6

class Highscore {
public:
    Highscore();
    
    void loadListEEPROM();
    
    void draw();
    void reset_();
    void addNewRecord(uint32_t score);
    bool m_closeHighScore;
    
private:
    enum STATE {
        STATE_NORMAL,
        STATE_INSERT_NAME,
    };
    struct highScore {
        uint8_t name[3];
        uint32_t score;
    } m_highscoreList[HIGHSCORE_LIST_SIZE];
    
    
    uint8_t m_state;
    uint8_t m_newNamePlace;
    uint8_t m_cusorBlink;
    uint8_t m_currEditLocation;
    
    char m_currName[3];
    
    void saveListEEPROM();
    
};

#endif /* HIGHSCORE_H */
