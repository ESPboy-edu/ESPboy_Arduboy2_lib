#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include "globals.h"
#include "world.h"
#include "player.h"

/*
  DRAWING ORDER
  ------------------------------------------------------------------------
  Painter's algorithm, far to near, with the screen cut in two layers:

  1. GROUND. The screen is cleared to white (= grass) and the flat stuff is
     painted straight into the display buffer: road, water, dashes, grass
     tufts. Nothing in this layer ever has to be drawn over an object, so the
     order inside it does not matter.

  2. OBJECTS, row by row from the FARTHEST row (highest v, up-right on screen)
     to the NEAREST row. Inside a row, objects are drawn by increasing X
     (lane direction, down-right on screen = nearer to the viewer).
     Bushes, cars, trucks and logs all go through this one loop, so a tall
     object in a near row correctly covers things in the rows behind it.

     The chicken is slipped into that same sequence: it is drawn right before the
     first object that overlaps its sprite on screen and is IN FRONT of it
     (further along X, or in a lower row, than the chicken's footprint). Objects
     under it (the log it stands on) or behind it are already drawn by then. If
     nothing is in front of it, it is drawn last.

     One case cannot be solved by sorting alone: while the chicken hops between
     two lanes it can have one object in front of it in the far lane and another
     one behind it in the near lane, and the far lane is drawn first. When an
     object turns up that belongs BEHIND the chicken after the chicken has been
     drawn, it is drawn anyway, then the chicken and the objects that were in
     front of it are repainted over the small area where they overlap.

  Every sprite is placed from the floor origin (X, row) of its object, so
  everything that stands on a lane uses the same formula.
*/

// screen position of the left corner of the floor at world (X, row-relative Y)
static inline int16_t screenX(int16_t X, int16_t rowRel) { return (X - camX) + rowRel + SCREEN_CX; }
static inline int16_t screenY(int16_t X, int16_t rowRel) { return (((X - camX) - rowRel) >> 1) + SCREEN_CY; }

// is any part of row band 'rowRel' (near edge, relative to the camera) on screen ?
static inline bool bandVisible(int16_t rowRel)
{
  int16_t yb = SCREEN_CY - rowRel - 8;                 // band top at x == SCREEN_CX
  return (yb + ((127 - SCREEN_CX) >> 1) + 8 > 0) && (yb + ((0 - SCREEN_CX) >> 1) < 64);
}

#define FRONT_MAX 8
#define CHICKEN_HALF 3                    // half the size of the chicken's footprint (X and Y), in px

static int16_t pRX, pRY;                 // top-left of the chicken sprite on screen
static bool    pDrawn;                   // chicken already drawn this frame ?

// objects drawn after the chicken that overlap it (needed for the repair, see above)
static const unsigned char *fSpr[FRONT_MAX];
static int16_t fX[FRONT_MAX], fY[FRONT_MAX];            // screen
static int16_t fBX0[FRONT_MAX], fBY1[FRONT_MAX];        // world box: near end along X, far edge across Y
static uint8_t fN;
static uint8_t fFr[FRONT_MAX];                                    // their frames
static uint8_t objFrame = 0;                                      // frame of the sprite drawObject() is about to draw (animated pickups)

static inline bool rectsOverlap(int16_t x0, int16_t y0, int16_t w0, int16_t h0,
                                int16_t x1, int16_t y1, int16_t w1, int16_t h1)
{
  return (x0 < x1 + w1) && (x1 < x0 + w0) && (y0 < y1 + h1) && (y1 < y0 + h0);
}

static void drawChicken()
{
  if (pDrawn) return;
  player.drawSprite();
  pDrawn = true;
}

