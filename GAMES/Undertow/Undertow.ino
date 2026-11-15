// UNDERTOW -- woodcut tactics for the Arduboy FX-C
//
// Code lives here; content lives in data:
//   story.h            names, classes, items, hero base stats   (tools/chapters/game_data.py)
//   fxdata/chapters    every chapter: map, units, objective, dialogue (chapters/*.txt)
//   fxdata/ (images)   map tiles, unit sprites, font, battle figures, backdrops, shields,
//                      portraits, chapter cards                  (tools/woodcut/make_gfx.py)
//   sfx.h              sound effects and motifs                   (tools/audio/make_sfx.py)
// Rebuild all of it with tools/build_data.sh.
//
// Controls (map):  D-pad cursor
//   A on your unit   -> move range, A again to walk there, then ATTACK / ITEM / WAIT
//   A on an enemy    -> its danger zone (A/B to close)
//   A on empty tile  -> END TURN / SOUND
//   B                -> next unit that hasn't acted / cancel

#include <Arduboy2.h>
#include "fxdata.h"
#include <ArduboyFX.h>
//#include <avr/eeprom.h>
#include "fxdata/fxdata.h"
#include "story.h"
#include "sfx.h"

// No USB stack: frees ~2.7 KB of program space and 140 B of RAM (chapter 3's systems need it).
// To upload a new build to the device, put it in the bootloader first: hold UP + DOWN together
// for a moment while the game runs (the Arduboy core watches for that combo), then upload. Comment the line out to get normal USB uploads back (the game
// will then no longer fit until something else is cut).
//#define NO_USB_BUILD
#ifdef NO_USB_BUILD
ARDUBOY_NO_USB
#endif

struct __attribute__((packed)) Fc { uint8_t dmg, hit, crit, pierce; bool can, dbl; };   // pierce: a crit's floor, DEF ignored
enum State : uint8_t { ST_TITLE, ST_CHAPTER, ST_TALK, ST_DEPLOY, ST_BANNER, ST_SELECT, ST_MOVE, ST_WALK, ST_MENU,
                       ST_ITEMS, ST_TARGET, ST_FORECAST, ST_BATTLE, ST_HEALFX, ST_GOT, ST_EXP, ST_LEVELUP,
                       ST_ENEMY, ST_DANGER, ST_END, ST_TRADE, ST_SHOP, ST_STATUS, ST_EPILOGUE };



// --- ESP8266 EEPROM COMPATIBILITY ---
bool eeprom_dirty = false;
#define eeprom_read_byte(addr) EEPROM.read((int)(addr))

inline void eeprom_update_byte(uint8_t* addr, uint8_t val) {
  if (EEPROM.read((int)addr) != val) {
    EEPROM.write((int)addr, val);
    eeprom_dirty = true;
  }
}

inline uint16_t eeprom_read_word(const uint16_t* addr) {
  uint16_t val;
  EEPROM.get((int)addr, val);
  return val;
}

inline void eeprom_update_word(uint16_t* addr, uint16_t val) {
  uint16_t current;
  EEPROM.get((int)addr, current);
  if (current != val) {
    EEPROM.put((int)addr, val);
    eeprom_dirty = true;
  }
}

inline void eeprom_read_block(void* dst, const void* src, size_t n) {
  uint8_t* d = (uint8_t*)dst;
  for (size_t i = 0; i < n; i++) d[i] = EEPROM.read((int)src + i);
}

inline void eeprom_update_block(const void* src, void* dst, size_t n) {
  const uint8_t* s = (const uint8_t*)src;
  for (size_t i = 0; i < n; i++) {
    if (EEPROM.read((int)dst + i) != s[i]) {
      EEPROM.write((int)dst + i, s[i]);
      eeprom_dirty = true;
    }
  }
}
// ------------------------------------


Arduboy2Base ab;                  // Base: no built-in text/font/logo -- we draw our own

const uint8_t NONE = 255;

// ================================================================= sound
// One square-wave voice on the speaker pin, the way ArduboyTones drives it (Timer3 in CTC mode at
// /8, the pin toggled from the compare interrupt), but reading sfx.h's byte pairs: (pitch, length).
// Pitch 0 is a rest, else 27.5 Hz * 2^(pitch/24); length is in 1/256 s; (0,0) ends, (1,0) loops.
// With sound off the pins are inputs (Arduboy2Audio), so the toggling is silent.

/*
#ifdef __AVR__
// per quarter tone k of the lowest octave (27.5-55 Hz): the frequency x 16, and half a period
// in 0.5 us ticks (Timer3 at /8). An octave up doubles one and halves the other.
const uint16_t PITCH_F[24] PROGMEM = { 440, 453, 466, 480, 494, 508, 523, 539, 554, 571, 587, 605, 622, 640, 659, 679, 698, 719, 740, 762, 784, 807, 831, 855 };
const uint16_t PITCH_T[24] PROGMEM = { 36364, 35328, 34323, 33346, 32396, 31474, 30578, 29708, 28862, 28040, 27242, 26466, 25713, 24981, 24270, 23579, 22908, 22256, 21622, 21006, 20408, 19827, 19263, 18715 };
const uint8_t* sndAt; const uint8_t* sndStart;
volatile uint16_t sndLeft;                         // pin toggles left in this note
volatile bool sndOn;
bool sndSilent;
void sndStop() { TIMSK3 = 0; TCCR3B = 0; bitClear(SPEAKER_1_PORT, SPEAKER_1_BIT); sndOn = false; }
void sndNext() {
  uint8_t p, d;
  for (;;) {
    p = pgm_read_byte(sndAt); d = pgm_read_byte(sndAt + 1); sndAt += 2;
    if (d) break;
    if (!p) { sndStop(); return; }
    sndAt = sndStart;                               // (1, 0): from the top
  }
  uint16_t f = 1000 * 16, t = 1000;                 // a rest ticks at 1 kHz with the pin low
  sndSilent = !p;
  if (p) { uint8_t o = 0; while (p >= 24) { p -= 24; o++; } f = pgm_read_word(&PITCH_F[p]) << o; t = pgm_read_word(&PITCH_T[p]) >> o; }
  else bitClear(SPEAKER_1_PORT, SPEAKER_1_BIT);
  OCR3A = t - 1;
  sndLeft = ((uint32_t)f * d >> 11) | 1;            // 2 toggles a cycle for d/256 s
}
ISR(TIMER3_COMPA_vect) {
  if (!sndSilent) SPEAKER_1_PORT ^= _BV(SPEAKER_1_BIT);
  if (!--sndLeft) sndNext();
}
void sndPlay(const uint8_t* s) {
  TIMSK3 = 0;
  sndStart = sndAt = s; sndOn = true;
  TCCR3A = 0; TCCR3B = _BV(WGM32) | _BV(CS31); TCNT3 = 0;
  sndNext();
  if (sndOn) TIMSK3 = _BV(OCIE3A);
}
#else
#include "SimTones.h"                              // tools/sim: the harness's stand-in
#endif
*/



#if defined(ESP8266) || defined(ARDUINO_ARCH_ESP8266)

// Убираем Ticker, оставляем только частоты
const uint16_t PITCH_F[24] PROGMEM = { 440, 453, 466, 480, 494, 508, 523, 539, 554, 571, 587, 605, 622, 640, 659, 679, 698, 719, 740, 762, 784, 807, 831, 855 };

const uint8_t* sndAt; 
const uint8_t* sndStart;
volatile bool sndOn = false;
uint32_t sndNextNoteTime = 0; // Время, когда нужно переключить ноту

#define ESPBOY_SPEAKER_PIN D3

void sndStop() { 
  noTone(ESPBOY_SPEAKER_PIN); 
  sndOn = false; 
}

void sndNext() {
  uint8_t p, d;
  for (;;) {
    p = pgm_read_byte(sndAt); 
    d = pgm_read_byte(sndAt + 1); 
    sndAt += 2;
    if (d) break;
    if (!p) { sndStop(); return; }
    sndAt = sndStart;
  }
  
  uint32_t durationMs = (d * 1000) / 256;
  
  if (p) { 
    uint8_t o = 0; 
    while (p >= 24) { p -= 24; o++; } 
    uint16_t freqHz = (pgm_read_word(&PITCH_F[p]) << o) / 16;
    tone(ESPBOY_SPEAKER_PIN, freqHz);
  } else { 
    noTone(ESPBOY_SPEAKER_PIN);
  }
  
  // Запоминаем, когда нужно включить следующую ноту
  sndNextNoteTime = millis() + durationMs;
}

void sndPlay(const uint8_t* s) {
  sndStop(); 
  sndStart = sndAt = s; 
  sndOn = true;
  sndNext();
}

// Новая функция для программного обновления звука
void sndUpdate() {
  if (sndOn && millis() >= sndNextNoteTime) {
    sndNext();
  }
}

#else
// Фолбэк для эмуляторов
#include "SimTones.h"
#endif



// Music motifs own the voice while they play; effects wait.
bool musicOn;
void sfx(const uint8_t* s) { if (!musicOn) sndPlay(s); }
void music(const uint8_t* s) { musicOn = true; sndPlay(s); }
void musicStop() { if (musicOn) { sndStop(); musicOn = false; } }

// =================================================================== map
const uint8_t MAP_W = 24, MAP_H = 16;
const uint8_t VIEW_W = 16, VIEW_H = 7;     // 7 rows of 8px, HUD below
const uint16_t MAP_N = MAP_W * MAP_H;

enum Terrain : uint8_t { T_PLAIN, T_WOOD, T_HILL, T_WATER, T_WALL, T_KEEP, T_FORD, T_VILLAGE, T_CHEST, T_PALISADE, T_GATE, T_PORTAL, T_FLOOR, T_DECK, T_RAIL };
const uint8_t NO_PASS = 255;
//                                PLAIN WOOD HILL WATER   WALL     KEEP FORD VILLAGE CHEST PALISADE GATE PORTAL FLOOR DECK RAIL
const uint8_t T_COST[]  PROGMEM = { 1,   2,   3,  NO_PASS, NO_PASS, 1,   2,   1,      1,   NO_PASS,  1,   1,     1,    1,   NO_PASS };
const uint8_t T_AVOID[] PROGMEM = { 0,  20,  30,  0,       0,      20,   0,  10,     10,   0,       10,   0,     0,    0,   0 };
const uint8_t T_DEF[]   PROGMEM = { 0,   1,   2,  0,       0,       2,   0,   1,      1,   0,        1,   0,     0,    0,   0 };
// battle backdrop by the defender's ground: 0 field 1 forest 2 hills 3 river 4 stronghold 5 village 6 vault 7 ship
const uint8_t T_BACK[]  PROGMEM = { 0,   1,   2,  3,       4,       4,   3,   5,      4,   4,        4,   6,     6,    7,   7 };

// the current chapter's terrain, two tiles per byte (loaded from FX)
uint8_t mapT[MAP_N / 2];
__attribute__((noinline)) uint16_t idx(uint8_t x, uint8_t y) { return y * MAP_W + x; }
uint8_t terrainAt(uint8_t x, uint8_t y) {
  uint8_t b = mapT[idx(x, y) >> 1];
  return (x & 1) ? (b & 15) : (b >> 4);
}
__attribute__((noinline)) uint8_t tCost(uint8_t t)  { return pgm_read_byte(&T_COST[t]); }
inline uint8_t tAvoid(uint8_t t) { return pgm_read_byte(&T_AVOID[t]); }
inline uint8_t tDef(uint8_t t)   { return pgm_read_byte(&T_DEF[t]); }

// ================================================================= units
const uint8_t MAX_UNITS = NUM_HEROES + MAX_ENEMIES;
Unit units[MAX_UNITS];            // [0..NUM_HEROES) is the army, the rest this chapter's foes
uint8_t nUnits;

inline bool alive(uint8_t u)   { return units[u].flags & F_ALIVE; }
__attribute__((noinline)) bool isEnemy(uint8_t u) { return units[u].flags & F_ENEMY; }
inline bool joined(uint8_t u)  { return units[u].flags & F_JOINED; }
inline bool fighter(uint8_t u) { return alive(u) && joined(u); }          // a hero on the field
inline bool available(uint8_t u) { return joined(u) && !(units[u].flags & F_FALLEN); }   // in the army
uint8_t unitAt(uint8_t x, uint8_t y) {
  for (uint8_t i = 0; i < nUnits; i++)
    if (alive(i) && units[i].x == x && units[i].y == y) return i;
  return NONE;
}
__attribute__((noinline)) uint8_t manhattan(uint8_t ax, uint8_t ay, uint8_t bx, uint8_t by) {
  return (ax > bx ? ax - bx : bx - ax) + (ay > by ? ay - by : by - ay);
}
uint8_t dist2(uint8_t a, uint8_t b) { return manhattan(units[a].x, units[a].y, units[b].x, units[b].y); }
__attribute__((noinline)) bool canWield(uint8_t u, uint8_t kind) {                 // a class wields up to two weapon kinds
  const uint8_t* c = CLASS_ARMS[units[u].cls];
  return kind == pgm_read_byte(&c[0]) || kind == pgm_read_byte(&c[1]);
}
uint8_t clsMov(uint8_t u) {                       // a hero's F_BOOTS (WINGED BOOTS) adds 1
  return pgm_read_byte(&CLASS_ARMS[units[u].cls][2]) + ((units[u].flags & (F_BOOTS | F_ENEMY)) == F_BOOTS);
}
// the rest of a class (promo, tier, bonus) is on FX: field = offsetof in ClassDef
uint8_t clsField(uint8_t cls, uint8_t field) { return FX::readIndexedUInt8(FX_GFX::tables + TB_CLASSES + cls * sizeof(ClassDef), field); }

// ================================================================= items
ItemDef idef(uint8_t id) { ItemDef d; FX::readDataBytes(FX_GFX::tables + TB_ITEMS + id * sizeof d, (uint8_t*)&d, sizeof d); return d; }
__attribute__((noinline)) uint8_t kindOf(uint8_t id) { return pgm_read_byte(&ITEM_KIND[id]) & 15; }

// the first weapon (staff = false) or staff this unit's class can wield, or NONE
uint8_t findSlot(uint8_t u, bool staff) {
  for (uint8_t s = 0; s < ITEM_SLOTS; s++) {
    uint8_t id = units[u].item[s], k = kindOf(id);
    if (id && (k == K_STAFF) == staff && canWield(u, k)) return s;
  }
  return NONE;
}
uint8_t equipSlot(uint8_t u) { return findSlot(u, false); }   // the weapon in hand
// the staff for a wound of `need` HP: the weakest that heals it all, else the strongest
uint8_t staffFor(uint8_t u, uint8_t need) {
  uint8_t best = NONE, bm = 0;
  for (uint8_t s = 0; s < ITEM_SLOTS; s++) {
    uint8_t id = units[u].item[s];
    if (!id || kindOf(id) != K_STAFF) continue;
    uint8_t m = idef(id).might;
    if (best == NONE || (bm < need ? m > bm : (m >= need && m < bm))) { best = s; bm = m; }
  }
  return best;
}
void removeItem(uint8_t u, uint8_t s) {
  Unit& U = units[u];
  for (; s < ITEM_SLOTS - 1; s++) { U.item[s] = U.item[s + 1]; U.uses[s] = U.uses[s + 1]; }
  U.item[ITEM_SLOTS - 1] = 0; U.uses[ITEM_SLOTS - 1] = 0;
}
__attribute__((noinline)) bool giveItem(uint8_t u, uint8_t id, uint8_t uses) {
  for (uint8_t s = 0; s < ITEM_SLOTS; s++)
    if (!units[u].item[s]) { units[u].item[s] = id; units[u].uses[s] = uses & 0x7F; return true; }
  return false;
}
// the finder keeps it if there's room, else it goes to the first of the army who has some
bool wagonPut(uint8_t id, uint8_t uses);
uint8_t stash(uint8_t finder, uint8_t id, uint8_t uses) {   // the finder, else the wagon, else anyone with room
  if (giveItem(finder, id, uses)) return finder;
  if (wagonPut(id, uses)) return NUM_HEROES;        // = WAGON
  for (uint8_t h = 0; h < NUM_HEROES; h++)
    if (available(h) && giveItem(h, id, uses)) return h;
  return NONE;
}
// Common weapons and staves never wear out. Fine ones (uses > 1 in the item table) do; potions
// and crests are used up. Anything that runs out shows its count.
__attribute__((noinline)) bool consumable(uint8_t id) { return pgm_read_byte(&ITEM_KIND[id]) & K_COUNTED; }   // potions, crests, fine weapons
// one use off a fine weapon or staff in slot s; true if it broke (and is gone)
bool wear(uint8_t u, uint8_t s) {
  uint8_t k = pgm_read_byte(&ITEM_KIND[units[u].item[s]]);
  if ((k & K_COUNTED) == 0 || (k & 15) == K_HEAL || (k & 15) == K_PROMO) return false;   // only fine weapons wear here
  uint8_t& w = units[u].uses[s];
  w = (w & USES_DROP) | ((w & 0x7F) - 1);
  if (w & 0x7F) return false;
  removeItem(u, s);
  return true;
}
// attack range of the equipped weapon (lo > hi when unarmed)
void rangeOf(uint8_t u, uint8_t& lo, uint8_t& hi) {
  uint8_t s = equipSlot(u);
  if (s == NONE) { lo = 1; hi = 0; return; }
  ItemDef d = idef(units[u].item[s]);
  lo = d.minR; hi = d.maxR;
}

