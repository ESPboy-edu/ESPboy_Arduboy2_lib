#!/usr/bin/env python3
"""Compile chapters/*.txt + game_data.py into story.h and fxdata/chapters.bin.

Usage (from the sketch folder):  python3 tools/chapters/make_chapters.py
       test fixtures:            python3 tools/chapters/make_chapters.py --src DIR --bin OUT.bin --no-header
Validates everything it can so a typo fails here, not on the device.

chapters.bin layout (multi-byte values big-endian, as ArduboyFX reads them):
  [0]                chapter count
  [1 + 3*i]          24-bit offset of chapter i
  chapter +0         objective: type (0 rout, 1 seize, 2 boss, 3 survive), x, y, turns
          +4         enemy count
          +5         deploy (heroes allowed), slot count
          +7         captive: hero id (255 none), x, y, leaves-after-turn (0 = never)
          +11        village/chest count
          +12        event count
          +13        requires: story flag number (0 = always played)
          +14        label: chapter number on cards / title (+0x80 = an 'X' side chapter)
          +15        joins: 16-bit mask of heroes who join the army as the chapter opens
          +17        shop: 6 item ids on sale at the prep screen (0 = none; first 0 = no shop)
          +23        reward: gold for clearing the chapter, / 10
          +24        deployment slots: 8 x (x, y); slot 0 is the lord's
          +40        villages/chests: 4 x (x, y, gift, flag set on visiting); gift: item id, or 0x80 | gold / 10
          +56        events: 8 x 10 bytes (see EVENT below)
          +136       19 x 16-bit scene offsets, relative to the chapter:
                     open, boss, win, lose, recruit, warn, taken, village 0..3, event 0..7
          +174       map: 24x16 terrain ids, two per byte (high nibble = even x)
          +366       enemies: count x Unit (the sketch's 21-byte struct, loaded verbatim;
                     an enemy's exp byte holds its group number)
          scenes     line count, then per line: condition, speaker id, length, text ('\\n' breaks)
                     condition: 0 always; flag N (must be set) or 0x80|hero (must be in the army), 0xA0|hero
                     (joined and fell); +0x40 = not. Speaker 255 = a guard (no text): the next line
                     also needs its condition -- how [A][B] SPEAKER: ... is stored

EVENT (10 bytes): when | flags, p1, p2, actions, group, set-flag, until-flag, warp x, warp y, needs-flag
  when: 1 tile X Y (a hero walks over it; for warps, stops on it), 2 turn N [every K],
        3 a unit falls (p1 = unit index), 4 talk (p1 = hero id or 255 = anyone, p2 = unit index)
  flags: 0x10 repeat, 0x20 demon music
  actions: 1 wake group, 2 spawn group, 4 warp, 8 set flag, 16 join, 32 has a scene, 64 win
"""
import argparse, glob, json, os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, HERE)
import game_data as G

MAP_W, MAP_H = 24, 16
LEGEND = {'.': 0, 'T': 1, 'h': 2, '~': 3, '#': 4, 'K': 5, '=': 6, 'V': 7, 'C': 8, 'P': 9, 'G': 10, 'O': 11, '_': 12, 'D': 13, 'R': 14}
PASSABLE = {0, 1, 2, 5, 6, 7, 8, 10, 11, 12, 13}
VISITABLE = {7, 8}                    # villages and chests
OBJ = {'rout': 0, 'seize': 1, 'boss': 2, 'survive': 3}
SCENES = ['open', 'boss', 'win', 'lose', 'recruit', 'warn', 'taken']
MAX_SLOTS, MAX_VILLAGES, MAX_EVENTS, EV_SIZE = 8, 4, 8, 10
H_SHOP, SHOP_SIZE = 17, 6
H_SLOTS, H_VILL, H_EVENTS, H_SCENES, H_MAP = 24, 40, 56, 136, 174
ITEM_SLOTS = 4
F_ALIVE, F_ENEMY, F_MOVED, F_HOLD, F_LORD, F_BOSS, F_FALLEN, F_JOINED = 1, 2, 4, 8, 16, 32, 64, 128
F_HIDDEN, F_RESERVE = F_FALLEN, F_JOINED    # the same bits mean these on an enemy
EV_TILE, EV_TURN, EV_FALL, EV_TALK = 1, 2, 3, 4
EV_REPEAT, EV_DEMON = 0x10, 0x20
A_WAKE, A_SPAWN, A_WARP, A_SET, A_JOIN, A_SCENE, A_WIN = 1, 2, 4, 8, 16, 32, 64
STORY_FLAGS, LOCAL_FLAGS = 16, 8      # flags 1..16 are saved with the army; 17..24 last one chapter
DROP = 0x80
TEXT_COLS, TEXT_ROWS = 17, 5          # the dialogue panel is 68 px; 17 chars x 4 px - 1
MAX_ENEMIES = 10                       # RAM: every enemy slot is 21 bytes (reinforcements reuse slots)
DEATH_SLOT = 128                       # bytes per hero's last-words scene in tables.bin
STR_LEN = 16                           # string records on FX: up to 15 characters + 0
NAME_MAX = 12                          # names, classes, items, terrain: what fits the HUD and planks
TERRAIN_NAMES = ['PLAIN', 'WOOD', 'HILL', 'WATER', 'WALL', 'KEEP', 'FORD', 'VILLAGE', 'CHEST', 'PALISADE', 'GATE', 'PORTAL', 'FLOOR', 'DECK', 'RAIL']   # = LEGEND ids

