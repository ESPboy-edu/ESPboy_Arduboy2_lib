// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 MostlyBuilds

/*
 * Arduino entry points for Mostly Logic.
 *
 * Implementation lives in ordinary C++ modules so declaration order, module
 * boundaries, and host-side testing do not depend on Arduino's sketch preprocessor.
 */

#include "Game.h"

void setup() {
  MostlyLogic::Game::setup();
}

void loop() {
  MostlyLogic::Game::loop();
}
