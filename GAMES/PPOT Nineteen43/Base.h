#pragma once

#include "FixedPoints.h"
#include "FixedPointsCommon.h"

class Base {

  public:

    Base(SQ15x16 x, SQ15x16 y);

    const SQ15x16 getX() const;
    const SQ15x16 getY() const;
    const bool getEnabled() const;

    void setX(const SQ15x16 value);
    void setY(const SQ15x16 value);
    void setEnabled(const bool value);

  protected:

    SQ15x16 _x;               
    SQ15x16 _y;               
    bool _enabled;

};

