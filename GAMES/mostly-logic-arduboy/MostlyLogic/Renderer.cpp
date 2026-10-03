// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 MostlyBuilds

/*
 * Drawing code for gameplay, menus, and visual effects.
 *
 * Renderer functions read live state but do not change puzzle rules or advance
 * timers. Drawing is immediate-mode into Arduboy2's one-bit framebuffer.
 */

#include "Renderer.h"
#include "Artwork.h"
#include "GameState.h"

namespace MostlyLogic {
namespace Renderer {

using namespace Game;

namespace {

void drawWall(uint8_t col, uint8_t row) {
  const int16_t x = BOARD_X + col * CELL_SIZE;
  const int16_t y = BOARD_Y + row * CELL_SIZE;

  arduboy.fillRect(x + 1, y + 1, 6, 6, WHITE);
  arduboy.drawPixel(x + 2, y + 2, BLACK);
  arduboy.drawPixel(x + 5, y + 2, BLACK);
  arduboy.drawFastHLine(x + 2, y + 5, 4, BLACK);
}

void drawShatterEffect() {
  if (!shatterEffectActive) return;
  if (!isValidPosition(shatterEffectPosition)) return;
  const int16_t cx = cellCenterX(shatterEffectPosition.col);
  const int16_t cy = cellCenterY(shatterEffectPosition.row);
  const uint8_t frame = shatterEffectFrame;
  if (frame < 5) {
    arduboy.drawPixel(cx - frame, cy, WHITE);
    arduboy.drawPixel(cx + frame, cy, WHITE);
    arduboy.drawPixel(cx, cy - frame, WHITE);
    arduboy.drawPixel(cx, cy + frame, WHITE);
  }
}

void drawDeflectorIcon(int16_t x, int16_t y, bool isX) {
  // Both icons are rotationally symmetrical: X turns counterclockwise and dot
  // turns clockwise without either symbol implying a travel direction.
  const Artwork::DeflectorFrame frame =
      isX ? Artwork::DEFLECTOR_X_FRAME : Artwork::DEFLECTOR_DOT_FRAME;
  Sprites::drawSelfMasked(x, y, Artwork::DEFLECTOR_SPRITES, frame);
}

void drawDeflector(uint8_t col, uint8_t row, bool isX) {
  const int16_t x = BOARD_X + col * CELL_SIZE;
  const int16_t y = BOARD_Y + row * CELL_SIZE;

  drawDeflectorIcon(x, y, isX);
}

void drawCannon(uint8_t col, uint8_t row) {
  if (celebrationState == CELEBRATION_EXPLOSION) {
    return;
  }

  const int16_t x = BOARD_X + col * CELL_SIZE;
  const int16_t y = BOARD_Y + row * CELL_SIZE;
  int8_t kick = cannonKickFrames > 0 ? 1 : 0;
  int8_t hop = 0;

  if (celebrationState == CELEBRATION_CANNON) {
    // Two quick celebratory hops with a tiny barrel recoil.
    const uint8_t phase = celebrationFrame % 12;
    hop = (phase >= 2 && phase <= 7) ? -1 : 0;
    if (phase >= 4 && phase <= 6) {
      hop = -2;
    }
    kick = (phase >= 1 && phase <= 4) ? 2 : 0;
  }

  arduboy.fillRect(x, y + 2 + hop, 5, 4, WHITE);
  arduboy.fillRect(x + 4 - kick, y + 3 + hop, 4, 2, WHITE);
  arduboy.drawPixel(x + 1, y + 1 + hop, WHITE);
  arduboy.drawPixel(x + 1, y + 6 + hop, WHITE);
}

void drawTarget(uint8_t col, uint8_t row) {
  const int16_t cx = cellCenterX(col);
  const int16_t cy = cellCenterY(row);

  if (celebrationState == CELEBRATION_TARGET) {
    // A slow 30-frame breathing cycle holds attention on the successful hit
    // before the existing return pulse launches back toward the cannon.
    const uint8_t cycle = celebrationFrame % 30;
    const uint8_t swell = cycle < 15 ? cycle : 29 - cycle;
    const uint8_t pulseRadius = 2 + swell / 5;  // 2, 3, 4, then back down.

    arduboy.fillCircle(cx, cy, 1, WHITE);
    arduboy.drawCircle(cx, cy, 3, WHITE);

    // Expanding/contracting halo. At radius 3, invert the ring so the
    // breathing motion remains visible instead of becoming a solid blob.
    if (pulseRadius == 3) {
      arduboy.drawCircle(cx, cy, pulseRadius, BLACK);
      arduboy.drawCircle(cx, cy, 2, WHITE);
    } else {
      arduboy.drawCircle(cx, cy, pulseRadius, WHITE);
    }

    // Four restrained sparkle beats per cycle.
    if (cycle == 2 || cycle == 9 || cycle == 16 || cycle == 23) {
      arduboy.drawPixel(cx - 4, cy, WHITE);
      arduboy.drawPixel(cx + 4, cy, WHITE);
      arduboy.drawPixel(cx, cy - 4, WHITE);
      arduboy.drawPixel(cx, cy + 4, WHITE);
    }
  } else if ((!levelFailed &&
              (celebrationState == CELEBRATION_PULSE || celebrationState == CELEBRATION_CANNON ||
               celebrationState == CELEBRATION_EXPLOSION)) ||
             won) {
    arduboy.fillCircle(cx, cy, 3, WHITE);
    arduboy.drawPixel(cx, cy, BLACK);
  } else {
    arduboy.drawCircle(cx, cy, 3, WHITE);
    arduboy.drawCircle(cx, cy, 1, WHITE);
  }
}

void drawCircuitSwitch(uint8_t col, uint8_t row, bool isOn) {
  const int16_t x = BOARD_X + col * CELL_SIZE;
  const int16_t y = BOARD_Y + row * CELL_SIZE;
  const int16_t cy = y + 4;

  if (isOn) {
    // Latched state: use the exact same plain solid block as the linked gate.
    // Matching silhouettes make the relationship obvious without extra symbols.
    arduboy.fillRect(x + 1, y + 1, 6, 6, WHITE);
    return;
  }

  // Unlatched state: a tiny mechanical switch/contact icon. Its squared
  // silhouette deliberately differs from the circular end target.
  arduboy.drawRect(x + 1, y + 1, 6, 6, WHITE);
  arduboy.drawFastHLine(x + 2, y + 2, 2, WHITE);
  arduboy.drawFastHLine(x + 4, y + 5, 2, WHITE);
  arduboy.drawLine(x + 3, y + 3, x + 5, y + 4, WHITE);
  if (resetFlashFrames > 0 && (resetFlashFrames & 2)) {
    arduboy.drawPixel(x, cy, WHITE);
    arduboy.drawPixel(x + 7, cy, WHITE);
  }
}

void drawSplitter(uint8_t col, uint8_t row) {
  const int16_t x = BOARD_X + col * CELL_SIZE;
  const int16_t y = BOARD_Y + row * CELL_SIZE;
  Sprites::drawSelfMasked(x, y, Artwork::SPLITTER_SPRITE, 0);
}

void drawGate(uint8_t col, uint8_t row) {
  // An open gate is rendered as completely empty space: a clean hole in
  // the surrounding wall.
  if (gateOpen) {
    return;
  }

  const int16_t x = BOARD_X + col * CELL_SIZE;
  const int16_t y = BOARD_Y + row * CELL_SIZE;

  // Closed gate: a plain solid special block, deliberately different from
  // the patterned normal wall tile. The activated switch uses this same art.
  arduboy.fillRect(x + 1, y + 1, 6, 6, WHITE);
}

void drawVisibleCircuitDot(int16_t x, int16_t y) {
  // Give every control-wire dot a tiny black halo. This keeps the circuit
  // readable when it passes beside or across patterned map blocks.
  arduboy.drawPixel(x - 1, y, BLACK);
  arduboy.drawPixel(x + 1, y, BLACK);
  arduboy.drawPixel(x, y - 1, BLACK);
  arduboy.drawPixel(x, y + 1, BLACK);
  arduboy.drawPixel(x, y, WHITE);
}

void drawDottedSegment(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t phase) {
  if (x1 == x2) {
    const int16_t lo = min(y1, y2);
    const int16_t hi = max(y1, y2);
    for (int16_t y = lo; y <= hi; ++y) {
      if (((y + phase) % 4) == 0) drawVisibleCircuitDot(x1, y);
    }
  } else if (y1 == y2) {
    const int16_t lo = min(x1, x2);
    const int16_t hi = max(x1, x2);
    for (int16_t x = lo; x <= hi; ++x) {
      if (((x + phase) % 4) == 0) drawVisibleCircuitDot(x, y1);
    }
  }
}

void getPrimaryCircuitGeometry(
    int16_t& sx, int16_t& sy, int16_t& gx, int16_t& gy, int16_t& trunkX) {
  sx = cellCenterX(activeLevel.switchA.col);
  sy = cellCenterY(activeLevel.switchA.row);
  gx = cellCenterX(activeLevel.gate.col);
  gy = cellCenterY(activeLevel.gate.row);
  trunkX = gx - (CELL_SIZE * 2);
  if (trunkX <= BOARD_X) trunkX = gx + (CELL_SIZE * 2);
}

void getTripwireMidpoint(int16_t& lx, int16_t& ly) {
  const int16_t x1 = cellCenterX(activeLevel.tripwire.start.col);
  const int16_t y1 = cellCenterY(activeLevel.tripwire.start.row);
  const int16_t x2 = cellCenterX(activeLevel.tripwire.end.col);
  const int16_t y2 = cellCenterY(activeLevel.tripwire.end.row);
  if (tripwireResetCircuitActive && isValidPosition(tripwireResetContact)) {
    lx = cellCenterX(tripwireResetContact.col);
    ly = cellCenterY(tripwireResetContact.row);
  } else {
    lx = (x1 + x2) / 2;
    ly = (y1 + y2) / 2;
  }
}

void drawControlConnection() {
  if (!isValidPosition(activeLevel.switchA) || !isValidPosition(activeLevel.gate)) return;

  int16_t ax, ay, gx, gy, trunkX;
  getPrimaryCircuitGeometry(ax, ay, gx, gy, trunkX);

  if (isAndCircuit(activeLevel.circuitMode)) {
    const int16_t bx = cellCenterX(activeLevel.switchB.col);
    const int16_t by = cellCenterY(activeLevel.switchB.row);
    const int16_t junctionX = gx - (CELL_SIZE * 2);
    const int16_t junctionY = gy;

    if (!switchAOn || (activationState != ACTIVATION_NONE &&
                       activationSwitchIndex == ACTIVATION_PRIMARY_SWITCH)) {
      drawDottedSegment(ax, ay, junctionX, ay, 0);
      drawDottedSegment(junctionX, ay, junctionX, junctionY, 0);
    }
    if (!switchBOn || (activationState != ACTIVATION_NONE &&
                       activationSwitchIndex == ACTIVATION_SECONDARY_SWITCH)) {
      drawDottedSegment(bx, by, junctionX, by, 0);
      drawDottedSegment(junctionX, by, junctionX, junctionY, 0);
    }
    if (!gateOpen || activationState != ACTIVATION_NONE) {
      drawDottedSegment(junctionX, junctionY, gx, gy, 0);
    }
    // While the deployed tripwire exists, show its temporary control switch
    // as a separate circuit near A. The line disappears with the tripwire.
    if (activeLevel.circuitMode == CIRCUIT_AND_DYNAMIC_TRIPWIRE && dynamicTripwireActive) {
      const int16_t wx = cellCenterX(SNEAKY_TRIPWIRE_SWITCH_POSITION.col);
      const int16_t wy = cellCenterY(SNEAKY_TRIPWIRE_SWITCH_POSITION.row);
      int16_t lx, ignoredTripwireY;
      getTripwireMidpoint(lx, ignoredTripwireY);
      // Center the long vertical leg in the open column between the surrounding
      // block groups. Drop below the four-block stack before turning right.
      const int16_t wireBendX = cellCenterX(5);
      const int16_t wireBendY = cellCenterY(4);
      drawDottedSegment(wx, wy, wireBendX, wy, 0);
      drawDottedSegment(wireBendX, wy, wireBendX, wireBendY, 0);
      drawDottedSegment(wireBendX, wireBendY, lx, wireBendY, 0);
    }
    if (activeLevel.circuitMode == CIRCUIT_SPLITTER_OVERLOAD) {
      if (finaleTripwire2Active) {
        const int16_t wx = cellCenterX(FINALE_TRIPWIRE_SWITCH_POSITION.col);
        const int16_t wy = cellCenterY(FINALE_TRIPWIRE_SWITCH_POSITION.row);
        const int16_t lx = cellCenterX(activeLevel.tripwire2.start.col);
        const int16_t ly = (cellCenterY(activeLevel.tripwire2.start.row) +
                            cellCenterY(activeLevel.tripwire2.end.row)) /
                           2;
        drawDottedSegment(wx, wy, lx - CELL_SIZE, wy, 0);
        drawDottedSegment(lx - CELL_SIZE, wy, lx - CELL_SIZE, ly, 0);
        drawDottedSegment(lx - CELL_SIZE, ly, lx, ly, 0);
      }
    }
    return;
  }

  if (isSeparateRelayCircuit(activeLevel.circuitMode)) {
    // Circuit A: outer switch -> gate. The complete line disappears when A
    // latches, leaving no orphaned trunk pieces.
    if (!switchAOn || tripwireResetCircuitActive ||
        (activationState != ACTIVATION_NONE &&
         activationSwitchIndex == ACTIVATION_PRIMARY_SWITCH)) {
      drawDottedSegment(ax, ay, trunkX, ay, 0);
      drawDottedSegment(trunkX, ay, trunkX, gy, 0);
      drawDottedSegment(trunkX, gy, gx, gy, 0);
    }

    // Circuit B: inner switch -> tripwire. It is fully independent of the gate
    // circuit and also disappears completely when B de-energizes the wire.
    if ((!switchBOn || (activationState != ACTIVATION_NONE &&
                        activationSwitchIndex == ACTIVATION_SECONDARY_SWITCH)) &&
        isValidSegment(activeLevel.tripwire)) {
      const int16_t bx = cellCenterX(activeLevel.switchB.col);
      const int16_t by = cellCenterY(activeLevel.switchB.row);
      int16_t lx, ly;
      getTripwireMidpoint(lx, ly);
      const int16_t branchX = lx - CELL_SIZE;
      drawDottedSegment(bx, by, branchX, by, 0);
      drawDottedSegment(branchX, by, branchX, ly, 0);
      drawDottedSegment(branchX, ly, lx, ly, 0);
    }
    return;
  }

  // Original shared circuit used by TRIP CIRCUIT and FAILSAFE.
  int16_t lx = 0, ly = 0;
  getTripwireMidpoint(lx, ly);
  if (!switchAOn || tripwireResetCircuitActive) {
    drawDottedSegment(ax, ay, trunkX, ay, 0);
    drawDottedSegment(trunkX, ay, trunkX, gy, 0);
  }
  drawDottedSegment(trunkX, gy, gx, gy, 0);
  if (isValidSegment(activeLevel.tripwire)) {
    drawDottedSegment(lx, ly, trunkX, ly, 0);
    drawDottedSegment(trunkX, ly, trunkX, gy, 0);
  }
  if (isValidSegment(activeLevel.tripwire2)) {
    const int16_t lx2 = (cellCenterX(activeLevel.tripwire2.start.col) +
                         cellCenterX(activeLevel.tripwire2.end.col)) /
                        2;
    const int16_t ly2 = (cellCenterY(activeLevel.tripwire2.start.row) +
                         cellCenterY(activeLevel.tripwire2.end.row)) /
                        2;
    drawDottedSegment(lx2, ly2, trunkX, ly2, 0);
    drawDottedSegment(trunkX, ly2, trunkX, gy, 0);
  }
}

void drawControlPulse() {
  if (activationState != ACTIVATION_CONTROL_LINE || !isValidPosition(activeLevel.switchA)) return;

  const bool separate = isSeparateRelayCircuit(activeLevel.circuitMode);
  const CellPosition pulseStart = getActivationSwitchPosition(activationSwitchIndex);
  if (!isValidPosition(pulseStart)) return;
  const int16_t sx = cellCenterX(pulseStart.col);
  const int16_t sy = cellCenterY(pulseStart.row);

  int16_t endpointX;
  int16_t endpointY;
  int16_t bendX;

  if (activeLevel.circuitMode == CIRCUIT_SPLITTER_OVERLOAD &&
      activationSwitchIndex == ACTIVATION_FINALE_TRIPWIRE_SWITCH) {
    endpointX = cellCenterX(activeLevel.tripwire2.start.col);
    endpointY = (cellCenterY(activeLevel.tripwire2.start.row) +
                 cellCenterY(activeLevel.tripwire2.end.row)) /
                2;
    bendX = endpointX - CELL_SIZE;
  } else if ((separate && activationSwitchIndex == ACTIVATION_SECONDARY_SWITCH) ||
             (isAndCircuit(activeLevel.circuitMode) &&
              activationSwitchIndex == ACTIVATION_SNEAKY_TRIPWIRE_SWITCH)) {
    getTripwireMidpoint(endpointX, endpointY);
    // Match SNEAKY's visible control connection exactly: the long vertical leg
    // is centered in the open column, then turns right below the wall stack.
    if (activeLevel.circuitMode == CIRCUIT_AND_DYNAMIC_TRIPWIRE) {
      bendX = cellCenterX(5);
      endpointY = cellCenterY(4);
    } else {
      bendX = endpointX - CELL_SIZE;
    }
  } else {
    endpointX = cellCenterX(activeLevel.gate.col);
    endpointY = cellCenterY(activeLevel.gate.row);
    bendX = endpointX - (CELL_SIZE * 2);
    if (bendX <= BOARD_X) bendX = endpointX + (CELL_SIZE * 2);
  }

  const uint8_t elapsed = min(activationFrame, CONTROL_PULSE_FRAMES);
  const uint8_t third =
      max(static_cast<uint8_t>(1), static_cast<uint8_t>(CONTROL_PULSE_FRAMES / 3));
  int16_t px = sx;
  int16_t py = sy;

  if (elapsed < third) {
    px = sx + ((bendX - sx) * elapsed) / third;
  } else if (elapsed < third * 2) {
    px = bendX;
    py = sy + ((endpointY - sy) * (elapsed - third)) / third;
  } else {
    px = bendX + ((endpointX - bendX) * (elapsed - third * 2)) / third;
    py = endpointY;
  }

  arduboy.fillCircle(px, py, 1, WHITE);
}

void drawTripwireResetCircuitPulse() {
  if (!tripwireResetCircuitActive || !isValidPosition(activeLevel.switchA)) return;

  int16_t sx, sy, gx, gy, trunkX;
  getPrimaryCircuitGeometry(sx, sy, gx, gy, trunkX);
  int16_t lx, ly;
  getTripwireMidpoint(lx, ly);

  // Finale hazard contact resets both AND inputs and the final gate.
  if (activeLevel.circuitMode == CIRCUIT_SPLITTER_OVERLOAD) {
    if (isValidPosition(tripwireResetContact)) {
      lx = cellCenterX(tripwireResetContact.col);
      ly = cellCenterY(tripwireResetContact.row);
    }
    const int16_t ax = cellCenterX(activeLevel.switchA.col);
    const int16_t ay = cellCenterY(activeLevel.switchA.row);
    const int16_t bx = cellCenterX(activeLevel.switchB.col);
    const int16_t by = cellCenterY(activeLevel.switchB.row);
    const uint8_t half =
        max(static_cast<uint8_t>(1), static_cast<uint8_t>(RESET_CIRCUIT_FRAMES / 2));
    const uint8_t frame = min(tripwireResetCircuitFrame, RESET_CIRCUIT_FRAMES);
    const int16_t junctionX = cellCenterX(6);
    const int16_t junctionY = cellCenterY(3);
    int16_t px = lx, py = ly;
    if (frame < half) {
      px = lx + ((junctionX - lx) * frame) / half;
      py = ly + ((junctionY - ly) * frame) / half;
      arduboy.fillCircle(px, py, 2, WHITE);
    } else {
      const uint8_t f = frame - half;
      const int16_t p1x = junctionX + ((ax - junctionX) * f) / half;
      const int16_t p1y = junctionY + ((ay - junctionY) * f) / half;
      const int16_t p2x = junctionX + ((bx - junctionX) * f) / half;
      const int16_t p2y = junctionY + ((by - junctionY) * f) / half;
      arduboy.fillCircle(p1x, p1y, 2, WHITE);
      arduboy.fillCircle(p2x, p2y, 2, WHITE);
    }
    return;
  }

  // SNEAKY's tripwire is owned by Switch B, so its reset pulse travels directly
  // from the contact point to B rather than splitting toward A and the gate.
  if (activeLevel.circuitMode == CIRCUIT_AND_DYNAMIC_TRIPWIRE) {
    const int16_t bx = cellCenterX(activeLevel.switchB.col);
    const int16_t by = cellCenterY(activeLevel.switchB.row);
    const int16_t bendX = lx + CELL_SIZE;
    const uint8_t third =
        max(static_cast<uint8_t>(1), static_cast<uint8_t>(RESET_CIRCUIT_FRAMES / 3));
    const uint8_t frame = min(tripwireResetCircuitFrame, RESET_CIRCUIT_FRAMES);
    int16_t px = lx;
    int16_t py = ly;
    if (frame < third) {
      px = lx + ((bendX - lx) * frame) / third;
    } else if (frame < third * 2) {
      px = bendX;
      py = ly + ((by - ly) * (frame - third)) / third;
    } else {
      px = bendX + ((bx - bendX) * (frame - third * 2)) / third;
      py = by;
    }
    arduboy.fillCircle(px, py, 2, WHITE);
    arduboy.drawPixel(px, py, BLACK);
    return;
  }

  const uint8_t half = max(static_cast<uint8_t>(1), static_cast<uint8_t>(RESET_CIRCUIT_FRAMES / 2));
  const uint8_t t = min(tripwireResetCircuitFrame, half);

  // First half: travel from the tripwire contact to the circuit junction.
  if (tripwireResetCircuitFrame < half) {
    const int16_t px = lx + ((trunkX - lx) * t) / half;
    const int16_t py = ly;
    arduboy.fillCircle(px, py, 2, WHITE);
    arduboy.drawPixel(px, py, BLACK);
    return;
  }

  // Second half: split from the junction toward both controlled endpoints.
  const uint8_t branchT = min(static_cast<uint8_t>(tripwireResetCircuitFrame - half), half);
  const uint8_t quarter = max(static_cast<uint8_t>(1), static_cast<uint8_t>(half / 2));

  int16_t gatePx = trunkX;
  int16_t gatePy = ly;
  if (branchT < quarter) {
    gatePy = ly + ((gy - ly) * branchT) / quarter;
  } else {
    gatePy = gy;
    gatePx = trunkX + ((gx - trunkX) * (branchT - quarter)) / quarter;
  }

  int16_t switchPx = trunkX;
  int16_t switchPy = ly;
  if (branchT < quarter) {
    switchPy = ly + ((sy - ly) * branchT) / quarter;
  } else {
    switchPy = sy;
    switchPx = trunkX + ((sx - trunkX) * (branchT - quarter)) / quarter;
  }

  arduboy.fillCircle(gatePx, gatePy, 2, WHITE);
  arduboy.drawPixel(gatePx, gatePy, BLACK);
  arduboy.fillCircle(switchPx, switchPy, 2, WHITE);
  arduboy.drawPixel(switchPx, switchPy, BLACK);
}

void drawGateBurst() {
  if (gateBurstFrames == 0 || !isValidPosition(activeLevel.gate)) return;
  const int16_t cx = cellCenterX(activeLevel.gate.col);
  const int16_t cy = cellCenterY(activeLevel.gate.row);
  const uint8_t age = gateBurstDuration - gateBurstFrames;
  const uint8_t r = 1 + age / 2;
  arduboy.drawCircle(cx, cy, r, WHITE);
  arduboy.drawPixel(cx - age, cy, WHITE);
  arduboy.drawPixel(cx + age, cy, WHITE);
  arduboy.drawPixel(cx, cy - age / 2, WHITE);
  arduboy.drawPixel(cx, cy + age / 2, WHITE);
}

void drawTripwireSegment(GridSegment segment) {
  if (!isValidSegment(segment)) return;

  const int16_t x1 = cellCenterX(segment.start.col);
  const int16_t y1 = cellCenterY(segment.start.row);
  const int16_t x2 = cellCenterX(segment.end.col);
  const int16_t y2 = cellCenterY(segment.end.row);
  const bool flash = resetFlashFrames > 0 && (resetFlashFrames & 2);

  if (flash) {
    if (x1 == x2) {
      arduboy.drawFastVLine(x1 - 1, min(y1, y2), abs(y2 - y1) + 1, WHITE);
      arduboy.drawFastVLine(x1 + 1, min(y1, y2), abs(y2 - y1) + 1, WHITE);
    } else {
      arduboy.drawFastHLine(min(x1, x2), y1 - 1, abs(x2 - x1) + 1, WHITE);
      arduboy.drawFastHLine(min(x1, x2), y1 + 1, abs(x2 - x1) + 1, WHITE);
    }
  }
  arduboy.drawLine(x1, y1, x2, y2, WHITE);
  arduboy.fillRect(x1 - 1, y1 - 1, 3, 3, WHITE);
  arduboy.fillRect(x2 - 1, y2 - 1, 3, 3, WHITE);
}

void drawResetTripwire() {
  if (relayTripwireDisabled) return;
  if (activeLevel.circuitMode == CIRCUIT_SPLITTER_OVERLOAD) {
    if (finaleTripwire1Active) drawTripwireSegment(activeLevel.tripwire);
    if (finaleTripwire2Active) {
      // Draw the second hazard as an upside-down L: the existing vertical leg
      // plus a horizontal leg from its top corner to the left edge.
      drawTripwireSegment(activeLevel.tripwire2);
      drawTripwireSegment({{0, activeLevel.tripwire2.start.row}, activeLevel.tripwire2.start});
    }
    return;
  }
  if (activeLevel.circuitMode == CIRCUIT_AND_DYNAMIC_TRIPWIRE && !dynamicTripwireActive) return;
  drawTripwireSegment(activeLevel.tripwire);
  drawTripwireSegment(activeLevel.tripwire2);
}

void drawActivationPulse() {
  if (activationState != ACTIVATION_CLEANUP || visualPath.length < 2) return;
  const uint16_t totalSteps = visualPath.length - 1;
  const uint16_t elapsedSteps = activationFrame / ACTIVATION_FRAMES_PER_CELL;
  const uint8_t subFrame = activationFrame % ACTIVATION_FRAMES_PER_CELL;
  if (elapsedSteps >= totalSteps) return;

  const int16_t x1 = cellCenterX(visualPath.cells[elapsedSteps].col);
  const int16_t y1 = cellCenterY(visualPath.cells[elapsedSteps].row);
  const int16_t x2 = cellCenterX(visualPath.cells[elapsedSteps + 1].col);
  const int16_t y2 = cellCenterY(visualPath.cells[elapsedSteps + 1].row);
  const int16_t px = x1 + ((x2 - x1) * (subFrame + 1)) / ACTIVATION_FRAMES_PER_CELL;
  const int16_t py = y1 + ((y2 - y1) * (subFrame + 1)) / ACTIVATION_FRAMES_PER_CELL;
  arduboy.fillCircle(px, py, 2, WHITE);
  arduboy.drawPixel(px, py, BLACK);
}

void drawGrid() {
  arduboy.drawRect(
      BOARD_X - 1, BOARD_Y - 1, GRID_COLS * CELL_SIZE + 2, GRID_ROWS * CELL_SIZE + 2, WHITE);

  // Control wiring is part of the background. Draw it before map objects so
  // walls, switches, gates, splitters, and player deflectors naturally mask it.
  // This prevents dots from visually merging with foreground tile artwork.
  drawControlConnection();

  for (uint8_t row = 0; row < GRID_ROWS; ++row) {
    for (uint8_t col = 0; col < GRID_COLS; ++col) {
      switch (board[row][col]) {
        case TILE_EMPTY:
          break;

        case TILE_WALL:
          drawWall(col, row);
          break;

        case TILE_DEFLECT_X:
          drawDeflector(col, row, true);
          break;

        case TILE_DEFLECT_DOT:
          drawDeflector(col, row, false);
          break;

        case TILE_CANNON:
          drawCannon(col, row);
          break;

        case TILE_TARGET:
          drawTarget(col, row);
          break;

        case TILE_SWITCH_A:
          drawCircuitSwitch(col, row, switchAOn);
          break;

        case TILE_SWITCH_B:
          drawCircuitSwitch(col, row, switchBOn);
          break;
        case TILE_TRIPWIRE_SWITCH:
          drawCircuitSwitch(col, row, false);
          break;
        case TILE_SPLITTER:
          drawSplitter(col, row);
          break;
        case TILE_GATE:
          drawGate(col, row);
          break;

        default:
          break;
      }
    }
  }

  // Tripwires remain foreground hazards, but dotted control wiring stays behind
  // every map tile and player-placed deflector.
  drawResetTripwire();
}

void drawTripwireBackblast() {
  if (!tripwireBackblastActive || !isValidPosition(tripwireBackblastStart) ||
      !isValidPosition(tripwireBackblastTarget))
    return;

  const int16_t sx = cellCenterX(tripwireBackblastStart.col);  // tripwire contact
  const int16_t sy = cellCenterY(tripwireBackblastStart.row);
  const int16_t tx = cellCenterX(tripwireBackblastTarget.col);  // guilty deflector
  const int16_t ty = cellCenterY(tripwireBackblastTarget.row);
  const uint8_t legFrames = max(static_cast<uint8_t>(1), tripwireBackblastTotalFrames);

  const bool returning = tripwireBackblastFrame > legFrames;
  const uint8_t legT =
      returning ? min(static_cast<uint8_t>(tripwireBackblastFrame - legFrames), legFrames)
                : min(tripwireBackblastFrame, legFrames);

  const int16_t fromX = returning ? tx : sx;
  const int16_t fromY = returning ? ty : sy;
  const int16_t toX = returning ? sx : tx;
  const int16_t toY = returning ? sy : ty;
  const int16_t px = fromX + ((toX - fromX) * legT) / legFrames;
  const int16_t py = fromY + ((toY - fromY) * legT) / legFrames;

  // Keep the complete tripwire-deflector segment visible as a flickering
  // electrical trail, tying the outbound and rebound legs together.
  const uint8_t distanceCells =
      max(static_cast<uint8_t>(abs(static_cast<int>(tripwireBackblastStart.col) -
                                   static_cast<int>(tripwireBackblastTarget.col))),
          static_cast<uint8_t>(abs(static_cast<int>(tripwireBackblastStart.row) -
                                   static_cast<int>(tripwireBackblastTarget.row))));
  for (uint8_t i = 0; i <= distanceCells; ++i) {
    const int16_t dx = sx + ((tx - sx) * i) / max(static_cast<uint8_t>(1), distanceCells);
    const int16_t dy = sy + ((ty - sy) * i) / max(static_cast<uint8_t>(1), distanceCells);
    const uint8_t reachedFrame =
        static_cast<uint16_t>(i) * legFrames / max(static_cast<uint8_t>(1), distanceCells);
    const bool reached = returning || reachedFrame <= legT;
    if (reached && ((i + tripwireBackblastFrame) & 1) == 0) {
      arduboy.drawPixel(dx, dy, WHITE);
    }
  }

  arduboy.fillCircle(px, py, 2, WHITE);
  arduboy.drawPixel(px, py, BLACK);

  // Tail points toward the immediately previous position on the active leg.
  const int16_t tailX =
      px - (toX - fromX) / max(static_cast<int16_t>(1), static_cast<int16_t>(distanceCells) * 3);
  const int16_t tailY =
      py - (toY - fromY) / max(static_cast<int16_t>(1), static_cast<int16_t>(distanceCells) * 3);
  arduboy.drawPixel(tailX, tailY, WHITE);
}

void drawBalls() {
  for (uint8_t i = 0; i < MAX_BALLS; ++i) {
    const Ball& ball = balls[i];

    if (!ball.active) {
      continue;
    }

    const int16_t startX = cellCenterX(ball.col);
    const int16_t startY = cellCenterY(ball.row);

    const int16_t x = startX + (directionDeltaX(ball.direction) * ball.progress);
    const int16_t y = startY + (directionDeltaY(ball.direction) * ball.progress);

    // Tiny trailing pixel makes the ball read as a moving projectile.
    if (ball.progress > 2) {
      arduboy.drawPixel(
          x - directionDeltaX(ball.direction) * 2, y - directionDeltaY(ball.direction) * 2, WHITE);
    }

    arduboy.fillCircle(x, y, 1, WHITE);
  }
}

void drawSuccessPulse() {
  if (celebrationState != CELEBRATION_PULSE || visualPath.length < 2) {
    return;
  }

  const uint16_t totalSteps = visualPath.length - 1;
  const uint16_t elapsedSteps = celebrationFrame / PULSE_FRAMES_PER_CELL;
  const uint8_t subFrame = celebrationFrame % PULSE_FRAMES_PER_CELL;
  const int16_t reverseSegment = static_cast<int16_t>(totalSteps) - 1 - elapsedSteps;

  if (reverseSegment < 0) {
    return;
  }

  const int16_t x1 = cellCenterX(visualPath.cells[reverseSegment + 1].col);
  const int16_t y1 = cellCenterY(visualPath.cells[reverseSegment + 1].row);
  const int16_t x2 = cellCenterX(visualPath.cells[reverseSegment].col);
  const int16_t y2 = cellCenterY(visualPath.cells[reverseSegment].row);

  // Draw a dim dotted trace over the already-traveled portion.
  for (uint8_t i = reverseSegment + 1; i < visualPath.length; ++i) {
    const int16_t tx = cellCenterX(visualPath.cells[i].col);
    const int16_t ty = cellCenterY(visualPath.cells[i].row);
    if (((i + celebrationFrame) & 1) == 0) {
      arduboy.drawPixel(tx, ty, WHITE);
    }
  }

  // Interpolate the bright pulse backward across the current cell segment.
  const int16_t px = x1 + ((x2 - x1) * (subFrame + 1)) / PULSE_FRAMES_PER_CELL;
  const int16_t py = y1 + ((y2 - y1) * (subFrame + 1)) / PULSE_FRAMES_PER_CELL;

  arduboy.fillCircle(px, py, 2, WHITE);
  arduboy.drawPixel(px, py, BLACK);

  // Short sparkling tail.
  arduboy.drawPixel((px + x1) / 2, (py + y1) / 2, WHITE);
}

void drawCannonExplosion() {
  if (celebrationState != CELEBRATION_EXPLOSION) {
    return;
  }

  const int16_t cx = cellCenterX(activeLevel.cannon.col);
  const int16_t cy = cellCenterY(activeLevel.cannon.row);

  // Initial flash and expanding shock ring.
  if (celebrationFrame < 8) {
    const uint8_t radius = 1 + celebrationFrame / 2;
    arduboy.drawCircle(cx, cy, radius, WHITE);
    if (celebrationFrame < 3) {
      arduboy.fillCircle(cx, cy, 3, WHITE);
    }
  }

  for (uint8_t i = 0; i < MAX_CANNON_FRAGMENTS; ++i) {
    const CannonFragment& fragment = cannonFragments[i];
    if (fragment.life == 0) {
      continue;
    }

    const int16_t x = fragment.x8 / 8;
    const int16_t y = fragment.y8 / 8;
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
      continue;
    }

    if (fragment.shape == 0) {
      arduboy.drawPixel(x, y, WHITE);
    } else if (fragment.shape == 1) {
      arduboy.drawFastHLine(x, y, 2, WHITE);
    } else {
      arduboy.drawFastVLine(x, y, 2, WHITE);
    }
  }
}

void drawCursor() {
  if (won || levelFailed || celebrationState != CELEBRATION_NONE ||
      activationState != ACTIVATION_NONE) {
    return;
  }

  const int16_t x = BOARD_X + cursorCol * CELL_SIZE;
  const int16_t y = BOARD_Y + cursorRow * CELL_SIZE;

  const bool blinkOn = ((millis() / 180) & 1) == 0;
  if (!blinkOn) {
    return;
  }

  // Four corner brackets leave the tile itself visible.
  arduboy.drawFastHLine(x, y, 3, WHITE);
  arduboy.drawFastVLine(x, y, 3, WHITE);

  arduboy.drawFastHLine(x + 5, y, 3, WHITE);
  arduboy.drawFastVLine(x + 7, y, 3, WHITE);

  arduboy.drawFastHLine(x, y + 7, 3, WHITE);
  arduboy.drawFastVLine(x, y + 5, 3, WHITE);

  arduboy.drawFastHLine(x + 5, y + 7, 3, WHITE);
  arduboy.drawFastVLine(x + 7, y + 5, 3, WHITE);
}

void drawTime(uint32_t elapsedMs, int16_t x, int16_t y) {
  const uint32_t totalSeconds = elapsedMs / 1000UL;
  const uint32_t minutes = totalSeconds / 60UL;
  const uint8_t seconds = totalSeconds % 60;

  arduboy.setCursor(x, y);
  arduboy.print(minutes);
  arduboy.print(':');
  if (seconds < 10) {
    arduboy.print('0');
  }
  arduboy.print(seconds);
}

void drawCenteredLevelName(int16_t y, uint8_t color) {
  char name[LEVEL_NAME_LENGTH];
  memcpy(name, activeLevel.name, LEVEL_NAME_LENGTH);
  name[LEVEL_NAME_LENGTH - 1] = '\0';
  const uint8_t width = strlen(name) * 6;
  arduboy.setTextColor(color);
  arduboy.setCursor((WIDTH - width) / 2, y);
  arduboy.print(name);
}

void drawLevelIntro() {
  if (levelIntroFrames == 0) return;

  arduboy.fillRect(10, 17, 108, 30, BLACK);
  arduboy.drawRect(10, 17, 108, 30, WHITE);
  arduboy.setCursor(48, 21);
  arduboy.print(F("LEVEL "));
  arduboy.print(currentLevel + 1);
  drawCenteredLevelName(32, WHITE);
}

void drawWinPopup() {
  arduboy.fillRect(10, 11, 108, 47, BLACK);
  arduboy.drawRect(10, 11, 108, 47, WHITE);
  arduboy.drawRect(12, 13, 104, 43, WHITE);

  arduboy.setCursor(35, 16);
  arduboy.print(F("COMPLETE!"));
  drawCenteredLevelName(25, WHITE);

  arduboy.setCursor(38, 35);
  arduboy.print(F("TIME "));
  drawTime(winTimeMs, 68, 35);

  arduboy.setCursor(20, 47);
  arduboy.print(F("A NEXT  B RETRY"));
}

void drawFailurePopup() {
  arduboy.fillRect(16, 16, 96, 34, BLACK);
  arduboy.drawRect(16, 16, 96, 34, WHITE);
  arduboy.drawRect(18, 18, 92, 30, WHITE);
  arduboy.setCursor(33, 22);
  arduboy.print(F("CANNON HIT!"));
  arduboy.setCursor(27, 35);
  arduboy.print(F("A/B  RESTART"));
}

void drawSpeakerIcon(int16_t x, int16_t y, bool enabled) {
  const Artwork::SpeakerFrame frame =
      enabled ? Artwork::SPEAKER_ON_FRAME : Artwork::SPEAKER_OFF_FRAME;
  Sprites::drawSelfMasked(x, y, Artwork::SPEAKER_SPRITES, frame);
}

void drawTitleCannon(int16_t x, int16_t y, int8_t recoil) {
  // Simple fixed launcher with no wheels: square housing, short barrel,
  // and a small pedestal. It matches the compact in-game cannon silhouette.
  arduboy.fillRect(x + 1, y + 2, 7, 6, WHITE);
  arduboy.drawPixel(x + 2, y + 1, WHITE);
  arduboy.fillRect(x + 7 - recoil, y + 4, 6, 2, WHITE);
  arduboy.fillRect(x + 2, y + 8, 5, 2, WHITE);
  arduboy.drawFastHLine(x, y + 10, 10, WHITE);
}

void drawTitleTarget(int16_t cx, int16_t cy, uint8_t pulse) {
  arduboy.drawCircle(cx, cy, 5, WHITE);
  arduboy.drawCircle(cx, cy, 2, WHITE);
  if (pulse > 0) {
    arduboy.fillCircle(cx, cy, 1, WHITE);
    if (pulse & 1) {
      arduboy.drawPixel(cx - 7, cy, WHITE);
      arduboy.drawPixel(cx + 7, cy, WHITE);
      arduboy.drawPixel(cx, cy - 7, WHITE);
      arduboy.drawPixel(cx, cy + 7, WHITE);
    }
  }
}

void drawTitleScreen() {
  arduboy.clear();

  // Only the title is reversed: a centered solid white label with
  // square corners and black lettering. Everything else remains unchanged.
  // The label is sized tightly around the text so the title reads as centered.
  arduboy.fillRect(24, 0, 80, 13, WHITE);
  arduboy.setTextColor(BLACK);
  arduboy.setCursor(28, 2);
  arduboy.print(F("MOSTLY LOGIC"));
  arduboy.setTextColor(WHITE);

  // One continuous miniature solution. Timing comes from a wide counter so
  // the animation cannot jump when an 8-bit frame counter wraps.
  const uint16_t frame = (titleBobFrame / 2UL) % 120UL;
  const int16_t cannonX = 7;
  const int16_t cannonY = 28;
  const int16_t turnX = 48;
  const int16_t lowerY = 34;
  const int16_t upperY = 20;
  const int16_t targetX = 108;

  // Sparse guide marks make the intended route readable without looking busy.
  for (int16_t x = 21; x <= turnX; x += 5) {
    arduboy.drawPixel(x, lowerY, WHITE);
  }
  for (int16_t y = upperY; y <= lowerY; y += 4) {
    arduboy.drawPixel(turnX, y, WHITE);
  }
  for (int16_t x = turnX; x <= targetX - 7; x += 5) {
    arduboy.drawPixel(x, upperY, WHITE);
  }

  drawDeflectorIcon(turnX - 4, lowerY - 4, true);
  drawDeflectorIcon(turnX - 4, upperY - 4, false);

  uint8_t targetPulse = 0;
  int8_t cannonRecoil = 0;
  bool drawBall = false;
  int16_t ballX = 20;
  int16_t ballY = lowerY;
  uint8_t motionPhase = 0;

  if (frame < 10) {
    // Brief pause before firing so the loop has a clear beginning.
    cannonRecoil = frame >= 7 ? 1 : 0;
  } else if (frame <= 42) {
    // Cannon to X block.
    const uint8_t p = frame - 10;
    ballX = 20 + (static_cast<int16_t>(p) * (turnX - 20)) / 32;
    ballY = lowerY;
    drawBall = true;
    motionPhase = 0;
    cannonRecoil = frame < 15 ? 2 : 0;
  } else if (frame <= 58) {
    // X block turns the same ball upward.
    const uint8_t p = frame - 43;
    ballX = turnX;
    ballY = lowerY - (static_cast<int16_t>(p) * (lowerY - upperY)) / 15;
    drawBall = true;
    motionPhase = 1;
  } else if (frame <= 101) {
    // Dot block turns that same ball right toward the target.
    const uint8_t p = frame - 59;
    ballX = turnX + (static_cast<int16_t>(p) * (targetX - turnX)) / 42;
    ballY = upperY;
    drawBall = true;
    motionPhase = 2;
  } else {
    // The ball has entered the target; only the target pulses until restart.
    targetPulse = 1 + ((frame - 102) / 3);
  }

  drawTitleCannon(cannonX, cannonY, cannonRecoil);
  drawTitleTarget(targetX, upperY, targetPulse);

  if (drawBall) {
    arduboy.fillCircle(ballX, ballY, 1, WHITE);
    if (motionPhase == 0 || motionPhase == 2) {
      arduboy.drawPixel(ballX - 3, ballY, WHITE);
    } else {
      arduboy.drawPixel(ballX, ballY + 3, WHITE);
    }
  }

  // Two explicit, independent menu actions.
  arduboy.setCursor(24, 47);
  arduboy.print(F("A  START"));

  arduboy.setCursor(24, 56);
  arduboy.print(F("B  SOUND: "));
  arduboy.print(arduboy.audio.enabled() ? F("ON") : F("OFF"));

  drawSpeakerIcon(108, 53, arduboy.audio.enabled());
  arduboy.display();
}

void drawLevelSelectScreen() {
  arduboy.clear();

  arduboy.fillRect(18, 0, 92, 13, WHITE);
  arduboy.setTextColor(BLACK);
  arduboy.setCursor(28, 2);
  arduboy.print(F("LEVEL SELECT"));
  arduboy.setTextColor(WHITE);

  constexpr uint8_t SELECTOR_ENTRY_COUNT = LEVEL_COUNT + 1;
  uint8_t firstVisible = 0;
  if (SELECTOR_ENTRY_COUNT > LEVEL_SELECTOR_VISIBLE_ROWS) {
    if (levelSelectIndex > 2) firstVisible = levelSelectIndex - 2;
    const uint8_t maxFirst = SELECTOR_ENTRY_COUNT - LEVEL_SELECTOR_VISIBLE_ROWS;
    if (firstVisible > maxFirst) firstVisible = maxFirst;
  }

  if (firstVisible > 0) {
    arduboy.fillTriangle(121, 14, 117, 18, 125, 18, WHITE);
  }
  if (firstVisible + LEVEL_SELECTOR_VISIBLE_ROWS < SELECTOR_ENTRY_COUNT) {
    arduboy.fillTriangle(121, 62, 117, 58, 125, 58, WHITE);
  }

  for (uint8_t row = 0; row < LEVEL_SELECTOR_VISIBLE_ROWS; ++row) {
    const uint8_t i = firstVisible + row;
    if (i >= SELECTOR_ENTRY_COUNT) break;
    const int16_t y = 17 + row * 8;
    if (i == levelSelectIndex) {
      arduboy.fillRect(5, y - 1, 111, 8, WHITE);
      arduboy.setTextColor(BLACK);
    } else {
      arduboy.setTextColor(WHITE);
    }

    arduboy.setCursor(9, y);
    arduboy.print(i == levelSelectIndex ? '>' : ' ');

    if (i == 0) {
      arduboy.print(F("  TITLE SCREEN"));
    } else {
      arduboy.print(i);
      arduboy.print(F(" "));

      LevelDef preview;
      memcpy_P(&preview, &levels[i - 1], sizeof(LevelDef));
      arduboy.print(preview.name);
    }
  }

  arduboy.setTextColor(WHITE);
  arduboy.display();
}

void drawGameOverScreen() {
  arduboy.clear();

  arduboy.fillRect(12, 7, 104, 16, WHITE);
  arduboy.setTextColor(BLACK);
  arduboy.setCursor(22, 12);
  arduboy.print(F("YOU MOSTLY WON"));

  arduboy.setTextColor(WHITE);
  arduboy.setCursor(25, 30);
  arduboy.print(F("ALL "));
  arduboy.print(LEVEL_COUNT);
  arduboy.print(F(" LEVELS"));
  arduboy.setCursor(43, 39);
  arduboy.print(F("CLEARED"));

  // Center the 16-character action row across the 128-pixel display.
  arduboy.setCursor(16, 51);
  arduboy.print(F("A AGAIN  B TITLE"));
  arduboy.display();
}

}  // namespace

void drawGame() {
  if (gameScreen == SCREEN_TITLE) {
    drawTitleScreen();
    return;
  }
  if (gameScreen == SCREEN_LEVEL_SELECT) {
    drawLevelSelectScreen();
    return;
  }
  if (gameScreen == SCREEN_GAME_OVER) {
    drawGameOverScreen();
    return;
  }

  arduboy.clear();
  drawGrid();
  drawBalls();
  drawTripwireBackblast();
  drawTripwireResetCircuitPulse();
  drawShatterEffect();
  drawControlPulse();
  drawGateBurst();
  drawActivationPulse();
  drawSuccessPulse();
  drawCannonExplosion();
  drawCursor();
  drawLevelIntro();

  if (won) {
    drawWinPopup();
  } else if (levelFailed && celebrationState == CELEBRATION_DONE) {
    drawFailurePopup();
  }

  arduboy.display();
}

}  // namespace Renderer
}  // namespace MostlyLogic