// plus-mask sprite, frame 'frame', but only on the pixels inside the rectangle [cx0,cx1) x [cy0,cy1) that are
// opaque in the 'gate' sprite (placed at gx, gy): used to repaint exactly what 'gate' has just painted over.
static void drawPlusMaskGated(int16_t x, int16_t y, const unsigned char *bm, uint8_t frame,
                              int16_t cx0, int16_t cy0, int16_t cx1, int16_t cy1,
                              const unsigned char *gate, int16_t gx, int16_t gy, bool flip = false)
{
  int16_t w = pgm_read_byte(bm), h = pgm_read_byte(bm + 1);
  int16_t gw = pgm_read_byte(gate), gh = pgm_read_byte(gate + 1);
  const unsigned char *d = bm + 2 + (uint16_t)frame * w * ((h + 7) >> 3) * 2;
  const unsigned char *g = gate + 2;
  int16_t xa = (x > cx0) ? x : cx0, xb = (x + w < cx1) ? x + w : cx1;
  int16_t ya = (y > cy0) ? y : cy0, yb = (y + h < cy1) ? y + h : cy1;
  for (int16_t yy = ya; yy < yb; yy++)
  {
    uint8_t row = yy - y, bit = 1 << (row & 7);
    const unsigned char *page = d + (uint16_t)(row >> 3) * w * 2;
    uint8_t grow = yy - gy, gbit = 1 << (grow & 7);
    const unsigned char *gpage = g + (uint16_t)(grow >> 3) * gw * 2;
    for (int16_t xx = xa; xx < xb; xx++)
    {
      if (!(pgm_read_byte(gpage + (uint16_t)(xx - gx) * 2 + 1) & gbit)) continue;      // 'gate' is not here
      const unsigned char *c = page + (uint16_t)(flip ? (w - 1 - (xx - x)) : (xx - x)) * 2;
      if (pgm_read_byte(c + 1) & bit) arduboy.drawPixel(xx, yy, (pgm_read_byte(c) & bit) ? WHITE : BLACK);
    }
  }
}

// Draws one object standing on the lane whose near edge is 'rowRel' (relative to the camera).
// Its box in the world is  X + bx0 .. X + bx1  along the lane  and  Y + by0 .. Y + by1  across it
// (a bush fills its cell: 0,8 / 0,8 ; vehicles and logs are a bit slimmer: 1,len+1 / 1,7).
void drawObject(const unsigned char *spr, int16_t X, int16_t rowRel, int8_t ox, int8_t oy,
                int8_t bx0, uint8_t bx1, int8_t by0, int8_t by1)
{
  int16_t x = screenX(X, rowRel) + ox;
  int16_t y = screenY(X, rowRel) + oy;
  int16_t w = pgm_read_byte(spr), h = pgm_read_byte(spr + 1);
  if (x >= 128 || y >= 64 || x + w <= 0 || y + h <= 0) return;

  if (rectsOverlap(x, y, w, h, pRX, pRY, 17, 24))        // touches the chicken's sprite
  {
    int16_t Y = rowRel + camY;                           // world Y of the lane's near edge
    if ((X + bx0) >= (player.x + CHICKEN_HALF) || (Y + by1) <= (player.y - CHICKEN_HALF))
    {
      // in front of the chicken (footprint X +-3, Y +-3): chicken first, and remember this one
      drawChicken();
      if (fN < FRONT_MAX) { fSpr[fN] = spr; fFr[fN] = objFrame; fX[fN] = x; fY[fN] = y; fBX0[fN] = X + bx0; fBY1[fN] = Y + by1; fN++; }
    }
    else if (pDrawn)
    {
      // behind or under the chicken, but arriving after it: draw it, then put the chicken back on top
      sprites.drawPlusMask(x, y, spr, objFrame);
      if (player.characteristics & CH_VISIBLE)
      {
        int16_t cx0 = (x > pRX) ? x : pRX, cx1 = (x + w < pRX + 17) ? x + w : pRX + 17;
        int16_t cy0 = (y > pRY) ? y : pRY, cy1 = (y + h < pRY + 24) ? y + h : pRY + 24;
        drawPlusMaskGated(pRX, pRY, player.spriteBitmap(), player.spriteFrame(), cx0, cy0, cx1, cy1, spr, x, y, player.spriteFlip());
        for (uint8_t k = 0; k < fN; k++)
        {
          // only those that are really in front of this object as well
          if (fBX0[k] >= (X + bx1) || fBY1[k] <= (Y + by0)) drawPlusMaskGated(fX[k], fY[k], fSpr[k], fFr[k], cx0, cy0, cx1, cy1, spr, x, y);
        }
      }
      return;
    }
  }
  sprites.drawPlusMask(x, y, spr, objFrame);
}


// 1. ground ///////////////////////////////////////////////////////////////////

