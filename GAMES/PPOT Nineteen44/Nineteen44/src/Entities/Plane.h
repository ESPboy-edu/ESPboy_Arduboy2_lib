#pragma once

#include "Sprites.h"
#include "Base.h"
#include "../Utils/Enums.h"
#include "FixedPoints.h"
#include "FixedPointsCommon.h"

class Plane : public Base {

  public:

    Plane(const uint8_t * const * images);
    
    const Rect getRect() const;
    const SQ7x8 getHealth() const;
    
    void setHealth(const SQ7x8 value);
    void addHealth(const SQ7x8 value);
    void decHealth(const SQ7x8 value);

  protected:

    SQ7x8 _health;
    const uint8_t * const * _bitmaps;

};