// ======================================================== reach / danger
// dist: low 7 bits = move cost to each tile (0x7F = unreachable); bit 7 =
// "marked" (attackable after moving, or, for the AI, inside the unit's reach).
// One 384-byte buffer serves player range, danger zones, routes and the AI.
uint8_t dist[MAP_N];
const uint8_t UNREACH = 0x7F;
__attribute__((noinline)) uint8_t dcost(uint16_t i) { return dist[i] & 0x7F; }
inline bool reachable(uint16_t i) { return dcost(i) != UNREACH; }
inline bool marked(uint16_t i) { return dist[i] & 0x80; }

bool canStand(uint8_t x, uint8_t y, uint8_t u) { uint8_t o = unitAt(x, y); return o == NONE || o == u; }

// Grow costs outward until stable. If `side` is a unit, its foes block movement.
void relax(uint8_t limit, uint8_t side, uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1) {
  bool changed = true;
  while (changed) {
    changed = false;
    for (uint8_t y = y0; y <= y1; y++)
      for (uint8_t x = x0; x <= x1; x++) {
        uint8_t d = dcost(idx(x, y));
        if (d == UNREACH) continue;
        for (uint8_t k = 0; k < 4; k++) {
          int8_t nx = x + (k == 0) - (k == 1), ny = y + (k == 2) - (k == 3);
          if (nx < 0 || ny < 0 || nx >= MAP_W || ny >= MAP_H) continue;
          uint8_t cost = tCost(terrainAt(nx, ny));
          if (cost == NO_PASS) continue;
          if (side != NONE) {
            uint8_t o = unitAt(nx, ny);
            if (o != NONE && isEnemy(o) != isEnemy(side)) continue;
          }
          uint16_t n = idx(nx, ny);
          uint8_t nd = d + cost;
          if (nd <= limit && nd < dcost(n)) { dist[n] = (dist[n] & 0x80) | nd; changed = true; }
        }
      }
  }
}

void computeReach(uint8_t u) {
  memset(dist, UNREACH, sizeof dist);
  Unit& U = units[u];
  uint8_t mov = (U.flags & F_HOLD) ? 0 : clsMov(u);
  dist[idx(U.x, U.y)] = 0;
  relax(mov, u, U.x > mov ? U.x - mov : 0, U.x + mov < MAP_W ? U.x + mov : MAP_W - 1,
                U.y > mov ? U.y - mov : 0, U.y + mov < MAP_H ? U.y + mov : MAP_H - 1);
  uint8_t lo, hi; rangeOf(u, lo, hi);
  for (uint8_t y = 0; y < MAP_H; y++)
    for (uint8_t x = 0; x < MAP_W; x++) {
      if (!reachable(idx(x, y)) || !canStand(x, y, u)) continue;
      for (int8_t dy = -hi; dy <= hi; dy++)
        for (int8_t dx = -hi; dx <= hi; dx++) {
          uint8_t m = abs(dx) + abs(dy);
          int8_t tx = x + dx, ty = y + dy;
          if (m < lo || m > hi || tx < 0 || ty < 0 || tx >= MAP_W || ty >= MAP_H) continue;
          dist[idx(tx, ty)] |= 0x80;
        }
    }
}

// ================================================================ combat
enum Weapon : uint8_t { W_SWORD = K_SWORD, W_LANCE = K_LANCE, W_AXE = K_AXE, W_BOW = K_BOW };
// sword > axe > lance > sword ; bows are neutral
int8_t triangle(uint8_t a, uint8_t b) {
  if (a >= W_BOW || b >= W_BOW || a == b) return 0;        // bows, staves, unarmed: neutral
  if ((a == W_SWORD && b == W_AXE) || (a == W_AXE && b == W_LANCE) || (a == W_LANCE && b == W_SWORD)) return 1;
  return -1;
}


// a strikes d from where they currently stand, with their equipped weapon
void forecast(uint8_t a, uint8_t d, Fc& f) {
  Unit& A = units[a]; Unit& D = units[d];
  memset(&f, 0, sizeof f);
  uint8_t sa = equipSlot(a);
  if (sa == NONE) return;
  ItemDef w = idef(A.item[sa]);
  uint8_t sd = equipSlot(d);
  uint8_t m = dist2(a, d);
  uint8_t terr = terrainAt(D.x, D.y);
  int8_t tri = triangle(w.kind, sd == NONE ? NONE : kindOf(D.item[sd]));
  f.can = m >= w.minR && m <= w.maxR;
  uint8_t df = D.def;
  if (w.kind == K_TOME) df >>= 1;                // magic finds the gaps in armour: half DEF
  int16_t dmg = A.str + w.might + tri - df - tDef(terr);
  f.dmg = dmg < 0 ? 0 : dmg;
  f.pierce = A.str + w.might;
  int16_t hit = A.skl * 2 + w.hit + tri * 15 - D.spd * 2 - tAvoid(terr);
  f.hit = hit < 0 ? 0 : (hit > 100 ? 100 : hit);
  f.crit = A.skl / 2 + w.crit;
  f.dbl = A.spd >= D.spd + 4;
}

uint16_t rngState = 0xACE1;
uint8_t rnd(uint8_t n) {                     // 0 .. n-1: xorshift16, scaled by a multiply (no division)
  rngState ^= rngState << 7; rngState ^= rngState >> 9; rngState ^= rngState << 8;
  return ((uint32_t)rngState * n) >> 16;
}
// FE-style "true hit": average of two rolls, so 80% feels like ~92%
bool rollHit(uint8_t hit) { return (uint8_t)((rnd(100) + rnd(100)) / 2) < hit; }

// ================================================================= state
State state;
uint8_t curX, curY, camX, camY;
uint8_t sel = NONE, origX, origY;     // unit being commanded
uint8_t turn, timer;
bool enemyPhase, won;

// chapter (header layout: tools/chapters/make_chapters.py)
uint8_t chapter, objType, objX, objY, objTurns;
uint8_t deployMax;
uint16_t deployMask;                             // bit per hero chosen to fight
__attribute__((noinline)) uint16_t bit16(uint8_t h) { return (uint16_t)1 << h; }
uint8_t capId, capX, capY, capLeaves;            // a captive waiting to be talked to (NONE if none)
uint8_t nVill, villX[MAX_VILLAGES], villY[MAX_VILLAGES], villDone;   // villages and chests
uint8_t nEv, evDone;                             // the chapter's events; bit per one-shot event that has fired
uint16_t storyFlags;                             // flags 1..16: saved with the army; chapters may require them
uint8_t localFlags;                              // flags 17..24: this chapter only
bool flagSet(uint8_t n) { n--; return n < 16 ? (storyFlags >> n) & 1 : (localFlags >> (n - 16)) & 1; }
void setFlag(uint8_t n) { n--; if (n < 16) storyFlags |= bit16(n); else localFlags |= 1 << (n - 16); }
uint24_t chAddr;
enum : uint8_t { OBJ_ROUT, OBJ_SEIZE, OBJ_BOSS, OBJ_SURVIVE };

enum : uint8_t { OPT_ATTACK, OPT_SEIZE, OPT_TALK, OPT_VISIT, OPT_HEAL, OPT_ITEM, OPT_WAIT, OPT_END, OPT_SOUND, OPT_RESTART, OPT_QUIT, OPT_ARMY };
uint8_t confirmRow = NONE;                       // RESTART / QUIT ask "SURE?" first
uint8_t titleSel;
uint8_t menuOpt[7], menuN, menuSel;
uint8_t tgt[6], tgtN, tgtSel;
bool healMode;                                   // the target list is wounded allies, not foes

// battle
uint8_t bA, bD, bStep, bKind, bDmg, bStriker, bBroke;   // bKind 0 none 1 hit 2 miss 3 crit; bBroke: whose weapon broke
bool bOver, bFell, bossTalked, bHeroHit;
State bReturn;
Fc fA, fD;
const uint8_t STRIKE_T = 44, IMPACT = 30;
uint8_t gotItem, gotTo;                          // item won, and who carries it (NONE = nobody had room, WAGON)
const uint8_t GOT_GOLD = 255;                      // gotItem: a purse of gotGold
uint16_t gotGold;
bool gotOnMap;                                   // ...from a village rather than a battle

// experience
uint8_t xHero, xFrom, xGain, xT, lvGains;        // lvGains: bit per stat that rose
uint8_t lvAmt[5];                                // ...and by how much (1 on a level-up; promotion bonuses)
bool promoting;                                  // the level-up screen is showing a class change
bool xOnMap;                                     // EXP from healing: drawn over the map

// dialogue (streamed from FX)
uint24_t talkAddr;
uint8_t talkLeft, talkWho, talkLen, talkShown;
State talkReturn;

// enemy phase
uint8_t eIdx, eStep, ePlanX, ePlanY, eTarget;

void clampCam() {
  if (curX < camX + 1) camX = curX > 0 ? curX - 1 : 0;
  if (curX > camX + VIEW_W - 2) camX = curX - VIEW_W + 2;
  if (curY < camY + 1) camY = curY > 0 ? curY - 1 : 0;
  if (curY > camY + VIEW_H - 2) camY = curY - VIEW_H + 2;
  if (camX > MAP_W - VIEW_W) camX = MAP_W - VIEW_W;
  if (camY > MAP_H - VIEW_H) camY = MAP_H - VIEW_H;
}
void centerOn(uint8_t x, uint8_t y) {
  int8_t cx = x - VIEW_W / 2, cy = y - VIEW_H / 2;
  camX = cx < 0 ? 0 : (cx > MAP_W - VIEW_W ? MAP_W - VIEW_W : cx);
  camY = cy < 0 ? 0 : (cy > MAP_H - VIEW_H ? MAP_H - VIEW_H : cy);
  curX = x; curY = y;
}

void startBanner(bool enemy) { enemyPhase = enemy; state = ST_BANNER; timer = 50; sfx(enemy ? S_PHASE_E : S_PHASE_P); }
void startTalk(uint8_t scene, State ret);
void startTalkAt(uint24_t addr, State ret);
void startExp();
void finishBattle();
bool showEvents(State ret);
void wakeGroupOf(uint8_t d);
uint8_t talkEvent(uint8_t u);
void runEvent(uint8_t i, uint8_t u);
void fallEvents(uint8_t u);
void addStats(Unit& U);
bool menuKeys(uint8_t& selIdx, uint8_t n);
enum : uint8_t { SC_OPEN, SC_BOSS, SC_WIN, SC_LOSE, SC_RECRUIT, SC_WARN, SC_TAKEN, SC_VILLAGE, SC_EVENT = SC_VILLAGE + MAX_VILLAGES };

// ============================================================ save / load
// A save is written at the start of every chapter: the army and where it is.
// Continue resumes there; a defeat retries from there.
const uint16_t SAVE_AT = EEPROM_STORAGE_SPACE_START + 64;
const uint8_t SAVE_MAGIC = 0xE4;                 // bump whenever Unit, the roster or this layout changes
// layout: magic, chapter, story flags (2 bytes), gold (2), army[NUM_HEROES]; then the supply wagon
// twice -- as it was when the chapter began, and as it is now -- so a restart puts it back too.
// The wagon lives only in EEPROM (no RAM): 6 pages of 4 slots, (item, uses) each.
const uint8_t WAGON = NUM_HEROES, WAGON_PAGES = 6, WAGON_N = WAGON_PAGES * ITEM_SLOTS;   // trade "heroes" WAGON.. are its pages
const uint16_t TALLY = SAVE_AT + 6 + 21 * 15;     // turns taken by the chapters behind us (the record)
const uint16_t WAGON_SAVED = TALLY + 4, WAGON_LIVE = WAGON_SAVED + WAGON_N * 2;   // room for 15 heroes
const uint16_t SAVE_LEN = WAGON_LIVE - SAVE_AT;     // what a chapter start saves (the live wagon isn't)
uint16_t gold;
__attribute__((noinline)) uint16_t tally() { uint16_t t = eeprom_read_word((const uint16_t*)TALLY); return t == 0xFFFF ? 0 : t; }   // (blank EEPROM: 0)
// EEPROM is shared by every game on the device, so the save is mirrored to this game's own block of
// FX flash and copied back at power-on: another game writing over bytes 80-500 can't lose it. The
// last page is written first, so the page with the magic byte only lands once the rest is there.
void mirrorSave() {
#ifdef __AVR__
  FX::eraseSaveBlock(0); FX::waitWhileBusy();
  for (int8_t pg = (SAVE_LEN - 1) >> 8; pg >= 0; pg--) {
    FX::writeEnable();
    FX::seekCommand(SFC_WRITE, ((uint24_t)FX::programSavePage + pg) << 8);
    uint16_t a = pg << 8;
    do FX::writeByte(eeprom_read_byte((const uint8_t*)(SAVE_AT + a))); while ((++a & 0xFF) && a < SAVE_LEN);
    FX::disable(); FX::waitWhileBusy();
  }
#endif
}
void restoreSave() {
#ifdef __AVR__
  FX::seekSave(0);
  if (FX::readPendingUInt8() == SAVE_MAGIC) {
    eeprom_update_byte((uint8_t*)SAVE_AT, SAVE_MAGIC);
    for (uint16_t a = 1; a < SAVE_LEN; a++) eeprom_update_byte((uint8_t*)(SAVE_AT + a), FX::readPendingUInt8());
  }
  FX::readEnd();
#endif
}
void copyWagon(uint16_t from, uint16_t to) {
  for (uint8_t i = 0; i < WAGON_N * 2; i++) eeprom_update_byte((uint8_t*)(to + i), from ? eeprom_read_byte((uint8_t*)(from + i)) : 0);
}
void saveGame() {
  eeprom_update_byte((uint8_t*)SAVE_AT, SAVE_MAGIC);
  eeprom_update_byte((uint8_t*)(SAVE_AT + 1), chapter);
  eeprom_update_word((uint16_t*)(SAVE_AT + 2), storyFlags);
  eeprom_update_word((uint16_t*)(SAVE_AT + 4), gold);
  eeprom_update_block(units, (void*)(SAVE_AT + 6), sizeof(Unit) * NUM_HEROES);
  copyWagon(WAGON_LIVE, WAGON_SAVED);
  mirrorSave();
}
bool hasSave() { return eeprom_read_byte((const uint8_t*)SAVE_AT) == SAVE_MAGIC; }
__attribute__((noinline)) uint8_t savedChapter() { return eeprom_read_byte((const uint8_t*)(SAVE_AT + 1)); }
uint8_t loadSave() {
  storyFlags = eeprom_read_word((const uint16_t*)(SAVE_AT + 2));
  gold = eeprom_read_word((const uint16_t*)(SAVE_AT + 4));
  eeprom_read_block(units, (const void*)(SAVE_AT + 6), sizeof(Unit) * NUM_HEROES);
  for (uint8_t h = 0; h < NUM_HEROES; h++)        // a save from before a hero existed: they start fresh
    if (units[h].name != h) FX::readDataBytes(FX_GFX::tables + TB_HEROES + h * sizeof(Unit), (uint8_t*)&units[h], sizeof(Unit));
  copyWagon(WAGON_SAVED, WAGON_LIVE);
  return savedChapter();
}
// item slots of a hero, or of a wagon page (h >= WAGON)
__attribute__((noinline)) uint8_t* wagonPtr(uint8_t h, uint8_t s) { return (uint8_t*)(WAGON_LIVE + ((h - WAGON) * ITEM_SLOTS + s) * 2); }
uint8_t slotItem(uint8_t h, uint8_t s) { return h < WAGON ? units[h].item[s] : eeprom_read_byte(wagonPtr(h, s)); }
uint8_t slotUses(uint8_t h, uint8_t s) { return h < WAGON ? units[h].uses[s] : eeprom_read_byte(wagonPtr(h, s) + 1); }
void setSlot(uint8_t h, uint8_t s, uint8_t id, uint8_t uses) {
  if (h < WAGON) { units[h].item[s] = id; units[h].uses[s] = uses; }
  else { eeprom_update_byte(wagonPtr(h, s), id); eeprom_update_byte(wagonPtr(h, s) + 1, uses); }
}
bool wagonPut(uint8_t id, uint8_t uses) {
  for (uint8_t p = WAGON; p < WAGON + WAGON_PAGES; p++)
    for (uint8_t s = 0; s < ITEM_SLOTS; s++)
      if (!slotItem(p, s)) { setSlot(p, s, id, uses & 0x7F); return true; }
  return false;
}

