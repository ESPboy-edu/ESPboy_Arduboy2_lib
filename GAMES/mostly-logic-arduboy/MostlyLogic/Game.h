// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 MostlyBuilds
#pragma once

/* Public lifecycle API used by the Arduino sketch. */

namespace MostlyLogic {
namespace Game {

/* Initializes Arduboy hardware and the initial title-screen state. */
void setup();

/* Runs one frame when the configured frame limiter permits it. */
void loop();

}  // namespace Game
}  // namespace MostlyLogic