# UI text on FX (strings.bin, after the names): (enum suffix, text). The menu labels come first,
# in the sketch's OPT_ order, then SOUND OFF / OPEN / SURE? (menuLabel() relies on this order).
UI = [('ATTACK', 'ATTACK'), ('SEIZE', 'SEIZE'), ('TALK', 'TALK'), ('VISIT', 'VISIT'), ('HEAL', 'HEAL'),
      ('ITEM', 'ITEM'), ('WAIT', 'WAIT'), ('END', 'END TURN'), ('SOUND', 'SOUND ON'), ('RESTART', 'RESTART'),
      ('QUIT', 'QUIT'), ('ARMY', 'ARMY'), ('SOUNDOFF', 'SOUND OFF'), ('OPEN', 'OPEN'), ('SURE', 'SURE?'),
      ('CAPTIVE', 'CAPTIVE  TALK'), ('BYT', 'BY T'), ('IMPASS', 'IMPASSABLE'), ('EMPTY', 'EMPTY'), ('VISITED', 'VISITED'),
      ('STOPOPEN', 'STOP TO OPEN'), ('STOPVISIT', 'STOP TO VISIT'), ('DEF', 'DEF '), ('AVO', 'AVO '),
      ('HP', 'HP'), ('STR', 'STR'), ('SKL', 'SKL'), ('SPD', 'SPD'), ('DEFS', 'DEF'),          # level-up rows
      ('DMG', 'DMG'), ('HIT', 'HIT'), ('CRT', 'CRT'),                                           # forecast rows (after HP)
      ('MISS', 'MISS'), ('CRIT', 'CRIT -'), ('NOROOM', 'NO ROOM'), ('TO', 'TO '), ('EXP', 'EXP'), ('LEVELUP', 'LEVEL UP!'),
      ('DEPLOY', 'DEPLOY '), ('ITEMS', 'ITEMS'), ('BEGIN', 'BEGIN'), ('ATRADE', 'A  TRADE'), ('ATOFIGHT', 'A  TO FIGHT'),
      ('THELORD', 'THE LORD'), ('ALWAYS', 'ALWAYS GOES'),
      ('TR1', 'A TAKE  B DONE'), ('TR2', '  ROW 1: HERO'), ('TR3', 'A PUT HERE / '), ('TR4', 'SWAP   B UNDO'),
      ('NOW', 'NOW '), ('LVUPLV', 'LEVEL UP  LV '), ('ENEMYPH', 'ENEMY PHASE'), ('PLAYERPH', 'PLAYER PHASE'), ('TURN', 'TURN '),
      ('CONTINUED', 'THE BELL TOLLS'), ('CLEAR', 'CHAPTER CLEAR'), ('ONWARD', 'ONWARD'),
      ('FELL', 'FELL: '), ('DISCARD', 'THROW AWAY'), ('US', 'ARMY '), ('FOES', 'FOES '),
      ('BROKE', 'BROKE!'), ('WAGON', 'WAGON '), ('TOWAGON', 'TO WAGON'), ('GOLD', 'GOLD '), ('GOLDW', ' GOLD'),
      ('SHOP', 'SHOP'), ('BUYHINT', 'A BUY: TO WAGON'), ('ABUY', 'A  TO BUY'),
      ('CONTINUE', 'CONTINUE CH '), ('NEWGAME', 'NEW GAME'), ('SNDON', 'B SND ON'), ('SNDOFF', 'B SND OFF'),
      ('LV', 'LV'), ('MV', 'MV '), ('X2', 'X2'), ('DASH', '--'),
      ('RANK0', 'BELLWRIGHT'), ('RANK1', "HOLLIN'S WARDEN"), ('RANK2', 'CAPTAIN'),
      ('RANK3', 'SELLSWORD'), ('RANK4', 'SURVIVOR')]

NAME_ID = {n[0]: i for i, n in enumerate(G.NAMES)}
CLASS_ID = {c[0]: i for i, c in enumerate(G.CLASSES)}
ITEM_ID = {it[0]: i for i, it in enumerate(G.ITEMS)}
KIND_ID = {k: i for i, k in enumerate(G.KINDS)}
if G.NUM_HEROES > 15:   # RAM: every hero is 21 bytes, all in memory; 15 leaves ~50 bytes of stack margin
    sys.exit("game_data: at most 15 heroes fit in the Arduboy's RAM (see README, Budget)")
if any(c[3] > 9 for c in G.CLASSES): sys.exit("game_data: movement is at most 9 (the walk route buffer holds 10 steps, boots included)")


class ChapterError(Exception):
    pass


def class_weapons(cls):
    w = G.CLASSES[cls][2]
    return (w,) if isinstance(w, str) else tuple(w)


def item_slots(spec, where):
    ids, uses = [0] * ITEM_SLOTS, [0] * ITEM_SLOTS
    names = [s.strip() for s in spec.split(',') if s.strip() and s.strip() != '-'] if isinstance(spec, str) else spec
    if len(names) > ITEM_SLOTS: raise ChapterError(f"{where}: more than {ITEM_SLOTS} items")
    for k, n in enumerate(names):
        drop = n.endswith('*')
        n = n.rstrip('*').strip()
        if n not in ITEM_ID: raise ChapterError(f"{where}: unknown item '{n}' (known: {', '.join(ITEM_ID)})")
        ids[k] = ITEM_ID[n]
        uses[k] = G.ITEMS[ITEM_ID[n]][7] | (DROP if drop else 0)
    return ids, uses


def check_weapon(cls, ids, where):
    want = class_weapons(cls)
    if not any(G.ITEMS[i][1] in want for i in ids if i):
        raise ChapterError(f"{where}: carries no {'/'.join(want)} -- a {G.CLASSES[cls][0]} could never act")


def unit_bytes(x, y, cls, name, hp, st, sk, sp, df, flags, lv, group, ids, uses):
    return bytes([x, y, cls, name, hp, hp, st, sk, sp, df, flags, lv, group] + ids + uses)


def wrap_check(text, where):  # noqa
    lines = text.split('/')
    if len(lines) > TEXT_ROWS: raise ChapterError(f"{where}: {len(lines)} lines (max {TEXT_ROWS})")
    for ln in lines:
        if len(ln) > TEXT_COLS: raise ChapterError(f"{where}: line '{ln}' is {len(ln)} chars (max {TEXT_COLS})")
        bad = [c for c in ln if not (32 <= ord(c) <= 90)]
        if bad: raise ChapterError(f"{where}: unsupported characters {bad} (font is A-Z, 0-9, punctuation)")
    return '\n'.join(lines)


