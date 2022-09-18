#include "Base.h"
#include "FixedPoints.h"
#include "FixedPointsCommon.h"

Base::Base(SQ15x16 x, SQ15x16 y) {

  _x = x;
  _y = y;

}

const SQ15x16 Base::getX() const {

  return _x;

}

void Base::setX(const SQ15x16 value) {

  _x = value;

}

const SQ15x16 Base::getY() const {

  return _y;

}

void Base::setY(const SQ15x16 value) {

  _y = value;

}

const bool Base::getEnabled() const {

  return _enabled;

}

void Base::setEnabled(const bool value) {

  _enabled = value;

}



