// Pocket Stage - the stage: pictures and sprites drawn straight off the
// flash chip (the crowd and the spotlights of the title page, the menu and
// the booed-off page; the flames on the pads; the booed-off page's title),
// the fast drawing of the play page, and the play page's stage behind the
// highway: a photograph of the concert, one frame of the screen for every
// camera, every position of its lights and every pose of the band and the
// crowd (tools/scene.py), copied off the flash chip straight into the
// screen buffer. The band plays along: the beat shifts the players'
// weight, bobs the bassist's head and brings the drummer's sticks down, a
// hit note strums the guitar, the crowd pumps its fists while the band has
// it, and with star power on the guitarist jumps on the beat and the wide
// shot gets its V of beams.
#pragma once

static uint8_t title_frame;                 // counts frames for the lights and the blinking

// n of the screen buffer's pages (rows of eight), from page first on, straight
// off the chip: a layer of a picture (the crowd and its spotlights)
static void fx_pages(uint24_t addr, uint8_t first, uint8_t n) {
  for (uint8_t p = first; p < first + n; p++, addr += 128) fx_read(addr, arduboy.sBuffer + (uint16_t)p * 128, 128);
}

// a crowd's layer off the chip over the screen's last pages, from page
// CROWD_PAGE on: the rows above CROWD_TOP, where the people never reach, keep
// what is there (the spotlights come down into the glow behind the people,
// the credits go up behind them), the crowd's own rows from there down
static_assert(CROWD_TOP >= CROWD_PAGE * 8 && CROWD_TOP < CROWD_PAGE * 8 + 8, "CROWD_TOP in page CROWD_PAGE");
#define CROWD_KEEP ((1 << (CROWD_TOP - CROWD_PAGE * 8)) - 1)
static void fx_crowd(uint24_t addr) {
  uint8_t buf[32];
  uint8_t *q = arduboy.sBuffer + CROWD_PAGE * 128;
  for (uint8_t c = 0; c < 128; c += 32, addr += 32) {
    fx_read(addr, buf, 32);
    for (uint8_t i = 0; i < 32; i++, q++) *q = (*q & CROWD_KEEP) | buf[i];
  }
  fx_pages(addr, CROWD_PAGE + 1, 7 - CROWD_PAGE);
}

// a sprite off the chip at x, y: w columns wide, so many rows of eight tall;
// for each row of eight, every column's image byte and then its mask byte
// (the mask keeps what is behind out of it), read from the chip a few
// columns at a time. Any row will do for y: a page's bytes are shifted into
// the two pages of the screen they land in.
#define FX_SPRITE_CHUNK 32                         // columns read in one go
static void fx_sprite(uint24_t addr, int16_t x, uint8_t y, uint8_t w, uint8_t pages) {
  uint8_t buf[2 * FX_SPRITE_CHUNK];
  uint8_t i0 = x < 0 ? -x : 0, i1 = x + w > 128 ? 128 - x : w;    // the columns on the screen
  if (i0 >= i1) return;
  uint8_t shift = y & 7;
  uint8_t page = y >> 3;
  for (uint8_t p = 0; p < pages && page < 8; p++, page++, addr += 2 * w) {
    bool two = shift && page + 1 < 8;
    for (uint8_t c = i0; c < i1; c += FX_SPRITE_CHUNK) {
      uint8_t n = i1 - c < FX_SPRITE_CHUNK ? i1 - c : FX_SPRITE_CHUNK;
      fx_read(addr + 2 * c, buf, 2 * n);
      uint8_t *q = arduboy.sBuffer + (uint16_t)page * 128 + x + c;
      const uint8_t *b = buf;
      for (uint8_t i = 0; i < n; i++, q++) {
        uint16_t bi = (uint16_t)*b++ << shift, bm = (uint16_t)*b++ << shift;
        *q = (*q & ~(uint8_t)bm) | (uint8_t)bi;
        if (two) q[128] = (q[128] & ~(uint8_t)(bm >> 8)) | (uint8_t)(bi >> 8);
      }
    }
  }
}