// ================================================================ chapters
__attribute__((noinline)) uint24_t chapterAddr(uint8_t n) { return FX_GFX::chapters + FX::readIndexedUInt24(FX_GFX::chapters + 1, n); }
void loadChapter(uint8_t n) {
  chapter = n;
  chAddr = chapterAddr(n);
  uint8_t hdr[CH_SLOTS];
  FX::readDataBytes(chAddr, hdr, sizeof hdr);
  objType = hdr[0]; objX = hdr[1]; objY = hdr[2]; objTurns = hdr[3];
  uint8_t nEnemies = hdr[4];
  deployMax = hdr[5];
  capId = hdr[7]; capX = hdr[8]; capY = hdr[9]; capLeaves = hdr[10];
  nVill = hdr[11]; villDone = 0;
  nEv = hdr[12]; evDone = 0; localFlags = 0;
  for (uint8_t v = 0; v < nVill; v++) {
    villX[v] = FX::readIndexedUInt8(chAddr + CH_VILL, v * 4);
    villY[v] = FX::readIndexedUInt8(chAddr + CH_VILL, v * 4 + 1);
  }
  FX::readDataBytes(chAddr + CH_MAP, mapT, sizeof mapT);
  deployMask = 0;
  uint8_t picked = 0;
  uint16_t joins = (uint16_t)hdr[15] << 8 | hdr[16];   // heroes who join as the chapter opens
  for (uint8_t h = 0; h < NUM_HEROES; h++) {           // heal up; default deployment in roster order
    Unit& U = units[h];
    U.flags &= ~(F_ALIVE | F_MOVED);
    if ((joins & bit16(h)) && !(U.flags & F_FALLEN)) U.flags |= F_JOINED;
    U.hp = U.maxHp;
    if (available(h) && picked < deployMax) { deployMask |= bit16(h); picked++; }
  }
  if (capId != NONE && !(units[capId].flags & (F_JOINED | F_FALLEN))) {
    units[capId].x = capX; units[capId].y = capY; units[capId].flags |= F_ALIVE;
  } else capId = NONE;
  FX::readDataBytes(chAddr + CH_MAP + sizeof mapT, (uint8_t*)&units[NUM_HEROES], nEnemies * sizeof(Unit));
  nUnits = NUM_HEROES + nEnemies;
  turn = 1; sel = NONE; bossTalked = false;
}

// Put the chosen heroes on the chapter's deployment slots, lord first.
__attribute__((noinline)) void placeArmy() {
  uint8_t s = 0;
  for (uint8_t h = 0; h < NUM_HEROES; h++)
    if (deployMask & bit16(h)) {
      units[h].x = FX::readIndexedUInt8(chAddr + CH_SLOTS, s * 2);
      units[h].y = FX::readIndexedUInt8(chAddr + CH_SLOTS, s * 2 + 1);
      units[h].flags |= F_ALIVE;
      s++;
    }
  centerOn(units[0].x, units[0].y);
}

// ------------------------------------------------------------ deployment
// The army in roster order, then BEGIN. Shown before every chapter after the first:
// tick who fights (up to the chapter's limit; the lord always goes).
uint8_t rosterCount() { uint8_t n = 0; for (uint8_t h = 0; h < NUM_HEROES; h++) n += available(h); return n; }
uint8_t rosterAt(uint8_t row) {
  for (uint8_t h = 0; h < NUM_HEROES; h++) if (available(h) && !row--) return h;
  return NONE;                                                   // the BEGIN row
}
__attribute__((noinline)) uint8_t deployedCount() { uint8_t n = 0; for (uint8_t h = 0; h < NUM_HEROES; h++) n += (deployMask >> h) & 1; return n; }
void beginBattle() { placeArmy(); startBanner(false); }

// ---------------------------------------------------------------- trade
// From the deployment screen: two heroes side by side. Row 0 is the name (A: the next hero).
// A on an item picks it up; A on any slot then swaps the two (an empty slot makes it a give;
// the same column reorders). B puts it back down, or returns to the roster.
uint8_t trHero[2], trCol, trRow, trHeld;          // trHeld: column * 8 + slot, or NONE
uint8_t nextHero(uint8_t h, uint8_t other) {      // the army, then the wagon's pages
  do h = h + 1 < WAGON + WAGON_PAGES ? h + 1 : 0; while ((h < WAGON && !available(h)) || h == other);
  return h;
}
void compact(uint8_t h) {                          // empty slots sink to the bottom
  uint8_t n = 0;
  for (uint8_t s = 0; s < ITEM_SLOTS; s++) { uint8_t id = slotItem(h, s), u = slotUses(h, s); if (id) setSlot(h, n++, id, u); }
  while (n < ITEM_SLOTS) setSlot(h, n++, 0, 0);
}
__attribute__((noinline)) void updateTrade() {
  if (ab.justPressed(LEFT_BUTTON) || ab.justPressed(RIGHT_BUTTON)) { trCol ^= 1; sfx(S_MENU); }
  if (menuKeys(trRow, ITEM_SLOTS + 2)) confirmRow = NONE;   // name, the slots, THROW AWAY
  if (ab.justPressed(B_BUTTON)) { sfx(S_CANCEL); confirmRow = NONE; if (trHeld != NONE) trHeld = NONE; else state = ST_DEPLOY; return; }
  if (!ab.justPressed(A_BUTTON)) return;
  uint8_t h = trHero[trCol];
  sfx(S_CONFIRM);
  if (!trRow) { trHero[trCol] = nextHero(h, trHero[trCol ^ 1]); trHeld = NONE; return; }
  if (trRow > ITEM_SLOTS) {                          // THROW AWAY the item in hand: A, then A again (SURE?)
    if (trHeld == NONE) sfx(S_CANCEL);
    else if (confirmRow != trRow) confirmRow = trRow;
    else { uint8_t g = trHero[trHeld >> 3]; setSlot(g, trHeld & 7, 0, 0); compact(g); trHeld = confirmRow = NONE; }
    return;
  }
  uint8_t s = trRow - 1;
  if (trHeld == NONE) { if (slotItem(h, s)) trHeld = trCol * 8 + s; else sfx(S_CANCEL); return; }
  uint8_t g = trHero[trHeld >> 3], gs = trHeld & 7, gi = slotItem(g, gs), gu = slotUses(g, gs);
  setSlot(g, gs, slotItem(h, s), slotUses(h, s)); setSlot(h, s, gi, gu);
  compact(g); compact(h);
  trHeld = NONE;
}

// ----------------------------------------------------------------- shop
// Town chapters put up to 6 items on sale at the prep screen (chapter header); bought goods go
// into the wagon. Prices are on FX with the item records.
uint8_t hasShop() { return FX::readIndexedUInt8(chAddr, CH_SHOP) != 0; }
uint8_t shopItem(uint8_t i) { return FX::readIndexedUInt8(chAddr + CH_SHOP, i); }
uint8_t shopSel;
void updateShop() {
  uint8_t n = 0; while (n < SHOP_SIZE && shopItem(n)) n++;
  menuKeys(shopSel, n);
  if (ab.justPressed(B_BUTTON)) { sfx(S_CANCEL); state = ST_DEPLOY; return; }
  if (!ab.justPressed(A_BUTTON)) return;
  uint8_t id = shopItem(shopSel); ItemDef d = idef(id);
  uint16_t price = d.price * 10;
  if (gold < price || !wagonPut(id, d.uses)) { sfx(S_CANCEL); return; }   // too dear, or the wagon is full
  gold -= price; sfx(S_DING);
}

// ---------------------------------------------------------------- status
// A unit's card: portrait, class, stats, items. From the map (A on a hero who has acted or a
// captive, A again on an enemy's danger zone, ARMY in the game menu) or the deployment list
// (RIGHT); LEFT/RIGHT step through that side (or the roster), A/B close it.
uint8_t stU; bool stDeploy;
__attribute__((noinline)) void openStatus(uint8_t u, bool fromDeploy) { stU = u; stDeploy = fromDeploy; state = ST_STATUS; sfx(S_CONFIRM); }
__attribute__((noinline)) void updateStatus() {
  if (ab.justPressed(RIGHT_BUTTON) || ab.justPressed(LEFT_BUTTON)) {   // the next of the same side (or of the roster)
    uint8_t u = stU, e = isEnemy(stU);
    do if (++u >= nUnits) u = 0;
    while (isEnemy(u) != e || !(alive(u) || (stDeploy && available(u))));
    stU = u; sfx(S_MENU);
  }
  if (!ab.justPressed(A_BUTTON) && !ab.justPressed(B_BUTTON)) return;
  sfx(S_CANCEL);
  state = stDeploy ? ST_DEPLOY : ST_SELECT;
  if (!stDeploy) centerOn(units[stU].x, units[stU].y);
}

void updateDeploy() {
  uint8_t n = rosterCount(), shop = hasShop();
  menuKeys(menuSel, n + 2 + shop);                               // the army, ITEMS, [SHOP], BEGIN
  if (ab.justPressed(RIGHT_BUTTON) && rosterAt(menuSel) != NONE) { openStatus(rosterAt(menuSel), true); return; }
  if (!ab.justPressed(A_BUTTON)) return;
  if (shop && menuSel == n + 1) { shopSel = 0; state = ST_SHOP; sfx(S_CONFIRM); return; }
  if (menuSel == n) {
    trHero[0] = rosterAt(0); trHero[1] = nextHero(trHero[0], trHero[0]);
    trCol = 0; trRow = 1; trHeld = confirmRow = NONE;
    state = ST_TRADE; sfx(S_CONFIRM); return;
  }
  uint8_t h = rosterAt(menuSel);
  if (h == NONE) { sfx(S_CONFIRM); beginBattle(); return; }
  uint16_t bit = bit16(h);
  if (units[h].flags & F_LORD) sfx(S_CANCEL);                    // the lord always fights
  else if (deployMask & bit) { deployMask &= ~bit; sfx(S_CANCEL); }
  else if (deployedCount() < deployMax) { deployMask |= bit; sfx(S_CONFIRM); }
  else sfx(S_CANCEL);
}

void startChapter(uint8_t n) {
  chapter = n;
  saveGame();
  loadChapter(n);
  centerOn(units[0].x, units[0].y);
  state = ST_CHAPTER;
  musicStop(); music(S_M_CHAPTER);
}

// per-name data on FX (tables.bin): k = 0 portrait, 1 voice, 2 battle figure
uint8_t nameInfo(uint8_t name, uint8_t k) { return FX::readIndexedUInt8(FX_GFX::tables + name * 3, k); }
void loadHeroes() { FX::readDataBytes(FX_GFX::tables + TB_HEROES, (uint8_t*)units, sizeof(Unit) * NUM_HEROES); }
#ifndef START_AT
#define START_AT 0                                // test builds: -DSTART_AT=N starts a new game at chapter N
#endif
void newGame() {
  loadHeroes();
  storyFlags = 0; gold = 0;
  eeprom_update_word((uint16_t*)TALLY, 0);
  copyWagon(0, WAGON_LIVE);                        // an empty wagon
  startChapter(START_AT);
}

// the chapter after n: side chapters are skipped unless their story flag was earned
uint8_t nextChapter(uint8_t n) {
  while (++n < NUM_CHAPTERS) {
    uint8_t need = FX::readIndexedUInt8(chapterAddr(n), CH_REQUIRES);
    if (!need || flagSet(need)) break;
  }
  return n;
}

bool anyAlive(bool enemy, uint8_t flag = 0) {
  for (uint8_t i = 0; i < nUnits; i++)
    if (alive(i) && isEnemy(i) == enemy && (units[i].flags & flag) == flag) return true;
  return false;
}
void endChapter(bool victory) { won = victory; titleSel = 0; startTalk(victory ? SC_WIN : SC_LOSE, ST_END); }
bool checkEnd() {
  if (!anyAlive(false, F_LORD)) { endChapter(false); return true; }
  bool foes = false;                                    // ambushers yet to appear still count
  for (uint8_t i = NUM_HEROES; i < nUnits; i++) if (units[i].flags & (F_ALIVE | F_HIDDEN)) foes = true;
  if (!foes || (objType == OBJ_BOSS && !anyAlive(true, F_BOSS))) { endChapter(true); return true; }
  return false;
}
bool anyReady() {
  for (uint8_t i = 0; i < NUM_HEROES; i++)
    if (fighter(i) && !(units[i].flags & F_MOVED)) return true;
  return false;
}

// ============================================================ input help
uint8_t holdT;
void moveCursor() {
  uint8_t held = ab.buttonsState() & (UP_BUTTON | DOWN_BUTTON | LEFT_BUTTON | RIGHT_BUTTON);
  if (!held) { holdT = 0; return; }
  if (holdT == 0 || (holdT >= 10 && (holdT & 3) == 0)) {
    uint8_t ox = curX, oy = curY;
    if ((held & LEFT_BUTTON) && curX > 0) curX--;
    if ((held & RIGHT_BUTTON) && curX < MAP_W - 1) curX++;
    if ((held & UP_BUTTON) && curY > 0) curY--;
    if ((held & DOWN_BUTTON) && curY < MAP_H - 1) curY++;
    if (curX != ox || curY != oy) sfx(S_CURSOR);
    clampCam();
  }
  if (holdT < 255) holdT++;
}
bool menuKeys(uint8_t& selIdx, uint8_t n) {     // up/down through a list, wrapping round; true if it moved
  if (ab.justPressed(UP_BUTTON)) selIdx = selIdx ? selIdx - 1 : n - 1;
  else if (ab.justPressed(DOWN_BUTTON)) selIdx = selIdx + 1 < n ? selIdx + 1 : 0;
  else return false;
  sfx(S_MENU); return true;
}

