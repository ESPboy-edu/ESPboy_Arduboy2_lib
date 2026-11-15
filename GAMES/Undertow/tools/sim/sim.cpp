// Headless campaign harness: runs Undertow.ino unchanged against PC
// stand-ins for Arduboy2Base / ArduboyFX / the tone player / EEPROM, plays whole
// campaigns with a bot that presses real buttons, and checks invariants.
//
//   ./sim 300          play 300 campaigns
//   SHOTS=1 ./sim 1    also save screenshots of every screen to shots/
#include "Undertow.ino"
#include <string>
#include <set>
#include <vector>

static bool wantShots;
uint8_t itemId(const char* name);
static std::set<std::string> shotTaken;
void savePBM(const std::string& name) {
  if (!wantShots || shotTaken.count(name)) return;
  shotTaken.insert(name);
  std::string path = "shots/" + name + ".pbm";
  FILE* f = fopen(path.c_str(), "w"); if (!f) return;
  fprintf(f, "P1\n128 64\n");
  for (int y = 0; y < 64; y++) { for (int x = 0; x < 128; x++) fputc(ab.shown[y][x] ? '1' : '0', f); fputc('\n', f); }
  fclose(f);
}

// ------------------------------------------------------------------ the bot
uint8_t stepToward(uint8_t tx, uint8_t ty) {
  if (curX < tx) return RIGHT_BUTTON; if (curX > tx) return LEFT_BUTTON;
  if (curY < ty) return DOWN_BUTTON;  if (curY > ty) return UP_BUTTON;
  return 0;
}
uint16_t pathD[MAP_N];
void buildPathDist() {                         // walking distance to the nearest enemy, ignoring units
  for (auto& v : pathD) v = 9999;
  bool boss = (objType == OBJ_BOSS || objType == OBJ_SEIZE) && anyAlive(true, F_BOSS);   // boss chapters (or a boss on the seize square): head for the boss
  for (uint8_t e = NUM_HEROES; e < nUnits; e++)
    if (alive(e) && (!boss || (units[e].flags & F_BOSS))) pathD[idx(units[e].x, units[e].y)] = 0;
  bool ch = true;
  while (ch) { ch = false;
    for (uint8_t y = 0; y < MAP_H; y++) for (uint8_t x = 0; x < MAP_W; x++) {
      uint16_t d = pathD[idx(x, y)]; if (d == 9999) continue;
      const int DX[4] = {1,-1,0,0}, DY[4] = {0,0,1,-1};
      for (int k = 0; k < 4; k++) { int nx = x + DX[k], ny = y + DY[k];
        if (nx < 0 || ny < 0 || nx >= MAP_W || ny >= MAP_H) continue;
        uint8_t c = tCost(terrainAt(nx, ny)); if (c == NO_PASS) continue;
        if (d + c < pathD[idx(nx, ny)]) { pathD[idx(nx, ny)] = d + c; ch = true; } } } }
}
uint8_t destX, destY, destFor = NONE;
bool wantHeal;                                 // this unit should drink its vulnerary instead
uint8_t rescuer = NONE;                        // the hero sent to talk to the captive
bool botRescues = true, botVisits = true;      // varied per game so both outcomes get tested
bool careful = false;
bool botTrades = true, botHunts = true;
uint8_t hunter = NONE;
uint8_t tradeStep = 99;                          // CAREFUL=1: reload the chapter whenever a hero dies
uint16_t capD[MAP_N];
void distFrom(uint16_t* d, uint8_t tx, uint8_t ty) {   // walking distance to (tx,ty)'s neighbours
  for (int i = 0; i < MAP_N; i++) d[i] = 9999;
  d[idx(tx, ty)] = 0;
  bool ch = true;
  while (ch) { ch = false;
    for (uint8_t y = 0; y < MAP_H; y++) for (uint8_t x = 0; x < MAP_W; x++) {
      uint16_t dd = d[idx(x, y)]; if (dd == 9999) continue;
      const int DX[4] = {1,-1,0,0}, DY[4] = {0,0,1,-1};
      for (int k = 0; k < 4; k++) { int nx = x + DX[k], ny = y + DY[k];
        if (nx < 0 || ny < 0 || nx >= MAP_W || ny >= MAP_H) continue;
        uint8_t c = tCost(terrainAt(nx, ny)); if (c == NO_PASS) continue;
        if (dd + c < d[idx(nx, ny)]) { d[idx(nx, ny)] = dd + c; ch = true; } } } }
}
bool captivePending() { return capId != NONE && alive(capId) && !joined(capId); }
// move to the reachable standable tile closest (by walking) to (tx, ty); true if adjacent
bool closestTo(uint8_t tx, uint8_t ty) {
  distFrom(capD, tx, ty);
  uint16_t best = 9999;
  for (uint8_t y = 0; y < MAP_H; y++) for (uint8_t x = 0; x < MAP_W; x++) {
    if (!reachable(idx(x, y)) || !canStand(x, y, sel)) continue;
    uint16_t d = capD[idx(x, y)];
    if (manhattan(x, y, tx, ty) == 0) continue;
    if (objType == OBJ_SEIZE && x == objX && y == objY && !(units[sel].flags & F_LORD)) continue;   // keep the lord's square free
    if (d < best) { best = d; destX = x; destY = y; }
  }
  if (best == 9999) { destX = units[sel].x; destY = units[sel].y; }   // nowhere closer: stay
  destFor = sel;
  return best != 9999;
}
void pickDest() {
  if (units[sel].cls == C_CLERIC) {                    // the healer: next to the most wounded ally
    uint8_t w = NONE; int worst = 0;
    for (uint8_t h = 0; h < NUM_HEROES; h++)
      if (h != sel && fighter(h) && units[h].maxHp - units[h].hp > worst) { worst = units[h].maxHp - units[h].hp; w = h; }
    if (w == NONE) w = 0;
    closestTo(units[w].x, units[w].y);
    return;
  }
  // the chest-hunter: straight for any unopened chest (ledger runs)
  if (sel == hunter && botHunts)
    for (uint8_t v = 0; v < nVill; v++)
      if (!(villDone & (1 << v)) && terrainAt(villX[v], villY[v]) == T_CHEST) {
        if (reachable(idx(villX[v], villY[v])) && canStand(villX[v], villY[v], sel)) { destX = villX[v]; destY = villY[v]; destFor = sel; }
        else closestTo(villX[v], villY[v]);
        return;
      }
  bool rushing = false;
  if (sel == rescuer && captivePending()) {          // kill on the way, but rush near the deadline
    rushing = !capLeaves || turn + 2 >= capLeaves;
    if (rushing) { closestTo(capX, capY); return; }
  }
  if ((units[sel].flags & F_LORD) && objType == OBJ_SEIZE && !anyAlive(true, F_BOSS)) {   // march on the objective
    int foes = 0; for (uint8_t e = NUM_HEROES; e < nUnits; e++) if (alive(e) && units[e].name >= NUM_HEROES && !(units[e].flags & F_RESERVE)) foes++;
    if (foes <= 2 || turn > 12) {                       // (respawning chapters: push on after a while)
      if (reachable(idx(objX, objY)) && canStand(objX, objY, sel)) { destX = objX; destY = objY; destFor = sel; }
      else closestTo(objX, objY);
      return;
    }
  }
  if (objType == OBJ_SEIZE && turn > 20 && !(units[sel].flags & F_LORD)) {   // a long seize chapter: everyone heads for it
    closestTo(objX, objY); return;                                             // (so the army can't jam a corridor)
  }
  uint8_t lo, hi; rangeOf(sel, lo, hi);
  int army = 0; for (uint8_t h = 0; h < NUM_HEROES; h++) army += fighter(h) && units[h].cls != C_CLERIC;
  bool lord = (units[sel].flags & F_LORD) && army > 1;       // alone, the lord fights like anyone
  int enemiesLeft = 0; for (uint8_t e = NUM_HEROES; e < nUnits; e++) if (alive(e)) enemiesLeft++;
  buildPathDist();
  int best = 1 << 30; destX = units[sel].x; destY = units[sel].y;
  for (uint8_t y = 0; y < MAP_H; y++) for (uint8_t x = 0; x < MAP_W; x++) {
    if (!reachable(idx(x, y)) || !canStand(x, y, sel)) continue;
    if (objType == OBJ_SEIZE && x == objX && y == objY && !(units[sel].flags & F_LORD)) continue;
    for (uint8_t e = NUM_HEROES; e < nUnits; e++) {
      if (!alive(e) || units[e].name < NUM_HEROES) continue;      // never cut down someone who could join
      uint8_t m = manhattan(x, y, units[e].x, units[e].y);
      int s;
      if (m >= lo && m <= hi) {
        uint8_t ox = units[sel].x, oy = units[sel].y; units[sel].x = x; units[sel].y = y;
        Fc f; forecast(sel, e, f); units[sel].x = ox; units[sel].y = oy;
        bool kill = f.dmg * (f.dbl ? 2 : 1) >= units[e].hp;
        if (lord && !kill && enemiesLeft > 1) continue;      // the lord only strikes to finish
        s = kill ? 0 : units[e].hp;
      } else if (lord && enemiesLeft > 1) s = 2000 + dcost(idx(x, y));
      else s = 1000 + pathD[idx(x, y)] * 4 + dcost(idx(x, y));
      if (s < best) { best = s; destX = x; destY = y; }
    }
  }
  if (sel == rescuer && captivePending() && best != 0) { closestTo(capX, capY); return; }
  // nothing to hit: drop by an unvisited village we can reach
  if (best >= 1000 && botVisits && !(units[sel].flags & F_LORD))
    for (uint8_t v = 0; v < nVill; v++)
      if (!(villDone & (1 << v)) && reachable(idx(villX[v], villY[v])) && canStand(villX[v], villY[v], sel)) {
        destX = villX[v]; destY = villY[v]; destFor = sel; return;
      }
  // seize when the lord can reach the objective and the boss is gone
  if ((units[sel].flags & F_LORD) && objType == OBJ_SEIZE && reachable(idx(objX, objY)) && canStand(objX, objY, sel) && !anyAlive(true, F_BOSS)) {
    destX = objX; destY = objY;
  }
  destFor = sel;
}
int healSlot(uint8_t u) {
  for (int s = 0; s < ITEM_SLOTS; s++) if (units[u].item[s] && kindOf(units[u].item[s]) == K_HEAL) return s;
  return -1;
}
uint8_t pressFor(uint8_t want, uint8_t cur) { return cur < want ? DOWN_BUTTON : cur > want ? UP_BUTTON : A_BUTTON; }