// ---------------------------------------------------------------- fast drawing
// The library draws a box or a line a pixel at a time, with the bounds
// checked for each; the play page draws a lot of them a frame, so these go
// straight into the screen buffer, a byte (eight rows) at a time.

// a black box
static void rect_clear(int16_t x, int16_t y, uint8_t w, uint8_t h) {
  if (x < 0) {
    if (w <= -x) return;
    w += x;
    x = 0;
  }
  if (x + w > 128) {
    if (x >= 128) return;
    w = 128 - x;
  }
  if (y < 0) {
    if (h <= -y) return;
    h += y;
    y = 0;
  }
  if (y + h > 64) {
    if (y >= 64) return;
    h = 64 - y;
  }
  uint8_t y1 = y + h - 1;
  for (uint8_t page = y >> 3; page <= (y1 >> 3); page++) {
    uint8_t top = page * 8;
    uint8_t first = y > top ? y - top : 0;
    uint8_t last = y1 < top + 7 ? y1 - top : 7;
    uint8_t mask = (uint8_t)(0xFF << first) & (uint8_t)(0xFF >> (7 - last));
    uint8_t *p = arduboy.sBuffer + (uint16_t)page * 128 + x;
    for (uint8_t i = 0; i < w; i++) *p++ &= ~mask;
  }
}

// white pixels along a row, every step-th one, from x0 to x1 (both on the screen)
static void row_dots(uint8_t x0, uint8_t x1, uint8_t y, uint8_t step) {
  uint8_t bit = 1 << (y & 7);
  uint8_t *p = arduboy.sBuffer + (uint16_t)(y >> 3) * 128 + x0;
  for (uint8_t x = x0; x <= x1; x += step, p += step) *p |= bit;
}

// the small digits of the play page, at the top of the screen (page 0),
// with black behind them: the score and the multiplier. The library's
// print would do, but goes a pixel at a time. 0..9, then x, then a space.
static const uint8_t DIGITS[12 * 5] PROGMEM = {
  0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x42, 0x7F, 0x40, 0x00, 0x42, 0x61, 0x51, 0x49, 0x46,
  0x21, 0x41, 0x45, 0x4B, 0x31, 0x18, 0x14, 0x12, 0x7F, 0x10, 0x27, 0x45, 0x45, 0x45, 0x39,
  0x3C, 0x4A, 0x49, 0x49, 0x30, 0x01, 0x71, 0x09, 0x05, 0x03, 0x36, 0x49, 0x49, 0x49, 0x36,
  0x06, 0x49, 0x49, 0x29, 0x1E, 0x44, 0x28, 0x10, 0x28, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00};

static void hud_text(uint8_t x, const char *s) {
  uint8_t *p = arduboy.sBuffer + x;
  for (char c; (c = *s++) && x <= 122; x += 6) {
    uint8_t g = c == 'x' ? 10 : c == ' ' ? 11 : c - '0';
    const uint8_t *col = DIGITS + g * 5;
    for (uint8_t i = 0; i < 5; i++) *p++ = pgm_read_byte(col++);
    *p++ = 0;
  }
}