def flag_number(tok, where, story_only=False):
    n = int(tok)
    hi = STORY_FLAGS if story_only else STORY_FLAGS + LOCAL_FLAGS
    if not 1 <= n <= hi: raise ChapterError(f"{where}: flags are 1..{hi}" + ("" if story_only else " (17+ last one chapter)"))
    return n


HEADER = re.compile(r'^\s*(title|goal|objective|deploy|captive|requires|label|joins|shop|reward|map|slots|enemies|'
                    r'scene\s+\w+|event\s+\w+|(?:village|chest)\s+\d+\s+\d+)\s*:(.*)$', re.I)


def parse(path):
    fn = os.path.basename(path)
    sec, ch = None, dict(map=[], slots=[], enemies=[], villages=[], events=[], scenes={k: [] for k in SCENES})
    for n, raw in enumerate(open(path), 1):
        line = raw.rstrip('\n')
        where = f"{fn}:{n}"
        m = HEADER.match(line)
        if m:
            key, val = m.group(1).lower(), m.group(2).strip()
            if key in ('title', 'goal', 'objective', 'deploy', 'captive', 'requires', 'label', 'joins', 'shop', 'reward'):
                ch[key] = (val, where); sec = None
            elif key.startswith('scene'):
                sec = 'scene:' + key.split()[1]
                ch['scenes'].setdefault(sec[6:], [])
            elif key.startswith('event'):
                name = key.split()[1]
                if any(e['name'] == name for e in ch['events']): raise ChapterError(f"{where}: two events called '{name}'")
                ch['events'].append(dict(name=name, spec=val, where=where)); sec = None
            elif key.startswith('village') or key.startswith('chest'):
                kind, vx, vy = key.split()
                ch['villages'].append(dict(x=int(vx), y=int(vy), lines=[], gift=None, flag=0, kind=kind, where=where))
                sec = 'village'
            else: sec = key
            continue
        if not line.strip(): continue
        if sec == 'map':                       # a map row is exactly MAP_W legend symbols
            row = line.strip()
            if len(row) == MAP_W and all(c in LEGEND for c in row) or not row.startswith('#'):
                ch['map'].append((row, where))
            continue
        if line.lstrip().startswith('#'): continue
        if sec == 'slots':
            p = line.split()
            ch['slots'].append((int(p[0]), int(p[1]), where))
        elif sec == 'enemies':
            p = line.split(None, 10)
            if len(p) < 10: raise ChapterError(f"{where}: enemy line needs name class x y lv hp str skl spd def [flags] [items]")
            toks = (p[10] if len(p) > 10 else '').split()
            flags = []
            while toks and re.fullmatch(r'boss|hold|-|reserve\d*|ambush\d*|g\d+', toks[0]): flags.append(toks.pop(0))
            p[0] = p[0].replace('_', ' ')          # ROYAL_GUARD -> ROYAL GUARD (names may hold spaces)
            ch['enemies'].append((p[:10], flags, ' '.join(toks), where))
        elif sec == 'village' and line.strip().lower().startswith('gift:'):
            ch['villages'][-1]['gift'] = (line.split(':', 1)[1].strip(), where)
        elif sec == 'village' and line.strip().lower().startswith('gold:'):
            ch['villages'][-1]['gold'] = (int(line.split(':', 1)[1]), where)
        elif sec == 'village' and line.strip().lower().startswith('flag:'):
            ch['villages'][-1]['flag'] = flag_number(line.split(':', 1)[1], where)
        elif sec and (sec.startswith('scene:') or sec == 'village'):
            if ':' not in line: raise ChapterError(f"{where}: dialogue lines look like 'NAME: text'")
            m = re.match(r'^((?:\[[^\]]*\]\s*)*[^:\[]*):(.*)$', line)   # [FELL:X] conditions hold colons too
            if not m: raise ChapterError(f"{where}: a scene line is SPEAKER: text")
            who, text = m.group(1), m.group(2)
            (ch['villages'][-1]['lines'] if sec == 'village' else ch['scenes'][sec[6:]]).append((who.strip(), text.strip(), where))
        else:
            raise ChapterError(f"{where}: don't know what this line is: '{line}'")
    return ch


def line_condition(tok, where):
    """[14] flag 14 set, [!14] not set, [HESK] Hesk in the army, [!HESK] not, [FELL:HESK] Hesk joined
    and fell (for endings and the roll of the dead), [!FELL:HESK] he didn't."""
    neg = tok.startswith('!')
    tok = tok.lstrip('!').strip()
    fell = tok.startswith('FELL:')
    if fell: tok = tok[5:].strip()
    if tok.isdigit() and not fell: c = flag_number(tok, where)
    elif tok in NAME_ID and 0 < NAME_ID[tok] < G.NUM_HEROES: c = 0x80 | (0x20 if fell else 0) | NAME_ID[tok]
    else: raise ChapterError(f"{where}: a line condition is [N] / [!N] for flag N, [HERO] / [!HERO], or [FELL:HERO] (not the lord)")
    return c | (0x40 if neg else 0)


GUARD = 255                                    # speaker id of a guard line: no text, just a condition


def scene_bytes(lines):
    b, n = b'', 0
    for who, text, where in lines:
        conds = []
        while True:                            # [A][B] SPEAKER: every condition must hold
            m = re.match(r'^\[([^\]]+)\]\s*(.*)$', who)
            if not m: break
            conds.append(line_condition(m.group(1), where)); who = m.group(2).strip()
        if who not in NAME_ID: raise ChapterError(f"{where}: unknown speaker '{who}' -- add it to game_data.NAMES")
        if G.NAMES[NAME_ID[who]][1] is None: raise ChapterError(f"{where}: {who} has no portrait, so can't speak")
        t = wrap_check(text, where).encode('ascii')
        for c in conds[:-1]: b += bytes([c, GUARD, 0]); n += 1
        b += bytes([conds[-1] if conds else 0, NAME_ID[who], len(t)]) + t; n += 1
    if n > 255: raise ChapterError(f"{lines[0][2]}: a scene holds at most 255 lines (each extra [condition] is one)")
    return bytes([n]) + b


