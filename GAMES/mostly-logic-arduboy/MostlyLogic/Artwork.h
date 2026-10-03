// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 MostlyBuilds
#pragma once

#include <stdint.h>

#include <Arduino.h>

namespace MostlyLogic {
namespace Artwork {

enum DeflectorFrame : uint8_t {
  DEFLECTOR_X_FRAME,
  DEFLECTOR_DOT_FRAME,
};

enum SpeakerFrame : uint8_t {
  SPEAKER_ON_FRAME,
  SPEAKER_OFF_FRAME,
};

extern const uint8_t DEFLECTOR_SPRITES[] PROGMEM;
extern const uint8_t SPLITTER_SPRITE[] PROGMEM;
extern const uint8_t SPEAKER_SPRITES[] PROGMEM;

}  // namespace Artwork
}  // namespace MostlyLogic
