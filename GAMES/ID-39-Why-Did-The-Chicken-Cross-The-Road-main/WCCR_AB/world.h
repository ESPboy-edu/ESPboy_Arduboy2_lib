#ifndef WORLD_H
#define WORLD_H

#include "globals.h"
#include "bitmaps_world.h"

/*
  WORLD / LANES
  ------------------------------------------------------------------------
  Isometric 2:1 projection. World coordinates are in pixels:

      X : along a lane   -> screen goes DOWN-RIGHT   (1 cell = 8 px)
      Y : across lanes   -> screen goes UP-RIGHT     (1 lane = 8 units, +Y = forward)

      screen x = (X - camX) + (Y - camY) + SCREEN_CX
      screen y = ((X - camX) - (Y - camY)) / 2 + SCREEN_CY

  One lane (row v) covers Y = 8v .. 8v+7, one cell (column u) covers X = 8u .. 8u+7.
  The camera (camX, camY) is centred on the chicken, which sits at (SCREEN_CX, SCREEN_CY).

  Lanes are generated procedurally (see generateLane) into a small ring buffer.
  Cars / logs are NOT stored: every lane only keeps a phase, and the position of
  object i is computed from it, so the whole world costs RING * sizeof(Lane) bytes.

  Row numbers are int16, so the world is good for ~4000 rows before Y overflows.
*/

#define SCREEN_CX         62      // screen position of the chicken's cell centre
#define SCREEN_CY         40

#define PLAY_MIN_U        (-8)    // playable columns (17 wide); outside = bush wall
#define PLAY_MAX_U        8

#define RING              32      // lanes kept in memory (power of 2)
#define ROWS_BEHIND       7       // rows drawn below the chicken's row
#define ROWS_AHEAD        11      // rows drawn above it (also how far we generate)

#define LANE_LOOP              240     // length (px) of the loop cars / logs travel on
#define LANE_LOOP_Q            (LANE_LOOP * 16)

#define LANE_GRASS        0
#define LANE_ROAD         1
#define LANE_WATER        2
#define LANE_RUNWAY       3      // two lanes wide: kind 0 = near lane, kind 1 = far lane. A plane crosses it, always in +X (left to right)

// runway: the plane is one object on a long loop, the same for both lanes (the far lane uses the phase of the near lane)
#define RW_LOOP           1280   // px between two planes
#define RW_LOOP_Q         (RW_LOOP * 16)
#define RW_SPEED          88     // 1/16 px per frame = 5.5 px per frame, very fast
#define RW_XLO            (-300) // X of the plane's tail when it starts its run: ~200 px before it shows up on the field
#define RW_AWAKE_X        64     // the windsocks fly while the plane is left of this
#define PLANE_LEN         24

// water lane with kind == LEAF_KIND: fixed leaves (bushMask = cells that have a leaf) instead of moving logs
#define LEAF_KIND         3
#define LEAF_STEP         12     // frames per sinking stage: whole, 3 stages going down, then gone
#define LEAF_GONE         (LEAF_STEP * 4)
#define LEAF_BACK         (LEAF_GONE + 90)   // frames until a sunk leaf comes back up

// pickups: at most one per grass / road lane
#define ITEM_NONE         0
#define ITEM_CORN         1      // +5 points
#define ITEM_EGG          2      // golden egg: an extra life
#define ITEM_HOURGLASS    3      // the screen stops scrolling for a while
#define ITEM_MAGNET       4      // pulls corn in
#define HOURGLASS_TIME    225    // frames (45 fps = 5 s)
#define MAGNET_TIME       450    // 10 s
#define MAX_EGGS          3
#define MAGNET_RANGE      4      // the magnet reaches this many columns to each side
#define MAGNET_AHEAD      4      // ... and this many rows ahead
#define MAGNET_BACK       1      // ... and this many rows behind
#define MAGNET_FLY        4      // pieces of corn that can fly at the same time

