#pragma once
 
#include "EnemyBullet.h"
#include "Bullet.h"
#include "../Utils/Arduboy2Ext.h"
#include "../Utils/Enums.h"

class EnemyBullet : public Bullet {

  public:

    EnemyBullet();

    EnemyType getEnemyType();
    uint8_t getX();
    uint8_t getY();
    BulletImage getBulletImage();

    void setEnemyType(const EnemyType value);
    void move();
  
  protected:
  
    EnemyType _enemyType;

};
