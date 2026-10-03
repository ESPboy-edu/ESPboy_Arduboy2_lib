// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 MostlyBuilds

/*
 * Game lifecycle, input, simulation, circuit rules, and effects.
 *
 * Game.cpp is the sole owner of mutable gameplay state. The renderer observes
 * selected state through GameState.h but never advances the simulation. All
 * collections are fixed-capacity to keep SRAM and execution time bounded on AVR.
 */

#include "Game.h"
#include "GameState.h"
#include "Renderer.h"

#include <string.h>

namespace MostlyLogic {
namespace Game {

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

// Screen and campaign selection.
GameScreen gameScreen = SCREEN_TITLE;
uint8_t levelSelectIndex = 0;
uint8_t levelSelectComboUpCount = 0;
bool levelSelectComboActive = false;
uint32_t titleBobFrame = 0;
uint8_t lastPulseSoundStep = INVALID_PULSE_STEP;

// Live board, projectiles, cursor, and active level.
Tile board[GRID_ROWS][GRID_COLS];
Ball balls[MAX_BALLS];

uint8_t cursorCol = GAMEPLAY_CURSOR_START.col;
uint8_t cursorRow = GAMEPLAY_CURSOR_START.row;
uint8_t currentLevel = 0;
uint8_t levelIntroFrames = 0;
LevelDef activeLevel;

uint32_t levelStartMs = 0;
uint32_t lastFireMs = 0;
uint32_t lastBallStepMs = 0;

// Transient visual effects.
CannonFragment cannonFragments[MAX_CANNON_FRAGMENTS];

bool shatterEffectActive = false;
CellPosition shatterEffectPosition = NO_POSITION;
uint8_t shatterEffectFrame = 0;

// Circuit state and reset animations.
bool switchAOn = false;
bool switchBOn = false;
bool gateOpen = false;
bool relayTripwireDisabled = false;
bool dynamicTripwireActive = false;
bool finaleTripwire1Active = false;
bool finaleTripwire2Active = false;
ActivationSwitch activationSwitchIndex = ACTIVATION_PRIMARY_SWITCH;
uint8_t resetFlashFrames = 0;
uint8_t gateBurstFrames = 0;
uint8_t gateBurstDuration = 0;
bool tripwireBackblastActive = false;
uint8_t tripwireBackblastFrame = 0;
CellPosition tripwireBackblastStart = NO_POSITION;
CellPosition tripwireBackblastTarget = NO_POSITION;
uint8_t tripwireBackblastTotalFrames = DEFAULT_BACKBLAST_FRAMES;
bool tripwireBackblastDeflectorDestroyed = false;
bool tripwireResetCircuitPending = false;
bool tripwireResetCircuitActive = false;
uint8_t tripwireResetCircuitFrame = 0;
CellPosition tripwireResetContact = NO_POSITION;

// Activation, completion, and failure sequences.
ActivationState activationState = ACTIVATION_NONE;
uint8_t activationFrame = 0;
// Activation cleanup and victory celebration are mutually exclusive, so one
// bounded path buffer can serve both effects and save scarce AVR SRAM.
CellPath visualPath = {};
CelebrationState celebrationState = CELEBRATION_NONE;
uint8_t celebrationFrame = 0;

bool won = false;
bool levelFailed = false;
uint32_t winTimeMs = 0;
uint8_t cannonKickFrames = 0;

CellPosition getActivationSwitchPosition(ActivationSwitch which) {
  switch (which) {
    case ACTIVATION_PRIMARY_SWITCH:
      return activeLevel.switchA;
    case ACTIVATION_SECONDARY_SWITCH:
      return activeLevel.switchB;
    case ACTIVATION_SNEAKY_TRIPWIRE_SWITCH:
      return SNEAKY_TRIPWIRE_SWITCH_POSITION;
    case ACTIVATION_FINALE_TRIPWIRE_SWITCH:
      return FINALE_TRIPWIRE_SWITCH_POSITION;
  }
  return NO_POSITION;
}

namespace {

// Cross-function declarations for helpers defined later in this file. Unlike
// .ino files, .cpp files are not processed by Arduino's sketch preprocessor.
void beginCannonExplosion();
Direction deflectX(Direction direction);
Direction deflectDot(Direction direction);
bool buildVisualPathTo(CellPosition target, CellPath& path);
void beginSwitchActivation(ActivationSwitch switchIndex);
bool ballTouchesTripwireSegment(const Ball& ball, GridSegment segment);
bool ballTouchesResetTripwire(const Ball& ball);
bool spawnSplitChild(const Ball& source, Direction direction);

void playTone(uint16_t frequency, uint16_t durationMs) {
  if (arduboy.audio.enabled()) {
    sound.tone(frequency, durationMs);
  }
}

void playTwoTone(uint16_t firstFrequency,
                 uint16_t firstDurationMs,
                 uint16_t secondFrequency,
                 uint16_t secondDurationMs) {
  if (arduboy.audio.enabled()) {
    sound.tone(firstFrequency, firstDurationMs, secondFrequency, secondDurationMs);
  }
}

void startGateBurst(uint8_t duration) {
  gateBurstDuration = duration;
  gateBurstFrames = duration;
}

void clearBalls() {
  for (uint8_t i = 0; i < MAX_BALLS; ++i) {
    balls[i].active = false;
  }
}

void loadActiveLevel() {
  memcpy_P(&activeLevel, &levels[currentLevel], sizeof(LevelDef));
}

void beginLevelPlay() {
  const uint32_t now = millis();
  levelStartMs = now;
  lastFireMs = now - FIRE_INTERVAL_MS;  // Launch exactly one ball immediately.
  lastBallStepMs = now;
}

void buildLevel() {
  loadActiveLevel();

  for (uint8_t row = 0; row < GRID_ROWS; ++row) {
    for (uint8_t col = 0; col < GRID_COLS; ++col) {
      board[row][col] = TILE_EMPTY;
    }
  }

  board[activeLevel.cannon.row][activeLevel.cannon.col] = TILE_CANNON;
  board[activeLevel.target.row][activeLevel.target.col] = TILE_TARGET;

  for (uint8_t i = 0; i < activeLevel.wallCount; ++i) {
    const uint8_t packed = pgm_read_byte(activeLevel.walls + i);
    const uint8_t col = packed & 0x0F;
    const uint8_t row = packed >> 4;
    if (isInsideGrid(col, row)) {
      board[row][col] = TILE_WALL;
    }
  }

  if (isValidPosition(activeLevel.switchA)) {
    board[activeLevel.switchA.row][activeLevel.switchA.col] = TILE_SWITCH_A;
  }
  if (isValidPosition(activeLevel.switchB)) {
    board[activeLevel.switchB.row][activeLevel.switchB.col] = TILE_SWITCH_B;
  }
  if (isValidPosition(activeLevel.gate)) {
    board[activeLevel.gate.row][activeLevel.gate.col] = TILE_GATE;
  }
  if (isValidPosition(activeLevel.splitter)) {
    board[activeLevel.splitter.row][activeLevel.splitter.col] = TILE_SPLITTER;
  }
}

void restartLevel() {
  buildLevel();
  clearBalls();

  cursorCol = GAMEPLAY_CURSOR_START.col;
  cursorRow = GAMEPLAY_CURSOR_START.row;
  levelIntroFrames = LEVEL_INTRO_FRAMES;

  // Simulation and completion timing begin after the intro card is dismissed.
  levelStartMs = 0;
  lastFireMs = 0;
  lastBallStepMs = 0;

  visualPath.length = 0;
  celebrationState = CELEBRATION_NONE;
  celebrationFrame = 0;
  won = false;
  levelFailed = false;
  winTimeMs = 0;
  cannonKickFrames = 0;
  lastPulseSoundStep = INVALID_PULSE_STEP;
  shatterEffectActive = false;
  shatterEffectPosition = NO_POSITION;
  shatterEffectFrame = 0;
  switchAOn = false;
  switchBOn = false;
  gateOpen = false;
  relayTripwireDisabled = false;
  dynamicTripwireActive = false;
  finaleTripwire1Active = false;
  finaleTripwire2Active = false;
  activationSwitchIndex = ACTIVATION_PRIMARY_SWITCH;
  resetFlashFrames = 0;
  gateBurstFrames = 0;
  gateBurstDuration = 0;
  tripwireBackblastActive = false;
  tripwireBackblastFrame = 0;
  tripwireBackblastStart = NO_POSITION;
  tripwireBackblastTarget = NO_POSITION;
  tripwireBackblastTotalFrames = DEFAULT_BACKBLAST_FRAMES;
  tripwireBackblastDeflectorDestroyed = false;
  tripwireResetCircuitPending = false;
  tripwireResetCircuitActive = false;
  tripwireResetCircuitFrame = 0;
  tripwireResetContact = NO_POSITION;
  activationState = ACTIVATION_NONE;
  activationFrame = 0;
  for (uint8_t i = 0; i < MAX_CANNON_FRAGMENTS; ++i) {
    cannonFragments[i].life = 0;
  }
}

bool isEditableTile(Tile tile) {
  return tile == TILE_EMPTY || isDeflectorTile(tile);
}

// ------------------------------------------------------------
// Input
// ------------------------------------------------------------

void handleTitleInput() {
  if (arduboy.justPressed(B_BUTTON)) {
    if (arduboy.audio.enabled()) {
      arduboy.audio.off();
    } else {
      arduboy.audio.on();
      playTone(880, 45);
    }
    arduboy.audio.saveOnOff();
  }

  if (arduboy.justPressed(A_BUTTON)) {
    playTone(660, 55);
    currentLevel = 0;
    gameScreen = SCREEN_GAME;
    restartLevel();
  }
}

void startSelectedLevel() {
  if (levelSelectIndex == 0) {
    gameScreen = SCREEN_TITLE;
    playTone(440, 40);
    return;
  }

  currentLevel = levelSelectIndex - 1;
  gameScreen = SCREEN_GAME;
  playTone(660, 55);
  restartLevel();
}

void handleLevelSelectInput() {
  constexpr uint8_t SELECTOR_ENTRY_COUNT = LEVEL_COUNT + 1;

  if (arduboy.justPressed(UP_BUTTON) || arduboy.justPressed(LEFT_BUTTON)) {
    levelSelectIndex =
        (levelSelectIndex == 0) ? (SELECTOR_ENTRY_COUNT - 1) : (levelSelectIndex - 1);
    playTone(440, 28);
  }

  if (arduboy.justPressed(DOWN_BUTTON) || arduboy.justPressed(RIGHT_BUTTON)) {
    levelSelectIndex = (levelSelectIndex + 1) % SELECTOR_ENTRY_COUNT;
    playTone(520, 28);
  }

  if (arduboy.justPressed(A_BUTTON)) {
    startSelectedLevel();
  }

  if (arduboy.justPressed(B_BUTTON)) {
    gameScreen = SCREEN_TITLE;
    playTone(440, 40);
  }
}

void cycleTileAtCursor() {
  Tile& tile = board[cursorRow][cursorCol];
  if (!isEditableTile(tile)) return;

  if (tile == TILE_EMPTY) {
    tile = TILE_DEFLECT_X;
    playTone(740, 30);
  } else if (tile == TILE_DEFLECT_X) {
    tile = TILE_DEFLECT_DOT;
    playTone(560, 35);
  } else {
    tile = TILE_EMPTY;
    playTone(260, 35);
  }
}

void handleInput() {
  // Hidden release shortcut: hold A+B and press Up three separate times.
  // It is checked before every screen/state handler, so it works during
  // gameplay, intro cards, celebrations, failures, and the title screen.
  const bool levelSelectComboHeld = arduboy.pressed(A_BUTTON) && arduboy.pressed(B_BUTTON);

  if (levelSelectComboHeld) {
    levelSelectComboActive = true;
    if (arduboy.justPressed(UP_BUTTON)) {
      ++levelSelectComboUpCount;
      playTone(static_cast<uint16_t>(420 + levelSelectComboUpCount * 120), 35);
      if (levelSelectComboUpCount >= 3) {
        levelSelectIndex = currentLevel < LEVEL_COUNT ? currentLevel + 1 : 0;
        gameScreen = SCREEN_LEVEL_SELECT;
        levelSelectComboUpCount = 0;
        levelSelectComboActive = false;
        clearBalls();
        playTone(1040, 80);
      }
    }
    return;
  }

  if (levelSelectComboActive) {
    // Releasing A+B before the third Up cancels the shortcut.
    levelSelectComboUpCount = 0;
    levelSelectComboActive = false;
    return;
  }

  if (gameScreen == SCREEN_TITLE) {
    handleTitleInput();
    return;
  }
  if (gameScreen == SCREEN_LEVEL_SELECT) {
    handleLevelSelectInput();
    return;
  }
  if (gameScreen == SCREEN_GAME_OVER) {
    if (arduboy.justPressed(A_BUTTON)) {
      currentLevel = 0;
      gameScreen = SCREEN_GAME;
      playTone(660, 55);
      restartLevel();
    } else if (arduboy.justPressed(B_BUTTON)) {
      gameScreen = SCREEN_TITLE;
      playTone(440, 40);
    }
    return;
  }
  if (levelFailed) {
    if (celebrationState == CELEBRATION_DONE &&
        (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON))) {
      restartLevel();
    }
    return;
  }

