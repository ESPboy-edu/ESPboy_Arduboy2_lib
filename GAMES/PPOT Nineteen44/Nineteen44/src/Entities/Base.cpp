#include "Base.h"
#include "FixedPoints.h"
#include "FixedPointsCommon.h"

Base::Base(const SQ15x16 x, const SQ15x16 y) {

  _x = x;
  _y = y;

}

SQ15x16 Base::getX() {

  return _x;

}

void Base::setX(const SQ15x16 value) {

  _x = value;

}

SQ15x16 Base::getY() {

  return _y;

}

void Base::setY(const SQ15x16 value) {

  _y = value;

}

bool Base::getEnabled() {

  return _enabled;

}

void Base::setEnabled(const bool value) {

  _enabled = value;

}