void drawGround()
{
  uint8_t *buf = arduboy.getBuffer();
  memset(buf, 0xFF, 128 * 8);                           // grass = white

  int16_t vc = camY >> 3;
  for (int16_t v = vc - ROWS_BEHIND; v <= vc + ROWS_AHEAD; v++)
  {
    const Lane &L = laneAt(v);
    int16_t rr = relY(v);
    if (!bandVisible(rr)) continue;
    const bool dead = v < deadRow();                    // dead zone: plain black band, no lane data needed
    const uint8_t lt = dead ? LANE_ROAD : L.type;

    if (lt == LANE_GRASS)                           // little grass tufts on free cells
    {
      int16_t xlo = camX - rr - SCREEN_CX - 40;
      for (int16_t u = xlo >> 3; u <= ((xlo + 176) >> 3); u++)
      {
        if ((((u * 5 + v * 3) ^ (u * v)) & 7) != 0 || bushAt(v, u)) continue;
        int16_t cx = screenX(u * 8, rr) + 8, cy = screenY(u * 8, rr);
        arduboy.drawPixel(cx - 1, cy, BLACK);
        arduboy.drawPixel(cx + 1, cy, BLACK);
        arduboy.drawPixel(cx, cy + 1, BLACK);
      }
      continue;
    }

    int16_t yb = SCREEN_CY - rr - 8;                    // top of the band at x == SCREEN_CX

    // dashed line on the border between two road lanes
    bool dashes = !dead && (lt == LANE_ROAD) && (laneAt(v + 1).type == LANE_ROAD);
    int16_t dashPhase = camX + camY - 8 * (v + 1) - SCREEN_CX;

    for (uint8_t x = 0; x < 128; x++)
    {
      int16_t y = yb + (((int16_t)x - SCREEN_CX) >> 1);
      if (y <= -8 || y >= 64) continue;

      uint8_t val = 0x00;                               // road = black
      if (lt == LANE_WATER) val = ((x + (worldFrame >> 4) + v) & 1) ? 0xAA : 0x55;   // shimmering checker

      int8_t  p = y >> 3;                               // display page (floor)
      uint8_t s = y & 7;
      uint8_t m1 = 0xFF << s;
      if (p >= 0) buf[p * 128 + x] = (buf[p * 128 + x] & ~m1) | (val & m1);
      if (s && (p + 1) < 8)
      {
        uint8_t m2 = 0xFF >> (8 - s);
        buf[(p + 1) * 128 + x] = (buf[(p + 1) * 128 + x] & ~m2) | (val & m2);
      }

      if (dashes && y >= 0 && (((x + dashPhase) & 15) < 8)) arduboy.drawPixel(x, y, WHITE);

      if (lt == LANE_RUNWAY)                        // runway: a line along both edges, long dashes down the middle
      {
        if (L.kind == 1) { if (y >= 0 && y < 64) arduboy.drawPixel(x, y, WHITE); }
        else
        {
          if (y + 7 >= 0 && y + 7 < 64) arduboy.drawPixel(x, y + 7, WHITE);
          if (y >= 0 && y < 64 && (((x + dashPhase) & 31) < 16)) arduboy.drawPixel(x, y, WHITE);
        }
      }
    }
  }
}


// the lane that is being eaten right now: black diamonds, 8 frames, one per 8th of a row of scrolling.
// When the last frame has been shown the scroll line has moved one row on and the frames start over.
static void drawDeadEdge()
{
  int16_t v = deadRow();
  int16_t vc = camY >> 3;
  if (v < vc - ROWS_BEHIND || v > vc + ROWS_AHEAD) return;
  int16_t rr = relY(v);
  if (!bandVisible(rr)) return;
  byte frame = scrollY & 7;
  int16_t ulo = (camX - rr - SCREEN_CX - 16) >> 3;
  for (int16_t u = ulo; u <= ((camX - rr - SCREEN_CX + 128) >> 3); u++)
    sprites.drawErase(screenX(u * 8, rr), screenY(u * 8, rr) - 4, deadTile, frame);
}


