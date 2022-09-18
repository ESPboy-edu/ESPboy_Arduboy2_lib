#pragma once
 
#include "Sprites.h"
#include "Enums.h"
#include "Plane.h"

class Enemy : public Plane {

  public: 

    Enemy(const EnemyType type, 
          const uint8_t * const * images);
        
    const Rect getRect() const;
    void renderImage();
    void move();
    
    const EnemyType getEnemyType() const;
    const Direction getDirection() const;
    const Direction getTurretDirection() const;
    const int8_t *getStartingPos() const;
    const int8_t getOffsetX() const;
    const int8_t getOffsetY() const;
    const uint8_t getDelayStart() const;
    const bool getInvertX() const;
    const bool getInvertY() const;
    const uint8_t getNumberOfBulletsFired() const;
    const uint8_t getWidth() const;
    const uint8_t getHeight() const;
    
    void setHealth(const SQ7x8 value);
    void decHealth(const SQ7x8 value);    
        
    void setEnemyType(const EnemyType value);
    void setDirection(const Direction value);
    void setTurretDirection(const Direction value);
    void setStartingPos(const int8_t *startingPos);
    void setOffsetX(const int8_t value);
    void setOffsetY(const int8_t value);
    void setDelayStart(const uint8_t value);
    void setInvertX(const bool value);
    void setInvertY(const bool value);
    void setNumberOfBulletsFired(const uint8_t value);
    void decNumberOfBulletsFired();
       
  private:
  
    EnemyType _type;
    Direction _direction;               
    Direction _turretDirection;               
    int8_t _offsetX;
    int8_t _offsetY;
    SQ7x8 _explosionImage;
    uint8_t _bulletsFired;
    
    
    // Used to track progress along sequence ..

    bool _invertX;
    bool _invertY;
    uint8_t _delayStart;
    int16_t _currentPos;
    const int8_t *_startingPos;

    
    // Store the details of a 'repeat' step in a sequence ..
    
    uint8_t _repeatSeqStep;
    int8_t _repeatSeqXInc;
    int8_t _repeatSeqYInc;

};