struct __attribute__((packed)) Lane       // packed: no padding bytes (RAM is tight, RING lanes are kept)
{
  uint8_t  type;       // LANE_GRASS / LANE_ROAD / LANE_WATER
  int8_t   speed;      // 1/16 px per frame, sign = direction (0 on grass)
  int8_t   dx;         // whole pixels the objects moved this frame (riders use it)
  uint8_t  seed;       // random bits for spacing jitter
  uint8_t  kind;       // road: 0 car, 1 truck   water: log length = 2 + kind cells
  uint8_t  count;      // cars / logs per loop (2..4)
  union                // grass and leaf lanes never move, the other lanes have no bushes: they share these 2 bytes
  {
    uint16_t phase;    // moving lanes: 0 .. LANE_LOOP_Q-1  (1/16 px)
    uint16_t bushMask; // grass: bit (u - PLAY_MIN_U) set = bush;  leaf lane: bit set = leaf  (bits 16+ live in 'count', see laneMask)
  };
  uint8_t  item;       // pickup: bits 5..7 = type (ITEM_*), bits 0..4 = column (u - PLAY_MIN_U)
};

Lane     lanes[RING];
int16_t  camX = 0, camY = 0;
int16_t  scrollY = 0;      // the scroll line: rows below it are black (dead zone)
uint16_t worldFrame = 0;

static inline Lane& laneAt(int16_t v) { return lanes[v & (RING - 1)]; }

// 17 columns do not fit in 16 bits: grass and leaf lanes keep 'count' (unused there) as the top byte of the mask
static inline uint32_t laneMask(const Lane &L) { return (uint32_t)L.bushMask | ((uint32_t)L.count << 16); }
static inline void setLaneMask(Lane &L, uint32_t m) { L.bushMask = (uint16_t)m; L.count = (uint8_t)(m >> 16); }
static inline uint8_t itemType(const Lane &L) { return L.item >> 5; }
static inline int16_t itemU(const Lane &L)    { return (int16_t)(L.item & 31) + PLAY_MIN_U; }

// Y of the near edge of row v, relative to the camera
static inline int16_t deadRow() { return scrollY >> 3; }      // rows below this one are the black dead zone: touch it and you are lost
static inline int16_t relY(int16_t v) { return (v * 8) - camY; }


// random ////////////////////////////////////////////////////////////////////

uint16_t rngState = 0xACE1;

uint8_t rnd()
{
  rngState ^= rngState << 7;
  rngState ^= rngState >> 9;
  rngState ^= rngState << 8;
  return (uint8_t)(rngState ^ (rngState >> 8));
}


// leaves ///////////////////////////////////////////////////////////////////

// the one leaf that is sinking (the one the chicken stepped on)
bool    sinkOn = false;
int16_t sinkV = 0;
int16_t sinkU = 0;
uint8_t sinkT = 0;                // frames since the chicken stepped on it

static inline bool isLeafLane(const Lane &L) { return L.type == LANE_WATER && L.kind == LEAF_KIND; }

// sinking stage of the leaf at (v, u): 0 = whole ... 3 = almost under, 4 = gone
uint8_t leafStage(int16_t v, int16_t u)
{
  if (!sinkOn || sinkV != v || sinkU != u) return 0;
  if (sinkT >= LEAF_GONE) return 4;
  return sinkT / LEAF_STEP;
}

bool leafAt(const Lane &L, int16_t u)
{
  if (u < PLAY_MIN_U || u > PLAY_MAX_U) return false;
  return (laneMask(L) >> (u - PLAY_MIN_U)) & 1;
}

// is there a (not sunk) leaf under a chicken centred on world X = x in row v ?
bool leafUnder(int16_t v, int16_t x)
{
  int16_t u = x >> 3;
  return leafAt(laneAt(v), u) && leafStage(v, u) < 4;
}

// generation ////////////////////////////////////////////////////////////////