// a pickup standing on its cell (bobs up and down a little)
static void drawItem(const Lane &L, int16_t rr)
{
  const unsigned char *spr; int8_t ox, oy;
  switch (itemType(L))
  {
    case ITEM_CORN:      spr = cornSprite;   ox = cornSprite_OX;   oy = cornSprite_OY;   break;
    case ITEM_EGG:       spr = eggSprite;    ox = eggSprite_OX;    oy = eggSprite_OY;    break;
    case ITEM_HOURGLASS: spr = hourSprite;   ox = hourSprite_OX;   oy = hourSprite_OY;   break;
    case ITEM_MAGNET:    spr = magnetSprite; ox = magnetSprite_OX; oy = magnetSprite_OY; break;
    default: return;
  }
  if (itemType(L) >= ITEM_HOURGLASS) objFrame = (worldFrame >> 3) & 3;          // the hourglass and the magnet have 4 frames
  drawObject(spr, itemU(L) * 8, rr, ox, oy - ((itemType(L) <= ITEM_EGG && ((worldFrame >> 3) & 1)) ? 1 : 0), 0, 8, 0, 8);     // corn and egg bob, the hourglass and magnet only turn
  objFrame = 0;
}


// 2. objects, far to near ////////////////////////////////////////////////////////

void drawObjects()
{
  int16_t vc = camY >> 3;
  pDrawn = false;
  fN = 0;
  player.spriteTopLeft(&pRX, &pRY);

  for (int16_t v = vc + ROWS_AHEAD; v >= vc - ROWS_BEHIND; v--)
  {
    if (v < deadRow()) continue;                          // black dead zone: nothing stands there
    const Lane &L = laneAt(v);
    int16_t rr = relY(v);
    if (!bandVisible(rr) && !(L.type == LANE_RUNWAY && L.kind == 1 && bandVisible(rr - 8))) continue;   // (a plane stands on two lanes)

    // world X range that can show up on screen for this row
    int16_t xlo = camX - rr - SCREEN_CX - 40;

    if (L.type == LANE_GRASS)
    {
      for (int16_t u = xlo >> 3; u <= ((xlo + 176) >> 3); u++)
      {
        if (bushAt(v, u)) drawObject(bushSprite, u * 8, rr, bushSprite_OX, bushSprite_OY, 0, 8, 0, 8);
        else if (L.item && itemU(L) == u) drawItem(L, rr);
      }
    }
    else if (isLeafLane(L))
    {
      for (int16_t u = xlo >> 3; u <= ((xlo + 176) >> 3); u++)
      {
        if (!leafAt(L, u)) continue;
        uint8_t st = leafStage(v, u);
        if (st >= 4) continue;                          // under water
        const unsigned char *spr; int8_t ox, oy;
        if      (st == 0) { spr = leaf0Sprite; ox = leaf0Sprite_OX; oy = leaf0Sprite_OY; }
        else if (st == 1) { spr = leaf1Sprite; ox = leaf1Sprite_OX; oy = leaf1Sprite_OY; }
        else if (st == 2) { spr = leaf2Sprite; ox = leaf2Sprite_OX; oy = leaf2Sprite_OY; }
        else              { spr = leaf3Sprite; ox = leaf3Sprite_OX; oy = leaf3Sprite_OY; }
        drawObject(spr, u * 8, rr, ox, oy, 0, 8, 1, 7);
      }
    }
    else if (L.type == LANE_RUNWAY)
    {
      if (L.kind == 1)                                  // the plane is drawn with the far lane: everything nearer comes after it
      {
        int16_t px = planeX(laneAt(v - 1));
        if (px + PLANE_LEN > xlo && px <= xlo + 176)
        {
          drawObject(planeSprite, px, rr - 8, planeSprite_OX, planeSprite_OY, 1, PLANE_LEN + 1, 1, 15);
          if (gameState == STATE_GAME_PLAYING && !(worldFrame & 63)) sfx(SFX_AIRPLANE);   // the propeller stutters while a plane is in view
        }
      }
      else                                              // a windsock at each end of the runway, at its near edge: limp, flying when a plane is coming
      {
        const unsigned char *spr = windCalmSprite;
        if (planeX(L) < RW_AWAKE_X)
        {
          byte f = (worldFrame >> 2) % 3;
          spr = (f == 0) ? windFly0Sprite : ((f == 1) ? windFly1Sprite : windFly2Sprite);
        }
        for (int16_t u = PLAY_MIN_U - 1; u <= PLAY_MAX_U + 1; u += (PLAY_MAX_U - PLAY_MIN_U + 2))
          drawObject(spr, u * 8, rr, windCalmSprite_OX - 3, windCalmSprite_OY + 2, 3, 5, 0, 2);
      }
    }
    else
    {
      int16_t pos[4];
      uint8_t n = L.count;
      for (uint8_t i = 0; i < n; i++) pos[i] = laneObjPos(L, i, xlo);
      for (uint8_t i = 1; i < n; i++)                   // tiny insertion sort, ascending X
      {
        int16_t key = pos[i]; int8_t j = i - 1;
        while (j >= 0 && pos[j] > key) { pos[j + 1] = pos[j]; j--; }
        pos[j + 1] = key;
      }

      if (L.item) drawItem(L, rr);                      // a pickup lying on the road (the vehicles drive over it)

      const unsigned char *spr; int8_t ox, oy;
      laneObjSprite(L, &spr, &ox, &oy);

      for (uint8_t i = 0; i < n; i++)
      {
        if (pos[i] <= xlo + 176) drawObject(spr, pos[i], rr, ox, oy, 1, laneObjLen(L) + 1, 1, 7);
      }
    }
  }
  drawChicken();                                         // nothing in front of it
}