  if ((celebrationState != CELEBRATION_NONE && !won) || activationState != ACTIVATION_NONE) {
    return;
  }

  if (won) {
    if (arduboy.justPressed(A_BUTTON)) {
      if (currentLevel + 1 < LEVEL_COUNT) {
        ++currentLevel;
        restartLevel();
      } else {
        gameScreen = SCREEN_GAME_OVER;
        playTwoTone(784, 100, 988, 140);
      }
    } else if (arduboy.justPressed(B_BUTTON)) {
      restartLevel();
    }
    return;
  }

  if (levelIntroFrames > 0) {
    if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
      levelIntroFrames = 0;
      beginLevelPlay();
    }
    return;
  }

  const bool restartHeld = arduboy.pressed(LEFT_BUTTON) && arduboy.pressed(RIGHT_BUTTON);
  const bool restartTriggered =
      arduboy.justPressed(LEFT_BUTTON) || arduboy.justPressed(RIGHT_BUTTON);
  if (restartHeld && restartTriggered) {
    restartLevel();
    return;
  }

  if (arduboy.justPressed(LEFT_BUTTON) && cursorCol > 0) {
    --cursorCol;
  }
  if (arduboy.justPressed(RIGHT_BUTTON) && cursorCol + 1 < GRID_COLS) {
    ++cursorCol;
  }
  if (arduboy.justPressed(UP_BUTTON) && cursorRow > 0) {
    --cursorRow;
  }
  if (arduboy.justPressed(DOWN_BUTTON) && cursorRow + 1 < GRID_ROWS) {
    ++cursorRow;
  }

