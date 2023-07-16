/*

BSD 3-Clause License

Copyright (c) 2018, Filmote
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

From https://github.com/filmote/Font4x6

*/

#pragma once

#include <Arduino.h>
#include <Print.h>

class Font4x6 : public Print {

  public:

    Font4x6(uint8_t lineSpacing = 8);

    virtual size_t write(uint8_t); // used by the Arduino Print class
    void printChar(const char c, const int8_t x, int8_t y);
    void setCursor(const int8_t x, const int8_t y);

    void setTextColor(const uint8_t color);
    void setHeight(const uint8_t color);


  private:

    int8_t _cursorX;    // Default is 0.
    int8_t _baseX;      // needed for linebreak.
    int8_t _cursorY;    // Default is 0.

    int8_t _textColor;  // BLACK == 0, everything else is WHITE. Default is WHITE.

    uint8_t _letterSpacing;  // letterSpacing controls the distance between letters. Default is 1.
    uint8_t _lineHeight;     // lineHeight controls the height between lines breakend by \n. Default is 8.

};
