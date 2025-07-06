#ifndef BRANCH_H
#define BRANCH_H

#include <Arduboy2.h>

// SPRITES
uint8_t PROGMEM smallBranch[] = {
7, 16,
0x01, 0x01, 0xc3, 0xff, 0xff, 0xff, 0x00, 
0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0x0c, 
};

uint8_t PROGMEM medBranch[] = {
7, 24,
0x01, 0x01, 0x03, 0xff, 0xff, 0xff, 0x00, 
0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x03, 
0x00, 0x00, 0x18, 0xff, 0xff, 0xff, 0x00, 
};

uint8_t PROGMEM largeBranch[] =
{
// width, height,
7, 32,
// FRAME 00
0x01, 0x01, 0x03, 0xff, 0xff, 0xff, 0x00, 
0x00, 0x00, 0x00, 0xf3, 0xff, 0xff, 0x00, 
0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x03, 
0x00, 0x00, 0x0c, 0xff, 0xff, 0xff, 0x00,
};

 uint8_t PROGMEM smallBranchR[] = {
7, 16,
0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0x30, 
0x80, 0x80, 0xc3, 0xff, 0xff, 0xff, 0x00, 
};

 uint8_t PROGMEM medBranchR[] = {
7, 24,
0x00, 0x00, 0x18, 0xff, 0xff, 0xff, 0x00, 
0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc0, 
0x80, 0x80, 0xc0, 0xff, 0xff, 0xff, 0x00, 
};

 uint8_t PROGMEM largeBranchR[] = {
7, 32,
0x00, 0x00, 0x30, 0xff, 0xff, 0xff, 0x00, 
0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc0, 
0x00, 0x00, 0x00, 0xcf, 0xff, 0xff, 0x00, 
0x80, 0x80, 0xc0, 0xff, 0xff, 0xff, 0x00, 
};

class Branch {
  private:
    int _x = 140;
    int _y = 3;
    int _length;
    int _height = 7;
    int _rnd;
    bool _leftOrRight;
    bool _active = false;
    uint8_t *_currSprite;
    uint8_t *_currMask;

  public:
    int getX() {
      return _x;
    }

    int getY() {
      return _y;
    }

    int getLength() {
      return _length;
    }

    int getHeight() {
      return _height;
    }

    bool getActive() {
      return _active;
    }

    void spawn() {
      _x = 140;
      _y = 3;
      _rnd = random(10);
      _leftOrRight = random(2);
      _active = true;
      
      // Decide size of branch
      if (_rnd == 2) {
        _currSprite = (_leftOrRight == 0) ? largeBranch : largeBranchR;
        _length = 32;
      }
      else if (_rnd >= 7) {
        _currSprite = (_leftOrRight == 0) ? medBranch : medBranchR;
        _length = 24;
      }
      else {
        _currSprite = (_leftOrRight == 0) ? smallBranch : smallBranchR;
        _length = 16;
      }

      // Decide what side of screen branch will spawn
      if (_leftOrRight == 1) {
        _y = 61 - _length;
      } 
    }

    void reset() {
      _x = 140;
      _y = 3;
      _length = 16;
      _height = 7;
      _active = false;
    }

    void update() {
      if (_active == true) {
        _x--;
        
        if (_x < -5) {
          _x = 140;
          _active = false;
        }
      }
    }

    void draw() {
      //Sprites::drawExternalMask(_x, _y, _currSprite, _currMask, 0, 0);
      Sprites::drawSelfMasked(_x, _y, _currSprite, 0);
    }
};

#endif