  // During puzzle play, A and B intentionally perform the same action.
  // This makes the placement controls discoverable without requiring the
  // player to learn a separate delete-only button.
  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
    cycleTileAtCursor();
  }
}

// ------------------------------------------------------------
// Simulation and circuit state
// ------------------------------------------------------------

Direction deflectX(Direction direction) {
  // X always turns the projectile 90 degrees counterclockwise.
  // E->N, N->W, W->S, S->E.
  return turnCounterclockwise(direction);
}

Direction deflectDot(Direction direction) {
  // Dot always turns the projectile 90 degrees clockwise.
  // E->S, S->W, W->N, N->E.
  return turnClockwise(direction);
}

// Zero-allocation bounded path tracing. This intentionally follows one concrete
// route from the cannon instead of recursively exploring the board. At a
// splitter, the branch whose next cell is closer to the requested destination
// is selected. That is sufficient for the campaign layouts and avoids both
// AVR stack overflow and large SRAM buffers during startup.
bool buildVisualPathTo(CellPosition target, CellPath& path) {
  path.length = 0;
  int8_t col = activeLevel.cannon.col;
  int8_t row = activeLevel.cannon.row;
  Direction direction = activeLevel.cannonDirection;
  bool canSplit = true;

  for (uint8_t step = 0; step < MAX_VISUAL_PATH; ++step) {
    if (!isInsideGrid(col, row)) {
      path.length = 0;
      return false;
    }

    path.cells[path.length] = {static_cast<uint8_t>(col), static_cast<uint8_t>(row)};
    ++path.length;

    if (static_cast<uint8_t>(col) == target.col && static_cast<uint8_t>(row) == target.row) {
      return true;
    }

    const Tile tile = board[row][col];
    if (tile == TILE_DEFLECT_X) {
      direction = deflectX(direction);
    } else if (tile == TILE_DEFLECT_DOT) {
      direction = deflectDot(direction);
    } else if (tile == TILE_SPLITTER && canSplit) {
      const Direction branchA = static_cast<Direction>((static_cast<uint8_t>(direction) + 2) % 8);
      const Direction branchB = static_cast<Direction>((static_cast<uint8_t>(direction) + 6) % 8);
      const int8_t aCol = col + directionDeltaX(branchA);
      const int8_t aRow = row + directionDeltaY(branchA);
      const int8_t bCol = col + directionDeltaX(branchB);
      const int8_t bRow = row + directionDeltaY(branchB);

      const uint16_t distanceA =
          static_cast<uint16_t>(abs(static_cast<int>(aCol) - static_cast<int>(target.col))) +
          static_cast<uint16_t>(abs(static_cast<int>(aRow) - static_cast<int>(target.row)));
      const uint16_t distanceB =
          static_cast<uint16_t>(abs(static_cast<int>(bCol) - static_cast<int>(target.col))) +
          static_cast<uint16_t>(abs(static_cast<int>(bRow) - static_cast<int>(target.row)));
      direction = distanceA <= distanceB ? branchA : branchB;
      canSplit = false;
    }

    const int8_t nextCol = col + directionDeltaX(direction);
    const int8_t nextRow = row + directionDeltaY(direction);
    if (!isInsideGrid(nextCol, nextRow)) {
      path.length = 0;
      return false;
    }

    const Tile nextTile = board[nextRow][nextCol];
    if (nextTile == TILE_WALL || nextTile == TILE_CANNON || (nextTile == TILE_GATE && !gateOpen)) {
      path.length = 0;
      return false;
    }

    col = nextCol;
    row = nextRow;
  }

  path.length = 0;
  return false;
}

