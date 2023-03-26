#pragma once

#include <Arduboy2.h>

class Arduboy2Ext : public Arduboy2Base {

  public:

    Arduboy2Ext();

    uint8_t justPressedButtons() const;
    uint8_t pressedButtons() const;
    uint16_t getFrameCount() const;
    void setFrameCount(uint16_t val);
    
    uint8_t getFrameCount(uint8_t mod, int8_t offset = 0) const;
    bool getFrameCountHalf(uint8_t mod) const;
    bool isFrameCount(uint8_t mod) const;
    bool isFrameCount(uint8_t mod, uint8_t val) const;
    
    void clearButtonState();
    void resetFrameCount();
    uint8_t randomLFSR(uint8_t min, uint8_t max);
        
};