def compile_event(e, unit_of, walkable, groups, where):
    """'tile 12 6 ; wake 1 ; demon' -> 10 bytes"""
    parts = [p.strip() for p in e['spec'].split(';') if p.strip()]
    if not parts: raise ChapterError(f"{where}: an event needs a condition, e.g. 'tile 12 6 ; wake 1'")
    w = parts[0].split()
    b = [0] * EV_SIZE
    if w[0] == 'tile' and len(w) == 3:
        x, y = int(w[1]), int(w[2])
        if not walkable(x, y): raise ChapterError(f"{where}: tile {x},{y} can't be walked on")
        b[0], b[1], b[2] = EV_TILE, x, y
    elif w[0] == 'turn' and len(w) in (2, 4) and (len(w) == 2 or w[2] == 'every'):
        b[0], b[1] = EV_TURN, int(w[1])
        if b[1] < 2: raise ChapterError(f"{where}: turn events start from turn 2 (turn 1 is the opening scene)")
        if len(w) == 4: b[2] = int(w[3]); b[0] |= EV_REPEAT
    elif w[0] in ('falls', 'fall') and len(w) >= 2:
        b[0], b[1] = EV_FALL, unit_of(' '.join(w[1:]), where)
    elif w[0] == 'talk' and 'to' in w:
        k = w.index('to')
        who, target = ' '.join(w[1:k]), ' '.join(w[k + 1:])
        if who == 'anyone': h = 255
        elif who in NAME_ID and NAME_ID[who] < G.NUM_HEROES: h = NAME_ID[who]
        else: raise ChapterError(f"{where}: 'talk HERO to UNIT' -- {who} isn't a hero (or use 'anyone')")
        b[0], b[1], b[2] = EV_TALK, h, unit_of(target, where)
    else:
        raise ChapterError(f"{where}: condition is 'tile X Y', 'turn N [every K]', 'falls UNIT' or 'talk HERO to UNIT'")
    for a in parts[1:]:
        t = a.split()
        if t[0] in ('wake', 'spawn') and len(t) == 2:
            g = int(t[1])
            if g not in groups: raise ChapterError(f"{where}: no enemy is in group {g} (mark them reserve{g}/ambush{g}/g{g})")
            if b[4] and b[4] != g: raise ChapterError(f"{where}: one group per event")
            b[3] |= A_WAKE if t[0] == 'wake' else A_SPAWN; b[4] = g
        elif t[0] == 'warp' and len(t) == 3:
            if b[0] & 15 != EV_TILE: raise ChapterError(f"{where}: warp needs a 'tile' condition")
            x, y = int(t[1]), int(t[2])
            if not walkable(x, y): raise ChapterError(f"{where}: warp target {x},{y} can't be stood on")
            b[3] |= A_WARP; b[7], b[8] = x, y
        elif t[0] == 'set' and len(t) == 2: b[3] |= A_SET; b[5] = flag_number(t[1], where)
        elif t[0] == 'until' and len(t) == 2: b[6] = flag_number(t[1], where)
        elif t[0] == 'needs' and len(t) == 2:
            b[9] = flag_number(t[1].lstrip('!'), where) | (0x80 if t[1].startswith('!') else 0)
        elif t[0] == 'join' and len(t) == 1:
            if b[0] & 15 != EV_TALK: raise ChapterError(f"{where}: join needs a 'talk' condition")
            b[3] |= A_JOIN
        elif t[0] == 'win' and len(t) == 1: b[3] |= A_WIN
        elif t[0] == 'demon' and len(t) == 1: b[0] |= EV_DEMON
        elif t[0] == 'repeat' and len(t) == 1: b[0] |= EV_REPEAT
        else: raise ChapterError(f"{where}: unknown action '{a}' (wake G, spawn G, warp X Y, set N, until N, needs N, join, win, demon, repeat)")
    if b[3] & A_WARP: b[0] |= EV_REPEAT          # a portal works every time
    if (b[0] & 15) == EV_TALK and not b[3] & A_JOIN and not e.get('has_scene'):
        raise ChapterError(f"{where}: a talk event with no 'join' needs a scene (or it does nothing)")
    if b[0] & EV_REPEAT and (b[0] & 15) == EV_TURN and not b[2]:
        raise ChapterError(f"{where}: a repeating turn event needs 'every K'")
    if b[0] & EV_REPEAT and (b[0] & 15) == EV_TALK:
        raise ChapterError(f"{where}: talk events can't repeat")
    return b