// =========================================================== player flow
void buildTargets(uint8_t u) {
  tgtN = 0; tgtSel = 0;
  uint8_t lo, hi; rangeOf(u, lo, hi);
  for (uint8_t i = NUM_HEROES; i < nUnits && tgtN < sizeof tgt; i++) {
    if (!alive(i)) continue;
    uint8_t m = dist2(u, i);
    if (m >= lo && m <= hi) tgt[tgtN++] = i;
  }
}
uint8_t villageAt(uint8_t x, uint8_t y) {
  for (uint8_t v = 0; v < nVill; v++) if (villX[v] == x && villY[v] == y) return v;
  return NONE;
}
bool canTalk(uint8_t u) { return capId != NONE && alive(capId) && !joined(capId) && dist2(u, capId) == 1; }
// wounded allies next to a staff-bearer
void buildHealTargets(uint8_t u) {
  tgtN = 0; tgtSel = 0;
  if (findSlot(u, true) == NONE) return;
  for (uint8_t i = 0; i < NUM_HEROES && tgtN < sizeof tgt; i++)
    if (i != u && fighter(i) && units[i].hp < units[i].maxHp && dist2(u, i) == 1) tgt[tgtN++] = i;
}
void openUnitMenu() {
  buildTargets(sel);
  menuN = 0; menuSel = 0; healMode = false;
  if (tgtN) menuOpt[menuN++] = OPT_ATTACK;
  if (objType == OBJ_SEIZE && (units[sel].flags & F_LORD) && units[sel].x == objX && units[sel].y == objY)
    menuOpt[menuN++] = OPT_SEIZE;
  if (canTalk(sel) || talkEvent(sel) != NONE) menuOpt[menuN++] = OPT_TALK;
  uint8_t v = villageAt(units[sel].x, units[sel].y);
  if (v != NONE && !(villDone & (1 << v))) menuOpt[menuN++] = OPT_VISIT;
  buildHealTargets(sel);
  if (tgtN) menuOpt[menuN++] = OPT_HEAL;
  if (units[sel].item[0]) menuOpt[menuN++] = OPT_ITEM;
  menuOpt[menuN++] = OPT_WAIT;
  state = ST_MENU;
}
void afterAction() {
  if (checkEnd()) return;
  state = ST_SELECT;
  if (!anyReady()) startBanner(true);
}
void finishUnit() {
  units[sel].flags |= F_MOVED;
  sel = NONE;
  afterAction();
}
// TALK: the captive joins the army (and waits until next turn to act)
void recruit() {
  units[capId].flags |= F_JOINED | F_MOVED;
  units[sel].flags |= F_MOVED; sel = NONE;
  music(S_LVLUP);
  startTalk(SC_RECRUIT, ST_SELECT);
}
// VISIT: hear the villagers out, take their gift
void visit(uint8_t v) {
  villDone |= 1 << v;
  uint8_t gift = FX::readIndexedUInt8(chAddr + CH_VILL, v * 4 + 2);
  uint8_t f = FX::readIndexedUInt8(chAddr + CH_VILL, v * 4 + 3);
  if (f) setFlag(f);
  gotItem = 0; gotOnMap = true;
  if (gift & 0x80) { gotGold = (gift & 0x7F) * 10; gold += gotGold; gotItem = GOT_GOLD; }   // a purse, not an item
  else if (gift) { gotItem = gift; gotTo = stash(sel, gift, idef(gift).uses); }
  units[sel].flags |= F_MOVED; sel = NONE;
  startTalk(SC_VILLAGE + v, gotItem ? ST_GOT : ST_SELECT);
}
void jumpToNextReady() {
  uint8_t start = unitAt(curX, curY);
  for (uint8_t k = 1; k <= NUM_HEROES; k++) {
    uint8_t i = (start < NUM_HEROES ? start + k : k - 1) % NUM_HEROES;
    if (fighter(i) && !(units[i].flags & F_MOVED)) { curX = units[i].x; curY = units[i].y; clampCam(); return; }
  }
}

void startBattle(uint8_t a, uint8_t d, State ret) {
  bA = a; bD = d; bStep = 0; bKind = 0; bReturn = ret; timer = 16;
  bStriker = NONE; bBroke = NONE; bOver = false; bFell = false; bHeroHit = false; gotItem = 0;
  forecast(a, d, fA); forecast(d, a, fD);
  state = ST_BATTLE;
  if (!bossTalked && (units[isEnemy(a) ? a : d].flags & F_BOSS)) {   // the boss speaks before the first clash
    bossTalked = true;
    startTalk(SC_BOSS, ST_BATTLE);
  } else if (isEnemy(d) && (units[d].flags & F_RESERVE)) {            // hitting a sleeper wakes the camp
    wakeGroupOf(d);
    showEvents(ST_BATTLE);
  }
}

// ----------------------------------------------------------------- walking
// Units travel their route instead of popping into place. The route is read
// back out of the reach map: from the goal, step to any neighbour whose cost
// plus this tile's entry cost equals this tile's cost, until cost 0.
const uint8_t WALK_F = 3;                 // frames per tile
uint8_t walkX[11], walkY[11], walkN, walkI, walkF, walkU;   // a route: start + up to 10 steps (max move 9, +1 boots)
State walkReturn;

void startWalk(uint8_t u, uint8_t tx, uint8_t ty, State ret) {
  uint8_t n = 0, x = tx, y = ty;
  walkX[0] = x; walkY[0] = y; n = 1;
  while (dcost(idx(x, y)) && n < sizeof walkX) {
    uint8_t c = tCost(terrainAt(x, y)), d = dcost(idx(x, y));
    bool found = false;
    for (uint8_t k = 0; k < 4 && !found; k++) {
      int8_t nx = x + (k == 0) - (k == 1), ny = y + (k == 2) - (k == 3);
      if (nx < 0 || ny < 0 || nx >= MAP_W || ny >= MAP_H) continue;
      uint8_t nd = dcost(idx(nx, ny));
      if (nd != UNREACH && nd + c == d) { x = nx; y = ny; found = true; }
    }
    if (!found) break;
    walkX[n] = x; walkY[n] = y; n++;
  }
  for (uint8_t i = 0; i < n / 2; i++) {    // reverse in place: start .. goal
    uint8_t t = walkX[i]; walkX[i] = walkX[n - 1 - i]; walkX[n - 1 - i] = t;
    t = walkY[i]; walkY[i] = walkY[n - 1 - i]; walkY[n - 1 - i] = t;
  }
  walkN = n; walkI = 0; walkF = 0; walkU = u; walkReturn = ret;
  state = ST_WALK;
}
// ================================================================ events
// Up to 8 per chapter (layout: tools/chapters/make_chapters.py): a condition (a hero
// walks over a tile, a turn starts, a unit falls, a hero talks to a unit) and actions
// (wake or spawn a group, warp, set a flag, recruit, win), maybe with a scene.
// Groups live in each enemy's exp byte, which enemies never otherwise use.
char strBuf[STR_LEN+1];                            // a name read from FX (see str())...
uint8_t* const ev = (uint8_t*)strBuf;            // ...or an event record: never needed at the same time
static_assert(STR_LEN >= EV_SIZE, "the event record shares the name buffer");
uint8_t evScene = NONE;                          // the scene the events just fired want shown
bool evWin;
bool loadEvent(uint8_t i) {                      // into ev[]; false if spent or switched off
  FX::readDataBytes(chAddr + CH_EVENTS + i * EV_SIZE, ev, EV_SIZE);
  if (!(ev[0] & EV_REPEAT) && (evDone >> i & 1)) return false;
  uint8_t n = ev[9] & 0x7F;                      // 'needs N' (bit 7: 'needs !N')
  return !(ev[6] && flagSet(ev[6])) && !(n && flagSet(n) == bool(ev[9] & 0x80));   // ...and 'until N'
}
// put unit u on (x, y), or the first free tile beside it; false if there's no room
bool placeNear(uint8_t u, uint8_t x, uint8_t y) {
  for (uint8_t k = 0; k < 5; k++) {
    int8_t nx = x + (k == 1) - (k == 2), ny = y + (k == 3) - (k == 4);
    if (nx < 0 || ny < 0 || nx >= MAP_W || ny >= MAP_H || tCost(terrainAt(nx, ny)) == NO_PASS || unitAt(nx, ny) != NONE) continue;
    units[u].x = nx; units[u].y = ny;
    return true;
  }
  return false;
}
// an enemy (re)enters, fresh from the chapter record, at its post
void spawnUnit(uint8_t i) {
  Unit& E = units[i];
  FX::readDataBytes(chAddr + CH_MAP + sizeof mapT + (i - NUM_HEROES) * sizeof(Unit), (uint8_t*)&E, sizeof(Unit));
  E.flags &= ~(F_HIDDEN | F_RESERVE | F_ALIVE);
  if (placeNear(i, E.x, E.y)) E.flags |= F_ALIVE;
}
void runEvent(uint8_t i, uint8_t u) {             // ev[] holds event i; u set it off
  bool first = !(evDone >> i & 1);                // a repeating event plays its scene only once
  evDone |= 1 << i;
  uint8_t a = ev[3], g = ev[4];
  for (uint8_t k = NUM_HEROES; k < nUnits; k++)
    if (g && units[k].exp == g) {
      if (a & A_WAKE) units[k].flags &= ~F_RESERVE;
      if ((a & A_SPAWN) && !alive(k)) spawnUnit(k);
    }
  if (a & A_SET) setFlag(ev[5]);
  if ((a & A_WARP) && placeNear(u, ev[7], ev[8])) { centerOn(units[u].x, units[u].y); sfx(S_SEAL); }
  if (a & A_JOIN) {                              // the talked-to enemy turns: same stats, now ours
    uint8_t e = ev[2], h = units[e].name;
    units[h] = units[e];
    units[h].flags = F_ALIVE | F_JOINED | F_MOVED; units[h].exp = 0;
    for (uint8_t s = 0; s < ITEM_SLOTS; s++) units[h].uses[s] &= 0x7F;
    units[e].flags = 0; units[e].exp = 0;
    music(S_LVLUP);
  }
  if (ev[0] & EV_DEMON) { musicStop(); music(S_M_DEMON); }
  else if (a & (A_WAKE | A_SPAWN)) sfx(S_ALARM);
  if ((a & A_SCENE) && first && evScene == NONE) evScene = SC_EVENT + i;
  if (a & A_WIN) evWin = true;
}
// play what the fired events asked for; true if the game moved on to it
bool showEvents(State ret) {
  uint8_t sc = evScene; evScene = NONE;
  if (evWin) { evWin = false; endChapter(true); return true; }
  if (sc == NONE) return false;
  startTalk(sc, ret);
  return true;
}
// a hero finished walking: tripwires along the route, portals where it stopped
bool tileEvents() {
  for (uint8_t i = 0; i < nEv; i++) {
    if (!loadEvent(i) || (ev[0] & 15) != EV_TILE) continue;
    for (uint8_t k = (ev[3] & A_WARP) ? walkN - 1 : 0; k < walkN; k++)
      if (walkX[k] == ev[1] && walkY[k] == ev[2]) { runEvent(i, walkU); return showEvents(ST_MENU); }
  }
  return false;
}
// the start of a turn (all of this turn's events fire; the first scene plays)
bool turnEvents() {
  for (uint8_t i = 0; i < nEv; i++)
    if (loadEvent(i) && (ev[0] & 15) == EV_TURN &&
        (turn == ev[1] || ((ev[0] & EV_REPEAT) && turn > ev[1] && (turn - ev[1]) % ev[2] == 0)))
      runEvent(i, NONE);
  return showEvents(ST_BANNER);
}
void fallEvents(uint8_t u) {                     // shown once the battle is over
  for (uint8_t i = 0; i < nEv; i++)
    if (loadEvent(i) && (ev[0] & 15) == EV_FALL && ev[1] == u) runEvent(i, u);
}
// a hero standing next to the unit some talk event is waiting for
uint8_t talkEvent(uint8_t u) {
  for (uint8_t i = 0; i < nEv; i++)
    if (loadEvent(i) && (ev[0] & 15) == EV_TALK && (ev[1] == NONE || ev[1] == u) && alive(ev[2]) && dist2(u, ev[2]) == 1) return i;
  return NONE;
}
// striking a sleeper: whatever event wakes its group fires (else the group just wakes)
void wakeGroupOf(uint8_t d) {
  for (uint8_t i = 0; i < nEv; i++)
    if (loadEvent(i) && (ev[3] & A_WAKE) && ev[4] == units[d].exp) { runEvent(i, NONE); break; }
  for (uint8_t k = NUM_HEROES; k < nUnits; k++) if (units[k].exp == units[d].exp) units[k].flags &= ~F_RESERVE;
}
void finishWalk() {
  units[walkU].x = walkX[walkN - 1]; units[walkU].y = walkY[walkN - 1];
  state = walkReturn;
  if (!isEnemy(walkU) && tileEvents()) return;
  if (state == ST_MENU) openUnitMenu();
}
void updateWalk() {
  if (walkN > 1 && ++walkF >= WALK_F) { walkF = 0; walkI++; sfx(S_STEP); }
  if (walkI >= walkN - 1) { curX = walkX[walkN - 1]; curY = walkY[walkN - 1]; clampCam(); finishWalk(); return; }
  curX = walkX[walkI]; curY = walkY[walkI]; clampCam();     // camera rides along
}

// B: tap = the next hero who hasn't acted; hold = the whole map (see drawMinimap)
uint8_t bHold;                                    // frames B has been held (0: not pressed here)
const uint8_t MINIMAP_HOLD = 8;
void updateSelect() {
  if (ab.justPressed(B_BUTTON)) bHold = 1;
  else if (ab.pressed(B_BUTTON)) { if (bHold && bHold < 255 && ++bHold == MINIMAP_HOLD) sfx(S_MENU); }
  else { if (bHold && bHold < MINIMAP_HOLD) { jumpToNextReady(); sfx(S_CURSOR); } bHold = 0; }
  if (bHold >= MINIMAP_HOLD) return;
  moveCursor();
  if (ab.justPressed(A_BUTTON)) {
    uint8_t u = unitAt(curX, curY);
    if (u == NONE) {                                      // the game menu
      menuN = 5; menuOpt[0] = OPT_END; menuOpt[1] = OPT_ARMY; menuOpt[2] = OPT_SOUND; menuOpt[3] = OPT_RESTART; menuOpt[4] = OPT_QUIT;
      menuSel = 0; confirmRow = NONE; state = ST_MENU; sfx(S_MENU);
    }
    else if (isEnemy(u)) { computeReach(u); state = ST_DANGER; sfx(S_CONFIRM); }
    else if (!joined(u) || (units[u].flags & F_MOVED)) openStatus(u, false);   // a captive, or done for the turn
    else {
      sel = u; origX = units[u].x; origY = units[u].y;
      computeReach(u); state = ST_MOVE; sfx(S_CONFIRM);
    }
  }
}

void updateMove() {
  moveCursor();
  if (ab.justPressed(A_BUTTON) && reachable(idx(curX, curY)) && canStand(curX, curY, sel))
    startWalk(sel, curX, curY, ST_MENU);
  else if (ab.justPressed(B_BUTTON)) { curX = origX; curY = origY; clampCam(); sel = NONE; state = ST_SELECT; sfx(S_CANCEL); }
}

void updateMenu() {
  if (menuKeys(menuSel, menuN)) confirmRow = NONE;
  if (ab.justPressed(B_BUTTON)) {
    sfx(S_CANCEL);
    if (sel == NONE) { state = ST_SELECT; return; }
    units[sel].x = origX; units[sel].y = origY;   // undo the move
    curX = origX; curY = origY; clampCam();
    computeReach(sel);
    state = ST_MOVE;
    return;
  }
  if (!ab.justPressed(A_BUTTON)) return;
  sfx(S_CONFIRM);
  switch (menuOpt[menuSel]) {
    case OPT_SOUND:
      if (ab.audio.enabled()) { sndStop(); ab.audio.off(); } else { ab.audio.on(); sndPlay(S_CONFIRM); }
      ab.audio.saveOnOff();
      break;
    case OPT_ATTACK: buildTargets(sel); healMode = false; state = ST_TARGET; break;
    case OPT_HEAL: buildHealTargets(sel); healMode = true; state = ST_TARGET; break;
    case OPT_TALK:
      if (canTalk(sel)) { recruit(); break; }
      runEvent(talkEvent(sel), sel);                       // talkEvent leaves the event in ev[]
      units[sel].flags |= F_MOVED; sel = NONE;
      if (!showEvents(ST_SELECT)) afterAction();
      break;
    case OPT_VISIT: visit(villageAt(units[sel].x, units[sel].y)); break;
    case OPT_SEIZE: endChapter(true); break;
    case OPT_ITEM: menuSel = 0; state = ST_ITEMS; break;
    case OPT_WAIT: finishUnit(); break;
    case OPT_ARMY: openStatus(0, false); break;               // the lord's card first
    case OPT_END:
      for (uint8_t i = 0; i < NUM_HEROES; i++) units[i].flags |= F_MOVED;
      startBanner(true); break;
    case OPT_RESTART: case OPT_QUIT:
      if (confirmRow != menuSel) { confirmRow = menuSel; break; }   // first A: "SURE?"
      musicStop();
      if (menuOpt[menuSel] == OPT_RESTART) startChapter(loadSave());  // back to this chapter's start
      else { titleSel = 0; state = ST_TITLE; }
      break;
  }
}

// Healing of any kind: HP goes up, rings close in on the unit (ST_HEALFX), then a staff
// earns its bearer EXP; a vulnerary just ends the turn.
uint8_t healAmt;
void startHealFx(uint8_t t, uint8_t might) {
  Unit& T = units[t];
  healAmt = T.maxHp - T.hp < might ? T.maxHp - T.hp : might;
  T.hp += healAmt;
  bA = sel; bD = t;
  timer = 36; state = ST_HEALFX;
}