struct
{
  uint8_t runType;     // type of the run we are in
  uint8_t runLeft;     // lanes left in this run
  bool    leafRun;     // this water run is made of leaves instead of logs
  int8_t  dir;         // direction for the next moving lane
  uint8_t pathA, pathB; // two columns that are kept free of bushes / water, they random-walk (pathA < pathB - 1)
  int16_t top;         // highest row generated so far
} gen;

// move both guaranteed paths one lane on and open them in the mask (leaf lanes: set the bits, others: clear them).
// The two paths never touch: their columns stay at least 2 apart and their sideways steps never overlap.
static void stepPaths(uint32_t &m, bool set)
{
  const int8_t W = PLAY_MAX_U - PLAY_MIN_U;
  int8_t a = (int8_t)gen.pathA + (int8_t)(rnd() % 5) - 2;
  int8_t b = (int8_t)gen.pathB + (int8_t)(rnd() % 5) - 2;
  if (a < 0) a = 0;
  if (a > W - 3) a = W - 3;
  if (b < a + 2) b = a + 2;
  if (b > W) b = W;
  uint8_t loA = (a < gen.pathA) ? a : gen.pathA, hiA = (a < gen.pathA) ? gen.pathA : a;
  uint8_t loB = (b < gen.pathB) ? b : gen.pathB, hiB = (b < gen.pathB) ? gen.pathB : b;
  if (hiA >= loB)                                   // the sideways steps would run into each other: both go straight on
  {
    a = gen.pathA; b = gen.pathB;
    loA = hiA = a; loB = hiB = b;
  }
  for (uint8_t c = loA; c <= hiA; c++) { if (set) m |= (1UL << c); else m &= ~(1UL << c); }
  for (uint8_t c = loB; c <= hiB; c++) { if (set) m |= (1UL << c); else m &= ~(1UL << c); }
  gen.pathA = a; gen.pathB = b;
}

void generateLaneBase(int16_t v)
{
  Lane &L = laneAt(v);
  L.speed = 0; L.dx = 0; L.kind = 0; L.count = 0; L.bushMask = 0; L.item = 0;
  L.seed = rnd();
  L.phase = (((uint16_t)rnd() << 8) | rnd()) % LANE_LOOP_Q;

  if (v <= 1)                       // starting meadow, no bushes except the border
  {
    L.type = LANE_GRASS;
    L.bushMask = 0;                 // (shares its bytes with phase, which was just randomised)
    return;
  }

  if (gen.runLeft == 0)             // start a new run
  {
    if (gen.runType == LANE_GRASS)
    {
      byte r = rnd() % 10;
      gen.runType = (r < 5) ? LANE_ROAD : ((r < 8) ? LANE_WATER : LANE_RUNWAY);
      gen.leafRun = (gen.runType == LANE_WATER) && (rnd() % 3 == 0);
      gen.dir = (rnd() & 1) ? 1 : -1;
    }
    else
    {
      gen.runType = LANE_GRASS;     // always a bank / verge after road or water
    }
    gen.runLeft = (gen.runType == LANE_RUNWAY) ? 2 : 1 + rnd() % 3;
  }
  gen.runLeft--;
  L.type = gen.runType;

  if (L.type == LANE_RUNWAY)
  {
    L.kind  = (gen.runLeft == 0) ? 1 : 0;                                   // 0 = near lane, 1 = far lane
    L.speed = RW_SPEED;
    if (L.kind) L.phase = laneAt(v - 1).phase;                              // both lanes share the one plane
    else        L.phase = (((uint16_t)rnd() << 8) | rnd()) % RW_LOOP_Q;
  }
  else if (L.type == LANE_ROAD)
  {
    L.speed = (5 + rnd() % 9) * gen.dir;
    L.kind  = rnd() & 1;
    L.count = 2 + rnd() % 3;
    gen.dir = -gen.dir;
  }
  else if (L.type == LANE_WATER && gen.leafRun)
  {
    L.kind = LEAF_KIND;                                   // leaves: they do not move
    uint32_t m = 0;
    for (uint8_t c = 0; c <= (PLAY_MAX_U - PLAY_MIN_U); c++)
    {
      if ((rnd() & 7) < 2) m |= (1UL << c);
    }
    stepPaths(m, true);
    setLaneMask(L, m);
  }
  else if (L.type == LANE_WATER)
  {
    L.speed = (3 + rnd() % 6) * gen.dir;
    L.kind  = rnd() % 3;
    L.count = 2 + rnd() % 2;
    gen.dir = -gen.dir;
  }
  else
  {
    uint32_t m = 0;
    for (uint8_t c = 0; c <= (PLAY_MAX_U - PLAY_MIN_U); c++)
    {
      if ((rnd() & 7) < 3) m |= (1UL << c);
    }
    stepPaths(m, false);
    setLaneMask(L, m);
  }
}