// corn that the magnet has pulled loose, flying to the chicken (rises a little while it flies)
static void drawFlyers()
{
  if (player.characteristics & CH_DEAD) return;
  for (byte i = 0; i < MAGNET_FLY; i++)
  {
    if (!player.flyT[i]) continue;
    int16_t z = player.flyT[i] >> 1; if (z > 7) z = 7;
    int16_t fx = player.flyX[i] - camX, fy = player.flyY[i] - camY;
    sprites.drawPlusMask(fx + fy + SCREEN_CX + cornSprite_OX, ((fx - fy) >> 1) + SCREEN_CY + cornSprite_OY - z, cornSprite, 0);
  }
}


// score ///////////////////////////////////////////////////////////////////////
// font 3 wide x 5 high: 5 rows per digit, 3 bits per row (bit 2 = left pixel)
const unsigned char PROGMEM digitFont[10][5] =
{
  {7,5,5,5,7}, {2,6,2,2,7}, {7,1,7,4,7}, {7,1,7,1,7}, {5,5,7,1,1},
  {7,4,7,1,7}, {7,4,7,5,7}, {7,1,2,2,2}, {7,5,7,5,7}, {7,5,7,1,7},
};

// the score, white digits with a black outline, bottom left corner
void drawScore()
{
  char d[5]; byte n = 0;
  uint16_t s = player.score;
  do { d[n++] = s % 10; s /= 10; } while (s && n < 5);       // digits, last one first

  for (byte pass = 0; pass < 2; pass++)                       // pass 0: black outline, pass 1: white digits
  {
    for (byte i = 0; i < n; i++)
    {
      byte dg = d[n - 1 - i];
      int16_t bx = 6 + i * 4, by = 64 - 2 - 5;
      for (byte r = 0; r < 5; r++)
      {
        byte bits = pgm_read_byte(&digitFont[dg][r]);
        for (byte c = 0; c < 3; c++)
        {
          if (!(bits & (4 >> c))) continue;
          if (pass == 0) arduboy.fillRect(bx + c - 1, by + r - 1, 3, 3, BLACK);
          else           arduboy.drawPixel(bx + c, by + r, WHITE);
        }
      }
    }
  }
}


// eggs (lives) top left, active power-ups with a draining bar top right
static void drawHud()
{
  for (byte i = 0; i < player.eggs; i++) sprites.drawPlusMask(2 + i * 10, 0, HUD_plus_mask, 0);
  int16_t hx = 128 - 10;
  if (player.hourglass)
  {
    sprites.drawPlusMask(hx, 0, HUD_plus_mask, 1);
    arduboy.fillRect(hx - 1, 9, 10, 3, BLACK);
    arduboy.fillRect(hx, 10, (uint32_t)player.hourglass * 7 / HOURGLASS_TIME + 1, 1, WHITE);
    hx -= 12;
  }
  if (player.magnet)
  {
    sprites.drawPlusMask(hx, 0, HUD_plus_mask, 2);
    arduboy.fillRect(hx - 1, 9, 10, 3, BLACK);
    arduboy.fillRect(hx, 10, (uint32_t)player.magnet * 7 / MAGNET_TIME + 1, 1, WHITE);
  }
}

void drawPlayField()
{
  drawGround();
  drawDeadEdge();
  drawObjects();
  drawFlyers();
  player.drawSplash();
  drawScore();
  drawHud();
}

#endif