// ITEM: A on a weapon equips it (free); A on a healing item uses it (ends the turn).
__attribute__((noinline)) uint8_t itemCount(uint8_t u) { uint8_t n = 0; while (n < ITEM_SLOTS && units[u].item[n]) n++; return n; }
__attribute__((noinline)) void updateItems() {
  menuKeys(menuSel, itemCount(sel));
  if (ab.justPressed(B_BUTTON)) { sfx(S_CANCEL); openUnitMenu(); return; }
  if (!ab.justPressed(A_BUTTON)) return;
  Unit& U = units[sel];
  uint8_t id = U.item[menuSel];
  ItemDef d = idef(id);
  if (d.kind == K_HEAL) {
    if (!(--U.uses[menuSel] & 0x7F)) removeItem(sel, menuSel);
    startHealFx(sel, d.might);
    sfx(S_DING);
  } else if (d.kind == K_PROMO || d.kind == K_BOOST) {   // HERO CREST: class change at PROMOTE_LEVEL+; boosters: +stat for good
    bool promo = d.kind == K_PROMO;
    uint8_t c = clsField(U.cls, 3);
    if (promo && (c == NONE || U.lv < PROMOTE_LEVEL)) { sfx(S_CANCEL); return; }
    removeItem(sel, menuSel);
    lvGains = 0;
    for (uint8_t i = 0; i < 5; i++) {
      lvAmt[i] = promo ? clsField(c, 5 + i) : (i == d.hit ? d.might : 0);
      if (lvAmt[i]) lvGains |= 1 << i;
    }
    if (promo) { U.cls = c; U.lv = 1; U.exp = 0; }
    else if (d.hit == 5) U.flags |= F_BOOTS;
    addStats(U);
    bA = xHero = sel; xT = 0; promoting = promo;
    state = ST_LEVELUP; music(S_LVLUP);
  } else if (canWield(sel, d.kind)) {                     // equip: move it to the top
    uint8_t w = U.uses[menuSel];
    for (uint8_t s = menuSel; s > 0; s--) { U.item[s] = U.item[s - 1]; U.uses[s] = U.uses[s - 1]; }
    U.item[0] = id; U.uses[0] = w; menuSel = 0;
    sfx(S_CONFIRM);
    openUnitMenu();
  } else sfx(S_CANCEL);                                    // can't wield that
}

// HEAL: the staff restores its might in HP, wears a use, and teaches the healer
__attribute__((noinline)) void gainExp(uint8_t hero, uint8_t amount, bool onMap);
void startHeal(uint8_t t) {
  uint8_t s = staffFor(sel, units[t].maxHp - units[t].hp), might = idef(units[sel].item[s]).might;
  wear(sel, s);
  startHealFx(t, might);
  sfx(S_SEAL);
}
void updateTarget() {
  if (ab.justPressed(RIGHT_BUTTON) || ab.justPressed(DOWN_BUTTON)) { tgtSel = (tgtSel + 1) % tgtN; sfx(S_MENU); }
  if (ab.justPressed(LEFT_BUTTON) || ab.justPressed(UP_BUTTON)) { tgtSel = (tgtSel + tgtN - 1) % tgtN; sfx(S_MENU); }
  curX = units[tgt[tgtSel]].x; curY = units[tgt[tgtSel]].y; clampCam();
  if (ab.justPressed(A_BUTTON)) {
    if (healMode) { startHeal(tgt[tgtSel]); return; }
    forecast(sel, tgt[tgtSel], fA); forecast(tgt[tgtSel], sel, fD); state = ST_FORECAST; sfx(S_CONFIRM);
  }
  if (ab.justPressed(B_BUTTON)) { curX = units[sel].x; curY = units[sel].y; clampCam(); state = ST_MENU; sfx(S_CANCEL); }
}

void updateForecast() {
  if (ab.justPressed(A_BUTTON)) startBattle(sel, tgt[tgtSel], ST_SELECT);
  if (ab.justPressed(B_BUTTON)) { state = ST_TARGET; sfx(S_CANCEL); }
}

// ================================================================ battle
// Strike order: A, D counters, A again if faster, D again if faster.
// Each strike is an animation; the dice are rolled at the moment of impact.
void applyStrike() {
  bool aStrikes = bStriker == bA;
  Fc& f = aStrikes ? fA : fD;
  Unit& T = units[aStrikes ? bD : bA];
  if (rollHit(f.hit)) {
    bool crit = rnd(100) < f.crit;
    bDmg = f.dmg;
    if (crit) { bDmg *= 3; if (bDmg < f.pierce) bDmg = f.pierce; }   // a crit finds the gap in any armour
    bKind = crit ? 3 : 1;
    T.hp = T.hp > bDmg ? T.hp - bDmg : 0;
    if (!isEnemy(bStriker) && bDmg) bHeroHit = true;
    sfx(!T.hp ? S_FALL : (crit ? S_CRIT : S_HIT));
  } else { bKind = 2; sfx(S_MISS); }
  if (wear(bStriker, equipSlot(bStriker))) { f.can = false; bBroke = bStriker; }   // hit or miss, a swing wears it
}

// a fallen foe's marked item goes to the hero who felled it
void claimDrop(uint8_t hero, uint8_t foe) {
  Unit& F = units[foe];
  for (uint8_t s = 0; s < ITEM_SLOTS; s++)
    if (F.item[s] && (F.uses[s] & USES_DROP)) {
      gotItem = F.item[s]; gotTo = stash(hero, F.item[s], F.uses[s]);
      return;
    }
}

bool casts(uint8_t u);
void battleTick() {
  if (timer) {
    timer--;
    if (timer == IMPACT && bStriker != NONE) applyStrike();
    return;
  }
  if (bOver) {
    if (!bFell) {                                        // once: the fallen fall, then any last words
      bFell = true;
      uint8_t lastWords = NONE;
      for (uint8_t k = 0; k < 2; k++) {
        uint8_t u = k ? bD : bA, o = k ? bA : bD;
        if (units[u].hp) continue;
        units[u].flags &= ~F_ALIVE;
        if (!isEnemy(u)) {                                // permadeath; the chapter goes on the memorial
          units[u].flags |= F_FALLEN; units[u].exp = chapter;
          if (!(units[u].flags & F_LORD)) lastWords = u;
        }
        else if (!isEnemy(o)) claimDrop(o, u);
        fallEvents(u);
      }
      if (lastWords != NONE) {                           // so a death on the enemy phase can't slip by
        musicStop(); music(S_M_LOSE);
        startTalkAt(FX_GFX::tables + TB_DEATH + (uint16_t)units[lastWords].name * DEATH_SLOT, ST_BATTLE);
        return;
      }
    }
    if (gotItem) { gotOnMap = false; state = ST_GOT; timer = 50; sfx(S_LVLUP); }
    else startExp();
    return;
  }
  while (true) {
    if (!units[bA].hp || !units[bD].hp || bStep > 3) { bOver = true; bStriker = NONE; timer = 26; return; }
    uint8_t step = bStep++;
    bool aStrikes = !(step & 1);
    Fc& f = aStrikes ? fA : fD;
    if (!f.can || (step >= 2 && !f.dbl)) continue;   // this strike doesn't happen
    bStriker = aStrikes ? bA : bD; bKind = 0; timer = STRIKE_T;
    sfx(casts(bStriker) ? S_SPELL : (dist2(bA, bD) > 1 ? S_ARROW : S_SWING));
    return;
  }
}
void updateBattle() {
  battleTick();
  if (ab.pressed(A_BUTTON) && state == ST_BATTLE) battleTick();   // hold A to fast-forward
}

// ============================================================ experience
// FE-style: ~10 for landing a hit on an equal, 1 for a whiff, 20+ more for the
// kill, a bonus for a boss; scaled by the level gap. 100 EXP = a level.
void finishBattle() {                            // after a battle, a heal or a level-up screen
  if (!isEnemy(bA)) { units[bA].flags |= F_MOVED; sel = NONE; if (!showEvents(ST_SELECT)) afterAction(); }
  else if (!checkEnd() && !showEvents(bReturn)) state = bReturn;
}
void startExp() {
  xHero = isEnemy(bA) ? bD : bA;
  uint8_t foe = xHero == bA ? bD : bA;
  Unit& H = units[xHero];
  if (!H.hp || H.lv >= 20) { finishBattle(); return; }
  int8_t diff = units[foe].lv + clsField(units[foe].cls, 4) * 10 - H.lv - clsField(H.cls, 4) * 10;
  int16_t e = bHeroHit ? (31 + diff) / 3 : 1;
  if (e < 1) e = 1;
  if (!units[foe].hp) {
    int16_t k = 20 + diff * 3; if (k < 5) k = 5;
    e += k;
    if (units[foe].flags & F_BOSS) e += 40;
  }
  gainExp(xHero, e > 100 ? 100 : e, false);
}
void gainExp(uint8_t hero, uint8_t amount, bool onMap) {
  xHero = hero; xFrom = units[hero].exp; xGain = amount; xT = 0; xOnMap = onMap;
  if (units[hero].lv >= 20) { finishBattle(); return; }
  state = ST_EXP;
}
void addStats(Unit& U) {                           // lvAmt[]: HP STR SKL SPD DEF
  uint8_t* stat = &U.str;                            // str, skl, spd, def are consecutive
  U.maxHp += lvAmt[0]; U.hp += lvAmt[0];
  for (uint8_t i = 1; i < 5; i++) stat[i - 1] += lvAmt[i];
}
__attribute__((noinline)) void rollLevelUp(Unit& U) {
  lvGains = 0;
  while (!lvGains)                                   // never an empty level-up
    for (uint8_t i = 0; i < 5; i++)
      if (rnd(100) < FX::readIndexedUInt8(FX_GFX::tables + TB_GROWTH + U.name * 5, i)) lvGains |= 1 << i;
  for (uint8_t i = 0; i < 5; i++) lvAmt[i] = (lvGains >> i) & 1;
  addStats(U);
  U.lv++;
  promoting = false;
}
void updateExp() {
  if (ab.justPressed(A_BUTTON) && xT * 2 < xGain) xT = xGain / 2 + 1;
  if (xT * 2 < xGain) { xT++; if ((xT & 3) == 0) sfx(S_EXP); return; }
  if (++xT < xGain / 2 + 22 && !ab.justPressed(A_BUTTON)) return;   // let it sit a moment
  Unit& H = units[xHero];
  uint16_t total = H.exp + xGain;
  if (total >= 100) { H.exp = total - 100; rollLevelUp(H); xT = 0; state = ST_LEVELUP; music(S_LVLUP); }
  else { H.exp = total; finishBattle(); }
}
void updateLevelUp() {
  xT++;
  for (uint8_t r = 0; r < 5; r++)
    if (xT == 20 + r * 10 && (lvGains & (1 << r))) sfx(S_DING);
  if (ab.justPressed(A_BUTTON)) { if (xT < 70) xT = 70; else finishBattle(); }
}

// ============================================================== enemy AI
// Score every (tile, target) pair the unit can reach: expected damage,
// a big bonus for a kill, a nudge toward the lord, minus expected counter.
// No target in reach -> walk toward the nearest hero by real path distance.
// F_HOLD units only consider their own tile.
void planEnemy(uint8_t e) {
  Unit& E = units[e];
  computeReach(e);
  uint8_t lo, hi; rangeOf(e, lo, hi);
  uint8_t ox = E.x, oy = E.y;
  int16_t best = -32000; eTarget = NONE; ePlanX = ox; ePlanY = oy;
  for (uint8_t y = 0; y < MAP_H; y++)
    for (uint8_t x = 0; x < MAP_W; x++) {
      if (!reachable(idx(x, y)) || !canStand(x, y, e)) continue;
      for (uint8_t p = 0; p < NUM_HEROES; p++) {
        if (!fighter(p)) continue;                       // captives are cargo, not targets
        uint8_t m = manhattan(x, y, units[p].x, units[p].y);
        if (m < lo || m > hi) continue;
        E.x = x; E.y = y;
        Fc& f = fA; Fc& g = fD;                          // scratch: startBattle recomputes both
        forecast(e, p, f); forecast(p, e, g);
        uint8_t hits = f.dbl ? 2 : 1;
        int16_t s = (int16_t)f.dmg * f.hit / 100 * hits;
        if (f.dmg * hits >= units[p].hp) s += 50;
        if (units[p].flags & F_LORD) s += 8;
        if (g.can) s -= (int16_t)g.dmg * g.hit / 200;
        s += tDef(terrainAt(x, y)) * 2;
        if (s > best) { best = s; ePlanX = x; ePlanY = y; eTarget = p; }
      }
    }
  E.x = ox; E.y = oy;
  if (eTarget != NONE || (E.flags & F_HOLD)) return;
  // Mark the standable reach set in bit 7, then reuse the low bits as a
  // distance-to-nearest-hero field and pick the marked tile closest to one.
  for (uint16_t i = 0; i < MAP_N; i++)
    dist[i] = (reachable(i) && canStand(i % MAP_W, i / MAP_W, e)) ? 0x80 | UNREACH : UNREACH;
  for (uint8_t p = 0; p < NUM_HEROES; p++)
    if (fighter(p)) { uint16_t i = idx(units[p].x, units[p].y); dist[i] &= 0x80; }
  relax(UNREACH - 1, NONE, 0, MAP_W - 1, 0, MAP_H - 1);
  uint8_t bestD = UNREACH;
  for (uint16_t i = 0; i < MAP_N; i++)
    if (marked(i) && dcost(i) < bestD) { bestD = dcost(i); ePlanX = i % MAP_W; ePlanY = i / MAP_W; }
}

void updateEnemy() {
  if (timer) { timer--; return; }
  if (eStep == 0) {
    while (eIdx < nUnits && (!alive(eIdx) || (units[eIdx].flags & F_RESERVE))) eIdx++;   // sleepers sleep
    if (eIdx >= nUnits) {        // enemy phase over
      turn++;
      for (uint8_t i = 0; i < nUnits; i++) units[i].flags &= ~F_MOVED;
      if (objType == OBJ_SURVIVE && turn > objTurns) { endChapter(true); return; }
      if (capId != NONE && capLeaves && alive(capId) && !joined(capId)) {
        if (turn > capLeaves) {                                   // too late: gone for good
          units[capId].flags = (units[capId].flags & ~F_ALIVE) | F_FALLEN;
          startTalk(SC_TAKEN, ST_BANNER); return;
        }
        if (turn == capLeaves) { startTalk(SC_WARN, ST_BANNER); return; }
      }
      if (turnEvents()) return;
      startBanner(false);
      return;
    }
    planEnemy(eIdx);
    centerOn(units[eIdx].x, units[eIdx].y);
    timer = 18; eStep = 1;
  } else if (eStep == 1) {
    timer = 14; eStep = 2;
    if (units[eIdx].x != ePlanX || units[eIdx].y != ePlanY) {
      computeReach(eIdx);                 // the plan may have reused dist; rebuild it for the route
      startWalk(eIdx, ePlanX, ePlanY, ST_ENEMY);
      timer = 8;
    }
  } else {
    uint8_t e = eIdx++;
    eStep = 0;
    if (eTarget != NONE && alive(eTarget)) startBattle(e, eTarget, ST_ENEMY);
  }
}