def compile_chapter(path, last_number):
    fn = os.path.basename(path)
    ch = parse(path)
    number, side = last_number + 1, False          # chapters number themselves; 'label: 3X' for a side chapter
    if 'label' in ch:
        m = re.fullmatch(r'(\d+)(X?)', ch['label'][0].upper())
        if not m: raise ChapterError(f"{ch['label'][1]}: label is a number, optionally followed by X (e.g. 3X)")
        number, side = int(m.group(1)), bool(m.group(2))
    for k in ('title', 'goal', 'objective', 'deploy'):
        if k not in ch: raise ChapterError(f"{fn}: missing '{k}:'")
    if len(ch['title'][0]) > 14: raise ChapterError(f"{ch['title'][1]}: title is {len(ch['title'][0])} chars; the chapter card fits 14")
    if len(ch['goal'][0]) > 16: raise ChapterError(f"{ch['goal'][1]}: goal is {len(ch['goal'][0])} chars; the card's plank fits 16")
    # map
    rows = ch['map']
    if len(rows) != MAP_H: raise ChapterError(f"{fn}: map has {len(rows)} rows (need {MAP_H})")
    terr = []
    for r, (row, where) in enumerate(rows):
        if len(row) != MAP_W: raise ChapterError(f"{where}: map row is {len(row)} wide (need {MAP_W})")
        for c in row:
            if c not in LEGEND: raise ChapterError(f"{where}: unknown map symbol '{c}'")
        terr.append([LEGEND[c] for c in row])
    def walkable(x, y): return 0 <= x < MAP_W and 0 <= y < MAP_H and terr[y][x] in PASSABLE
    occupied = {}
    def place(x, y, who, where):
        if not walkable(x, y): raise ChapterError(f"{where}: {who} starts on an unwalkable tile {x},{y}")
        if (x, y) in occupied: raise ChapterError(f"{where}: {who} shares {x},{y} with {occupied[(x, y)]}")
        occupied[(x, y)] = who
    # objective
    o = ch['objective'][0].split()
    if o[0] not in OBJ: raise ChapterError(f"{ch['objective'][1]}: objective must be one of {list(OBJ)}")
    ox = oy = turns = 0
    if o[0] == 'seize':
        ox, oy = int(o[1]), int(o[2])
        if not walkable(ox, oy): raise ChapterError(f"{ch['objective'][1]}: seize tile {ox},{oy} is not walkable")
    if o[0] == 'survive': turns = int(o[1])
    # deployment
    deploy = int(ch['deploy'][0])
    if not 1 <= deploy <= MAX_SLOTS: raise ChapterError(f"{ch['deploy'][1]}: deploy must be 1..{MAX_SLOTS}")
    if len(ch['slots']) < deploy: raise ChapterError(f"{fn}: deploy {deploy} but only {len(ch['slots'])} slots")
    if len(ch['slots']) > MAX_SLOTS: raise ChapterError(f"{fn}: more than {MAX_SLOTS} slots")
    slots = []
    for i, (x, y, where) in enumerate(ch['slots']):
        place(x, y, 'the lord' if i == 0 else f'slot {i}', where); slots += [x, y]
    slots += [255, 255] * (MAX_SLOTS - len(ch['slots']))
    # joins
    joins = 0
    if 'joins' in ch:
        for nm in [s.strip() for s in ch['joins'][0].split(',') if s.strip()]:
            if nm not in NAME_ID or NAME_ID[nm] >= G.NUM_HEROES: raise ChapterError(f"{ch['joins'][1]}: {nm} isn't a hero")
            if G.HEROES[nm].get('joined', True): raise ChapterError(f"{ch['joins'][1]}: {nm} starts in the army already")
            joins |= 1 << NAME_ID[nm]
    # captive
    cap = [255, 0, 0, 0]
    if 'captive' in ch:
        c, where = ch['captive']
        p = c.split()
        name = p[0]
        while len(p) > 3 and not p[1].isdigit(): p = [p[0] + ' ' + p[1]] + p[2:]; name = p[0]
        if name not in NAME_ID or NAME_ID[name] >= G.NUM_HEROES: raise ChapterError(f"{where}: captive must be a hero name")
        if G.HEROES[name].get('joined', True): raise ChapterError(f"{where}: {name} is already in the army (joined=True in game_data)")
        cx, cy = int(p[1]), int(p[2])
        leaves = 0
        if len(p) > 3:
            if p[3] != 'leaves' or len(p) < 5: raise ChapterError(f"{where}: captive line is NAME X Y [leaves TURN]")
            leaves = int(p[4])
        place(cx, cy, name, where)
        if not any(walkable(cx + dx, cy + dy) for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1))):
            raise ChapterError(f"{where}: nobody can stand next to {name} to talk")
        cap = [NAME_ID[name], cx, cy, leaves]
        if not ch['scenes']['recruit']: raise ChapterError(f"{fn}: a captive needs a 'scene recruit:'")
        if leaves and not ch['scenes']['taken']: raise ChapterError(f"{fn}: a captive who leaves needs a 'scene taken:'")
    elif ch['scenes']['recruit'] or ch['scenes']['warn'] or ch['scenes']['taken']:
        raise ChapterError(f"{fn}: recruit/warn/taken scenes need a 'captive:'")
    # villages
    if len(ch['villages']) > MAX_VILLAGES: raise ChapterError(f"{fn}: more than {MAX_VILLAGES} villages/chests")
    vill = []
    vtiles = {(x, y) for y in range(MAP_H) for x in range(MAP_W) if terr[y][x] in VISITABLE}
    for v in ch['villages']:
        sym = 'V' if v['kind'] == 'village' else 'C'
        if (v['x'], v['y']) not in vtiles or terr[v['y']][v['x']] != LEGEND[sym]:
            raise ChapterError(f"{v['where']}: {v['kind']} {v['x']},{v['y']} isn't a {sym} on the map")
        vtiles.discard((v['x'], v['y']))
        if not v['lines']: raise ChapterError(f"{v['where']}: a village needs something to say")
        gift = 0
        if v['gift']:
            if v['gift'][0] not in ITEM_ID: raise ChapterError(f"{v['gift'][1]}: unknown item '{v['gift'][0]}'")
            gift = ITEM_ID[v['gift'][0]]
        if v.get('gold'):
            amt, where = v['gold']
            if gift: raise ChapterError(f"{where}: a village gives an item or gold, not both")
            if not 10 <= amt <= 1270 or amt % 10: raise ChapterError(f"{where}: gold is 10..1270, in tens")
            gift = 0x80 | amt // 10
        vill += [v['x'], v['y'], gift, v['flag']]
    if vtiles: raise ChapterError(f"{fn}: village/chest tile(s) {sorted(vtiles)} have no 'village X Y:' / 'chest X Y:' block")
    vill += [0, 0, 0, 0] * (MAX_VILLAGES - len(ch['villages']))
    # enemies
    if len(ch['enemies']) > MAX_ENEMIES: raise ChapterError(f"{fn}: {len(ch['enemies'])} enemies (max {MAX_ENEMIES})")
    eb = b''
    bosses = 0
    groups = {}                       # group -> kinds ('reserve' / 'ambush' / 'g')
    unit_index = {}                   # name -> unit index (None if the name isn't unique)
    talk_targets = set()
    for e in ch['events']:
        w = e['spec'].split(';')[0].split()
        if w and w[0] == 'talk' and 'to' in w: talk_targets.add(' '.join(w[w.index('to') + 1:]))
    for k, (p, flags, items, where) in enumerate(ch['enemies']):
        name, cls = p[0], p[1]
        if name not in NAME_ID: raise ChapterError(f"{where}: unknown name '{name}' -- add it to game_data.NAMES")
        if NAME_ID[name] < G.NUM_HEROES:
            if name not in talk_targets: raise ChapterError(f"{where}: {name} is a hero -- an enemy can only carry a hero's name if a 'talk ... to {name}' event can recruit them")
            if G.HEROES[name].get('joined', True): raise ChapterError(f"{where}: {name} starts in the army, so can't be fought here")
        if cls not in CLASS_ID: raise ChapterError(f"{where}: unknown class '{cls}'")
        x, y, lv, hp, st, sk, sp, df = map(int, p[2:10])
        place(x, y, name, where)
        f = F_ALIVE | F_ENEMY
        group = 0
        for t in flags:
            if t == 'boss': f |= F_BOSS; bosses += 1
            elif t == 'hold': f |= F_HOLD
            elif t.startswith('reserve'): f |= F_RESERVE; group = int(t[7:] or 1); groups.setdefault(group, set()).add('reserve')
            elif t.startswith('ambush'):
                if 'boss' in flags: raise ChapterError(f"{where}: an ambusher can't be the boss")
                f = (f & ~F_ALIVE) | F_HIDDEN; group = int(t[6:] or 1); groups.setdefault(group, set()).add('ambush')
            elif t.startswith('g'): group = int(t[1:]); groups.setdefault(group, set()).add('g')
        if not 0 <= group <= 255 or (group == 0 and any(t.startswith(('reserve', 'ambush', 'g')) for t in flags)):
            raise ChapterError(f"{where}: groups are 1..255")
        if (f & F_HIDDEN) and NAME_ID[name] < G.NUM_HEROES: raise ChapterError(f"{where}: a recruitable can't start hidden")
        ids, uses = item_slots(items, where)
        if any(ids) or 'hold' not in flags: check_weapon(CLASS_ID[cls], ids, where)   # unarmed + hold: a bystander
        eb += unit_bytes(x, y, CLASS_ID[cls], NAME_ID[name], hp, st, sk, sp, df, f, lv, group, ids, uses)
        unit_index[name] = None if name in unit_index else G.NUM_HEROES + k
    if o[0] == 'boss' and not bosses: raise ChapterError(f"{fn}: objective is 'boss' but no enemy has the boss flag")
    if bosses and not ch['scenes']['boss']: print(f"  note: {fn} has a boss but no 'scene boss:'")
    # events
    if len(ch['events']) > MAX_EVENTS: raise ChapterError(f"{fn}: more than {MAX_EVENTS} events")
    def unit_of(nm, where):
        if nm not in unit_index: raise ChapterError(f"{where}: no enemy called {nm} in this chapter")
        if unit_index[nm] is None: raise ChapterError(f"{where}: more than one {nm} -- give the one you mean a unique name")
        return unit_index[nm]
    evs = []
    for e in ch['events']:
        e['has_scene'] = bool(ch['scenes'].get(e['name']))
        b = compile_event(e, unit_of, walkable, groups, e['where'])
        if e['has_scene']: b[3] |= A_SCENE
        if b[3] & A_JOIN:
            tgt = ch['enemies'][b[2] - G.NUM_HEROES][0][0]
            if NAME_ID[tgt] >= G.NUM_HEROES:
                raise ChapterError(f"{e['where']}: 'join' needs the talk target to carry a hero's name (a hero with joined=False)")
        evs.append(b)
    woken = {b[4] for b in evs if b[3] & (A_WAKE | A_SPAWN)}
    for g, kinds in groups.items():
        if ('reserve' in kinds or 'ambush' in kinds) and g not in woken:
            raise ChapterError(f"{fn}: group {g} has reserves/ambushers but no event wakes or spawns it")
    evb = b''.join(bytes(b) for b in evs) + bytes(EV_SIZE * (MAX_EVENTS - len(evs)))
    for nm in ch['scenes']:
        if nm not in SCENES and not any(e['name'] == nm for e in ch['events']):
            raise ChapterError(f"{fn}: 'scene {nm}' matches no event (scenes: {SCENES} or an event's name)")
    requires = 0
    if 'requires' in ch:
        p = ch['requires'][0].split()
        if len(p) != 2 or p[0] != 'flag': raise ChapterError(f"{ch['requires'][1]}: requires is 'flag N'")
        requires = flag_number(p[1], ch['requires'][1], story_only=True)
    # scenes: 7 named + one per village slot + one per event slot
    if not ch['scenes']['open']: raise ChapterError(f"{fn}: 'scene open' needs at least one line")
    sb = [scene_bytes(ch['scenes'][k]) for k in SCENES]
    sb += [scene_bytes(v['lines']) for v in ch['villages']] + [bytes([0])] * (MAX_VILLAGES - len(ch['villages']))
    sb += [scene_bytes(ch['scenes'].get(e['name'], [])) for e in ch['events']] + [bytes([0])] * (MAX_EVENTS - len(ch['events']))
    packed = bytearray()
    for y in range(MAP_H):
        for x in range(0, MAP_W, 2):
            packed.append(terr[y][x] << 4 | terr[y][x + 1])
    shop = []
    if 'shop' in ch:
        for nm in [x.strip() for x in ch['shop'][0].split(',') if x.strip()]:
            if nm not in ITEM_ID: raise ChapterError(f"{ch['shop'][1]}: unknown item '{nm}'")
            if not G.PRICES.get(nm): raise ChapterError(f"{ch['shop'][1]}: {nm} has no price in game_data.PRICES")
            shop.append(ITEM_ID[nm])
        if len(shop) > SHOP_SIZE: raise ChapterError(f"{ch['shop'][1]}: at most {SHOP_SIZE} items on sale")
    shop += [0] * (SHOP_SIZE - len(shop))
    reward = int(ch['reward'][0]) if 'reward' in ch else 200 + 50 * number
    if not 0 <= reward <= 2550 or reward % 10: raise ChapterError(f"{fn}: reward is 0..2550 gold, in tens")
    header = bytes([OBJ[o[0]], ox, oy, turns, len(ch['enemies']), deploy, len(ch['slots'])] + cap +
                   [len(ch['villages']), len(ch['events']), requires, number | (0x80 if side else 0), joins >> 8, joins & 255] +
                   shop + [reward // 10] + slots + vill) + evb
    assert len(header) == H_SCENES, len(header)
    off = H_MAP + len(packed) + len(eb)
    scene_off = []
    for b in sb:
        scene_off += [off >> 8, off & 255]; off += len(b)
    rec = header + bytes(scene_off) + bytes(packed) + eb + b''.join(sb)
    assert H_SCENES + len(scene_off) == H_MAP
    sets = {v['flag'] for v in ch['villages'] if v['flag']} | {b[5] for b in evs if b[3] & A_SET}
    return rec, dict(title=ch['title'][0].upper(), goal=ch['goal'][0].upper(), file=fn, label=f"{number}{'X' if side else ''}",
                     requires=requires, number=number, sets=sorted(sets))


def write_story_h(n_chapters, side_flags):
    L = ["// Generated by tools/chapters/make_chapters.py from game_data.py -- edit those, not this.",
         "#pragma once", "#include <avr/pgmspace.h>", "",
         f"const uint8_t NUM_HEROES = {G.NUM_HEROES};", f"const uint8_t NUM_CHAPTERS = {n_chapters};",
         f"const uint8_t MAX_ENEMIES = {MAX_ENEMIES};", f"const uint8_t PROMOTE_LEVEL = {G.PROMOTE_LEVEL};",
         f"const uint8_t NUM_CLASSES = {len(G.CLASSES)};",
         f"const uint16_t SIDE_FLAGS = 0x{side_flags:04X};   // the story flags that open side chapters (the record counts them)", "",
         "enum Kind : uint8_t { " + ", ".join(f"K_{k}" for k in G.KINDS) + ", K_NONE = 255 };",
         "enum Cls : uint8_t { " + ", ".join(f"C_{c[0].replace(' ', '_')}" for c in G.CLASSES) + " };",
         "enum : uint8_t { F_ALIVE = 1, F_ENEMY = 2, F_MOVED = 4, F_HOLD = 8, F_LORD = 16, F_BOSS = 32, F_FALLEN = 64, F_JOINED = 128 };",
         "enum : uint8_t { F_HIDDEN = F_FALLEN, F_RESERVE = F_JOINED, F_BOOTS = F_BOSS };   // on an enemy: waiting in ambush / in reserve; on a hero: winged boots",
         f"const uint8_t MAX_SLOTS = {MAX_SLOTS}, MAX_VILLAGES = {MAX_VILLAGES}, MAX_EVENTS = {MAX_EVENTS}, EV_SIZE = {EV_SIZE};",
         f"const uint8_t CH_SLOTS = {H_SLOTS}, CH_VILL = {H_VILL}, CH_EVENTS = {H_EVENTS}, CH_SCENES = {H_SCENES}, CH_MAP = {H_MAP}, CH_REQUIRES = 13, CH_LABEL = 14, CH_SHOP = {H_SHOP}, SHOP_SIZE = {SHOP_SIZE}, CH_REWARD = {H_SHOP + SHOP_SIZE};",
         f"const uint8_t ITEM_SLOTS = {ITEM_SLOTS};",
         f"enum : uint8_t {{ EV_TILE = {EV_TILE}, EV_TURN, EV_FALL, EV_TALK, EV_REPEAT = 0x10, EV_DEMON = 0x20 }};",
         "enum : uint8_t { A_WAKE = 1, A_SPAWN = 2, A_WARP = 4, A_SET = 8, A_JOIN = 16, A_SCENE = 32, A_WIN = 64 };",
         "const uint8_t USES_DROP = 0x80;   // item flag in a uses byte: the enemy drops it", "",
         "struct Unit { uint8_t x, y, cls, name, hp, maxHp, str, skl, spd, def, flags, lv, exp, item[ITEM_SLOTS], uses[ITEM_SLOTS]; };",
         "struct ItemDef { uint8_t kind, might, hit, crit, minR, maxR, uses, price; };   // uses > 1 on a weapon: it wears out; price / 10 gold",
         "// promo: the class a HERO CREST makes it (255 none); tier 1 = promoted; bonus: HP STR SKL SPD DEF on promotion",
         "struct ClassDef { uint8_t weapon, weapon2, mov, promo, tier, bonus[5]; };", ""]
    n_names, n_cls, n_items = len(G.NAMES), len(G.CLASSES), len(G.ITEMS)
    L.append(f"// names live on FX (fxdata/strings.bin): {STR_LEN}-byte records, read with str()")
    str_ui = n_names + n_cls + n_items + len(TERRAIN_NAMES)
    if str_ui + len(UI) > 255: raise ChapterError("strings.bin: more than 255 records (str() takes a byte)")
    L.append(f"enum : uint8_t {{ STR_LEN = {STR_LEN}, STR_NAME = 0, STR_CLASS = {n_names}, STR_ITEM = {n_names + n_cls}, STR_TERRAIN = {n_names + n_cls + n_items}, STR_UI = {str_ui} }};")
    L.append("enum : uint8_t { " + ", ".join(f"U_{k} = {str_ui + i}" for i, (k, _) in enumerate(UI)) + " };")
    L.append("// fxdata/tables.bin: per name (portrait, voice, figure); the heroes at a new game (Unit records); their growths")
    tb_death = 3 * n_names + 21 * G.NUM_HEROES + 5 * G.NUM_HEROES
    tb_items = tb_death + DEATH_SLOT * G.NUM_HEROES
    L.append(f"const uint16_t TB_HEROES = {3 * n_names}, TB_GROWTH = {3 * n_names + 21 * G.NUM_HEROES}, TB_DEATH = {tb_death}, DEATH_SLOT = {DEATH_SLOT};")
    L.append(f"const uint16_t TB_ITEMS = {tb_items}, TB_CLASSES = {tb_items + 8 * len(G.ITEMS)};   // ItemDef / ClassDef records")
    L.append("")
    # hot fields stay in program flash (the AI reads them constantly); the full records go to FX
    cls_fx, arms = bytearray(), []
    for i, c in enumerate(G.CLASSES):
        w = class_weapons(i)
        if len(c) > 4 and c[4] and c[4] not in CLASS_ID: raise ChapterError(f"game_data.CLASSES: {c[0]} promotes to unknown {c[4]}")
        promo = CLASS_ID[c[4]] if len(c) > 4 and c[4] else 255
        tier = 1 if len(c) > 5 else 0
        bonus = c[5] if tier else (0, 0, 0, 0, 0)
        k1, k2 = KIND_ID[w[0]], (KIND_ID[w[1]] if len(w) > 1 else 255)
        cls_fx += bytes([k1, k2, c[3], promo, tier] + list(bonus))
        arms.append(f"{{ K_{w[0]}, {('K_' + w[1]) if len(w) > 1 else 'K_NONE'}, {c[3]} }}")
    L.append("// per class: weapon kinds and move (the rest -- promo, tier, bonus -- is on FX: ClassDef records)")
    L.append("const uint8_t CLASS_ARMS[][3] PROGMEM = { " + ", ".join(arms) + " };")
    L.append(f"const uint8_t NUM_ITEMS = {len(G.ITEMS)};")
    L.append("const uint8_t K_COUNTED = 0x80;   // in ITEM_KIND: used up (potion, crest) or wears out (a fine weapon)")
    L.append("const uint8_t ITEM_KIND[] PROGMEM = { " + ", ".join(f"K_{it[1]}" + (" | K_COUNTED" if it[1] in ('HEAL', 'PROMO') or it[7] > 1 else "") for it in G.ITEMS) + " };")
    item_fx = bytearray()
    for it in G.ITEMS: item_fx += bytes([KIND_ID[it[1]]] + list(it[2:8]) + [G.PRICES.get(it[0], 0) // 10])
    L.append("")
    tables = bytearray()
    for n in G.NAMES: tables += bytes([255 if n[1] is None else n[1], n[2], n[3]])
    growth = []
    for name, *_ in G.NAMES[:G.NUM_HEROES]:
        h = G.HEROES[name]
        ids, uses = item_slots(h['items'], f"game_data.HEROES[{name}]")
        check_weapon(CLASS_ID[h['cls']], ids, f"game_data.HEROES[{name}]")
        f = (F_JOINED if h.get('joined', True) else 0) | (F_LORD if h.get('lord') else 0)
        vals = [0, 0, CLASS_ID[h['cls']], NAME_ID[name], h['hp'], h['hp'], h['str'], h['skl'], h['spd'], h['df'], f, h['lv'], 0]
        tables += bytes(vals + ids + uses)
        growth.append(h['growth'])
    for g in growth: tables += bytes(g)
    for name, *_ in G.NAMES[:G.NUM_HEROES]:          # last words: a scene per hero, fixed-size slots
        lines = [(who, text, f"game_data.HEROES[{name}].death") for who, text in G.HEROES[name].get('death', [])]
        b = scene_bytes(lines)
        if len(b) > DEATH_SLOT: raise ChapterError(f"game_data.HEROES[{name}].death: {len(b)} bytes (max {DEATH_SLOT})")
        tables += b.ljust(DEATH_SLOT, b'\0')
    tables += item_fx + cls_fx
    with open(os.path.join(ROOT, 'fxdata', 'tables.bin'), 'wb') as f: f.write(tables)
    with open(os.path.join(ROOT, 'story.h'), 'w') as f:
        f.write("\n".join(L) + "\n")


def write_strings():
    recs = [n[0] for n in G.NAMES] + [c[1] for c in G.CLASSES] + [it[0] for it in G.ITEMS] + TERRAIN_NAMES
    out = bytearray()
    for r in recs:
        if len(r) > NAME_MAX: raise ChapterError(f"game_data: '{r}' is longer than {NAME_MAX} characters")
    for r in recs + [t for _, t in UI]:
        if len(r) >= STR_LEN: raise ChapterError(f"strings: '{r}' is longer than {STR_LEN - 1} characters")
        if any(not (32 <= ord(ch) <= 90) for ch in r): raise ChapterError(f"game_data: '{r}' has characters the font lacks")
        out += r.encode('ascii').ljust(STR_LEN, b'\0')
    with open(os.path.join(ROOT, 'fxdata', 'strings.bin'), 'wb') as f: f.write(out)


def load_all(src):
    files = sorted(glob.glob(os.path.join(src, '[0-9]*.txt')))
    if not files: raise ChapterError(f"no {src}/[0-9]*.txt found")
    out, last = [], 0
    for p in files:
        out.append(compile_chapter(p, last))
        last = out[-1][1]['number']
    seen = set()
    for _, m in out:
        if m['requires'] and m['requires'] not in seen:
            raise ChapterError(f"{m['file']}: requires flag {m['requires']}, which no earlier chapter's village/chest/event sets")
        seen |= {f for f in m['sets'] if f <= STORY_FLAGS}
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--src', default=os.path.join(ROOT, 'chapters'))
    ap.add_argument('--bin', default=os.path.join(ROOT, 'fxdata', 'chapters.bin'))
    ap.add_argument('--no-header', action='store_true', help="don't touch story.h / chapters.json (test fixtures)")
    a = ap.parse_args()
    try:
        chs = load_all(a.src)
    except ChapterError as e:
        print("CHAPTER ERROR:", e); sys.exit(1)
    blob = bytearray([len(chs)])
    off = 1 + 3 * len(chs)
    for rec, _ in chs:
        blob += bytes([off >> 16 & 255, off >> 8 & 255, off & 255]); off += len(rec)
    for rec, _ in chs: blob += rec
    os.makedirs(os.path.dirname(os.path.abspath(a.bin)), exist_ok=True)
    with open(a.bin, 'wb') as f: f.write(blob)
    if not a.no_header:
        with open(os.path.join(ROOT, 'fxdata', 'chapters.json'), 'w') as f: json.dump([m for _, m in chs], f, indent=1)
        write_story_h(len(chs), sum(1 << (m['requires'] - 1) for _, m in chs if m['requires']))
        write_strings()
    for (rec, m) in chs:
        print(f"  {m['file']}: '{m['title']}' -- {len(rec)} bytes")
    print(f"{os.path.basename(a.bin)}: {len(blob)} bytes, {len(chs)} chapters" + ("" if a.no_header else "; story.h written"))


if __name__ == '__main__':
    main()