void beginCelebration() {
  if (celebrationState != CELEBRATION_NONE) {
    return;
  }

  // Path reconstruction drives only the visual return pulse. The level is
  // already won because a live ball reached the target, so a failed trace must
  // never affect gameplay state.
  buildVisualPathTo(activeLevel.target, visualPath);
  clearBalls();
  celebrationState = CELEBRATION_TARGET;
  celebrationFrame = 0;
  winTimeMs = millis() - levelStartMs;
  playTone(880, 90);
}

void setDynamicTripwireSwitchVisible(bool visible) {
  if (activeLevel.circuitMode != CIRCUIT_AND_DYNAMIC_TRIPWIRE) return;
  Tile& tile = board[SNEAKY_TRIPWIRE_SWITCH_POSITION.row][SNEAKY_TRIPWIRE_SWITCH_POSITION.col];
  if (visible) {
    tile = TILE_TRIPWIRE_SWITCH;
  } else if (tile == TILE_TRIPWIRE_SWITCH) {
    tile = TILE_EMPTY;
  }
}

void setFinaleTripwire2SwitchVisible(bool visible) {
  if (activeLevel.circuitMode != CIRCUIT_SPLITTER_OVERLOAD) return;
  Tile& tile = board[FINALE_TRIPWIRE_SWITCH_POSITION.row][FINALE_TRIPWIRE_SWITCH_POSITION.col];
  if (visible)
    tile = TILE_TRIPWIRE_SWITCH;
  else if (tile == TILE_TRIPWIRE_SWITCH)
    tile = TILE_EMPTY;
}

void beginSwitchActivation(ActivationSwitch switchIndex) {
  if (activationState != ACTIVATION_NONE) return;
  if ((switchIndex == ACTIVATION_PRIMARY_SWITCH && switchAOn) ||
      (switchIndex == ACTIVATION_SECONDARY_SWITCH && switchBOn))
    return;

  activationSwitchIndex = switchIndex;
  const CellPosition target = getActivationSwitchPosition(switchIndex);
  // This trace drives cleanup and pulse animation only. Collision handling has
  // already proven that the switch was reached by a live ball.
  buildVisualPathTo(target, visualPath);
  if (switchIndex == ACTIVATION_PRIMARY_SWITCH)
    switchAOn = true;
  else if (switchIndex == ACTIVATION_SECONDARY_SWITCH)
    switchBOn = true;

  // SNEAKY uses a three-control state cycle:
  //   1) A and B are the two AND inputs for the target gate.
  //   2) Every activation of B deploys the full-height tripwire and reveals a
  //      temporary tripwire-control switch beside A.
  //   3) Touching the tripwire resets both A and B and recloses the gate, but the
  //      tripwire and its temporary switch remain so the player can disable it.
  //   4) The temporary switch retracts only the tripwire and then disappears.
  //   5) Rebuilding A and B deploys the hazard again, so a safe default cannon
  //      route must already be prepared.
  if (activeLevel.circuitMode == CIRCUIT_AND_DYNAMIC_TRIPWIRE) {
    if (switchIndex == ACTIVATION_SECONDARY_SWITCH) {
      // B remains a normal AND input. Its additional consequence is to deploy
      // the hazard and reveal the dedicated switch that can retract it.
      dynamicTripwireActive = true;
      setDynamicTripwireSwitchVisible(true);
      resetFlashFrames = RESET_FLASH_FRAMES;
    } else if (switchIndex == ACTIVATION_SNEAKY_TRIPWIRE_SWITCH) {
      // The temporary switch controls only the deployed tripwire. It never
      // changes A, B, or the gate directly.
      dynamicTripwireActive = false;
      setDynamicTripwireSwitchVisible(false);
      resetFlashFrames = RESET_FLASH_FRAMES;
    }
  }

  // Finale state machine. A creates a permanent first lower-branch hazard.
  // B deploys a second tripwire in the cannon's default path; only that second
  // hazard has a temporary shutdown switch.
  if (activeLevel.circuitMode == CIRCUIT_SPLITTER_OVERLOAD) {
    if (switchIndex == ACTIVATION_PRIMARY_SWITCH) {
      // The first finale tripwire is permanent once Switch A deploys it. It has no
      // shutdown switch; touching it always resets both main AND inputs.
      finaleTripwire1Active = true;
      resetFlashFrames = RESET_FLASH_FRAMES;
    } else if (switchIndex == ACTIVATION_SECONDARY_SWITCH) {
      finaleTripwire2Active = true;
      setFinaleTripwire2SwitchVisible(true);
      resetFlashFrames = RESET_FLASH_FRAMES;
    } else if (switchIndex == ACTIVATION_FINALE_TRIPWIRE_SWITCH) {
      // Only the second finale hazard has a temporary shutdown switch.
      finaleTripwire2Active = false;
      setFinaleTripwire2SwitchVisible(false);
      resetFlashFrames = RESET_FLASH_FRAMES;
    }
  }

  // The controlled object changes only after the signal reaches it. In RELAY,
  // A controls the gate and B controls the tripwire independently.
  if (!isSeparateRelayCircuit(activeLevel.circuitMode) && !isAndCircuit(activeLevel.circuitMode)) {
    gateOpen = switchAOn || switchBOn;
  }
  clearBalls();
  activationState = ACTIVATION_CLEANUP;
  activationFrame = 0;
  gateBurstFrames = 0;
  gateBurstDuration = 0;
  // Distinct positive two-note switch chime.
  playTone(1320, 70);
}

void resetPrimarySwitchImmediately() {
  if (!switchAOn) {
    resetFlashFrames = SHORT_RESET_FLASH_FRAMES;
    return;
  }

  switchAOn = false;
  gateOpen = false;
  resetFlashFrames = RESET_FLASH_FRAMES;
}