// ================================================================== text
// The woodcut 3x5 font (art.h). '\n' starts a new line.
uint8_t glyph(uint8_t x, uint8_t y, char ch, uint8_t color, uint8_t scale) {
  if (ch >= 'a' && ch <= 'z') ch -= 32;
  if (ch < 32 || ch > 90) ch = '?';
  uint8_t g[4];                                       // width, then 3 column bytes (from FX)
  FX::readDataBytes(FX_GFX::font + (ch - 32) * 4, g, 4);
  uint8_t w = g[0];
  for (uint8_t i = 0; i < w; i++) {
    uint8_t col = g[1 + i];
    for (uint8_t j = 0; j < 5; j++)
      if (col & (1 << j)) ab.fillRect(x + i * scale, y + j * scale, scale, scale, color);
  }
  return x + (w + 1) * scale;
}
uint8_t text(uint8_t x, uint8_t y, const char* s, uint8_t color = BLACK, uint8_t scale = 1) {
  for (; *s; s++) x = glyph(x, y, *s, color, scale);
  return x;
}
uint8_t textWidth(const char* s, uint8_t scale = 1) {
  uint8_t w = 0;
  for (; *s; s++) w += (FX::readIndexedUInt8(FX_GFX::font, (*s - 32) * 4) + 1) * scale;
  return w ? w - scale : 0;
}
char numBuf[6];
const char* num(uint16_t v) {
  char* p = numBuf + 5; *p = 0;
  do { *--p = '0' + v % 10; v /= 10; } while (v);
  return p;
}
uint8_t textN(uint8_t x, uint8_t y, uint16_t v, uint8_t color = BLACK) { return text(x, y, num(v), color); }
uint8_t textNR(uint8_t xr, uint8_t y, uint16_t v, uint8_t color = BLACK) {   // right-aligned
  const char* s = num(v);
  return text(xr - textWidth(s), y, s, color);
}
// Names (units, classes, items, terrain) live on FX; str() reads one into a buffer.
// The buffer is reused, so draw a name before fetching the next.
const char* str(uint8_t i) { 
  FX::readDataBytes(FX_GFX::strings + i * STR_LEN, (uint8_t*)strBuf, STR_LEN); 
  strBuf[STR_LEN] = '\0'; // Гарантированно обрываем строку, чтобы не читать мусор
  return strBuf; 
}
const char* nameOf(uint8_t u) { return str(STR_NAME + units[u].name); }
const char* itemName(uint8_t id) { return str(STR_ITEM + id); }
uint8_t textS(uint8_t x, uint8_t y, const char* s, uint8_t color = BLACK) { return text(x, y, s, color); }
// UI text lives on FX too (U_ ids from story.h)
uint8_t ui(uint8_t x, uint8_t y, uint8_t id, uint8_t color = BLACK, uint8_t scale = 1) { return text(x, y, str(id), color, scale); }
void uiCentred(uint8_t y, uint8_t id, uint8_t color, uint8_t scale = 1) { str(id); text(64 - textWidth(strBuf, scale) / 2, y, strBuf, color, scale); }

// A plank: white board with notched (chiselled) corners.
void plank(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  ab.fillRect(x, y, w, h, WHITE);
  ab.drawPixel(x, y, BLACK); ab.drawPixel(x + w - 1, y, BLACK);
  ab.drawPixel(x, y + h - 1, BLACK); ab.drawPixel(x + w - 1, y + h - 1, BLACK);
  ab.drawPixel(x, y + h / 2, BLACK); ab.drawPixel(x + w - 1, y + h / 2, BLACK);
}
__attribute__((noinline)) void prompt(uint8_t y) {                    // blinking "next" chevron
  if (ab.frameCount & 16) { ab.drawFastHLine(119, y, 5); ab.drawFastHLine(120, y + 1, 3); ab.drawPixel(121, y + 2); }
}

// =================================================================== map
// unit sprites on FX: player, enemy, greyed (spent heroes, sleeping garrison) x every class
uint8_t unitFrame(uint8_t u) {
  const Unit& U = units[u];
  uint8_t v = (U.flags & F_ENEMY) ? ((U.flags & F_RESERVE) ? 2 : 1) : ((U.flags & F_MOVED) ? 2 : 0);
  return v * NUM_CLASSES + U.cls;
}
uint8_t tileGfxIndex(uint8_t x, uint8_t y) {
  uint8_t t = terrainAt(x, y);
  if (t == T_WOOD) return ((x * 5 + y * 3 + x * y) % 3) ? T_WOOD : 9;   // spruce + twin pines
  if (t == T_VILLAGE || t == T_CHEST) { uint8_t v = villageAt(x, y); return (t == T_CHEST ? 12 : 10) + (v != NONE && (villDone & (1 << v))); }
  if (t >= T_PALISADE) return t + 5;                                   // palisade 14, gate 15, portal 16, floor 17, deck 18, rail 19
  if (t != T_PLAIN) return t;
  uint8_t h = (x * 7 + y * 13 + (x * y) % 5) % 7;                      // scattered grass dashes
  return h == 0 ? 7 : (h == 3 ? 8 : T_PLAIN);
}
inline bool reachXY(int8_t x, int8_t y) {
  return x >= 0 && y >= 0 && x < MAP_W && y < MAP_H && reachable(idx(x, y));
}
// Range overlay: a carved perimeter around where the unit can walk (dashed
// for an enemy's danger zone), and "/" ticks where it can strike.
void drawRange(bool dashed) {
  for (uint8_t ty = 0; ty < VIEW_H; ty++)
    for (uint8_t tx = 0; tx < VIEW_W; tx++) {
      int8_t mx = camX + tx, my = camY + ty;
      int16_t sx = tx * 8, sy = ty * 8;
      if (reachXY(mx, my)) {
        for (uint8_t k = 0; k < 8; k++) {
          if (dashed && (k & 2)) continue;
          if (!reachXY(mx, my - 1)) ab.drawPixel(sx + k, sy, WHITE);
          if (!reachXY(mx, my + 1)) ab.drawPixel(sx + k, sy + 7, WHITE);
          if (!reachXY(mx - 1, my)) ab.drawPixel(sx, sy + k, WHITE);
          if (!reachXY(mx + 1, my)) ab.drawPixel(sx + 7, sy + k, WHITE);
        }
      } else if (marked(idx(mx, my))) {
        ab.fillRect(sx + 2, sy + 2, 4, 4, BLACK);
        for (uint8_t k = 0; k < 4; k++) ab.drawPixel(sx + 2 + k, sy + 5 - k, WHITE);
      }
    }
}
void drawUnitAt(int16_t sx, int16_t sy, uint8_t u) { FX::drawBitmap(sx, sy, FX_GFX::units, unitFrame(u), dbmNormal); }
void drawMap(bool overlay, bool dashed = false) {
  for (uint8_t ty = 0; ty < VIEW_H; ty++)
    for (uint8_t tx = 0; tx < VIEW_W; tx++)
      FX::drawBitmap(tx * 8, ty * 8, FX_GFX::tiles, tileGfxIndex(camX + tx, camY + ty), dbmNormal);
  if (overlay) drawRange(dashed);
  for (uint8_t i = 0; i < nUnits; i++) {
    if (!alive(i)) continue;
    if (state == ST_WALK && i == walkU) {
      uint8_t a = walkI, b = walkI + 1 < walkN ? walkI + 1 : walkI;
      drawUnitAt((walkX[a] - camX) * 8 + (walkX[b] - walkX[a]) * 8 * walkF / WALK_F,
                 (walkY[a] - camY) * 8 + (walkY[b] - walkY[a]) * 8 * walkF / WALK_F, i);
      continue;
    }
    int8_t tx = units[i].x - camX, ty = units[i].y - camY;
    if (tx < 0 || ty < 0 || tx >= VIEW_W || ty >= VIEW_H) continue;
    if (i == sel && state == ST_MOVE && (ab.frameCount & 8)) continue;  // blink the active unit
    if (!isEnemy(i) && !joined(i) && (ab.frameCount & 48) == 48) continue;   // the captive fades in and out
    drawUnitAt(tx * 8, ty * 8, i);
    if (!isEnemy(i) && !joined(i))                                      // a captive: dotted cage
      for (uint8_t k = 0; k < 8; k += 2) {
        ab.drawPixel(tx * 8 + k, ty * 8); ab.drawPixel(tx * 8 + 7 - k, ty * 8 + 7);
        ab.drawPixel(tx * 8, ty * 8 + 7 - k); ab.drawPixel(tx * 8 + 7, ty * 8 + k);
      }
  }
}
void drawCursor() {
  int16_t x = (curX - camX) * 8, y = (curY - camY) * 8;
  uint8_t o = (ab.frameCount >> 3) & 1;
  x -= 1 + o; y -= 1 + o; uint8_t s = 9 + 2 * o;
  for (uint8_t c = 0; c < 4; c++) {                       // four corner brackets
    int16_t cx = (c & 1) ? x + s : x, cy = (c & 2) ? y + s : y;
    ab.drawFastHLine((c & 1) ? cx - 2 : cx, cy, 3);
    ab.drawFastVLine(cx, (c & 2) ? cy - 2 : cy, 3);
  }
}

// The whole map at 4 px a square (hold B): terrain as tiny patterns, heroes as solid blocks
// (hollow once they've acted), enemies flashing (a steady dot while asleep), the view as a frame.
//                                    PLAIN WOOD    HILL    WATER   WALL    KEEP    FORD    VILLAGE CHEST   PALISADE GATE   PORTAL  FLOOR
const uint16_t MINI[] PROGMEM = { 0,    0x0802, 0x0520, 0xC030, 0xFFFF, 0xF99F, 0x4030, 0x0660, 0x0660, 0x5555, 0x9999, 0x6996, 0x0001, 0x0F00, 0xFFFF };   // + DECK RAIL
void drawMinimap() {
  for (uint8_t y = 0; y < MAP_H; y++)
    for (uint8_t x = 0; x < MAP_W; x++) {
      uint16_t p = pgm_read_word(&MINI[terrainAt(x, y)]);
      for (uint8_t b = 0; b < 16; b++) if (p >> b & 1) ab.drawPixel(x * 4 + (b & 3), y * 4 + (b >> 2));
    }
  uint8_t n[2] = { 0, 0 };
  for (uint8_t i = 0; i < nUnits; i++) {
    if (!alive(i)) continue;
    uint8_t x = units[i].x * 4, y = units[i].y * 4, f = units[i].flags;
    ab.fillRect(x, y, 4, 4, BLACK);
    n[isEnemy(i)]++;
    if (!isEnemy(i)) { if (f & F_MOVED) ab.drawRect(x, y, 3, 3); else ab.fillRect(x, y, 3, 3); }
    else if (!(f & F_RESERVE) && (ab.frameCount & 8)) ab.fillRect(x, y, 3, 3);
    else ab.drawPixel(x + 1, y + 1);
  }
  ab.drawRect(camX * 4, camY * 4, VIEW_W * 4, VIEW_H * 4);
  ab.fillRect(96, 0, 32, 64, BLACK); ab.drawFastVLine(96, 0, 64);
  textN(ui(99, 3, U_TURN, WHITE), 3, turn, WHITE);
  textN(ui(99, 13, U_US, WHITE), 13, n[0], WHITE);
  textN(ui(99, 21, U_FOES, WHITE), 21, n[1], WHITE);
}

// HUD plank: BRANNOCK  CAV  LV1  HP 18/22  MV 6   (or the terrain under the cursor)
void drawHud() {
  ab.fillRect(0, 56, 128, 8, BLACK);
  plank(0, 57, 128, 7);
  uint8_t u = state == ST_WALK ? walkU : unitAt(curX, curY);
  uint8_t x;
  if (u != NONE && !isEnemy(u) && !joined(u)) {                  // the captive: what to do about her
    x = ui(textS(2, 58, nameOf(u)) + 5, 58, U_CAPTIVE);
    if (capLeaves) textN(ui(x + 4, 58, U_BYT), 58, capLeaves);
  } else if (u != NONE) {
    Unit& U = units[u];
    x = textS(2, 58, nameOf(u));
    x = textS(x + 5, 58, str(STR_CLASS + U.cls));
    x = ui(x + 5, 58, U_LV); x = textN(x + 1, 58, U.lv);
    x = ui(x + 5, 58, U_HP) + 3; x = textN(x, 58, U.hp); x = glyph(x + 1, 58, '/', BLACK, 1); x = textN(x + 1, 58, U.maxHp);
    x = ui(x + 5, 58, U_MV); textN(x, 58, (U.flags & F_HOLD) ? 0 : clsMov(u));
  } else {
    uint8_t t = terrainAt(curX, curY);
    x = textS(2, 58, str(STR_TERRAIN + t));
    if (tCost(t) == NO_PASS) { ui(x + 8, 58, U_IMPASS); return; }
    uint8_t v = villageAt(curX, curY);
    bool c = t == T_CHEST;
    if (v != NONE) ui(x + 8, 58, (villDone & (1 << v)) ? (c ? U_EMPTY : U_VISITED) : (c ? U_STOPOPEN : U_STOPVISIT));
    else {
      x = ui(x + 8, 58, U_DEF); x = textN(x, 58, tDef(t));
      x = ui(x + 6, 58, U_AVO); textN(x, 58, tAvoid(t));
    }
    x = glyph(96, 58, 'T', BLACK, 1); textN(x + 1, 58, turn);
    if (objType == OBJ_SURVIVE) { x = glyph(x + 9, 58, '/', BLACK, 1); textN(x + 1, 58, objTurns); }
  }
}

// a framed list box; labels come from fn(i). Used by the unit menu and items.
void listBox(uint8_t x, uint8_t w, uint8_t n, uint8_t selIdx, const char* (*label)(uint8_t), uint8_t (*right)(uint8_t)) {
  uint8_t h = n * 8 + 5, y = 3;
  ab.fillRect(x - 2, y - 2, w + 4, h + 4, BLACK);
  ab.drawRect(x, y, w, h, WHITE);
  for (uint8_t k = 4; k < w - 2; k += 4) ab.drawPixel(x + k, y, BLACK);   // gouged frame
  for (uint8_t i = 0; i < n; i++) {
    uint8_t ry = y + 3 + i * 8;
    uint8_t c = WHITE;
    if (i == selIdx) { plank(x + 2, ry - 1, w - 4, 8); c = BLACK; }
    text(x + 5, ry + 1, label(i), c);
    if (right) { uint8_t v = right(i); if (v != NONE) textNR(x + w - 5, ry + 1, v, c); }
  }
}
const char* menuLabel(uint8_t i) {                 // U_ATTACK.. follow the OPT_ order
  uint8_t li = U_ATTACK + menuOpt[i];
  if (li == U_SOUND && !ab.audio.enabled()) li = U_SOUNDOFF;
  if (li == U_VISIT && sel != NONE && terrainAt(units[sel].x, units[sel].y) == T_CHEST) li = U_OPEN;
  if (i == confirmRow) li = U_SURE;
  if (li == U_HEAL) return itemName(units[sel].item[staffFor(sel, 255)]);   // the strongest staff: HEAL, MEND
  return str(li);
}
const char* itemLabel(uint8_t i) { return itemName(units[sel].item[i]); }
uint8_t itemUses(uint8_t i) { uint8_t id = units[sel].item[i]; return consumable(id) ? units[sel].uses[i] & 0x7F : NONE; }
__attribute__((noinline)) uint8_t menuX(uint8_t w) { return (curX - camX) < VIEW_W / 2 ? 128 - w - 3 : 3; }
void drawMenu() { listBox(menuX(44), 44, menuN, menuSel, menuLabel, nullptr); }
void drawItems() {                                 // NAME ... E 20: the longest name ends at 51
  listBox(menuX(72), 72, itemCount(sel), menuSel, itemLabel, itemUses);
  uint8_t e = equipSlot(sel);
  if (e != NONE)                                    // "E" marks the wielded weapon, clear of the uses
    glyph(menuX(72) + 54, 7 + e * 8, 'E', e == menuSel ? BLACK : WHITE, 1);
}

// ============================================================== forecast
// carved shields with class charges, dotted stat rows, chiselled name planks
void x2Badge(uint8_t x, uint8_t y) {     // "x2" across the shield of whoever strikes twice
  ab.fillRect(x - 1, y - 1, 23, 16, BLACK);
  plank(x, y, 21, 14);
  ui(x + 3, y + 2, U_X2, BLACK, 2);
}
void drawForecast() {
  uint8_t A = sel, D = tgt[tgtSel];
  FX::drawBitmap(1, 2, FX_GFX::shields, units[A].cls * 2, dbmNormal);
  FX::drawBitmap(95, 2, FX_GFX::shields, units[D].cls * 2 + 1, dbmNormal);
  uint8_t lv[4] = { units[A].hp, fA.dmg, fA.hit, fA.crit };
  uint8_t rv[4] = { units[D].hp, fD.dmg, fD.hit, fD.crit };
  for (uint8_t r = 0; r < 4; r++) {
    uint8_t y = 4 + r * 9;
    uiCentred(y, r ? U_DMG - 1 + r : U_HP, WHITE);
    textNR(50, y, lv[r], WHITE);
    if (r == 0 || fD.can) textN(79, y, rv[r], WHITE); else ui(79, y, U_DASH, WHITE);
    if (r < 3) for (uint8_t x = 38; x < 92; x += 2) ab.drawPixel(x, y + 7, WHITE);
  }
  if (fA.dbl) x2Badge(18, 27);
  if (fD.can && fD.dbl) x2Badge(88, 27);
  plank(0, 47, 54, 7); textS(3, 48, nameOf(A));
  plank(74, 47, 54, 7); nameOf(D); textS(125 - textWidth(strBuf), 48, strBuf);
  plank(0, 57, 128, 7);
  uint8_t sa = equipSlot(A), sd = equipSlot(D);            // the weapons in play
  textS(3, 58, itemName(units[A].item[sa]));
  if (sd != NONE) { itemName(units[D].item[sd]); textS(125 - textWidth(strBuf), 58, strBuf); }
}

