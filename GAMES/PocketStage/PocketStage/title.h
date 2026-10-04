// Pocket Stage - the title page: the logo over the stage light, two
// spotlights swinging, the crowd jumping to the riff that loops underneath,
// and PRESS B blinking between the logo and the crowd. The main menu has the
// same crowd and spotlights; the booed-off page has the crowd booing.
//
// The crowd and the spotlights are pictures in the stage's look, on the
// flash chip (tools/crowd.py), in two layers so that each moves on its own:
// the light, a picture for every place of the spotlights' swing, and the
// crowd over its bottom, a picture for every step of its jumping. The logo
// is a sprite with a mask (tools/logo.py), so it reads over the spotlights.
#pragma once
#include "logo.h"
#include "stage.h"

#define LOGO_X ((128 - LOGO_W) / 2)
#define LOGO_Y 0
// the PRESS B line, halfway between the logo and the crowd (Joel, 2026-10-02):
// four rows of black under the logo (three as it nods), three over the glow
// behind the crowd, which is bright from row 53
#define PROMPT_Y 43

// the title page's and the menu's picture: the spotlights at their place in
// the swing (a place every 8 frames), the crowd jumping, everyone in their
// own time, two beats of the menus' song to a jump (sixteen steps)
static void crowd_scene() {
  title_frame++;
  fx_pages(FX_TITLE_LIGHT + (uint24_t)((title_frame >> 3) & 31) * (TITLE_LIGHT_PAGES * 128), 0, TITLE_LIGHT_PAGES);
  uint8_t step = (uint8_t)(now_ms() * 8 / TITLE_BEAT_MS) & 15;
  fx_crowd(FX_TITLE_CROWD + (uint24_t)step * (TITLE_CROWD_PAGES * 128));
}

// the booed-off page's picture, f frames after it opened (to 255): the
// spotlights on as it opens, then out one after the other; the crowd
// booing; the cups the two throwers let go, white, up into the dark and
// smaller as they fly at the stage (BOO_CUPS: where they leave the hand)
static void crowd_booing(uint8_t f) {
  uint8_t light = f >= BOO_RIGHT_OFF ? BOO_LIGHT_N - 1 : f < BOO_LEFT_OFF ? f >> 3 : (f >> 3) + 1;
  fx_pages(FX_BOO_LIGHT + (uint24_t)light * (BOO_LIGHT_PAGES * 128), 0, BOO_LIGHT_PAGES);
  fx_crowd(FX_BOO_CROWD + (uint24_t)(title_frame & 31) * (BOO_CROWD_PAGES * 128));
  for (uint8_t i = 0; i < sizeof(BOO_CUPS); i += 4) {
    int8_t side = pgm_read_byte(&BOO_CUPS[i + 2]);
    uint8_t k = ((title_frame - pgm_read_byte(&BOO_CUPS[i + 3])) & 31) - 8;    // frames in the air
    if (k >= 20) continue;
    int16_t cx = (int8_t)pgm_read_byte(&BOO_CUPS[i]) + side * ((26 * k + 10) / 20);
    int16_t cy = (int8_t)pgm_read_byte(&BOO_CUPS[i + 1]) - 2 - (28 * k + 10) / 20 + (k * k + 10) / 20;
    uint8_t size = k < 6 ? 3 : k < 14 ? 2 : 1;
    arduboy.fillRect(cx - 1, cy - 1, size + 2, size + 2, BLACK);
    arduboy.fillRect(cx, cy, size, size, WHITE);
  }
}

static void title_draw() {
  crowd_scene();
  // the logo nods on the beat of the menus' song
  uint8_t nod = (now_ms() % TITLE_BEAT_MS) < 70 ? 1 : 0;
  Sprites::drawExternalMask(LOGO_X, LOGO_Y + nod, LOGO, LOGO_MASK, 0, 0);
  // the prompt between the logo and the crowd, blinking: on for 24 frames, off for 8
  if ((title_frame & 31) < 24) {
    arduboy.setTextBackground(BLACK);
    arduboy.setCursor(64 - 3 * 7, PROMPT_Y);
    arduboy.print(F("PRESS B"));
  }
}