void beginTripwireResetBackblast(uint8_t triggeringBallIndex) {
  const Ball& ball = balls[triggeringBallIndex];
  // Tripwire contact always back-blasts to the most recent deflector. If the
  // switch is latched, defer the switch/gate reset until that first animation
  // finishes, then visibly propagate the reset through the shared circuit.
  // Shared/relay circuits reset A. SNEAKY's deployed tripwire always begins a
  // reset sequence that releases both AND inputs while leaving the tripwire and
  // its dedicated control switch present.
  if (activeLevel.circuitMode == CIRCUIT_AND_DYNAMIC_TRIPWIRE) {
    // Always perform the local tripwire-to-deflector back-blast. Only run the
    // longer switch/gate reset animation when at least one AND input is still
    // latched and there is therefore circuit state left to reset.
    tripwireResetCircuitPending = dynamicTripwireActive && (switchAOn || switchBOn);
  } else if (activeLevel.circuitMode == CIRCUIT_SPLITTER_OVERLOAD) {
    tripwireResetCircuitPending =
        (finaleTripwire1Active || finaleTripwire2Active) && (switchAOn || switchBOn);
  } else {
    tripwireResetCircuitPending = switchAOn && (activeLevel.circuitMode == CIRCUIT_SHARED_RESET ||
                                                activeLevel.circuitMode == CIRCUIT_SEPARATE_RELAY);
  }
  tripwireResetContact = {static_cast<uint8_t>(ball.col), static_cast<uint8_t>(ball.row)};
  resetFlashFrames = SHORT_RESET_FLASH_FRAMES;

  if (!isValidPosition(ball.lastDeflector)) {
    if (tripwireResetCircuitPending) {
      tripwireResetCircuitActive = true;
      tripwireResetCircuitFrame = 0;
    }
    return;
  }

  // Do not let another projectile restart an animation already in progress.
  if (tripwireBackblastActive) {
    return;
  }

  tripwireBackblastActive = true;
  tripwireBackblastFrame = 0;
  tripwireBackblastStart = {static_cast<uint8_t>(ball.col), static_cast<uint8_t>(ball.row)};
  tripwireBackblastTarget = ball.lastDeflector;
  tripwireBackblastDeflectorDestroyed = false;

  // Remove queued projectiles that were redirected by the same deflector.
  // They are following the same final route and would otherwise hit the wire
  // again before the back-blast has time to destroy that piece. Other balls
  // remain frozen in place until the animation finishes.
  for (uint8_t i = 0; i < MAX_BALLS; ++i) {
    Ball& other = balls[i];
    if (!other.active || i == triggeringBallIndex) {
      continue;
    }
    if (other.lastDeflector.col == ball.lastDeflector.col &&
        other.lastDeflector.row == ball.lastDeflector.row) {
      other.active = false;
    }
  }

  const uint8_t distanceCells =
      max(static_cast<uint8_t>(abs(static_cast<int>(tripwireBackblastStart.col) -
                                   static_cast<int>(tripwireBackblastTarget.col))),
          static_cast<uint8_t>(abs(static_cast<int>(tripwireBackblastStart.row) -
                                   static_cast<int>(tripwireBackblastTarget.row))));
  tripwireBackblastTotalFrames = max(MIN_BACKBLAST_FRAMES, static_cast<uint8_t>(distanceCells * 5));
  playTone(620, 35);
}

void updateTripwireBackblast() {
  if (!tripwireBackblastActive) return;

  ++tripwireBackblastFrame;
  const uint8_t legFrames = max(static_cast<uint8_t>(1), tripwireBackblastTotalFrames);
  // A simple tripwire hit ends when the pulse reaches and destroys the guilty
  // deflector. The pulse only rebounds to the tripwire when there is actual
  // switch/gate state to reset afterward.
  const uint8_t totalFrames =
      tripwireResetCircuitPending
          ? min(static_cast<uint16_t>(UINT8_MAX), static_cast<uint16_t>(legFrames) * 2)
          : legFrames;

  // Outbound tone falls toward the deflector. The rising return tone is used
  // only when the pulse must continue back into the reset circuit.
  if ((tripwireBackblastFrame % 4) == 0) {
    if (tripwireBackblastFrame <= legFrames) {
      playTone(
          620 - min(static_cast<uint16_t>(tripwireBackblastFrame * 14), static_cast<uint16_t>(330)),
          14);
    } else if (tripwireResetCircuitPending) {
      const uint8_t returnFrame = tripwireBackblastFrame - legFrames;
      playTone(300 + min(static_cast<uint16_t>(returnFrame * 16), static_cast<uint16_t>(360)), 14);
    }
  }

  // Destroy the guilty deflector exactly when the pulse reaches it, then let
  // the same pulse visibly rebound to the tripwire before the circuit reset.
  if (!tripwireBackblastDeflectorDestroyed && tripwireBackblastFrame >= legFrames) {
    tripwireBackblastDeflectorDestroyed = true;
    if (isValidPosition(tripwireBackblastTarget)) {
      Tile& tile = board[tripwireBackblastTarget.row][tripwireBackblastTarget.col];
      if (isDeflectorTile(tile)) {
        tile = TILE_EMPTY;
        shatterEffectActive = true;
        shatterEffectPosition = tripwireBackblastTarget;
        shatterEffectFrame = 0;
        playTone(180, 70);
      }
    }
  }

  if (tripwireBackblastFrame >= totalFrames) {
    tripwireBackblastActive = false;

    if (tripwireResetCircuitPending) {
      // The pulse has now returned to the tripwire. Continue directly into the
      // shared dotted circuit without a disconnected visual handoff.
      tripwireResetCircuitActive = true;
      tripwireResetCircuitFrame = 0;
      playTone(430, 45);
    } else {
      lastFireMs = millis();
      lastBallStepMs = lastFireMs;
    }
  }
}

void updateTripwireResetCircuit() {
  if (!tripwireResetCircuitActive) return;

  ++tripwireResetCircuitFrame;
  if ((tripwireResetCircuitFrame % 5) == 0) {
    playTone(
        420 - min(static_cast<uint16_t>(tripwireResetCircuitFrame * 5), static_cast<uint16_t>(180)),
        16);
  }

  if (tripwireResetCircuitFrame >= RESET_CIRCUIT_FRAMES) {
    if (activeLevel.circuitMode == CIRCUIT_AND_DYNAMIC_TRIPWIRE) {
      // The deployed hazard is the failure condition for the whole AND state:
      // both main switches return to their visible inactive forms and the
      // end-target gate closes. The tripwire and its dedicated switch remain so
      // the player can explicitly retract the hazard before rebuilding A+B.
      switchAOn = false;
      switchBOn = false;
      gateOpen = false;
      dynamicTripwireActive = true;
      setDynamicTripwireSwitchVisible(true);
      resetFlashFrames = RESET_FLASH_FRAMES;
    } else if (activeLevel.circuitMode == CIRCUIT_SPLITTER_OVERLOAD) {
      switchAOn = false;
      switchBOn = false;
      gateOpen = false;
      setFinaleTripwire2SwitchVisible(finaleTripwire2Active);
      resetFlashFrames = RESET_FLASH_FRAMES;
    } else {
      resetPrimarySwitchImmediately();
      startGateBurst(SHORT_GATE_BURST_FRAMES);
    }
    tripwireResetCircuitActive = false;
    tripwireResetCircuitPending = false;
    playTone(140, 120);
    lastFireMs = millis();
    lastBallStepMs = lastFireMs;
  }
}