uint8_t botInput() {
  switch (state) {
    case ST_TITLE: return A_BUTTON;
    case ST_BANNER: return wantShots ? 0 : A_BUTTON;
    case ST_CHAPTER: return A_BUTTON;
    case ST_TALK: return (wantShots && talkShown < talkLen) ? 0 : A_BUTTON;
    case ST_SELECT:
      if (hunter == NONE || !fighter(hunter)) {           // the fastest non-lord hero hunts chests
        int best = -1; hunter = NONE;
        for (uint8_t h = 1; h < NUM_HEROES; h++)
          if (fighter(h) && h != rescuer && units[h].cls != C_CLERIC) { int t = units[h].spd * 4 + units[h].hp; if (t > best) { best = t; hunter = h; } }
      }
      if (rescuer == NONE && captivePending() && botRescues) {        // pick the nearest non-lord hero
        int best = -1;                                   // the sturdiest non-lord hero
        for (uint8_t h = 1; h < NUM_HEROES; h++)
          if (fighter(h) && units[h].cls != C_CLERIC) { int t = units[h].maxHp + units[h].def * 3; if (t > best) { best = t; rescuer = h; } }
      }
      for (uint8_t k = 1; k <= NUM_HEROES; k++) { uint8_t i = k % NUM_HEROES;   // lord (0) goes last
        if (fighter(i) && !(units[i].flags & F_MOVED)) { uint8_t b = stepToward(units[i].x, units[i].y); return b ? b : A_BUTTON; } }
      return 0;
    case ST_DEPLOY: {                                    // half the time, swap the first pick for the bench hero
      static uint8_t step; static uint8_t lastTurnSeen = 255;
      if (lastTurnSeen != chapter) { lastTurnSeen = chapter; step = botRescues ? 2 : 0; tradeStep = botTrades ? 0 : 99; }
      if (tradeStep < 99 && rosterCount() >= 2) { uint8_t w = rosterCount(); if (menuSel != w) return menuSel < w ? DOWN_BUTTON : UP_BUTTON; return A_BUTTON; }
      uint8_t want = rosterCount() + 1 + hasShop();      // BEGIN
      if (step == 0) want = 1;
      if (step == 1) for (uint8_t r = 1; r < rosterCount(); r++) { uint8_t h = rosterAt(r); if (!(deployMask & (1 << h))) { want = r; break; } }
      if (menuSel != want) return menuSel < want ? DOWN_BUTTON : UP_BUTTON;
      step++;
      return A_BUTTON;
    }
    case ST_MOVE: {
      if (destFor != sel) {
        wantHeal = healSlot(sel) >= 0 && units[sel].hp * 5 < units[sel].maxHp * 2;   // under 40%: drink, stay put
        if (wantHeal) { destX = units[sel].x; destY = units[sel].y; destFor = sel; } else pickDest();
      }
      uint8_t b = stepToward(destX, destY); return b ? b : A_BUTTON;
    }
    case ST_MENU: {
      destFor = NONE;
      if (sel == NONE) return pressFor(0, menuSel);                    // END TURN menu: end it
      int want = -1;
      for (int i = 0; i < menuN; i++) if (menuOpt[i] == OPT_SEIZE) want = i;
      if (want < 0) for (int i = 0; i < menuN; i++) if (menuOpt[i] == OPT_TALK) want = i;
      if (want < 0) for (int i = 0; i < menuN; i++) if (menuOpt[i] == OPT_HEAL) want = i;
      if (want < 0) for (int i = 0; i < menuN; i++) if (menuOpt[i] == OPT_VISIT) want = i;
      if (want < 0 && wantHeal) for (int i = 0; i < menuN; i++) if (menuOpt[i] == OPT_ITEM) want = i;
      if (want < 0) for (int i = 0; i < menuN; i++) if (menuOpt[i] == OPT_ATTACK) want = i;
      if (want < 0) for (int i = 0; i < menuN; i++) if (menuOpt[i] == OPT_WAIT) want = i;
      return pressFor(want, menuSel);
    }
    case ST_TRADE: {
      // a scripted trade: cycle the right-hand hero once, hand the lord's last item to them,
      // then give it back via the empty slot, then leave
      static const uint8_t col[] = { 1, 0, 1, 1, 0, 0 }, row[] = { 0, 2, 4, 1, 4, 0 };
      if (tradeStep >= 6) return B_BUTTON;
      if (trCol != col[tradeStep]) return RIGHT_BUTTON;
      uint8_t r = tradeStep == 3 ? itemCount(trHero[1]) : row[tradeStep];   // step 3: take back what was given
      if (trRow != r) return trRow < r ? DOWN_BUTTON : UP_BUTTON;
      tradeStep++;
      return A_BUTTON;
    }
    case ST_ITEMS: { int h = healSlot(sel); return h < 0 ? B_BUTTON : pressFor(h, menuSel); }
    case ST_TARGET: case ST_FORECAST: return A_BUTTON;
    case ST_DANGER: return B_BUTTON;
    case ST_EXP: case ST_LEVELUP: return (wantShots && xT < 75) ? 0 : A_BUTTON;
    case ST_END: case ST_EPILOGUE: return A_BUTTON;
    case ST_STATUS: return B_BUTTON;
    default: return 0;
  }
}

// ------------------------------------------------------------- invariants
int reservesLeft() { int n = 0; for (uint8_t i = NUM_HEROES; i < nUnits; i++) if ((units[i].flags & F_RESERVE) && (units[i].flags & F_ENEMY)) n++; return n; }
// every one-shot event marked done must have done its job
bool eventsConsistent(std::string& why) {
  if (state == ST_CHAPTER || state == ST_TITLE) return true;
  for (uint8_t i = 0; i < nEv; i++) {
    if (!(evDone >> i & 1)) continue;
    uint8_t e[EV_SIZE]; FX::readDataBytes(chAddr + CH_EVENTS + i * EV_SIZE, e, EV_SIZE);
    if (e[3] & A_WAKE)
      for (uint8_t k = NUM_HEROES; k < nUnits; k++)
        if (units[k].exp == e[4] && (units[k].flags & F_RESERVE)) { why = "a group slept through its wake event"; return false; }
    if ((e[3] & A_SET) && !flagSet(e[5])) { why = "an event's flag isn't set"; return false; }
  }
  return true;
}
uint8_t resX[MAX_UNITS], resY[MAX_UNITS];          // where each reserve was posted
int itemTally[64];
void tally(int* t) { memset(t, 0, sizeof(int) * 64); for (int h = 0; h < WAGON + WAGON_PAGES; h++) for (int s = 0; s < ITEM_SLOTS; s++) if (slotItem(h, s)) t[slotItem(h, s)] += slotUses(h, s) & 0x7F; }
bool checkInvariants(std::string& why) {
  std::set<int> occ;
  for (uint8_t i = 0; i < nUnits; i++) {
    Unit& u = units[i];
    for (int s = 0; s < ITEM_SLOTS; s++) {
      if (u.item[s] >= NUM_ITEMS) { why = "bad item id"; return false; }
      if (!u.item[s]) { if (u.uses[s]) { why = "uses on an empty slot"; return false; } continue; }
      if (s > 0 && !u.item[s - 1]) { why = "gap in inventory"; return false; }
      uint8_t w = u.uses[s] & 0x7F;
      if (!w || w > idef(u.item[s]).uses) { why = "item uses out of range"; return false; }
    }
    if (i >= NUM_HEROES && (u.flags & F_HIDDEN) && alive(i)) { why = "an ambusher both hidden and on the field"; return false; }
    if (i >= NUM_HEROES && state != ST_CHAPTER && (u.flags & F_RESERVE) && (u.x != resX[i] || u.y != resY[i])) { why = "a sleeping reserve moved"; printf("unit %d flags %d at %d,%d posted %d,%d state %d\n", i, u.flags, u.x, u.y, resX[i], resY[i], state); return false; }
    if (!alive(i)) continue;
    if ((u.hp == 0 && state != ST_BATTLE) || u.hp > u.maxHp) { why = "bad hp"; return false; }
    if (u.exp >= 100 || u.lv < 1 || u.lv > 20) { why = "bad exp/level"; return false; }
    if (u.x >= MAP_W || u.y >= MAP_H || tCost(terrainAt(u.x, u.y)) == NO_PASS) { why = "unit off the walkable map"; return false; }
    if (!occ.insert(u.y * MAP_W + u.x).second) { why = "two units on one tile"; return false; }
    if (i >= NUM_HEROES && !isEnemy(i)) { why = "enemy slot without F_ENEMY"; return false; }
    if (i < NUM_HEROES && isEnemy(i)) { why = "hero slot flagged as enemy"; return false; }
    if (i < NUM_HEROES && !joined(i) && i != capId) { why = "an unjoined hero on the field who isn't the captive"; return false; }
  }
  if (captivePending() && capLeaves && turn > capLeaves && state != ST_TALK) { why = "captive still here after her deadline"; return false; }
  return true;
}

struct Stats { long games = 0, cleared[NUM_CHAPTERS] = {}, reached[NUM_CHAPTERS] = {}, retries = 0, retryOk = 0,
               battles = 0, walks = 0, levelups = 0, heals = 0, drops = 0, broke = 0, carryOk = 0, stuck = 0,
               deployScreens = 0, swaps = 0, recruited = 0, taken = 0, visits = 0, gifts = 0, staffHeals = 0, healedHp = 0,
               marenFought = 0, resets = 0, trades = 0, alarms = 0, ledgers = 0, ambushes = 0, demons = 0, vulnFx = 0, tripBefore = 0, noRoom = 0, passedOn = 0, fullCh2 = 0, fullCh2Clear = 0, fullCh2Rescue = 0; int topLv = 0; };


// ------------------------------------------------------------- scenario tests
// TESTS=1 FXDATA=test_fxdata.bin ./sim   (python3 make_test_fx.py builds the data from test_chapters/)
static int passes, fails;
#define CHECK(c, msg) do { if (c) passes++; else { printf("FAIL line %d: %s\n", __LINE__, msg); fails++; } } while (0)
void pump(int n, uint8_t in = 0) {                   // with input: a release frame first, then press / release
  if (in) { ab.nextInput = 0; loop(); }
  for (int i = 0; i < n; i++) { ab.nextInput = (in && !(i & 1)) ? in : 0; loop(); sndTick(); }
}
bool pumpUntil(State st, uint8_t in, int max = 3000) { for (int i = 0; i < max && state != st; i++) pump(1, (i & 1) ? 0 : in); return state == st; }
void enter(uint8_t ch) {
  memset(simEeprom(), 0xFF, 1024); setup();
  loadHeroes(); storyFlags = 0;
  loadChapter(ch); placeArmy(); state = ST_SELECT; turn = 1;
}
void selectUnit(uint8_t u) { sel = u; origX = units[u].x; origY = units[u].y; computeReach(u); }
uint8_t itemId(const char* name) { for (uint8_t i = 0; i < NUM_ITEMS; i++) if (!strcmp(str(STR_ITEM + i), name)) return i; return 0; }
int menuIndexOf(uint8_t opt) { for (int i = 0; i < menuN; i++) if (menuOpt[i] == opt) return i; return -1; }