// put a pickup on some grass / road lanes (not in the starting meadow, never on a bush)
void generateLane(int16_t v)
{
  generateLaneBase(v);
  Lane &L = laneAt(v);
  if (v <= 1 || (L.type != LANE_GRASS && L.type != LANE_ROAD)) return;
  byte r = rnd() % 100;
  uint8_t t = (r < 22) ? ITEM_CORN : (r < 25) ? ITEM_EGG : (r < 28) ? ITEM_HOURGLASS : (r < 31) ? ITEM_MAGNET : ITEM_NONE;
  if (!t) return;
  uint8_t c = rnd() % (PLAY_MAX_U - PLAY_MIN_U + 1);
  if (L.type == LANE_GRASS && ((laneMask(L) >> c) & 1)) return;
  L.item = (t << 5) | c;
}

// make sure rows up to (centreRow + ROWS_AHEAD + 1) exist
void extendWorld(int16_t centreRow)
{
  while (gen.top < centreRow + ROWS_AHEAD + 1) generateLane(++gen.top);
}

void worldInit()
{
  rngState = (uint16_t)micros() | 1;
  for (uint8_t i = 0; i < 8; i++) rnd();
  gen.runType = LANE_GRASS;
  gen.runLeft = 1;                  // one more grass row after the start meadow
  gen.leafRun = false;
  sinkOn = false;
  gen.dir = 1;
  gen.pathA = (-3 - PLAY_MIN_U);    // columns of u = -3 and u = +3
  gen.pathB = (3 - PLAY_MIN_U);
  gen.top = -ROWS_BEHIND - 4;
  worldFrame = 0;
  extendWorld(0);
}


// objects (cars, trucks, logs) ///////////////////////////////////////////////

static inline uint8_t laneObjLen(const Lane &L)
{
  if (L.type == LANE_ROAD) return L.kind ? 24 : 16;
  return 16 + 8 * L.kind;
}

// World X of the origin of object i, picked among its repeats every LANE_LOOP px so
// that the result lies in [xlo, xlo + LANE_LOOP).
int16_t laneObjPos(const Lane &L, uint8_t i, int16_t xlo)
{
  int16_t spacing = LANE_LOOP / L.count;
  uint8_t jitter  = ((uint8_t)(L.seed * (i * 2 + 3)) ^ (L.seed >> 3)) & 15;
  int16_t base    = i * spacing + jitter;
  int16_t ph      = L.phase >> 4;
  int16_t p       = (L.speed >= 0) ? base + ph : base - ph;
  int16_t d       = (p - xlo) % LANE_LOOP;
  if (d < 0) d += LANE_LOOP;
  return xlo + d;
}