bool ballTouchesTripwireSegment(const Ball& ball, GridSegment segment) {
  return positionTouchesSegment(ball.col, ball.row, segment);
}

bool ballTouchesResetTripwire(const Ball& ball) {
  // In RELAY, switch B de-energizes the fixed tripwire. In SNEAKY, the tripwire does
  // not exist until switch B deploys it.
  if (relayTripwireDisabled) return false;
  if (activeLevel.circuitMode == CIRCUIT_AND_DYNAMIC_TRIPWIRE && !dynamicTripwireActive)
    return false;
  if (activeLevel.circuitMode == CIRCUIT_SPLITTER_OVERLOAD) {
    const bool touchesTripwire1 =
        finaleTripwire1Active && ballTouchesTripwireSegment(ball, activeLevel.tripwire);

    // Finale tripwire 2 is an upside-down L. Its original vertical leg remains,
    // and a horizontal leg extends from the top corner to the left edge.
    const bool touchesTripwire2 =
        finaleTripwire2Active &&
        (ballTouchesTripwireSegment(ball, activeLevel.tripwire2) ||
         ballTouchesTripwireSegment(
             ball, {{0, activeLevel.tripwire2.start.row}, activeLevel.tripwire2.start}));

    return touchesTripwire1 || touchesTripwire2;
  }

  return ballTouchesTripwireSegment(ball, activeLevel.tripwire) ||
         ballTouchesTripwireSegment(ball, activeLevel.tripwire2);
}

void updateActivation() {
  ++activationFrame;

  // First, send the activation pulse forward from the cannon to the switch.
  // Any player deflectors used by the successful route are consumed as the
  // pulse reaches them.
  if (activationState == ACTIVATION_CLEANUP) {
    const uint8_t pulseStep = activationFrame / ACTIVATION_FRAMES_PER_CELL;

    if (activationFrame == 8) {
      playTone(1680, 85);
    }

    if (pulseStep != lastPulseSoundStep) {
      lastPulseSoundStep = pulseStep;
      playTone(420 + min(static_cast<uint16_t>(pulseStep * 18), static_cast<uint16_t>(260)), 14);
    }

    if (visualPath.length > 1) {
      const int16_t pathIndex = pulseStep;
      if (pathIndex < visualPath.length && (activationFrame % ACTIVATION_FRAMES_PER_CELL) == 0) {
        const uint8_t col = visualPath.cells[pathIndex].col;
        const uint8_t row = visualPath.cells[pathIndex].row;
        if (isDeflectorTile(board[row][col])) {
          board[row][col] = TILE_EMPTY;
          playTone(300, 22);
        }
      }

      const uint16_t totalFrames = (visualPath.length - 1) * ACTIVATION_FRAMES_PER_CELL;
      if (activationFrame >= totalFrames) {
        activationState = ACTIVATION_CONTROL_LINE;
        activationFrame = 0;
        lastPulseSoundStep = INVALID_PULSE_STEP;
      }
    } else {
      activationState = ACTIVATION_CONTROL_LINE;
      activationFrame = 0;
    }
    return;
  }

  // Then carry the signal along the dotted control wire. The gate remains a
  // solid block until the pulse actually arrives.
  if (activationState == ACTIVATION_CONTROL_LINE) {
    if ((activationFrame % 5) == 0) {
      playTone(760 + min(static_cast<uint16_t>(activationFrame * 12), static_cast<uint16_t>(360)),
               18);
    }

    if (activationFrame >= CONTROL_PULSE_FRAMES) {
      if (isSeparateRelayCircuit(activeLevel.circuitMode)) {
        if (activationSwitchIndex == ACTIVATION_PRIMARY_SWITCH) {
          gateOpen = true;
          startGateBurst(GATE_BURST_FRAMES);
          playTone(190, 120);
        } else {
          relayTripwireDisabled = true;
          resetFlashFrames = RESET_FLASH_FRAMES;
          playTone(980, 110);
        }
      } else if (isAndCircuit(activeLevel.circuitMode)) {
        const bool wasOpen = gateOpen;
        gateOpen = switchAOn && switchBOn;
        if (activationSwitchIndex == ACTIVATION_SNEAKY_TRIPWIRE_SWITCH) {
          playTwoTone(1180, 70, 1540, 70);
        } else if (activationSwitchIndex == ACTIVATION_SECONDARY_SWITCH) {
          // The tripwire deployed the instant B was struck. The control pulse now
          // confirms both the hazard deployment and the AND gate result.
          playTone(520, 55);
          if (gateOpen && !wasOpen) {
            startGateBurst(GATE_BURST_FRAMES);
            playTone(190, 120);
          } else {
            playTone(260, 110);
          }
        } else {
          if (gateOpen && !wasOpen) {
            startGateBurst(GATE_BURST_FRAMES);
            playTone(190, 120);
          } else {
            playTone(920, 70);
          }
        }
      } else {
        gateOpen = switchAOn || switchBOn;
        startGateBurst(GATE_BURST_FRAMES);
        playTone(190, 120);
      }
      activationState = ACTIVATION_HOLD;
      activationFrame = 0;
    }
    return;
  }

  if (activationState == ACTIVATION_HOLD && activationFrame >= ACTIVATION_HOLD_FRAMES) {
    activationState = ACTIVATION_NONE;
    activationFrame = 0;
    lastFireMs = millis();
    lastBallStepMs = millis();
  }
}