// ================================================================ battle
void ring(int16_t cx, int16_t cy, uint8_t r);
bool casts(uint8_t u) { uint8_t s = equipSlot(u); return s != NONE && kindOf(units[u].item[s]) == K_TOME; }
bool afar() { return dist2(bA, bD) > 1 || casts(bStriker); }   // the striker stays put: a shot or a spell
__attribute__((noinline)) int8_t advance(uint8_t u) {               // how far a melee striker has dashed in
  if (u != bStriker || afar()) return 0;
  uint8_t t = STRIKE_T - timer;
  if (t < 10) return t + t / 2;
  if (timer > 8) return 15;
  return timer + timer / 2;
}
bool striking(uint8_t u) {
  if (u != bStriker) return false;
  return afar() ? timer > 8 : (timer <= IMPACT + 5 && timer > 6);
}
void drawFigure(uint8_t u, int16_t baseX, bool left) {
  int16_t x = baseX + (left ? advance(u) : -advance(u));
  bool struck = bStriker != NONE && bStriker != u && (bKind == 1 || bKind == 3) && timer <= IMPACT && timer > IMPACT - 10;
  if (struck) x += (timer & 2) ? 2 : -2;
  if (!units[u].hp && (bOver || (timer <= IMPACT && (timer < IMPACT - 12 || (timer & 2))))) return;   // falls
  // a hero fought before they join faces left: their mirrored figure follows their own
  uint8_t fig = nameInfo(units[u].name, 2) + (isEnemy(u) && units[u].name < NUM_HEROES);
  FX::drawBitmap(x, 0, FX_GFX::figures, fig * 2 + (striking(u) ? 1 : 0), dbmMasked);
}
void spark(int16_t cx, int16_t cy, uint8_t r) {
  for (int8_t dx = -1; dx <= 1; dx++)
    for (int8_t dy = -1; dy <= 1; dy++)
      for (uint8_t d = 2; d <= r && (dx || dy); d++) ab.drawPixel(cx + dx * d, cy + dy * d, WHITE);
  ab.fillRect(cx - 1, cy - 1, 3, 3, WHITE);
}
void battlePlank(uint8_t x, uint8_t u) {
  plank(x, 54, 63, 10);
  textS(x + 2, 55, nameOf(u));
  textNR(x + 60, 55, units[u].hp);
  for (uint8_t i = 0; i < units[u].maxHp && i < 30; i++) {        // HP as carved notches
    uint8_t nx = x + 2 + i * 2;
    if (i < units[u].hp) ab.drawFastVLine(nx, 61 - ((i + 1) % 5 == 0), 2 + ((i + 1) % 5 == 0), BLACK);
    else ab.drawPixel(nx, 62, BLACK);
  }
}
void tagPlank(int16_t cx, int16_t y, const char* s) {       // a word on a board, centred on cx
  uint8_t w = textWidth(s) + 6;
  int16_t px = cx - w / 2; if (px < 1) px = 1; if (px + w > 127) px = 127 - w;
  ab.fillRect(px - 1, y - 1, w + 2, 11, BLACK);
  plank(px, y, w, 9); text(px + 3, y + 2, s);
}
void drawBattle() {
  uint8_t L = isEnemy(bA) ? bD : bA, R = isEnemy(bA) ? bA : bD;   // heroes on the left
  FX::drawBitmap(0, 0, FX_GFX::backdrop, pgm_read_byte(&T_BACK[terrainAt(units[bD].x, units[bD].y)]), dbmNormal);
  if (bStriker == L) { drawFigure(R, 70, false); drawFigure(L, -6, true); }   // striker drawn last
  else { drawFigure(L, -6, true); drawFigure(R, 70, false); }
  if (bStriker != NONE) {
    bool hitsRight = bStriker == L;
    int16_t tx = hitsRight ? 104 : 22;           // target's chest
    bool spell = casts(bStriker);
    if (afar() && timer > IMPACT) {              // arrow / javelin / spell in flight
      uint8_t t = STRIKE_T - timer, span = STRIKE_T - IMPACT;
      int16_t sx = hitsRight ? 40 : 86;
      int16_t ax = sx + (tx - sx) * t / span;
      int8_t d = hitsRight ? 1 : -1;
      if (spell) {                               // a white bolt with a black heart, shedding sparks
        ab.fillRect(ax - 2, 20, 5, 3, WHITE); ab.fillRect(ax - 1, 19, 3, 5, WHITE); ab.drawPixel(ax, 21, BLACK);
        for (uint8_t k = 1; k < 4; k++) ab.drawPixel(ax - d * k * 3, 21 + (((t + k) & 1) ? k : -k), WHITE);
      } else {
        ab.drawFastHLine(hitsRight ? ax - 6 : ax, 21, 7, WHITE);
        ab.drawPixel(ax + d, 20, WHITE); ab.drawPixel(ax + d, 22, WHITE); ab.drawPixel(ax + 2 * d, 21, WHITE);
      }
    }
    if (bKind && timer <= IMPACT) {
      bool hit = bKind == 1 || bKind == 3;
      if (hit && timer > IMPACT - 7) spark(tx, 22, bKind == 3 ? 9 : 6);
      if (hit && spell && timer > IMPACT - 12) {         // the blast spreads: two rings
        uint8_t r = (IMPACT + 4 - timer) * 2; ring(tx, 22, r); ring(tx, 22, r / 2 + 2);
      }
      char tag[10];
      if (bKind == 2) strcpy(tag, str(U_MISS));
      else { strcpy(tag, bKind == 3 ? str(U_CRIT) : "-"); strcat(tag, num(bDmg)); }
      tagPlank(tx, 2, tag);
    }
    if (bStep >= 3 && timer > 6) { char t2[3] = { 'X', '2', 0 }; tagPlank(hitsRight ? 34 : 92, 14, t2); }   // the follow-up blow
  }
  if (bBroke != NONE) tagPlank(bBroke == L ? 30 : 98, 30, str(U_BROKE));
  battlePlank(0, L); battlePlank(65, R);
}

// ========================================================= after battle
void drawGot() {
  if (gotOnMap) drawMap(false); else drawBattle();
  char t[32];
  if (gotItem == GOT_GOLD) { t[0] = '+'; strcpy(t + 1, num(gotGold)); strcat(t, str(U_GOLDW)); tagPlank(64, 24, t); return; }
  strcpy(t, str(gotTo == NONE ? U_NOROOM : (gotTo == WAGON ? U_TOWAGON : U_TO)));
  if (gotTo < WAGON) strcat(t, nameOf(gotTo));
  tagPlank(64, 18, t);
  strcpy(t, itemName(gotItem));
  tagPlank(64, 30, t);
}
void drawExp() {
  if (xOnMap) drawMap(false); else drawBattle();
  uint16_t shown = xFrom + (uint16_t)(xT * 2 < xGain ? xT * 2 : xGain);
  ab.fillRect(24, 22, 80, 18, BLACK);
  plank(26, 24, 76, 14);
  ui(29, 26, U_EXP);
  textNR(99, 26, shown % 100);
  ab.drawRect(29, 32, 70, 4, BLACK);
  ab.fillRect(30, 33, (shown % 100) * 68 / 100, 2, BLACK);
  if (shown >= 100 && (ab.frameCount & 4)) { ab.fillRect(42, 25, 40, 7, WHITE); ui(45, 26, U_LEVELUP); }
}
void drawPortrait(uint8_t name, uint8_t x);
// a circle of 16 points, radius 16 (one quadrant; the rest by symmetry)
const int8_t RING_Q[] PROGMEM = { 16, 0, 15, 6, 11, 11, 6, 15 };
void ring(int16_t cx, int16_t cy, uint8_t r) {      // a dotted circle; its shadow keeps it legible on white
  for (uint8_t k = 0; k < 16; k++) {
    int8_t a = pgm_read_byte(&RING_Q[(k & 3) * 2]), b = pgm_read_byte(&RING_Q[(k & 3) * 2 + 1]);
    int8_t dx = (k & 4) ? -b : a, dy = (k & 4) ? a : b;            // rotate a quarter turn
    if (k & 8) { dx = -dx; dy = -dy; }                              // and a half
    int16_t x = cx + dx * r / 16, y = cy + dy * r / 16;
    ab.drawPixel(x, y, WHITE); ab.drawPixel(x + 1, y + 1, BLACK);
  }
}
void drawHealFx() {
  drawMap(false);
  int16_t tx = (units[bD].x - camX) * 8 + 3, ty = (units[bD].y - camY) * 8 + 3;
  uint8_t t = 36 - timer;
  for (uint8_t k = 0; k < 2; k++) {                  // two dotted rings squeeze in on the unit
    int8_t r = 20 - (int8_t)t + k * 8;
    if (r <= 16 && r >= 1) ring(tx, ty, r);
  }
  if (t > 16) {
    char s[5] = { '+' }; strcpy(s + 1, num(healAmt));
    tagPlank(tx, ty > 24 ? ty - 20 : ty + 9, s);
  }
}
void drawDeploy() {
  uint8_t n = rosterCount(), h = rosterAt(menuSel);
  plank(0, 0, 70, 8);                                     // DEPLOY 5/6, but never more than the army has
  textN(glyph(textN(ui(3, 1, U_DEPLOY), 1, deployedCount()) + 1, 1, '/', BLACK, 1) + 1, 1, n < deployMax ? n : deployMax);
  uint8_t top = menuSel > 6 ? menuSel - 6 : 0;            // 7 rows show; the list scrolls
  uint8_t shop = hasShop();
  for (uint8_t r = top; r <= n + 1 + shop && r < top + 7; r++) {
    uint8_t y = 9 + (r - top) * 8;
    uint8_t u = rosterAt(r), c = WHITE;
    if (r == menuSel) { plank(0, y - 1, 70, 8); c = BLACK; }
    if (u == NONE) { ui(23, y, r == n ? U_ITEMS : (shop && r == n + 1 ? U_SHOP : U_BEGIN), c); continue; }
    ab.drawRect(3, y, 5, 5, c);
    if (deployMask & bit16(u)) ab.fillRect(4, y + 1, 3, 3, c);
    textS(11, y, nameOf(u), c);
    textN(ui(50, y, U_LV, c) + 1, y, units[u].lv, c);
  }
  if (h != NONE) drawPortrait(units[h].name, 72);
  else if (menuSel == n || (shop && menuSel == n + 1)) {
    if (menuSel == n) { ui(78, 20, U_ATRADE, WHITE); ui(78, 28, U_ITEMS, WHITE); } else ui(78, 24, U_ABUY, WHITE);
    textN(ui(78, 42, U_GOLD, WHITE), 42, gold, WHITE);
  }
  else { ui(78, 20, U_ATOFIGHT, WHITE); ui(78, 30, U_THELORD, WHITE); ui(78, 38, U_ALWAYS, WHITE); }
}
__attribute__((noinline)) void drawShop() {
  plank(0, 0, 128, 8);
  ui(3, 1, U_SHOP); textN(ui(70, 1, U_GOLD), 1, gold);
  for (uint8_t i = 0; i < SHOP_SIZE && shopItem(i); i++) {
    uint8_t y = 11 + i * 8, c = WHITE, id = shopItem(i);
    if (i == shopSel) { plank(0, y - 1, 128, 8); c = BLACK; }
    textS(5, y, itemName(id), c);
    textNR(124, y, idef(id).price * 10, c);
  }
  plank(0, 57, 128, 7); ui(3, 58, U_BUYHINT);
}
void drawTrade() {
  for (uint8_t c = 0; c < 2; c++) {
    uint8_t x = c * 65, h = trHero[c];
    bool on = c == trCol;
    for (uint8_t r = 0; r <= ITEM_SLOTS; r++) {
      uint8_t y = r ? 4 + r * 9 : 1, col = WHITE;
      if (on && r == trRow) { plank(x, y - 1, 63, 8); col = BLACK; }
      if (!r) { if (h < WAGON) textS(x + 3, y, nameOf(h), col); else textN(ui(x + 3, y, U_WAGON, col), y, h - WAGON + 1, col); continue; }
      uint8_t s = r - 1, id = slotItem(h, s);
      if (trHeld == c * 8 + s && (ab.frameCount & 8)) ab.fillRect(x + 1, y, 2, 5, col);   // the item in hand
      textS(x + 4, y, itemName(id), col);
      if (id && consumable(id)) textNR(x + 60, y, slotUses(h, s) & 0x7F, col);
    }
    if (on && (trHeld != NONE || trRow > ITEM_SLOTS)) {     // the bin, under the active column
      uint8_t y = 4 + (ITEM_SLOTS + 1) * 9, col = WHITE;
      if (trRow > ITEM_SLOTS) { plank(x, y - 1, 63, 8); col = BLACK; }
      ui(x + 4, y, confirmRow == trRow && trRow > ITEM_SLOTS ? U_SURE : U_DISCARD, col);
    }
  }
  for (uint8_t y = 0; y < 50; y += 2) ab.drawPixel(64, y);
  plank(0, 57, 128, 7);
  ui(ui(3, 58, trHeld == NONE ? U_TR1 : U_TR3), 58, trHeld == NONE ? U_TR2 : U_TR4);
}
// the card's numbers, right-aligned (labels, plank and rule are the FX 'card' image at x = 56):
// HP, max HP, STR, SKL, SPD, DEF (Unit order), then LV and MV
const uint8_t ST_VAL[] PROGMEM = { 76, 17, 89, 17, 89, 24, 125, 24, 89, 31, 125, 31, 125, 10, 125, 17 };
void drawStatus() {
  Unit& U = units[stU];
  if (nameInfo(U.name, 0) != NONE) drawPortrait(U.name, 0);       // the rank and file have no portrait: their figure
  else FX::drawBitmap(-8, 8, FX_GFX::figures, nameInfo(U.name, 2) * 2, dbmMasked);
  FX::drawBitmap(56, 0, FX_GFX::card, 0, dbmNormal);
  textS(60, 1, nameOf(stU));
  if (!isEnemy(stU)) ab.drawFastHLine(57, 8, (U.exp * 91) >> 7);  // EXP: a carved line under the name (x 0.71)
  textS(59, 10, str(STR_CLASS + U.cls), WHITE);
  uint8_t v[8]; memcpy(v, &U.hp, 6); v[6] = U.lv; v[7] = (U.flags & F_HOLD) ? 0 : clsMov(stU);
  for (uint8_t i = 0; i < 8; i++) textNR(pgm_read_byte(&ST_VAL[i * 2]), pgm_read_byte(&ST_VAL[i * 2 + 1]), v[i], WHITE);
  uint8_t e = equipSlot(stU);
  for (uint8_t k = 0; k < ITEM_SLOTS && U.item[k]; k++) {
    uint8_t y = 40 + k * 6;
    if (k == e) ab.fillRect(58, y, 2, 5);                   // the weapon in hand
    else if (U.uses[k] & USES_DROP) ab.drawPixel(59, y + 2); // an enemy's: it drops this one
    textS(62, y, itemName(U.item[k]), WHITE);
    if (consumable(U.item[k])) textNR(125, y, U.uses[k] & 0x7F, WHITE);
  }
}
void drawPortrait(uint8_t name, uint8_t x) {
  uint8_t pf = nameInfo(name, 0);
  if (pf != NONE) FX::drawBitmap(x, 0, FX_GFX::portraits, pf, dbmNormal);
}
void drawPortrait(uint8_t name) { drawPortrait(name, 0); }
void drawLevelUp() {
  Unit& H = units[xHero];
  drawPortrait(H.name);
  plank(60, 1, 67, 9);
  if (promoting) { uint8_t x = ui(63, 3, U_NOW) + 2; textS(x, 3, str(STR_CLASS + H.cls)); }   // (str() shares one buffer)
  else textN(ui(63, 3, U_LVUPLV), 3, H.lv);
  uint8_t val[5] = { H.maxHp, H.str, H.skl, H.spd, H.def };
  for (uint8_t r = 0; r < 5; r++) {
    uint8_t y = 13 + r * 10;
    bool shown = xT >= 20 + r * 10, rose = lvGains & (1 << r);
    uint8_t v = val[r] - (rose && !shown ? lvAmt[r] : 0);
    ui(62, y, U_HP + r, WHITE);
    textNR(90, y, v, WHITE);
    if (rose && shown) { plank(95, y - 1, 15, 7); textN(glyph(98, y, '+', BLACK, 1), y, lvAmt[r]); }
    uint8_t notches = r == 0 ? v / 2 : v;            // carved growth bar
    for (uint8_t i = 0; i < notches && i < 32; i++) ab.drawFastVLine(62 + i * 2, y + 6, 2, WHITE);
  }
  if (xT >= 70) prompt(59);
}

