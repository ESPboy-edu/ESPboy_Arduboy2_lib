// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 MostlyBuilds
#pragma once

/*
 * Internal read bridge from the game simulation to the renderer.
 *
 * Game.cpp owns all mutable state. Renderer.cpp includes this header to inspect
 * that state but must never mutate gameplay rules. This is intentionally not a
 * public API; new modules should depend on narrower interfaces where practical.
 */

// Arduboy2 includes Arduino's EEPROM.h, which defines a header-local EEPROM
// object. Translation units that only use other Arduboy2 features otherwise
// receive an upstream -Wunused-variable warning. Keep the suppression scoped to
// that third-party include so unused variables in Mostly Logic still warn.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#include <Arduboy2.h>
#pragma GCC diagnostic pop
#include <ArduboyTones.h>

#include "GameTypes.h"
#include "Levels.h"

namespace MostlyLogic {
namespace Game {

// Hardware services.
extern Arduboy2 arduboy;
extern ArduboyTones sound;

// Screen and campaign selection.
extern GameScreen gameScreen;
extern uint8_t levelSelectIndex;
extern uint32_t titleBobFrame;
extern uint8_t lastPulseSoundStep;

// Live board, projectiles, cursor, and active level.
extern Tile board[GRID_ROWS][GRID_COLS];
extern Ball balls[MAX_BALLS];
extern uint8_t cursorCol;
extern uint8_t cursorRow;
extern uint8_t currentLevel;
extern uint8_t levelIntroFrames;
extern LevelDef activeLevel;
extern uint32_t levelStartMs;

// Transient visual effects whose state is advanced by Game.cpp and drawn by
// Renderer.cpp.
extern CannonFragment cannonFragments[MAX_CANNON_FRAGMENTS];
extern bool shatterEffectActive;
extern CellPosition shatterEffectPosition;
extern uint8_t shatterEffectFrame;

// Circuit state for switches, gates, tripwires, and reset animations.
extern bool switchAOn;
extern bool switchBOn;
extern bool gateOpen;
extern bool relayTripwireDisabled;
extern bool dynamicTripwireActive;
extern bool finaleTripwire1Active;
extern bool finaleTripwire2Active;
extern ActivationSwitch activationSwitchIndex;
extern uint8_t resetFlashFrames;
extern uint8_t gateBurstFrames;
extern uint8_t gateBurstDuration;
extern bool tripwireBackblastActive;
extern uint8_t tripwireBackblastFrame;
extern CellPosition tripwireBackblastStart;
extern CellPosition tripwireBackblastTarget;
extern uint8_t tripwireBackblastTotalFrames;
extern bool tripwireResetCircuitPending;
extern bool tripwireResetCircuitActive;
extern uint8_t tripwireResetCircuitFrame;
extern CellPosition tripwireResetContact;

// Activation, completion, and failure sequences.
extern ActivationState activationState;
extern uint8_t activationFrame;
extern CellPath visualPath;
extern CelebrationState celebrationState;
extern uint8_t celebrationFrame;
extern bool won;
extern bool levelFailed;
extern uint32_t winTimeMs;
extern uint8_t cannonKickFrames;

CellPosition getActivationSwitchPosition(ActivationSwitch which);

}  // namespace Game
}  // namespace MostlyLogic