void resolveBall(uint8_t index) {
  balls[index].active = false;
}

void fireBall() {
  for (uint8_t i = 0; i < MAX_BALLS; ++i) {
    if (!balls[i].active) {
      balls[i].active = true;
      balls[i].col = activeLevel.cannon.col;
      balls[i].row = activeLevel.cannon.row;
      balls[i].direction = activeLevel.cannonDirection;
      balls[i].progress = 0;
      balls[i].lastDeflector = NO_POSITION;
      balls[i].canSplit = true;

      cannonKickFrames = CANNON_KICK_FRAMES;
      playTone(330, 24);
      return;
    }
  }

  // Every projectile slot is occupied. Skip this launch and try again at the
  // next normal firing interval.
}

void updateShatterEffect() {
  if (!shatterEffectActive) return;
  ++shatterEffectFrame;
  if (shatterEffectFrame >= SHATTER_EFFECT_FRAMES) {
    shatterEffectActive = false;
  }
}

void beginLevelFailure() {
  if (levelFailed || won || celebrationState != CELEBRATION_NONE) {
    return;
  }

  levelFailed = true;
  clearBalls();
  activationState = ACTIVATION_NONE;
  celebrationState = CELEBRATION_EXPLOSION;
  celebrationFrame = 0;
  beginCannonExplosion();
  playTone(85, 300);
}

bool spawnSplitChild(const Ball& source, Direction direction) {
  for (uint8_t i = 0; i < MAX_BALLS; ++i) {
    if (!balls[i].active) {
      balls[i].active = true;
      balls[i].col = source.col;
      balls[i].row = source.row;
      balls[i].direction = direction;
      balls[i].progress = 0;
      balls[i].lastDeflector = source.lastDeflector;
      balls[i].canSplit = false;
      return true;
    }
  }
  return false;
}

void processTileArrival(uint8_t index) {
  Ball& ball = balls[index];

  if (!isInsideGrid(ball.col, ball.row)) {
    resolveBall(index);
    return;
  }

  if (ballTouchesResetTripwire(ball)) {
    beginTripwireResetBackblast(index);
    // The tripwire absorbs the projectile. This prevents a ball traveling
    // lengthwise along the beam from retriggering it on every grid cell.
    resolveBall(index);
    return;
  }

  const Tile tile = board[ball.row][ball.col];

  switch (tile) {
    case TILE_WALL:
      resolveBall(index);
      playTone(150, 45);
      break;

    case TILE_CANNON:
      beginLevelFailure();
      break;

    case TILE_TARGET:
      beginCelebration();
      break;

    case TILE_SWITCH_A:
      if (!switchAOn) {
        beginSwitchActivation(ACTIVATION_PRIMARY_SWITCH);
      } else {
        // Once latched, the switch becomes a solid state block. This makes
        // the activated state visually and mechanically unambiguous.
        resolveBall(index);
        playTone(150, 45);
      }
      break;

    case TILE_SWITCH_B:
      if (!switchBOn) {
        beginSwitchActivation(ACTIVATION_SECONDARY_SWITCH);
      } else {
        resolveBall(index);
        playTone(150, 45);
      }
      break;

    case TILE_TRIPWIRE_SWITCH:
      if (activeLevel.circuitMode == CIRCUIT_SPLITTER_OVERLOAD) {
        // The permanent first finale tripwire has no control switch. The only
        // temporary hazard switch in this level controls tripwire 2.
        beginSwitchActivation(ACTIVATION_FINALE_TRIPWIRE_SWITCH);
      } else {
        beginSwitchActivation(ACTIVATION_SNEAKY_TRIPWIRE_SWITCH);
      }
      break;

    case TILE_SPLITTER:
      if (ball.canSplit) {
        const Ball source = ball;
        resolveBall(index);
        // Perpendicular fan-out: an east/west shot becomes north + south;
        // a north/south shot becomes east + west. Children cannot split again.
        spawnSplitChild(source,
                        static_cast<Direction>((static_cast<uint8_t>(source.direction) + 2) % 8));
        spawnSplitChild(source,
                        static_cast<Direction>((static_cast<uint8_t>(source.direction) + 6) % 8));
        playTwoTone(1460, 45, 1960, 65);
      } else {
        resolveBall(index);
        playTone(180, 45);
      }
      break;

    case TILE_GATE:
      if (!gateOpen) {
        resolveBall(index);
        playTone(150, 45);
      }
      break;

    case TILE_DEFLECT_X:
      ball.lastDeflector = {static_cast<uint8_t>(ball.col), static_cast<uint8_t>(ball.row)};
      ball.direction = deflectX(ball.direction);
      playTone(1040, 18);
      break;

    case TILE_DEFLECT_DOT:
      ball.lastDeflector = {static_cast<uint8_t>(ball.col), static_cast<uint8_t>(ball.row)};
      ball.direction = deflectDot(ball.direction);
      playTone(1240, 18);
      break;

    case TILE_EMPTY:
    default:
      break;
  }
}

void stepBalls() {
  // Only balls that existed at the start of this simulation tick may move.
  // Split children therefore begin together on the following tick regardless
  // of which free array slots they receive.
  const uint16_t activeAtStart = activeBallMask(balls);

  for (uint8_t i = 0; i < MAX_BALLS; ++i) {
    Ball& ball = balls[i];

    if ((activeAtStart & static_cast<uint16_t>(1U << i)) == 0 || !ball.active) {
      continue;
    }

    ++ball.progress;

    if (ball.progress >= CELL_SIZE) {
      ball.progress = 0;
      ball.col += directionDeltaX(ball.direction);
      ball.row += directionDeltaY(ball.direction);
      processTileArrival(i);
    }
  }
}