// The highway: its floor wiped black from the far end (row yf) to the near
// one (yn), a page of eight rows at a time (whole bytes where a column is
// inside for all the page's rows, single rows along the slanting sides);
// then its two edges (their half width e at the far end, and at the near),
// the two lane dividers (d), and with star power on a second edge a little
// further out. The floor is wiped three columns past the edges.
static void highway_draw(uint8_t yf, uint8_t yn, uint8_t ef, uint8_t en, uint8_t df, uint8_t dn, bool star) {
  uint8_t span = yn - yf;
  uint16_t e16 = (uint16_t)ef << 8, es = ((uint16_t)(en - ef) << 8) / span;
  uint16_t d16 = (uint16_t)df << 8, ds = ((uint16_t)(dn - df) << 8) / span;
  uint16_t o16 = 1 << 8, os = (2 << 8) / span;
  uint8_t r = yf;
  while (r <= yn) {
    uint8_t *base = arduboy.sBuffer + (uint16_t)(r >> 3) * 128 + 64;
    uint8_t e[8], d[8], o[8], i0 = r & 7, i1, hw0 = 255, hw1 = 0;
    do {
      uint8_t i = r & 7;
      e[i] = e16 >> 8;
      d[i] = d16 >> 8;
      o[i] = e[i] + (o16 >> 8);
      uint8_t h = e[i] + 3;
      if (h < hw0) hw0 = h;
      if (h > hw1) hw1 = h;
      e16 += es;
      d16 += ds;
      o16 += os;
      i1 = i;
      r++;
    } while (r <= yn && (r & 7));
    uint8_t rows = (uint8_t)(0xFF << i0) & (uint8_t)(0xFF >> (7 - i1));
    for (int8_t dx = -hw0; dx <= (int8_t)hw0; dx++) base[dx] &= ~rows;
    for (uint8_t dx = hw0 + 1; dx <= hw1; dx++) {
      uint8_t mask = 0, b = 1 << i0;
      for (uint8_t i = i0; i <= i1; i++, b <<= 1) {
        if (e[i] + 3 >= dx) mask |= b;
      }
      base[-(int8_t)dx] &= ~mask;
      base[dx] &= ~mask;
    }
    uint8_t b = 1 << i0;
    for (uint8_t i = i0; i <= i1; i++, b <<= 1) {
      base[-(int8_t)e[i]] |= b;
      base[e[i]] |= b;
      base[-(int8_t)d[i]] |= b;
      base[d[i]] |= b;
      if (star) {
        base[-(int8_t)o[i]] |= b;
        base[o[i]] |= b;
      }
    }
  }
}

// ---------------------------------------------------------------- the stage
#define CUT_BARS 4                         // the camera cuts every so many bars

static uint8_t band_strum;                 // frames the strum still shows (after a hit note)
static uint8_t band_beat;                  // which foot the weight is on: changes on every beat
static uint8_t scene;                      // the camera in use
static uint8_t bars;                       // bars gone by, for the cuts

static void band_reset() {
  band_strum = band_beat = 0;
  scene = CAM_WIDE;
  bars = 0;
}

// once a frame
static void band_update() {
  if (band_strum) band_strum--;
}

// a bar has gone by: now and then the camera cuts
static void band_bar() {
  if (++bars % CUT_BARS == 0) scene = pgm_read_byte(&SCENE_CUTS[(bars / CUT_BARS) % sizeof(SCENE_CUTS)]);
}

// a whole screen off the chip into the screen buffer
static void fx_screen(uint24_t addr) {
  fx_pages(addr, 0, 8);
}

// the stage: the camera's frame for where the lights are now, the band's
// pose and the crowd's. star: star power is on (the wide shot gets its V of
// beams, the lights swing twice as fast: play.h steps title_frame so);
// jump: on the beat with star power on; with_it: the rock meter is above
// the middle, the crowd pumps its fists on the beat
static void scene_draw(bool star, bool jump, bool with_it) {
  uint8_t cam = star && scene == CAM_WIDE ? CAM_WIDE_V : scene;
  uint8_t crowds = pgm_read_byte(&CAM_CROWDS[cam]);
  uint8_t light = (title_frame >> 4) & (CAM_LIGHTS - 1);
  uint8_t band = jump ? 4 : (band_strum ? 2 : 0) + band_beat;
  uint8_t crowd = with_it && crowds > 1 ? band_beat : 0;
  uint16_t frame = ((uint16_t)light * BAND_STATES + band) * crowds + crowd;
  fx_screen(pgm_read_dword(&CAM_FRAMES[cam]) + ((uint24_t)frame << 10));
}
