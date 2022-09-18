#pragma once
 
#include "Arduboy2.h"
#include "Sprites.h"
#include "Arduino.h"
#include "Enums.h"
#include "Plane.h"

class Player : public Plane {
  
  public:
   
    Player(const uint8_t * const * images);

    void renderImage();
    void initGame();
    void initMission();

    const SQ7x8 getFuel() const;
    const uint8_t getBullets() const;
    const uint16_t getScore() const;
    const uint16_t getGrandScore() const;
    const bool inRoll() const;
    const bool getPowerUp() const;

    void setFuel(const SQ7x8 value);
    void decFuel(const SQ7x8 value);
    void setBullets(const uint8_t value);
    void addBullets(const uint8_t value);
    void decBullets();
    void setScore(const uint16_t value);
    void setGrandScore(const uint16_t value);
    void setPowerUp(const boolean value);
    void decPowerUp();
    void startRoll();
  
  private:
    uint16_t _score;
    uint16_t _grandScore;
    uint8_t _bullets;
    SQ7x8 _fuel;
    uint8_t _rollState;
    uint8_t _powerUp;
    
};

