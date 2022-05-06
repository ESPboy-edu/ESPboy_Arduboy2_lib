#pragma once
 
#include "../Utils/Arduboy2Ext.h"
#include "Sprites.h"
#include "Arduino.h"
#include "../Utils/Enums.h"
#include "Plane.h"

class Player : public Plane {
  
  public:
   
    Player(const uint8_t * const * images);

    void renderImage(uint8_t frame);
    void initGame();
    void initMission(bool initConsumables);

    SQ7x8 getFuel();
    uint8_t getBullets();
    uint16_t getScore();
    bool inRoll();
    bool getPowerUp();

    void setY(const SQ15x16 value);
    void setFuel(const SQ7x8 value);
    void decFuel(const SQ7x8 value);
    void setBullets(const uint8_t value);
    void addBullets(const uint8_t value);
    void decBullets();
    void setScore(const uint16_t value);
    void setPowerUp(boolean value);
    void decPowerUp();
    void startRoll();
    
  private:

    uint16_t _score;
    uint8_t _bullets;
    SQ7x8 _fuel;
    uint8_t _rollState;
    uint8_t _powerUp;
    int8_t _movement;
    
};