void beginCannonExplosion() {
  const int16_t originX8 = cellCenterX(activeLevel.cannon.col) * 8;
  const int16_t originY8 = cellCenterY(activeLevel.cannon.row) * 8;

  // Hand-tuned, deterministic velocities keep the burst dramatic without
  // needing random-number state or much memory. Units are eighth-pixels/frame.
  static const int8_t vx[MAX_CANNON_FRAGMENTS] PROGMEM = {
      -15, -12, -9, -6, -3, 2, 5, 8, 11, 14, 17, -14, -10, -7, -4, 3, 6, 9};
  static const int8_t vy[MAX_CANNON_FRAGMENTS] PROGMEM = {
      -17, -11, -6, 2, 8, -15, -9, -4, 3, 9, 15, 14, 8, -14, 15, 12, 7, -12};

  for (uint8_t i = 0; i < MAX_CANNON_FRAGMENTS; ++i) {
    cannonFragments[i].x8 = originX8;
    cannonFragments[i].y8 = originY8;
    cannonFragments[i].vx8 = static_cast<int8_t>(pgm_read_byte(vx + i));
    cannonFragments[i].vy8 = static_cast<int8_t>(pgm_read_byte(vy + i));
    cannonFragments[i].life = CANNON_EXPLOSION_FRAMES - (i % 8);
    cannonFragments[i].shape = i % 3;
  }
}

void updateCannonExplosion() {
  for (uint8_t i = 0; i < MAX_CANNON_FRAGMENTS; ++i) {
    CannonFragment& fragment = cannonFragments[i];
    if (fragment.life == 0) {
      continue;
    }

    fragment.x8 += fragment.vx8;
    fragment.y8 += fragment.vy8;

    // Very light gravity gives the explosion a satisfying outward arc.
    if ((celebrationFrame & 1) == 0 && fragment.vy8 < 20) {
      ++fragment.vy8;
    }

    --fragment.life;
  }
}

void updateCelebration() {
  ++celebrationFrame;

  switch (celebrationState) {
    case CELEBRATION_TARGET:
      if ((celebrationFrame % 30) == 1) {
        const uint8_t pulseIndex = celebrationFrame / 30;
        playTone(620 + pulseIndex * 55, 55);
      }
      if (celebrationFrame >= TARGET_CHARGE_FRAMES) {
        celebrationState = CELEBRATION_PULSE;
        celebrationFrame = 0;
        lastPulseSoundStep = INVALID_PULSE_STEP;
      }
      break;

    case CELEBRATION_PULSE: {
      const uint8_t pulseStep = celebrationFrame / PULSE_FRAMES_PER_CELL;
      if (pulseStep != lastPulseSoundStep) {
        lastPulseSoundStep = pulseStep;
        playTone(1450 - min(static_cast<uint16_t>(pulseStep * 35), static_cast<uint16_t>(650)), 18);
      }
      const uint16_t pathSegments = visualPath.length > 1 ? visualPath.length - 1 : 1;
      const uint16_t totalFrames = pathSegments * PULSE_FRAMES_PER_CELL;
      if (celebrationFrame >= totalFrames) {
        celebrationState = CELEBRATION_CANNON;
        celebrationFrame = 0;
        playTone(220, 90);
      }
      break;
    }

    case CELEBRATION_CANNON:
      if (celebrationFrame >= CANNON_CELEBRATE_FRAMES) {
        celebrationState = CELEBRATION_EXPLOSION;
        celebrationFrame = 0;
        beginCannonExplosion();
        playTone(110, 260);
      }
      break;

    case CELEBRATION_EXPLOSION:
      updateCannonExplosion();
      if (celebrationFrame >= CANNON_EXPLOSION_FRAMES) {
        celebrationState = CELEBRATION_DONE;
        if (levelFailed) {
          playTone(120, 180);
        } else {
          won = true;
          playTone(660, 180);
        }
      }
      break;

    case CELEBRATION_NONE:
    case CELEBRATION_DONE:
      break;
  }
}

void updateGame() {
  if (gameScreen == SCREEN_TITLE) {
    ++titleBobFrame;
    return;
  }
  if (gameScreen == SCREEN_LEVEL_SELECT || gameScreen == SCREEN_GAME_OVER) {
    return;
  }

  if (won) {
    return;
  }

  if (levelIntroFrames > 0) {
    --levelIntroFrames;
    if (levelIntroFrames == 0) {
      beginLevelPlay();
    }
    return;
  }

  if (celebrationState != CELEBRATION_NONE) {
    updateCelebration();
    return;
  }

  if (activationState != ACTIVATION_NONE) {
    updateActivation();
    if (gateBurstFrames > 0) --gateBurstFrames;
    if (resetFlashFrames > 0) --resetFlashFrames;
    return;
  }

  updateShatterEffect();

  // A tripwire hit is an atomic event. Freeze every surviving projectile and
  // suspend cannon firing while the visible back-blast travels to the guilty
  // deflector. Projectiles on that same route were removed when the event
  // began, preventing a second trigger race.
  if (tripwireBackblastActive) {
    updateTripwireBackblast();
    if (resetFlashFrames > 0) --resetFlashFrames;
    return;
  }

  if (tripwireResetCircuitActive) {
    updateTripwireResetCircuit();
    if (resetFlashFrames > 0) --resetFlashFrames;
    if (gateBurstFrames > 0) --gateBurstFrames;
    return;
  }

  const uint32_t now = millis();

  // Never emit a burst of delayed launches after a stalled frame.
  if (now - lastFireMs >= FIRE_INTERVAL_MS) {
    lastFireMs = now;
    fireBall();
  }

  // Catch up a few simulation ticks after a slow frame, but cap the work to
  // avoid a long "spiral of death" after a debugger pause or device stall.
  uint8_t ballSteps = 0;
  while (now - lastBallStepMs >= BALL_STEP_MS && ballSteps < MAX_BALL_STEPS_PER_FRAME) {
    lastBallStepMs += BALL_STEP_MS;
    stepBalls();
    ++ballSteps;
  }
  if (now - lastBallStepMs >= BALL_STEP_MS) {
    lastBallStepMs = now;
  }

  if (cannonKickFrames > 0) {
    --cannonKickFrames;
  }
  if (gateBurstFrames > 0) {
    --gateBurstFrames;
  }
  if (resetFlashFrames > 0) {
    --resetFlashFrames;
  }
}

}  // namespace

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(60);
  currentLevel = 0;
  levelSelectIndex = 0;
  gameScreen = SCREEN_TITLE;
}

void loop() {
  if (!arduboy.nextFrame()) {
    return;
  }

  arduboy.pollButtons();
  handleInput();
  updateGame();
  Renderer::drawGame();
}

}  // namespace Game
}  // namespace MostlyLogic