// ============================================================== dialogue
// Lines stream from the chapter record on FX: condition, speaker, length, text.
// Condition: 0 always; else bits 0-5 = a flag that must be set, or with bit 7 a hero (bits 0-3) who
// must be in the army -- or, with bit 5 too, who joined and fell; bit 6 inverts it. A line with
// speaker 255 is a guard: the line after it also needs the guard's condition ([14][MAREN] ...).
// Lines whose conditions fail are skipped.
bool cond(uint8_t c) {
  uint8_t h = c & 15;
  bool ok = !(c & 0x3F) || (!(c & 0x80) ? flagSet(c & 0x3F) :
            (c & 0x20) ? (units[h].flags & (F_JOINED | F_FALLEN)) == (F_JOINED | F_FALLEN) : available(h));
  return ok != bool(c & 0x40);
}
bool loadLine() {                                         // false when the scene has run out
  bool skip = false;
  for (; talkLeft; talkLeft--) {
    talkWho = FX::readIndexedUInt8(talkAddr, 1);
    talkLen = FX::readIndexedUInt8(talkAddr, 2);
    talkShown = 0;
    bool ok = !skip && cond(FX::readIndexedUInt8(talkAddr, 0));
    if (talkWho != NONE) { if (ok) return true; skip = false; }
    else skip = !ok;
    talkAddr += 3 + talkLen;
  }
  return false;
}
__attribute__((noinline)) void finishTalk() {
  if (talkReturn == ST_BATTLE) musicStop();               // the boss's motif ends as steel meets steel
  state = talkReturn;
  switch (state) {
    case ST_BANNER: startBanner(false); break;
    case ST_SELECT: afterAction(); break;
    case ST_GOT: timer = 50; sfx(S_LVLUP); break;
    case ST_MENU: openUnitMenu(); break;
    case ST_DEPLOY:                                       // the team screen, from chapter 2 on
      menuSel = 0;
      if (chapter == 0 && rosterCount() <= deployMax) beginBattle();
      break;
    default: break;
  }
}
void startTalkAt(uint24_t addr, State ret) {        // a scene anywhere on FX: line count, then lines
  talkAddr = addr; talkLeft = FX::readIndexedUInt8(addr, 0);
  talkAddr += 1; talkReturn = ret;
  if (!loadLine()) { finishTalk(); return; }
  state = ST_TALK;
}
void startTalk(uint8_t scene, State ret) {
  talkAddr = chAddr + FX::readIndexedUInt16(chAddr + CH_SCENES, scene);
  talkLeft = FX::readIndexedUInt8(talkAddr, 0);
  talkAddr += 1; talkReturn = ret;
  if (scene == SC_WIN) { musicStop(); music(S_M_WIN); }
  else if (scene == SC_LOSE) { musicStop(); music(S_M_LOSE); }
  if (!loadLine()) { finishTalk(); return; }
  if (scene == SC_BOSS) music(S_M_BOSS);
  state = ST_TALK;
}
void updateTalk() {
  if (talkShown < talkLen) {
    char ch = FX::readIndexedUInt8(talkAddr + 3, talkShown++);
    if ((talkShown & 1) && ch != ' ' && ch != '\n') {          // a voice per speaker
      uint8_t v = nameInfo(talkWho, 1);
      sfx(v == 0 ? S_BLIP_LO : (v == 2 ? S_BLIP_OSK : S_BLIP_MID));
    }
  }
  if (ab.justPressed(A_BUTTON) || ab.justPressed(B_BUTTON)) {
    if (talkShown < talkLen) { talkShown = talkLen; return; }
    talkAddr += 3 + talkLen; talkLeft--;
    if (!loadLine()) finishTalk();
  }
}
__attribute__((noinline)) void drawTalk() {
  drawPortrait(talkWho);
  plank(60, 2, 66, 9); textS(63, 4, str(STR_NAME + talkWho));
  uint8_t x = 60, y = 16;
  for (uint8_t i = 0; i < talkShown; i++) {
    char ch = FX::readIndexedUInt8(talkAddr + 3, i);
    if (ch == '\n') { x = 60; y += 8; } else x = glyph(x, y, ch, WHITE, 1);
  }
  if (talkShown >= talkLen) prompt(57);
}

// ======================================================== banners & ends
void drawBanner() {
  drawMap(false);
  ab.fillRect(0, 18, 128, 27, BLACK);
  for (uint8_t x = 0; x < 128; x += 2) { ab.drawPixel(x, 18); ab.drawPixel(x + 1, 44); }
  str(enemyPhase ? U_ENEMYPH : U_PLAYERPH);
  uint8_t w = textWidth(strBuf, 2) + 10;
  plank(64 - w / 2, 21, w, 14);
  text(64 - w / 2 + 5, 23, strBuf, BLACK, 2);
  plank(48, 37, 32, 7); textN(ui(53, 38, U_TURN), 38, turn);
}
bool lastChapter() { return nextChapter(chapter) >= NUM_CHAPTERS; }
uint8_t chLabel(uint8_t x, uint8_t y, uint8_t ch, uint8_t c) {   // "3" or "3X": the chapter's own label
  uint8_t lab = FX::readIndexedUInt8(chapterAddr(ch), CH_LABEL);
  x = textN(x, y, lab & 0x7F, c);
  return (lab & 0x80) ? glyph(x, y, 'X', c, 1) : x;
}
// the fixed parts of these screens are FX images ('pages'): 0 victory, 1 defeat, 2 the record
__attribute__((noinline)) void frame(uint8_t page) { FX::drawBitmap(0, 0, FX_GFX::pages, page, dbmNormal); }
void drawEnd() {
  frame(!won);                                             // "DEFEAT / BRANNOCK HAS FALLEN / A RETRY" is all image
  if (!won) return;
  uiCentred(26, lastChapter() ? U_CONTINUED : U_CLEAR, WHITE);
  uint8_t n = 0, first = NONE;                             // who fell this chapter (deployed, now fallen)
  for (uint8_t h = 0; h < NUM_HEROES; h++)
    if ((deployMask & bit16(h)) && (units[h].flags & F_FALLEN)) { if (!n++) first = h; }
  if (n) {                                                 // FELL: HALE +1
    uint8_t x = ui(36, 34, U_FELL, WHITE); x = textS(x, 34, nameOf(first), WHITE);
    if (n > 1) textN(glyph(x + 3, 34, '+', WHITE, 1), 34, n - 1, WHITE);
  }
  for (uint8_t i = 0; i < 2; i++) {                        // ONWARD (saves) / RESTART (this chapter again)
    uint8_t y = 43 + i * 9, c = WHITE;
    if (i == titleSel) { plank(30, y - 1, 68, 8); c = BLACK; }
    uiCentred(y, i ? U_RESTART : U_ONWARD, c);
  }
}
// The chapter's save is only replaced when the player goes ONWARD, so a costly win can be replayed.
// After the last chapter, ONWARD opens the epilogue instead (and the save stays at its start).
void updateEnd() {
  if (won) menuKeys(titleSel, 2);
  if (!ab.justPressed(A_BUTTON)) return;
  if (!won || titleSel) { startChapter(loadSave()); return; }   // retry / restart from the chapter's start
  if (lastChapter()) { state = ST_EPILOGUE; sfx(S_CONFIRM); return; }
  gold += FX::readIndexedUInt8(chAddr, CH_REWARD) * 10;          // heroes carry their levels and items; the purse grows
  eeprom_update_word((uint16_t*)TALLY, tally() + turn - (objType == OBJ_SURVIVE));
  startChapter(nextChapter(chapter));
}

// ================================================================ epilogue
// After the last chapter (whose win scene carries the endings and the roll of the fallen):
// the record -- allies won, heroes lost, side chapters found, turns taken -- and a rank for it.
const uint8_t RANK_AT[] PROGMEM = { 70, 54, 40, 26 };             // S A B C, else D
const uint16_t TURNS_FAST = 230, TURNS_SLOW = 300;                // the whole campaign, in turns (about 60 of them are survive chapters)
__attribute__((noinline)) __attribute__((noinline)) uint8_t rankOf(int8_t score, uint16_t turns) {   // 0 S .. 4 D
  score += turns < TURNS_FAST ? 20 : turns < TURNS_SLOW ? 10 : 0;
  uint8_t k = 0;
  while (k < 4 && score < (int8_t)pgm_read_byte(&RANK_AT[k])) k++;
  return k;
}
void drawEpilogue() {
  frame(2);
  uint8_t allies = 0, lost = 0, found = 0;
  for (uint8_t h = 0; h < NUM_HEROES; h++) { allies += joined(h); lost += cond(0xA0 | h); }
  for (uint16_t f = storyFlags & SIDE_FLAGS; f; f >>= 1) found += f & 1;
  uint16_t turns = tally() + turn - (objType == OBJ_SURVIVE);
  uint8_t k = rankOf(found * 4 + allies * 2 - lost * 6, turns);
  textNR(50, 17, allies, WHITE); textNR(50, 25, lost, WHITE); textNR(50, 33, found, WHITE); textNR(50, 41, turns, WHITE);
  glyph(90, 24, k ? 'A' - 1 + k : 'S', WHITE, 3);
  uiCentred(52, U_RANK0 + k, WHITE);
}

void drawTitle() {
  FX::drawBitmap(0, 0, FX_GFX::title, 0, dbmNormal);
  ab.fillRect(30, 41, 68, 21, BLACK);
  for (uint8_t i = 0; i < 2; i++) {
    uint8_t y = 43 + i * 10;
    if (i == titleSel) plank(32, y, 64, 8);
    uint8_t c = i == titleSel ? BLACK : WHITE;
    if (i == 0) chLabel(ui(36, y + 2, U_CONTINUE, c), y + 2, savedChapter(), c);   // "CONTINUE CH 3X"
    else ui(36, y + 2, U_NEWGAME, c);
  }
}
__attribute__((noinline)) void updateTitle() {
  if (!hasSave()) { newGame(); return; }
  menuKeys(titleSel, 2);
  if (ab.justPressed(A_BUTTON)) { sfx(S_CONFIRM); if (titleSel == 0) startChapter(loadSave()); else newGame(); }
}

// ================================================================== main
void setup() {
#if defined(ESP8266) || defined(ARDUINO_ARCH_ESP8266)
  EEPROM.begin(4096); // Инициализируем ровно столько, сколько требует FX
#endif

  ab.boot();                    // the parts of begin() we want: no logo, no text engine
  ab.display();
  // (no flashlight / boot-time sound combos: sound is toggled in game; uploads use UP + DOWN while running)
  ab.audio.begin();
  ab.waitNoButtons();
  ab.setFrameRate(30);
  rngState = ab.generateRandomSeed() | 1;   // never 0: xorshift would stick there
  
  // ВАЖНО: Читаем EEPROM ДО инициализации FX, так как FX::begin может пересоздать буфер
  restoreSave();
  
  FX::begin(FX_DATA_PAGE, FX_SAVE_PAGE);
  // The chapter blob starts with the chapter count. If that doesn't read back, the FX data
  // is missing or the build targets the wrong flash pin (FX vs FX-C): say so instead of hanging.
  // (The font lives on FX too, so the warning is a big X and the red LED.)
  if (FX::readIndexedUInt8(FX_GFX::chapters, 0) != NUM_CHAPTERS) {
    for (uint8_t k = 0; k < 64; k++) { ab.drawPixel(k * 2, k, WHITE); ab.drawPixel(k * 2, 63 - k, WHITE); }
    ab.digitalWriteRGB(RED_LED, RGB_ON);
    ab.display();
    while (true) {}
  }
  
  state = ST_TITLE;
}

void loop() {
  if (!ab.nextFrame()) return;
  ab.pollButtons();
  
#if defined(ESP8266) || defined(ARDUINO_ARCH_ESP8266)
  sndUpdate();
  if (eeprom_dirty) {
    EEPROM.commit();
    eeprom_dirty = false;
  }

#endif

  if (musicOn && !sndOn) musicOn = false;         // a one-shot motif finished

  switch (state) {
    case ST_TITLE:    updateTitle();    if (state == ST_TITLE) drawTitle(); break;
    case ST_CHAPTER:
      FX::drawBitmap(0, 0, FX_GFX::chapter, chapter, dbmNormal);
      if (ab.justPressed(B_BUTTON)) {                       // make sound easy to find
        if (ab.audio.enabled()) { musicStop(); ab.audio.off(); }
        else { ab.audio.on(); music(S_M_CHAPTER); }
        ab.audio.saveOnOff();
      }
      ab.fillRect(89, 1, 38, 9, BLACK);
      plank(90, 2, 36, 7);
      ui(92, 3, ab.audio.enabled() ? U_SNDON : U_SNDOFF);
      if (ab.justPressed(A_BUTTON)) startTalk(SC_OPEN, ST_DEPLOY);
      break;
    case ST_DEPLOY:   updateDeploy();   drawDeploy(); break;
    case ST_TALK:     updateTalk();     if (state == ST_TALK) drawTalk(); break;
    case ST_BANNER:
      if (timer) timer--;
      if (!timer || ab.justPressed(A_BUTTON)) {
        if (enemyPhase) { state = ST_ENEMY; eIdx = NUM_HEROES; eStep = 0; timer = 0; }
        else { state = ST_SELECT; centerOn(units[0].x, units[0].y); }   // the lord opens every turn
      }
      if (state == ST_BANNER) drawBanner(); else drawMap(false);
      break;
    case ST_SELECT:   updateSelect();   if (bHold >= MINIMAP_HOLD) drawMinimap(); else { drawMap(false); drawCursor(); drawHud(); } break;
    case ST_MOVE:     updateMove();     drawMap(true);  drawCursor(); drawHud(); break;
    case ST_WALK:     updateWalk();     drawMap(false); drawHud(); break;
    case ST_DANGER:                                          // A again: that enemy's card
      if (ab.justPressed(A_BUTTON)) openStatus(unitAt(curX, curY), false);
      else if (ab.justPressed(B_BUTTON)) state = ST_SELECT;
      drawMap(true, true); drawCursor(); drawHud(); break;
    case ST_MENU:     updateMenu();     drawMap(false); drawCursor(); drawHud(); if (state == ST_MENU) drawMenu(); break;
    case ST_ITEMS:    updateItems();    drawMap(false); drawCursor(); drawHud(); if (state == ST_ITEMS) drawItems(); else if (state == ST_MENU) drawMenu(); break;
    case ST_TARGET:   updateTarget();   drawMap(false); drawCursor(); drawHud(); break;
    case ST_FORECAST: updateForecast(); if (state == ST_FORECAST) drawForecast(); break;
    case ST_BATTLE:   drawBattle();     updateBattle(); break;
    case ST_HEALFX:   drawHealFx();     if (!--timer) { if (bA == bD) finishBattle(); else gainExp(bA, 12, true); } break;
    case ST_TRADE:    updateTrade();    if (state == ST_TRADE) drawTrade(); else drawDeploy(); break;
    case ST_SHOP:     updateShop();     if (state == ST_SHOP) drawShop(); else drawDeploy(); break;
    case ST_STATUS:   updateStatus();   drawStatus(); break;      // (on closing, the card shows one frame more)
    case ST_GOT:
      drawGot();
      if (!--timer || ab.justPressed(A_BUTTON)) { if (gotOnMap) afterAction(); else startExp(); }
      break;
    case ST_EXP:      updateExp();      if (state == ST_EXP) drawExp(); else if (state == ST_LEVELUP) drawLevelUp(); break;
    case ST_LEVELUP:  updateLevelUp();  if (state == ST_LEVELUP) drawLevelUp(); break;
    case ST_ENEMY:    updateEnemy();    drawMap(false); if (state == ST_ENEMY) { drawCursor(); drawHud(); } break;
    case ST_END:      drawEnd();        updateEnd(); break;
    case ST_EPILOGUE:                                        // A: back to the title
      drawEpilogue();
      if (ab.justPressed(A_BUTTON)) { sfx(S_CONFIRM); titleSel = 0; state = ST_TITLE; }
      break;
  }
  FX::display(CLEAR_BUFFER);
}