int runTests() {
  wantShots = getenv("SHOTS") != nullptr;
  // ---- portal + tripwire + chest flag + branching (test chapter 1)
  enter(0);
  selectUnit(0); state = ST_MOVE; curX = 4; curY = 7; pump(1); savePBM("t_warp_before");
  startWalk(0, 4, 7, ST_MENU); pumpUntil(ST_MENU, 0);
  CHECK(units[0].x == 18 && units[0].y == 7, "stopping on the portal warps to 18,7");
  pump(1); savePBM("t_warp_after");
  pump(2, B_BUTTON);
  CHECK(state == ST_MOVE && units[0].x == 2 && units[0].y == 7, "B after a warp undoes the move");
  startWalk(0, 4, 7, ST_MENU); pumpUntil(ST_MENU, 0);
  CHECK(units[0].x == 18 && units[0].y == 7, "the portal works a second time");
  pump(2, B_BUTTON);
  state = ST_SELECT; sel = NONE;
  selectUnit(0); startWalk(0, 3, 7, ST_MENU); pumpUntil(ST_MENU, 0);
  CHECK(units[0].x == 3, "walking past (not onto) the portal does nothing");
  state = ST_SELECT; sel = NONE;
  units[1].x = 6; units[1].y = 5; selectUnit(1); startWalk(1, 6, 2, ST_MENU);
  pumpUntil(ST_TALK, 0, 50);
  CHECK(state == ST_TALK, "walking over the tripwire plays its scene");
  CHECK(reservesLeft() == 0, "...and wakes group 1");
  CHECK(pumpUntil(ST_MENU, A_BUTTON), "...then the unit menu opens");
  state = ST_SELECT; sel = NONE;
  units[2].x = 3; units[2].y = 12; sel = 2; visit(villageAt(3, 12));
  CHECK(flagSet(2), "opening the chest sets flag 2");
  CHECK(nextChapter(2) == 3, "flag 2 set: chapter 3X (index 3) follows");
  storyFlags = 0;
  CHECK(nextChapter(2) == 4, "flag 2 clear: 3X is skipped");

  // ---- conditional scene lines (the tripwire scene of test chapter 1)
  auto speakers = [](void) {                             // play the scene, list who spoke
    std::string who; uint32_t at = 0; startTalk(SC_EVENT + 1, ST_SELECT);
    for (int i = 0; i < 200 && state == ST_TALK; i++) {
      if (talkAddr != at) { at = talkAddr; who += char('0' + talkWho); }
      ab.nextInput = (i & 1) ? 0 : A_BUTTON; loop();
    }
    return who;
  };
  enter(0); storyFlags = 0;
  enter(0); storyFlags = 0; std::string a1 = speakers();
  enter(0); setFlag(2); units[4].flags |= F_JOINED; std::string a2 = speakers();
  enter(0); setFlag(2); units[2].flags |= F_JOINED | F_FALLEN; std::string a3 = speakers();
  CHECK(a1 == "0235", "no flag, no Maren: Brannock, Hale, Wren speak (and Oren: Hale didn't fall)");
  CHECK(a2 == "01465", "flag 2 and Maren: Brannock, Idra, Maren, and Cass on [2][MAREN]");
  CHECK(a3 == "0137", "flag 2, no Maren, Hale fallen: [2][MAREN] skipped, [FELL:HALE] speaks");
  if (a1 != "0235" || a2 != "01465" || a3 != "0137") printf("  got '%s' '%s' '%s'\n", a1.c_str(), a2.c_str(), a3.c_str());

  // ---- talk-to-recruit (test chapter 2)
  enter(1);
  uint8_t M = NUM_HEROES;                               // Maren, fighting for the enemy
  CHECK(isEnemy(M) && units[M].name == 4, "Maren starts as an enemy");
  units[1].x = 6; units[1].y = 7;                       // Idra beside her: wrong hero
  CHECK(talkEvent(1) == NONE, "only Brannock can talk her round");
  units[0].x = 4; units[0].y = 7; selectUnit(0); openUnitMenu();
  int ti = menuIndexOf(OPT_TALK);
  CHECK(ti >= 0, "TALK appears for Brannock next to her");
  menuSel = ti; pump(2, A_BUTTON);
  CHECK(state == ST_TALK, "TALK plays the recruit scene");
  pump(1); savePBM("t_talk_menu_scene");
  pumpUntil(ST_SELECT, A_BUTTON);
  CHECK(joined(4) && alive(4) && !isEnemy(4) && units[4].x == 5 && units[4].y == 7, "Maren joins where she stood");
  CHECK(units[4].lv == 3 && units[4].maxHp == 18 && !(units[4].uses[1] & USES_DROP), "...with her enemy-side stats, drop marks cleared");
  CHECK(units[M].flags == 0 && unitAt(5, 7) == 4, "...and her enemy slot is gone");
  CHECK(units[0].flags & F_MOVED, "talking ends Brannock's turn");

  // ---- spawns, repeats, until, falls (test chapter 3)
  enter(2);
  uint8_t B = NUM_HEROES, K = NUM_HEROES + 1, D1 = NUM_HEROES + 2, D2 = NUM_HEROES + 3;
  CHECK(!alive(D1) && (units[D1].flags & F_HIDDEN) && unitAt(12, 3) == NONE, "ambushers start off the map");
  units[B].flags &= ~F_ALIVE; units[B].hp = 0;
  turn = 2; turnEvents();
  CHECK(alive(B) && units[B].x == 15 && units[B].y == 7 && units[B].hp == 15, "turn 2: the group-2 bandit respawns fresh at its post");
  units[1].x = 12; units[1].y = 3;                      // a hero standing on an ambusher's post
  turn = 3; bool sc = turnEvents();
  CHECK(alive(D1) && alive(D2) && !(units[D1].flags & F_HIDDEN), "turn 3: the Drowned rise");
  CHECK(manhattan(units[D1].x, units[D1].y, 12, 3) == 1, "...beside an occupied post");
  CHECK(sc && state == ST_TALK && musicOn, "...with their scene and the demon motif");
  state = ST_SELECT; curX = 12; curY = 4; clampCam(); pump(1); savePBM("t_risen");
  units[B].flags &= ~F_ALIVE;
  units[K].hp = 0; units[K].flags &= ~F_ALIVE; fallEvents(K);
  CHECK(flagSet(17), "Kell falling sets local flag 17");
  turn = 4; turnEvents();
  CHECK(!alive(B), "'until 17' stops the reinforcements");
  enter(2);
  CHECK(!flagSet(17), "local flags reset with the chapter");
  for (uint8_t i = NUM_HEROES; i < nUnits; i++) if (i != D1 && i != D2) units[i].flags &= ~F_ALIVE;
  objType = OBJ_ROUT; state = ST_SELECT;
  CHECK(!checkEnd(), "rout waits for ambushers still to come");

  // ---- joins: (test chapter 4)
  loadHeroes();
  CHECK(!joined(4), "Maren starts outside the army");
  loadChapter(3);
  CHECK(joined(4) && (deployMask & bit16(4)), "'joins: MAREN' adds her and deploys her");
  loadHeroes(); units[4].flags |= F_FALLEN;
  loadChapter(3);
  CHECK(!joined(4), "...but not if she has fallen");

  // ---- promotion
  enter(0);
  uint8_t crest = itemId("HERO CREST");
  CHECK(crest != 0, "HERO CREST exists");
  units[0].lv = 5; giveItem(0, crest, 1);
  uint8_t slot = itemCount(0) - 1;
  sel = 0; state = ST_ITEMS; menuSel = slot; pump(2, A_BUTTON);
  CHECK(state == ST_ITEMS && units[0].cls == C_LORD, "a crest does nothing below level 10");
  Unit before = units[0];
  units[0].lv = 10; pump(2, A_BUTTON);
  CHECK(state == ST_LEVELUP && promoting, "at level 10 the crest promotes");
  for (int i = 0; i < 80; i++) { ab.nextInput = 0; loop(); }
  savePBM("t_promote");
  CHECK(units[0].cls == C_HIGH_LORD && units[0].lv == 1 && units[0].exp == 0, "...to HIGH LORD, level 1");
  CHECK(units[0].maxHp == before.maxHp + 4 && units[0].str == before.str + 2 && units[0].def == before.def + 2, "...with the class bonuses");
  CHECK(itemCount(0) == slot, "...and the crest is used up");
  CHECK(canWield(0, K_SWORD) && canWield(0, K_LANCE) && !canWield(0, K_AXE), "HIGH LORD wields swords and lances");
  pumpUntil(ST_SELECT, A_BUTTON);
  CHECK(units[0].flags & F_MOVED, "promoting ends the turn");
  units[0].lv = 12; units[0].flags &= ~F_MOVED; giveItem(0, crest, 1);
  sel = 0; state = ST_ITEMS; menuSel = itemCount(0) - 1; pump(2, A_BUTTON);
  CHECK(state == ST_ITEMS && units[0].cls == C_HIGH_LORD, "a promoted class can't promote again");
  // EXP: a promoted hero earns less from the same foe
  enter(0);
  units[0].lv = 5; bA = 0; bD = NUM_HEROES; units[bD].lv = 5; units[bD].hp = 0; bHeroHit = true; startExp(); uint8_t base = xGain;
  enter(0);
  units[0].lv = 5; units[0].cls = C_HIGH_LORD; bA = 0; bD = NUM_HEROES; units[bD].lv = 5; units[bD].hp = 0; bHeroHit = true; startExp();
  CHECK(xGain < base, "promoted heroes count as 10 levels higher for EXP");

  // ---- title screen (shown when there's a save)
  enter(0); saveGame(); state = ST_TITLE; titleSel = 0; pump(1);
  CHECK(state == ST_TITLE, "with a save, the title screen waits for a choice");
  savePBM("t_title");
  // ---- game menu: RESTART and QUIT ask first
  enter(0); saveGame(); state = ST_SELECT; sel = NONE;
  curX = 10; curY = 10; clampCam(); units[1].lv = 9;              // a change the restart must undo
  pump(2, A_BUTTON);
  CHECK(state == ST_MENU && menuN == 5 && menuOpt[1] == OPT_ARMY && menuOpt[3] == OPT_RESTART && menuOpt[4] == OPT_QUIT,
        "empty tile: END TURN, ARMY, SOUND, RESTART, QUIT");
  menuSel = 3; pump(2, A_BUTTON);
  CHECK(state == ST_MENU && confirmRow == 3, "RESTART first asks SURE?");
  pump(1); savePBM("t_menu_sure");
  pump(2, DOWN_BUTTON);
  CHECK(confirmRow == NONE, "moving off it cancels");
  menuSel = 3; pump(2, A_BUTTON); pump(2, A_BUTTON);
  CHECK(state == ST_CHAPTER && units[1].lv == 1, "RESTART twice: the chapter starts over from its save");
  musicStop();
  enter(0); saveGame(); state = ST_SELECT; sel = NONE; curX = 10; curY = 10; clampCam();
  pump(2, A_BUTTON); menuSel = 4; pump(2, A_BUTTON); pump(2, A_BUTTON);
  CHECK(state == ST_TITLE, "QUIT twice: back to the title");
  // ---- weapons never wear out; vulneraries do
  enter(0);
  { uint8_t w = units[0].uses[0]; bA = 0; bD = NUM_HEROES; units[bD].hp = 99; forecast(0, bD, fA); forecast(bD, 0, fD);
    bStriker = 0; applyStrike(); CHECK(units[0].uses[0] == w && units[0].item[0], "a swing costs the weapon nothing"); }

  // ---- 'needs N' / 'needs !N', and a repeating event's scene plays once (test chapter 1)
  enter(0); storyFlags = 0;
  selectUnit(0); startWalk(0, 4, 2, ST_MENU); pumpUntil(ST_TALK, 0, 60);
  CHECK(state == ST_TALK && units[0].x == 4 && units[0].y == 2, "flag 5 clear: the door is locked (its scene, no warp)");
  pumpUntil(ST_MENU, A_BUTTON); pump(2, B_BUTTON); state = ST_SELECT; sel = NONE;
  setFlag(5);
  selectUnit(0); startWalk(0, 4, 2, ST_MENU); pumpUntil(ST_TALK, 0, 60);
  CHECK(units[0].x == 18 && units[0].y == 2 && state == ST_TALK, "flag 5 set: the key warps, with its scene");
  pumpUntil(ST_MENU, A_BUTTON); pump(2, B_BUTTON); state = ST_SELECT; sel = NONE;
  selectUnit(0); startWalk(0, 4, 2, ST_MENU); pumpUntil(ST_MENU, 0, 60);
  CHECK(units[0].x == 18 && units[0].y == 2, "...again, every time");
  CHECK(state == ST_MENU, "...but its scene only plays the first time");

  // ---- magic: tomes reach 1-2, count half the target's DEF, and the caster doesn't dash in
  enter(0);
  { uint8_t fire = itemId("FIRE"), staff = itemId("HEAL"), sword = itemId("IRON SWORD");
    CHECK(fire && staff && sword, "FIRE, HEAL, IRON SWORD exist");
    uint8_t m = 1, e = NUM_HEROES;
    units[m].cls = C_MAGE; memset(units[m].item, 0, ITEM_SLOTS); memset(units[m].uses, 0, ITEM_SLOTS); giveItem(m, fire, 1);
    units[m].str = 8; units[m].x = 12; units[m].y = 7;
    units[e].x = 14; units[e].y = 7; units[e].def = 10; units[e].flags &= ~F_RESERVE;
    uint8_t lo, hi; rangeOf(m, lo, hi);
    CHECK(lo == 1 && hi == 2, "a tome strikes at range 1-2");
    Fc f; forecast(m, e, f);
    CHECK(f.can && f.dmg == 8 + 5 - 5, "magic counts half the target's DEF");
    units[m].cls = C_LORD; units[m].item[0] = sword; forecast(m, e, f); units[m].x = 13; forecast(m, e, f);
    CHECK(f.dmg == 8 + 5 + 1 - 10, "...a sword counts all of it (+1: sword beats his axe)");
    units[m].cls = C_MAGE; units[m].item[0] = fire; units[m].x = 13; units[m].name = 7;   // drawn as Aelis
    startBattle(m, e, ST_SELECT); bool dashed = false, spellHeard = false;
    for (int i = 0; i < 400 && state == ST_BATTLE; i++) { if (advance(m)) dashed = true; if (bStriker == m && casts(bStriker) && timer > IMPACT) spellHeard = true;
      if (bStriker == m && timer == IMPACT + 6) savePBM("t_spell_flight"); if (bStriker == m && timer == IMPACT - 6) savePBM("t_spell_burst");
      ab.nextInput = 0; loop(); }
    CHECK(!dashed && spellHeard, "a caster at range 1 stays put and casts");
    CHECK(sndHeard().count(S_SPELL), "...with the spell sound");
    // a sage: attacks with the tome, heals with the staff, whatever order they're carried in
    enter(0);
    units[1].cls = C_SAGE; memset(units[1].item, 0, ITEM_SLOTS); memset(units[1].uses, 0, ITEM_SLOTS);
    giveItem(1, staff, 1); giveItem(1, fire, 1);
    units[1].x = 12; units[1].y = 7; units[2].x = 12; units[2].y = 6; units[2].hp = 5;
    units[NUM_HEROES].x = 13; units[NUM_HEROES].y = 7; units[NUM_HEROES].flags &= ~F_RESERVE;
    CHECK(equipSlot(1) == 1 && findSlot(1, true) == 0, "a sage's weapon is the tome, the staff stays a staff");
    sel = 1; openUnitMenu();
    CHECK(menuIndexOf(OPT_ATTACK) >= 0 && menuIndexOf(OPT_HEAL) >= 0, "...so it can ATTACK and HEAL from the same square");
    menuSel = menuIndexOf(OPT_HEAL); pump(2, A_BUTTON); pump(2, A_BUTTON);
    CHECK(state == ST_HEALFX && units[2].hp == 15, "...and the staff heals 10");
    // a cleric never counters
    enter(0); units[4].flags |= F_JOINED | F_ALIVE; units[4].x = 13; units[4].y = 7;
    units[NUM_HEROES].x = 14; units[NUM_HEROES].y = 7;
    { Fc g; forecast(4, NUM_HEROES, g); CHECK(!g.can, "a cleric can't strike back"); }
  }

  // ---- menus wrap: UP from the top lands on the bottom
  enter(0); state = ST_SELECT; sel = NONE; curX = 10; curY = 10; clampCam();
  pump(2, A_BUTTON);
  CHECK(state == ST_MENU && menuSel == 0, "game menu opens on END TURN");
  pump(2, UP_BUTTON);
  CHECK(menuSel == menuN - 1, "UP from the top wraps to the bottom");
  pump(2, DOWN_BUTTON);
  CHECK(menuSel == 0, "...and DOWN from the bottom back to the top");

  // ---- trade: THROW AWAY the item in hand (A, then A again)
  enter(0); state = ST_DEPLOY;
  { uint8_t before = itemCount(0);
    trHero[0] = 0; trHero[1] = 1; trCol = 0; trRow = 1; trHeld = confirmRow = NONE; state = ST_TRADE;
    pump(2, A_BUTTON);                                  // pick up Brannock's first item
    CHECK(trHeld == 0, "A picks the item up");
    for (int i = 0; i < ITEM_SLOTS; i++) pump(2, DOWN_BUTTON);
    CHECK(trRow == ITEM_SLOTS + 1, "the THROW AWAY row is under the slots");
    pump(1); savePBM("t_trade_bin");
    pump(2, A_BUTTON);
    CHECK(itemCount(0) == before && confirmRow == trRow, "first A asks SURE?");
    pump(1); savePBM("t_trade_sure");
    pump(2, A_BUTTON);
    CHECK(itemCount(0) == before - 1 && trHeld == NONE, "second A throws it away");
    pump(2, DOWN_BUTTON);
    CHECK(trRow == 0, "...and the trade rows wrap too"); }

  // ---- last words: a hero who falls says so before the game moves on
  enter(0);
  { uint8_t e = NUM_HEROES; units[e].flags &= ~F_RESERVE; units[e].str = 40; units[e].skl = 40; units[e].x = 13; units[e].y = 7;
    units[1].x = 12; units[1].y = 7; units[1].hp = 1; units[1].spd = 0;
    state = ST_ENEMY; startBattle(e, 1, ST_ENEMY);
    bool spoke = false;
    for (int i = 0; i < 600 && !spoke; i++) { if (state == ST_TALK) spoke = true; else { ab.nextInput = 0; loop(); } }
    CHECK(spoke && talkWho == 1 && (units[1].flags & F_FALLEN), "Idra falls and says her last words");
    pump(1); savePBM("t_last_words");
    pumpUntil(ST_ENEMY, A_BUTTON, 400);
    CHECK(state == ST_ENEMY, "...then the enemy phase carries on"); }

  // ---- chapter end: ONWARD or RESTART (the save is only replaced going ONWARD)
  enter(0); saveGame(); units[2].lv = 9; deployMask |= 2; units[1].flags |= F_FALLEN; units[1].flags &= ~F_ALIVE;
  endChapter(true); pumpUntil(ST_END, A_BUTTON, 400);
  CHECK(state == ST_END && titleSel == 0, "a win ends on the clear screen, ONWARD picked");
  pump(1); savePBM("t_clear_screen");
  pump(2, DOWN_BUTTON); pump(2, A_BUTTON);
  CHECK(state == ST_CHAPTER && chapter == 0 && units[2].lv == 1 && !(units[1].flags & F_FALLEN), "RESTART replays the chapter from its save");
  enter(0); saveGame(); endChapter(true); pumpUntil(ST_END, A_BUTTON, 400); pump(2, A_BUTTON);
  CHECK(state == ST_CHAPTER && chapter == nextChapter(0), "ONWARD goes to the next chapter");

  // ---- the minimap: hold B; a tap still jumps to the next hero
  enter(0); state = ST_SELECT; sel = NONE; curX = 10; curY = 10; clampCam();
  for (int i = 0; i < 12; i++) { ab.nextInput = B_BUTTON; loop(); }
  CHECK(bHold >= MINIMAP_HOLD, "holding B shows the whole map");
  savePBM("t_minimap");
  ab.nextInput = 0; loop();
  CHECK(curX == 10 && curY == 10, "...and letting go doesn't move the cursor");
  pump(2, B_BUTTON);
  CHECK(unitAt(curX, curY) != NONE && !isEnemy(unitAt(curX, curY)), "a tap of B jumps to a hero");

  // ---- durability: fine weapons wear out, common ones never do
  enter(0);
  { uint8_t silver = itemId("SILVER SWORD"), iron = itemId("IRON SWORD"), e = NUM_HEROES;
    CHECK(consumable(silver) && !consumable(iron), "silver counts its uses, iron doesn't");
    units[0].item[0] = silver; units[0].uses[0] = 2;
    units[e].hp = 99; units[e].x = units[0].x + 1; units[e].y = units[0].y;
    bA = 0; bD = e; bBroke = NONE; forecast(0, e, fA); forecast(e, 0, fD);
    bStriker = 0; applyStrike();
    CHECK(units[0].item[0] == silver && (units[0].uses[0] & 0x7F) == 1, "a swing costs a silver sword one use");
    applyStrike();
    CHECK(units[0].item[0] != silver && bBroke == 0 && !fA.can, "...the last use breaks it, and its wielder stops striking");
    pump(1); state = ST_BATTLE; timer = 20; bStriker = NONE; drawBattle(); FX::display(true); savePBM("t_broke");
    units[0].item[0] = iron; units[0].uses[0] = 1; bBroke = NONE; forecast(0, e, fA);
    bA = 0; bD = e; bStriker = 0;
    for (int i = 0; i < 30; i++) applyStrike();
    CHECK(units[0].item[0] == iron, "an iron sword never breaks"); }

  // ---- the supply wagon: overflow lands there, it trades like a hero, and a restart puts it back
  enter(0); copyWagon(0, WAGON_LIVE);
  { uint8_t vul = itemId("VULNERARY"), jav = itemId("JAVELIN");
    for (int k = 0; k < ITEM_SLOTS; k++) { units[1].item[k] = jav; units[1].uses[k] = 1; }
    for (int h = 0; h < NUM_HEROES; h++) if (h != 1) units[h].flags &= ~F_JOINED;
    uint8_t to = stash(1, vul, 3);
    CHECK(to == WAGON && slotItem(WAGON, 0) == vul && slotUses(WAGON, 0) == 3, "a full pack sends the find to the wagon");
    for (int h = 0; h < NUM_HEROES; h++) if (h < 4) units[h].flags |= F_JOINED;
    uint8_t h = 0, seen = 0; for (int i = 0; i < 20; i++) { h = nextHero(h, NONE); if (h >= WAGON) seen++; }
    CHECK(seen >= WAGON_PAGES, "the trade table cycles through the wagon's pages");
    trHero[0] = 1; trHero[1] = WAGON; trCol = 0; trRow = 1; trHeld = confirmRow = NONE; state = ST_TRADE;
    pump(2, A_BUTTON); pump(2, RIGHT_BUTTON); pump(2, DOWN_BUTTON); pump(2, A_BUTTON);
    CHECK(slotItem(WAGON, 1) == jav && itemCount(1) == 3, "a javelin handed to the wagon");
    pump(1); savePBM("t_wagon");
    saveGame(); wagonPut(jav, 1);
    CHECK(slotItem(WAGON, 2) == jav, "something else goes in mid-chapter...");
    loadSave();
    CHECK(slotItem(WAGON, 2) == 0 && slotItem(WAGON, 1) == jav, "...and a restart takes the wagon back to the chapter's start"); }

  // ---- gold and the shop (test chapter index 4 is the real Chapter 4: a market and a purse)
  enter(0); copyWagon(0, WAGON_LIVE); loadChapter(4); placeArmy();
  CHECK(hasShop() && shopItem(0) == itemId("VULNERARY"), "Chapter 4 has a market");
  gold = 1000; state = ST_SHOP; shopSel = 0; pump(2, A_BUTTON);
  CHECK(gold == 700 && slotItem(WAGON, 0) == itemId("VULNERARY") && slotUses(WAGON, 0) == 3, "a vulnerary for 300, into the wagon");
  pump(1); savePBM("t_shop");
  gold = 100; pump(2, A_BUTTON);
  CHECK(gold == 100 && !slotItem(WAGON, 1), "not enough gold: no sale");
  pump(2, B_BUTTON);
  CHECK(state == ST_DEPLOY, "B leaves the shop");
  { uint8_t v = 0; while (v < nVill && !(FX::readIndexedUInt8(chAddr + CH_VILL, v * 4 + 2) & 0x80)) v++;
    CHECK(v < nVill, "Chapter 4 has a village that gives gold");
    uint16_t g0 = gold; sel = 1; units[1].x = villX[v]; units[1].y = villY[v]; visit(v);
    CHECK(gold > g0 && gotItem == GOT_GOLD, "visiting it fills the purse");
    pumpUntil(ST_GOT, A_BUTTON, 200); pump(1); savePBM("t_gold_gift"); }
  { uint16_t g0 = gold; endChapter(true); pumpUntil(ST_END, A_BUTTON, 400); uint8_t was = chapter; pump(2, A_BUTTON);
    CHECK(chapter == nextChapter(was) && gold == g0 + FX::readIndexedUInt8(chapterAddr(was), CH_REWARD) * 10, "ONWARD pays the chapter's reward"); }

  // ---- crits find the gap: never less than STR + might, whatever the armour
  enter(0);
  { uint8_t e = NUM_HEROES; units[0].str = 10; units[0].item[0] = itemId("IRON SWORD"); units[0].uses[0] = 1;
    units[e].def = 20; units[e].hp = 99; units[e].x = units[0].x + 1; units[e].y = units[0].y; units[e].flags &= ~F_RESERVE;
    forecast(0, e, fA); forecast(e, 0, fD);
    CHECK(fA.dmg == 0 && fA.pierce == 15, "against DEF 20 a plain hit does 0, a crit's floor is 15");
    fA.crit = 100; fA.hit = 100; bA = 0; bD = e; bStriker = 0; bBroke = NONE; uint8_t hp = units[e].hp; applyStrike();
    CHECK(units[e].hp == hp - 15 && bKind == 3, "...and a crit lands for 15");
    units[e].def = 0; forecast(0, e, fA); fA.crit = 100; fA.hit = 100; hp = units[e].hp; uint8_t d3 = fA.dmg * 3; applyStrike();
    CHECK(d3 > 15 && units[e].hp == hp - d3, "against no armour a crit is still triple"); }

  // ---- staves: the weakest one that heals the wound, the strongest named in the menu
  enter(0);
  { uint8_t heal = itemId("HEAL"), mend = itemId("MEND"), m = 4;
    CHECK(heal && mend, "HEAL and MEND staves exist");
    memset(units[m].item, 0, ITEM_SLOTS); memset(units[m].uses, 0, ITEM_SLOTS);
    giveItem(m, mend, 15); giveItem(m, heal, 1);
    CHECK(units[m].item[staffFor(m, 6)] == heal, "a 6 HP scratch gets the HEAL staff");
    CHECK(units[m].item[staffFor(m, 15)] == mend, "a 15 HP wound gets MEND");
    CHECK(units[m].item[staffFor(m, 40)] == mend, "a wound past both gets the strongest");
    sel = m; menuN = 1; menuOpt[0] = OPT_HEAL; confirmRow = NONE;
    CHECK(!strcmp(menuLabel(0), "MEND"), "the menu says MEND when she carries it");
    units[m].item[0] = heal; units[m].item[1] = 0;
    CHECK(!strcmp(menuLabel(0), "HEAL"), "...and HEAL when that's all she has"); }

  // ---- boosters: +2 for good through the level-up board; the boots add a square of move
  enter(0);
  { uint8_t ox = itemId("OX HORN"), root = itemId("HEART ROOT"), boots = itemId("WINGED BOOTS");
    CHECK(ox && root && boots, "OX HORN, HEART ROOT, WINGED BOOTS exist");
    uint8_t h = 2; Unit& H = units[h]; memset(H.item, 0, ITEM_SLOTS); memset(H.uses, 0, ITEM_SLOTS);
    giveItem(h, ox, 1); giveItem(h, root, 1); giveItem(h, boots, 1);
    uint8_t str0 = H.str, hp0 = H.maxHp, mv0 = clsMov(h), lv0 = H.lv;
    sel = h; state = ST_ITEMS; menuSel = 0; pump(2, A_BUTTON);
    CHECK(state == ST_LEVELUP && H.str == str0 + 2 && H.item[0] == root && H.lv == lv0, "OX HORN: +2 STR, used up, no level gained");
    pump(1); savePBM("t_booster");
    pumpUntil(ST_SELECT, A_BUTTON, 200);
    CHECK(H.flags & F_MOVED, "...and it takes the turn");
    H.flags &= ~F_MOVED; sel = h; state = ST_ITEMS; menuSel = 0; pump(2, A_BUTTON); pumpUntil(ST_SELECT, A_BUTTON, 200);
    CHECK(H.maxHp == hp0 + 5, "HEART ROOT: +5 max HP");
    H.flags &= ~F_MOVED; sel = h; state = ST_ITEMS; menuSel = 0; pump(2, A_BUTTON); pumpUntil(ST_SELECT, A_BUTTON, 200);
    CHECK(clsMov(h) == mv0 + 1 && !H.item[0], "WINGED BOOTS: +1 MOV");
    uint8_t e = NUM_HEROES; units[e].flags |= F_BOSS;
    CHECK(clsMov(e) == pgm_read_byte(&CLASS_ARMS[units[e].cls][2]), "...a boss's flag gives an enemy nothing");
    saveGame(); H.flags &= ~F_BOOTS; loadSave();
    CHECK(units[h].flags & F_BOOTS, "...and the boots are kept in the save"); }

  // ---- an older save (fewer heroes): the new ones start fresh, everyone else as saved
  enter(0);
  { units[1].lv = 7; saveGame();
    for (uint16_t a = SAVE_AT + 6 + 21 * 9; a < SAVE_AT + 6 + 21 * NUM_HEROES; a++) eeprom_update_byte((uint8_t*)a, 0xFF);
    memset(units, 0, sizeof(Unit) * NUM_HEROES); loadSave();
    bool fresh = true; for (uint8_t h = 9; h < NUM_HEROES; h++) if (units[h].name != h || joined(h)) fresh = false;
    CHECK(fresh && units[1].lv == 7, "a save from before the new heroes: they're added, the army kept"); }

  // ---- the status card: from a hero who has acted, an enemy's danger zone, ARMY, the roster
  enter(0);
  { units[1].flags |= F_MOVED; centerOn(units[1].x, units[1].y); state = ST_SELECT; pump(2, A_BUTTON);
    CHECK(state == ST_STATUS && stU == 1 && !stDeploy, "A on a hero who has acted: their card");
    pump(1); savePBM("t_status_hero");
    pump(2, RIGHT_BUTTON);
    CHECK(state == ST_STATUS && stU != 1 && stU < NUM_HEROES && alive(stU), "RIGHT: the next hero on the field");
    uint8_t seen = 0; for (int i = 0; i < 20; i++) { pump(2, RIGHT_BUTTON); if (isEnemy(stU) || !alive(stU)) seen++; }
    CHECK(!seen, "...never an enemy, never someone off the field");
    uint8_t last = stU; pump(2, B_BUTTON);
    CHECK(state == ST_SELECT && curX == units[last].x && curY == units[last].y, "B: back to the map, cursor on them");
    uint8_t e = NUM_HEROES; units[e].flags &= ~F_RESERVE;
    centerOn(units[e].x, units[e].y); state = ST_SELECT; pump(2, A_BUTTON);
    CHECK(state == ST_DANGER, "A on an enemy: its danger zone");
    pump(2, A_BUTTON);
    CHECK(state == ST_STATUS && stU == e, "A again: the enemy's card");
    pump(1); savePBM("t_status_enemy");
    for (int i = 0; i < 6; i++) { pump(2, LEFT_BUTTON); if (!isEnemy(stU) || !alive(stU)) seen++; }
    CHECK(!seen, "...LEFT/RIGHT stay with the living enemies");
    pump(2, A_BUTTON); centerOn(10, 10); state = ST_SELECT; pump(2, A_BUTTON);
    menuSel = menuIndexOf(OPT_ARMY); pump(2, A_BUTTON);
    CHECK(state == ST_STATUS && stU == 0, "game menu ARMY: the lord's card first");
    pump(2, B_BUTTON);
    state = ST_DEPLOY; menuSel = 1; pump(2, RIGHT_BUTTON);
    CHECK(state == ST_STATUS && stDeploy && stU == rosterAt(1), "deployment: RIGHT shows the hero's card");
    for (int i = 0; i < 20; i++) { pump(2, RIGHT_BUTTON); if (!available(stU)) seen++; }
    CHECK(!seen, "...and steps through the roster only");
    pump(2, B_BUTTON);
    CHECK(state == ST_DEPLOY, "B: back to the deployment list"); }

  // ---- the save is mirrored to the game's own FX block; EEPROM wiped by another game comes back
  enter(0);
  { units[1].lv = 11; storyFlags = 0x0105; gold = 1234; saveGame();
    CHECK(FX::save()[0] == SAVE_MAGIC && FX::save()[SAVE_LEN - 1] == eeprom_read_byte((const uint8_t*)(SAVE_AT + SAVE_LEN - 1)), "saving writes the FX copy");
    memset(simEeprom() + 16, 0x5A, 1000);                       // someone else's game
    restoreSave();
    CHECK(hasSave() && loadSave() == 0 && units[1].lv == 11 && storyFlags == 0x0105 && gold == 1234, "power-on: the FX copy puts the save back");
    FX::eraseSaveBlock(0); memset(simEeprom() + SAVE_AT, 0, SAVE_LEN); saveGame(); uint8_t keep = eeprom_read_byte((const uint8_t*)(SAVE_AT + 9));
    FX::eraseSaveBlock(0); restoreSave();
    CHECK(hasSave() && eeprom_read_byte((const uint8_t*)(SAVE_AT + 9)) == keep, "...a blank FX block leaves EEPROM alone (emulators without FX saves)"); }

  // ---- the record: turns add up on ONWARD only; side chapters and the fallen count
  enter(0);
  { eeprom_update_word((uint16_t*)TALLY, 0xFFFF);
    CHECK(tally() == 0, "a blank tally (an old save) reads 0");
    newGame(); CHECK(tally() == 0, "a new game starts the tally at 0");
    turn = 7; won = true; titleSel = 0; state = ST_END; objType = OBJ_ROUT; pump(2, A_BUTTON);
    CHECK(tally() == 7 && chapter == 1, "ONWARD adds the chapter's turns");
    startChapter(loadSave()); CHECK(tally() == 7, "...a restart doesn't");
    storyFlags = SIDE_FLAGS & (SIDE_FLAGS - 1);                  // all but the lowest side chapter's flag
    units[2].flags |= F_JOINED | F_FALLEN; units[2].exp = 0;
    turn = 3; objType = OBJ_ROUT; state = ST_EPILOGUE; pump(1); savePBM("t_record");
    CHECK(rankOf(0, 9999) == 4 && rankOf(80, 0) == 0, "rank: D at the bottom, S at the top"); }

  printf("scenario tests: %d passed, %d failed\n", passes, fails);
  return fails ? 1 : 0;
}
// ENDINGS=1 ./sim : print the last chapter's win scene for a set of story flags and survivors
void dumpScene(const char* title, uint16_t flags, const char* fallen, const char* never) {
  memset(simEeprom(), 0xFF, 1024); setup(); loadHeroes();
  for (uint8_t h = 0; h < NUM_HEROES; h++) units[h].flags |= F_JOINED;
  auto mark = [](const char* list, uint8_t f) {
    for (uint8_t h = 1; h < NUM_HEROES; h++) if (strstr(list, (std::string(",") + str(STR_NAME + h) + ",").c_str())) {
      if (f) units[h].flags |= f; else units[h].flags &= ~F_JOINED; } };
  mark(fallen, F_FALLEN); mark(never, 0);
  storyFlags = flags; loadChapter(NUM_CHAPTERS - 1);
  printf("\n===== %s (flags 0x%04x)\n", title, flags);
  startTalk(SC_WIN, ST_END);
  while (state == ST_TALK) {
    std::string t; for (uint8_t i = 0; i < talkLen; i++) { char c = FX::readIndexedUInt8(talkAddr + 3, i); t += c == '\n' ? ' ' : c; }
    printf("  %-8s %s\n", str(STR_NAME + talkWho), t.c_str());
    talkAddr += 3 + talkLen; talkLeft--; if (!loadLine()) break;
  }
}
int main(int argc, char** argv) {
  if (getenv("TESTS")) return runTests();
  if (getenv("UISHOTS")) {                              // UISHOTS=1 ./sim : the item list and deployment count, real data
    wantShots = true; memset(simEeprom(), 0xFF, 1024); setup(); loadHeroes();
    for (uint8_t h = 0; h < 5; h++) units[h].flags |= F_JOINED;
    uint8_t c = 0; while (c < NUM_CHAPTERS && FX::readIndexedUInt8(chapterAddr(c), 5) < 6) c++;   // first chapter deploying 6+
    loadChapter(c); state = ST_DEPLOY; menuSel = 0; pump(2); savePBM("u_deploy");
    printf("chapter %d deploys %d; army %d; shown %d/%d\n", c, deployMax, rosterCount(), deployedCount(), rosterCount() < deployMax ? rosterCount() : deployMax);
    placeArmy(); state = ST_SELECT;
    Unit& I = units[1]; memset(I.item, 0, ITEM_SLOTS); memset(I.uses, 0, ITEM_SLOTS);
    giveItem(1, itemId("JAVELIN"), 1); giveItem(1, itemId("SILVER LANCE"), 17); giveItem(1, itemId("VULNERARY"), 3); giveItem(1, itemId("HAWK FEATHER"), 1);
    I.item[0] = itemId("SILVER LANCE"); I.uses[0] = 17; I.item[1] = itemId("JAVELIN"); I.uses[1] = 1;
    sel = 1; centerOn(units[1].x, units[1].y); state = ST_ITEMS; menuSel = 1; pump(1); savePBM("u_items");
    centerOn(18, 8); sel = 1; units[1].x = 18; units[1].y = 8; state = ST_ITEMS; menuSel = 0; pump(1); savePBM("u_items_left");
    return 0;
  }
  if (getenv("SHOWCASE")) {                             // SHOWCASE=1 ./sim : README screenshots of the v1.0 content
    wantShots = true;
    auto fresh = [](uint8_t ch, uint16_t flags) {
      memset(simEeprom(), 0xFF, 1024); setup(); loadHeroes(); storyFlags = flags;
      for (uint8_t h = 0; h < NUM_HEROES; h++) { units[h].flags |= F_JOINED; while (units[h].lv < 12) rollLevelUp(units[h]); }
      loadChapter(ch); placeArmy(); state = ST_SELECT; };
    auto talkTo = [](uint8_t scene, const char* who, int skip) {   // a scene's first line by `who` (after `skip` of them)
      startTalk(scene, ST_SELECT);
      while (state == ST_TALK) {
        if (!strcmp(str(STR_NAME + talkWho), who) && !skip--) { talkShown = talkLen; ab.nextInput = 0; drawTalk(); FX::display(true); return; }
        talkAddr += 3 + talkLen; talkLeft--; if (!loadLine()) break; } };
    fresh(NUM_CHAPTERS - 5, 0x3FFF); centerOn(6, 7); curX = 6; curY = 7; pump(2); savePBM("s22_swamp");
    fresh(NUM_CHAPTERS - 4, 0x3FFF); talkTo(SC_OPEN, "ODA", 2); savePBM("s23_oda");
    fresh(NUM_CHAPTERS - 4, 0x3FFF); centerOn(11, 7); pump(2); savePBM("s24_foundry");
    fresh(NUM_CHAPTERS - 1, 0x3FFF);
    { uint8_t n = NUM_HEROES; units[0].x = 11; units[0].y = 1; units[NUM_HEROES + 1].flags = 0; centerOn(11, 1);
      startBattle(0, n, ST_SELECT); while (state == ST_TALK) { talkAddr += 3 + talkLen; talkLeft--; if (!loadLine()) finishTalk(); }
      for (int i = 0; i < 200 && state == ST_BATTLE; i++) { if (bStriker == n && timer == IMPACT - 3) { savePBM("s25_nhal"); break; } ab.nextInput = 0; loop(); } }
    fresh(NUM_CHAPTERS - 1, 0x3FFF); talkTo(SC_WIN, "OLD TAM", 0); savePBM("s27_ending");
    fresh(NUM_CHAPTERS - 1, 0x3FFF); talkTo(SC_WIN, "ROOK", 0); savePBM("s28_coda");
    fresh(NUM_CHAPTERS - 2, 0x3FFF); stU = 9; stDeploy = false; units[9].flags |= F_ALIVE; units[9].exp = 64; drawStatus(); FX::display(true); savePBM("s26_status");
    fresh(NUM_CHAPTERS - 1, 0x3FFF); units[3].flags |= F_FALLEN; eeprom_update_word((uint16_t*)TALLY, 231); turn = 9; objType = OBJ_SEIZE;
    drawEpilogue(); FX::display(true); savePBM("s29_record");
    return 0;
  }
  if (getenv("ENDINGS")) {
    const uint16_t ALL = 0x3FFF;
    dumpScene("great bell, calling known, everyone alive", ALL, "", "");
    dumpScene("great bell, no 17X; Hale and Wren fell", ALL & ~(1 << 12), ",HALE,WREN,", ",LIR,");
    dumpScene("no bell: Maren rings from the doorway", ALL & ~(1 << 13) & ~(1 << 6), ",ROOK,", ",ODA,");
    dumpScene("no bell, Maren fell: Thane", ALL & ~(1 << 13) & ~(1 << 6), ",MAREN,", ",ODA,");
    dumpScene("no bell, Maren and Thane gone: Brannock", 0x0FFF & ~(1 << 6), ",MAREN,IDRA,", ",THANE,ODA,LIR,");
    return 0;
  }
  int games = argc > 1 ? atoi(argv[1]) : 200;
  Stats S;
  for (int g = 0; g < games; g++) {
    srand(g + 1);
    wantShots = getenv("SHOTS") && (g == 0 || getenv("SHOTSALL"));
    careful = getenv("CAREFUL") != nullptr;
    int resets = 0;
    botRescues = g % 4 != 3;                         // every 4th campaign ignores the captive
    botVisits = g % 3 != 2;
    botTrades = g % 2 == 0;
    botHunts = g % 2 == 1;
    rescuer = NONE;
    memset(simEeprom(), 0xFF, 1024);                 // a fresh device: no save
    destFor = NONE;
    setup();
    bool release = false, retried = false; int frames = 0; State last = ST_TITLE;
    Unit snap[NUM_HEROES]; uint8_t snapCh = 255;    // roster at the end of the last cleared chapter
    Unit startRoster[NUM_HEROES];                    // roster at the start of this chapter (for retry)
    bool snapJoined = false, takenCounted = false; uint8_t lastVill = 0; uint16_t lastFlags = 0;
    int lastSleepers = 0; bool wasAlive[MAX_UNITS] = {};
    int onlyCh = -1;                                 // ONLY=1 with START: stop once that chapter is cleared
    if (getenv("START")) {                           // START=N [LV=L] [FLAGS=mask]: jump to chapter N with a levelled army
      newGame();
      uint8_t L = getenv("LV") ? atoi(getenv("LV")) : 10;
      storyFlags = getenv("FLAGS") ? strtol(getenv("FLAGS"), 0, 0) : 0;
      int n = atoi(getenv("START"));
      for (uint8_t h = 0; h < NUM_HEROES; h++) {
        Unit& U = units[h];
        static const uint8_t joinsAfter[NUM_HEROES] = { 0, 0, 0, 0, 0, 0, 0, 11, 14, 18, 18, 20, 22, 25 };   // Aelis Ch 9, Rook Ch 11, Saoirse/Thane Ch 14, Hesk Ch 16, Lir 17X, Oda 19X
        if (n > joinsAfter[h]) U.flags |= F_JOINED;
        while (U.lv < L) rollLevelUp(U);
        static const char* up[] = { "SILVER SWORD", "STEEL LANCE", "STEEL AXE", "STEEL BOW", "STEEL LANCE", "HEAL", "", "STEEL LANCE", "SILVER SWORD", "THUNDER" };
        if (U.cls < 10 && *up[U.cls] && units[h].item[ITEM_SLOTS - 1] == 0) {
          for (int s = ITEM_SLOTS - 1; s > 0; s--) { U.item[s] = U.item[s - 1]; U.uses[s] = U.uses[s - 1]; }
          U.item[0] = itemId(up[U.cls]); U.uses[0] = 1;
        }
      }
      startChapter(n);
      for (uint8_t i = NUM_HEROES; i < nUnits; i++) { resX[i] = units[i].x; resY[i] = units[i].y; }
      memcpy(startRoster, units, sizeof startRoster); S.reached[n]++;
      if (getenv("ONLY")) onlyCh = n;
    }
    int tallyBefore[64];
    while (frames < 200000) {
      ab.nextInput = release ? 0 : botInput();
      release = !release;
      State before = state;
      loop(); frames++; sndTick();
      if (onlyCh >= 0 && chapter != onlyCh) break;
      if (careful && resets < 10 && state != ST_END && state != ST_CHAPTER) {   // a hero fell: power-cycle, CONTINUE
        bool fell = false;
        for (int h = 0; h < NUM_HEROES; h++) if ((units[h].flags & F_FALLEN) && joined(h) && !(startRoster[h].flags & F_FALLEN)) fell = true;
        if (fell) { resets++; S.resets++; startChapter(loadSave()); S.reached[chapter]++; continue; }
      }
      if (state == ST_CHAPTER && before == ST_END) resets = 0;
      std::string why;
      if (!checkInvariants(why)) { printf("game %d ch %d frame %d: %s\n", g, chapter, frames, why.c_str()); return 1; }
      // --- events
      if (state == ST_CHAPTER && before != ST_CHAPTER) rescuer = hunter = NONE;
      if (getenv("DBG") && chapter == atoi(getenv("DBG")) && state == ST_BANNER && before != ST_BANNER) {
        printf("turn %d rescuer %d", turn, rescuer);
        for (int h = 0; h < nUnits; h++) if (alive(h)) printf("  %s %d,%d hp%d", std::string(str(STR_NAME + units[h].name)).c_str(), units[h].x, units[h].y, units[h].hp);
        printf("\n"); }
      if (state == ST_DEPLOY && before != ST_DEPLOY && before != ST_TRADE) S.deployScreens++;
      if (state == ST_TRADE && before != ST_TRADE) tally(tallyBefore);
      if (before == ST_TRADE) {                                    // trading never makes or loses an item
        int now[64]; tally(now);
        if (memcmp(now, tallyBefore, sizeof now)) { printf("game %d: trade changed the army's items\n", g); return 1; }
        if (state != ST_TRADE) { S.trades++; tradeStep = 99; }
      }
      {                                                             // events: wakes must wake, spawns must appear
        int sleepers = reservesLeft();
        if (sleepers == 0 && lastSleepers > 0) S.alarms++;
        lastSleepers = sleepers;
        for (uint8_t i = NUM_HEROES; i < nUnits; i++) {
          bool a = alive(i);
          if (a && !wasAlive[i] && state != ST_CHAPTER && before != ST_CHAPTER) { S.ambushes++; if (units[i].cls == C_HORROR) S.demons++; }
          wasAlive[i] = a;
        }
        std::string why2;
        if (!eventsConsistent(why2)) { printf("game %d ch %d: %s\n", g, chapter, why2.c_str()); return 1; }
      }
      if (storyFlags & 1 && !(lastFlags & 1)) S.ledgers++;
      lastFlags = storyFlags;
      if (state == ST_BANNER && before == ST_DEPLOY) {               // the team that marched out
        uint8_t n = 0;
        for (int h = 0; h < NUM_HEROES; h++) if (fighter(h)) n++;
        if (n != deployedCount() || !alive(0)) { printf("game %d: deployed %d, chose %d\n", g, n, deployedCount()); return 1; }
        if (deployMask != 0x0F && deployMask != 0) S.swaps++;
      }
      if (before != ST_TALK && state == ST_TALK && capId != NONE) {
        if (joined(capId) && units[capId].flags & F_MOVED && !(units[capId].flags & F_FALLEN) && talkAddr) {}
      }
      if (capId != NONE && units[capId].flags & F_JOINED && !(snapJoined)) { snapJoined = true; S.recruited++; }
      if (state == ST_TALK && before != ST_TALK && capId != NONE && (units[capId].flags & F_FALLEN) && !takenCounted) { takenCounted = true; S.taken++; }
      if (__builtin_popcount(villDone) > __builtin_popcount(lastVill)) S.visits++;
      lastVill = villDone;
      if (state == ST_GOT && before == ST_TALK) S.gifts++;
      if (state == ST_GOT && before != ST_GOT) {
        if (gotTo == NONE) {
          for (int h = 0; h < NUM_HEROES; h++)
            if (available(h) && !units[h].item[ITEM_SLOTS - 1]) { printf("game %d: item lost though %d had room\n", g, h); return 1; }
          S.noRoom++;
        } else if (gotTo != (gotOnMap ? walkU : (isEnemy(bA) ? bD : bA))) S.passedOn++;
      }
      if (state == ST_HEALFX && before != ST_HEALFX) { if (bA == bD) S.vulnFx++; else S.staffHeals++; S.healedHp += healAmt;
        if (units[bD].hp > units[bD].maxHp) { printf("game %d: overheal\n", g); return 1; } }
      if (state == ST_BATTLE && before != ST_BATTLE && (bA == 4 || bD == 4)) S.marenFought++;
      if (state == ST_CHAPTER && before != ST_CHAPTER) {
        S.reached[chapter]++;
        if (getenv("ROSTER") && chapter == atoi(getenv("ROSTER"))) {
          printf("g%d ch%d:", g, chapter);
          for (int h = 0; h < NUM_HEROES; h++) if (available(h)) printf(" %s/%d/%s", std::string(str(STR_NAME + units[h].name)).c_str(), units[h].lv, std::string(itemName(units[h].item[0])).c_str());
          printf("\n"); }
        snapJoined = joined(4); takenCounted = false; lastVill = 0;
        for (uint8_t i = NUM_HEROES; i < nUnits; i++) { resX[i] = units[i].x; resY[i] = units[i].y; }
        if (snapCh != 255 && chapter == nextChapter(snapCh)) {      // carry-over check
          bool ok = true;
          for (int h = 0; h < NUM_HEROES; h++) {
            Unit &a = snap[h], &b = units[h];
            if (a.lv != b.lv || a.exp != b.exp || a.maxHp != b.maxHp || a.str != b.str || a.def != b.def ||
                memcmp(a.item, b.item, ITEM_SLOTS) || ((a.flags ^ b.flags) & F_FALLEN) ||
                ((a.flags & F_JOINED) && !(b.flags & F_JOINED))) ok = false;   // ('joins:' may add a hero)
            if (alive(h) && b.hp != b.maxHp) ok = false;
          }
          if (!ok) { printf("game %d: roster did not carry into chapter %d\n", g, chapter); return 1; }
          S.carryOk++;
        }
        memcpy(startRoster, units, sizeof startRoster);
      }
      if (state == ST_BATTLE && before != ST_BATTLE && last != ST_TALK) S.battles++;
      if (getenv("DEATHS") && before == ST_BATTLE && state != ST_BATTLE)
        for (uint8_t k = 0; k < 2; k++) { uint8_t u = k ? bD : bA, o = k ? bA : bD;
          if (u < NUM_HEROES && !units[u].hp) { std::string a = str(STR_NAME + units[u].name); std::string b = str(STR_NAME + units[o].name);
            printf("ch%d t%d: %s lv%d (str%d def%d spd%d) killed by %s lv%d [fA dmg%d hit%d x%d | fD dmg%d hit%d x%d]\n", chapter, turn, a.c_str(), units[u].lv, units[u].str, units[u].def, units[u].spd, b.c_str(), units[o].lv, fA.dmg, fA.hit, fA.dbl, fD.dmg, fD.hit, fD.dbl); } }
      if (state == ST_LEVELUP && before != ST_LEVELUP) { S.levelups++; if (units[xHero].lv > S.topLv) S.topLv = units[xHero].lv; }
      if (state == ST_GOT && before != ST_GOT) S.drops++;
      if (before == ST_ITEMS && state != ST_ITEMS && state != ST_MENU) S.heals++;
      if (state == ST_WALK && before != ST_WALK) {                    // validate every route walked
        S.walks++;
        Unit& w = units[walkU];
        int cost = 0; bool ok = walkX[0] == w.x && walkY[0] == w.y;
        for (int i = 1; i < walkN; i++) {
          int dx = abs(walkX[i] - walkX[i-1]), dy = abs(walkY[i] - walkY[i-1]);
          uint8_t c = tCost(terrainAt(walkX[i], walkY[i]));
          if (dx + dy != 1 || c == NO_PASS) ok = false;
          uint8_t o = unitAt(walkX[i], walkY[i]);
          if (o != NONE && isEnemy(o) != isEnemy(walkU)) ok = false;
          cost += c;
        }
        if (cost > clsMov(walkU)) ok = false;
        if (!ok) { printf("game %d: bad route for unit %d\n", g, walkU); return 1; }
      }
      if (state == ST_END && before != ST_END) {
        if (won) { S.cleared[chapter]++; memcpy(snap, units, sizeof snap); snapCh = chapter; }
        if (chapter == 1) {                                              // chapter 2 with nobody lost before it
          bool full = true; for (int h = 0; h < 4; h++) if (startRoster[h].flags & F_FALLEN) full = false;
          if (full) { S.fullCh2++; if (won) S.fullCh2Clear++; if (joined(4)) S.fullCh2Rescue++; }
        }
        if (won && lastChapter()) break;
        if (!won) {
          if (retried) break;
          retried = true; S.retries++;
          // the bot presses A: the chapter must restart from its save
          for (int k = 0; k < 4 && state == ST_END; k++) { ab.nextInput = k & 1 ? 0 : A_BUTTON; loop(); }
          bool ok = state == ST_CHAPTER;
          for (int h = 0; h < NUM_HEROES && ok; h++)
            if (startRoster[h].lv != units[h].lv || startRoster[h].exp != units[h].exp || memcmp(startRoster[h].item, units[h].item, ITEM_SLOTS)) ok = false;
          if (!ok) { printf("game %d: retry did not restore the chapter's starting roster\n", g); return 1; }
          S.retryOk++;
          S.reached[chapter]++;            // a retry is another attempt
          continue;
        }
      }
      // --- screenshots
      if (wantShots && before == state) {
        switch (state) {
          case ST_TITLE: break;
          case ST_CHAPTER: savePBM("0_chapter" + std::to_string(chapter + 1)); break;
          case ST_TALK: if (talkShown >= talkLen) savePBM("0b_talk_" + std::string(str(STR_NAME + talkWho)) + "_ch" + std::to_string(chapter + 1)); break;
          case ST_SELECT: if (turn == 2 && unitAt(curX, curY) != NONE) savePBM("2_select");
            if (turn == 3 && !shotTaken.count("2g_minimap_ch" + std::to_string(chapter + 1))) {
              ab.clear(); drawMinimap(); memcpy(ab.shown, ab.px, sizeof ab.px); savePBM("2g_minimap_ch" + std::to_string(chapter + 1)); ab.clear(); }
            if (chapter == 2) savePBM(reservesLeft() ? "2d_palisade_asleep" : "2e_palisade_awake");
            if (chapter == 3 && turn >= 3) savePBM("2f_grey_road_risen");
            if (captivePending() && !shotTaken.count("2b_captive_hud")) {     // stage it: the cursor on the captive
              uint8_t ox = curX, oy = curY, cx = camX, cy = camY; curX = capX; curY = capY; clampCam();
              ab.clear(); drawMap(false); drawCursor(); drawHud(); memcpy(ab.shown, ab.px, sizeof ab.px); savePBM("2b_captive_hud");
              curX = ox; curY = oy; camX = cx; camY = cy; ab.clear(); }
            if (villageAt(curX, curY) != NONE && unitAt(curX, curY) == NONE) savePBM("2c_village_hud");
            break;
          case ST_WALK: if (chapter == 1 && walkI == 1 && !isEnemy(walkU)) savePBM("4_walk_ch2"); break;
          case ST_MOVE: if (turn == 2 && sel == 0) savePBM("3_move"); break;
          case ST_BANNER: if (timer == 30) savePBM(enemyPhase ? "1d_banner_e" : "1c_banner_p"); break;
          case ST_LEVELUP: if (xT == 75) savePBM(promoting ? "7f_promote" : "7e_levelup"); break;
          case ST_EXP: if (xT * 2 >= xGain && xT == xGain / 2 + 4) savePBM("7g_exp"); break;
          case ST_MENU: if (sel != NONE && menuN > 2) savePBM("5_menu"); break;
          case ST_ITEMS: savePBM("5b_items"); break;
          case ST_DEPLOY: if (menuSel == 1) savePBM("1_deploy"); break;
          case ST_TRADE: if (trHeld != NONE) savePBM("1b_trade"); break;
          case ST_HEALFX: if (timer == 24) savePBM(bA == bD ? "7d_heal_vuln_a" : "7d_heal_a");
                          if (timer == 14) savePBM(bA == bD ? "7d_heal_vuln_b" : "7d_heal_b"); break;
          case ST_GOT: if (timer == 40) savePBM(gotOnMap ? "7b_gift" : "7a_got"); break;
          case ST_FORECAST: savePBM("6_forecast"); break;
          case ST_BATTLE: if (bKind == 1 && timer == IMPACT - 2) {
                            savePBM("7_battle");
                            { uint8_t fo = isEnemy(bA) ? bA : bD, he = isEnemy(bA) ? bD : bA;
                              savePBM("8_vs_" + std::string(str(STR_NAME + units[fo].name)));
                              savePBM("8_hero_" + std::string(str(STR_NAME + units[he].name))); }
                            savePBM("7c_back" + std::to_string(pgm_read_byte(&T_BACK[terrainAt(units[bD].x, units[bD].y)])) + (units[bA].cls == C_HORROR || units[bD].cls == C_HORROR ? "_demon" : ""));
                          } break;
          case ST_END: savePBM(won ? "9_clear" : "9_defeat"); break;
          default: break;
        }
      }
      last = state;
    }
    S.games++;
    if (frames >= 200000) { S.stuck++; printf("game %d stuck in state %d (chapter %d, turn %d)", g, state, chapter, turn);
      for (int i = 0; i < nUnits; i++) if (alive(i)) printf(" [%s %d,%d hp%d f%d]", std::string(str(STR_NAME + units[i].name)).c_str(), units[i].x, units[i].y, units[i].hp, units[i].flags);
      printf("\n"); }
  }
  printf("campaigns %ld:", S.games);
  for (int c = 0; c < NUM_CHAPTERS; c++) printf("  ch%d attempts %ld cleared %ld", c + 1, S.reached[c], S.cleared[c]);
  printf("\nretries %ld (roster restored %ld)  carry-overs verified %ld  stuck %ld\n", S.retries, S.retryOk, S.carryOk, S.stuck);
  printf("battles %ld  level-ups %ld (top lv %d)  item uses %ld  drops %ld\n", S.battles, S.levelups, S.topLv, S.heals, S.drops);
  printf("deploy screens %ld (non-default team %ld)  captive recruited %ld / taken %ld  villages visited %ld (gifts %ld)\n",
         S.deployScreens, S.swaps, S.recruited, S.taken, S.visits, S.gifts);
  printf("staff heals %ld (%ld HP)  battles involving Maren %ld  careful resets %ld\n", S.staffHeals, S.healedHp, S.marenFought, S.resets);
  printf("items handed to another hero (finder's bag full) %ld, lost for lack of room %ld\n", S.passedOn, S.noRoom);
  printf("prep-screen trades %ld  heal-ring animations: staff %ld, vulnerary %ld\n", S.trades, S.staffHeals, S.vulnFx);
  printf("alarms rung %ld  enemies appearing mid-chapter %ld (demons %ld)  ledgers found %ld\n", S.alarms, S.ambushes, S.demons, S.ledgers);
  printf("chapter 2 with the full army: %ld attempts, %ld cleared, %ld rescued Maren\n", S.fullCh2, S.fullCh2Clear, S.fullCh2Rescue);
  printf("sounds never heard:");
  struct { const uint8_t* s; const char* n; } SND[] = {
    {S_CURSOR,"cursor"},{S_MENU,"menu"},{S_CONFIRM,"confirm"},{S_CANCEL,"cancel"},{S_STEP,"step"},{S_EXP,"exp"},
    {S_LVLUP,"lvlup"},{S_DING,"ding"},{S_PHASE_P,"phase_p"},{S_PHASE_E,"phase_e"},{S_SWING,"swing"},{S_ARROW,"arrow"},
    {S_SPELL,"spell"},{S_HIT,"hit"},{S_CRIT,"crit"},{S_MISS,"miss"},{S_FALL,"fall"},{S_M_CHAPTER,"m_chapter"},{S_M_BOSS,"m_boss"},
    {S_M_WIN,"m_win"},{S_M_LOSE,"m_lose"},{S_ALARM,"alarm"},{S_M_DEMON,"m_demon"},{S_SEAL,"seal"} };
  for (auto& e : SND) if (!sndHeard().count(e.s)) printf(" %s", e.n);
  printf("\n");
  return S.stuck ? 1 : 0;
}