// sprite + anchor offsets for the objects of a lane
void laneObjSprite(const Lane &L, const unsigned char **spr, int8_t *ox, int8_t *oy)
{
  if (L.type == LANE_ROAD)
  {
    if (L.kind && L.speed < 0) { *spr = truckRSprite; *ox = truckRSprite_OX; *oy = truckRSprite_OY; }   // going -X: turned around (the low front part leads)
    else if (L.kind)            { *spr = truckSprite;  *ox = truckSprite_OX;  *oy = truckSprite_OY; }
    else        { *spr = carSprite;   *ox = carSprite_OX;   *oy = carSprite_OY;   }
  }
  else
  {
    if      (L.kind == 0) { *spr = log2Sprite; *ox = log2Sprite_OX; *oy = log2Sprite_OY; }
    else if (L.kind == 1) { *spr = log3Sprite; *ox = log3Sprite_OX; *oy = log3Sprite_OY; }
    else                  { *spr = log4Sprite; *ox = log4Sprite_OX; *oy = log4Sprite_OY; }
  }
}

// advance the lanes that are on (or near) the screen
void updateLanes(int16_t centreRow)
{
  worldFrame++;
  if (sinkOn)
  {
    if (sinkT < 255) sinkT++;
    if (sinkT >= LEAF_BACK) sinkOn = false;               // it floats up again
  }
  for (int16_t v = centreRow - ROWS_BEHIND; v <= centreRow + ROWS_AHEAD; v++)
  {
    Lane &L = laneAt(v);
    if (L.speed == 0) { L.dx = 0; continue; }
    uint8_t  s  = (L.speed < 0) ? -L.speed : L.speed;
    uint16_t np = L.phase + s;
    int8_t   step = (np >> 4) - (L.phase >> 4);
    L.dx = (L.speed < 0) ? -step : step;
    uint16_t loopQ = (L.type == LANE_RUNWAY) ? RW_LOOP_Q : LANE_LOOP_Q;
    if (np >= loopQ) np -= loopQ;
    L.phase = np;
  }
}


// queries ///////////////////////////////////////////////////////////////////

bool bushAt(int16_t v, int16_t u)
{
  const Lane &L = laneAt(v);
  if (L.type != LANE_GRASS) return false;
  if (u < PLAY_MIN_U || u > PLAY_MAX_U) return true;
  return (laneMask(L) >> (u - PLAY_MIN_U)) & 1;
}

// is a car / truck overlapping a chicken centred on world X = x ?
bool vehicleHit(const Lane &L, int16_t x)
{
  uint8_t len = laneObjLen(L);
  for (uint8_t i = 0; i < L.count; i++)
  {
    int16_t p = laneObjPos(L, i, x - LANE_LOOP / 2) + 1;   // +1: box starts 1px into the cell
    if (p < x + 3 && p + len > x - 3) return true;
  }
  return false;
}

// is a car / truck within 'm' px of a chicken centred on world X = x ? (the honk: m = 16 is two tiles)
bool vehicleNear(const Lane &L, int16_t x, int16_t m)
{
  uint8_t len = laneObjLen(L);
  for (uint8_t i = 0; i < L.count; i++)
  {
    int16_t p = laneObjPos(L, i, x - LANE_LOOP / 2) + 1;
    if (p < x + 3 + m && p + len > x - 3 - m) return true;
  }
  return false;
}

// is there a log under a chicken centred on world X = x ?
bool logUnder(const Lane &L, int16_t x)
{
  uint8_t len = laneObjLen(L);
  for (uint8_t i = 0; i < L.count; i++)
  {
    int16_t p = laneObjPos(L, i, x - LANE_LOOP / 2) + 1;
    if (x >= p + 1 && x <= p + len - 1) return true;
  }
  return false;
}

// plane on a runway: X of its tail (the far lane has no phase of its own, pass the near lane)
static inline int16_t planeX(const Lane &Near) { return RW_XLO + (int16_t)(Near.phase >> 4); }

// is the plane over a chicken centred on world X = x ?
bool planeHit(const Lane &Near, int16_t x)
{
  int16_t p = planeX(Near) + 1;
  return (p < x + 3) && (p + PLANE_LEN > x - 3);
}

#endif
