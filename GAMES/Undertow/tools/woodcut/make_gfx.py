#!/usr/bin/env python3
"""Build every graphic for Undertow. All of it lives on the FX flash chip:

  fxdata/: map tiles, map unit sprites, the 3x5 woodcut font, battle figures,
           battle backdrops, forecast shields, portraits, chapter cards
           -> run fxdata-build.py after (tools/build_data.sh does it all)

Usage:  python3 tools/woodcut/make_gfx.py     (from the sketch folder)
"""
import math, os, sys
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, HERE)
from wood import *                       # noqa
import figures

GFX = os.path.join(ROOT, 'fxdata', 'gfx')
os.makedirs(GFX, exist_ok=True)


# ======================================================== PROGMEM: map art
def g(rows): return [[ch == '#' for ch in r] for r in rows]

TILES = [  # order == Terrain enum in the sketch (+ extra grass variants at the end)
 ('PLAIN', ["........", "........", "........", "........", "........", "........", "........", "........"]),
 ('WOOD',  ["...#....", "..###...", "...#....", ".#####..", "..###...", "#######.", "...#....", "..###..."]),   # spruce
 ('HILL',  ["........", "...#....", "..###...", "..#.#...", ".##.##..", ".#...#..", "#######.", "........"]),
 ('WATER', ["########", "##..####", "#.##.###", "########", "#####..#", "####.##.", "########", "########"]),
 ('WALL',  ["########", "#...#...", "########", "..#...#.", "########", "#...#...", "########", "........"]),
 ('KEEP',  ["#.#.#.#.", "#######.", "##...##.", "#.###.#.", "#######.", "##.#.##.", "##.#.##.", "........"]),
 ('FORD',  ["########", "#..#####", "#..##..#", "#####..#", "##.#####", "#..#..##", "#..#####", "########"]),
 ('GRASS1',["........", "........", ".###....", "........", "........", "........", "........", "........"]),
 ('GRASS2',["........", "........", "........", "........", "........", "....###.", "........", "........"]),
 ('WOOD2', [".#......", "###..#..", ".#..###.", "###..#..", "###.###.", ".#.#####", "....#...", "........"]),   # twin pines
 ('VILLAGE', ["#.......", ".#......", "#..###..", "..#####.", ".#######", "..#...#.", "..#...#.", "..##.##."]),   # smoke: someone's home
 ('VILLAGE_DONE', ["........", "........", "...###..", "..#####.", ".#######", "..#####.", "..##.##.", "..##.##."]),   # visited: door shut
 ('CHEST',      ["........", ".######.", "#......#", "########", "#..##..#", "#..#...#", "#......#", "########"]),
 ('CHEST_OPEN', [".######.", "#......#", "#.####.#", "........", "########", "#......#", "#......#", "########"]),   # lid up, emptied
 ('PALISADE',   ["#..#..#.", "##.##.##", "##.##.##", "##.##.##", "########", "##.##.##", "##.##.##", "##.##.##"]),   # sharpened stakes
 ('GATE',       ["########", "#..##..#", "#..##..#", "#......#", "#......#", "#......#", "#......#", "#......#"]),   # gateway with its bell
 ('PORTAL',     ["..####..", ".#....#.", "#..##..#", "#.#..#.#", "#.#.#..#", "#..#...#", ".#....#.", "..####.."]),   # a swirl: step on it to cross
 ('FLOOR',      ["#.......", "........", "........", "........", "....#...", "........", "........", "........"]),   # flagstones: halls, vaults, caves
 ('DECK',       ["........", "........", "#.#.#...", "........", "........", "........", "....#.#.", "........"]),   # a ship's planking: dotted seams
 ('RAIL',       ["########", "#.#.#.#.", "#.#.#.#.", "#.#.#.#.", "########", "........", "........", "........"]),   # gunwale: impassable
]

# map sprite per class, in game_data.CLASSES order; promoted classes reuse their base
# class's sprite and shield charge, marked with a pennant / a second rim
import demons
sys.path.insert(0, os.path.join(HERE, '..', 'chapters'))
import game_data as GD
BASE_SPR = {'LORD': 'horse', 'LANCER': 'lance', 'FIGHTER': 'axe', 'ARCHER': 'bow', 'KNIGHT': 'boss', 'CLERIC': 'staff', 'HORROR': 'drowned',
            'CAVALIER': 'cavalier', 'SWORDSMAN': 'sword', 'MAGE': 'tome', 'WRAITH': 'bellwraith', 'SKYRIDER': 'eagle', 'MONK': 'monk'}
PROMOTED_FROM = {c[4]: c[0] for c in reversed(GD.CLASSES) if len(c) > 4 and c[4]}   # first base wins (SAGE <- MAGE)
CLASS_SPR = [(BASE_SPR[c[0]], False) if c[0] in BASE_SPR else (BASE_SPR[PROMOTED_FROM[c[0]]], True) for c in GD.CLASSES]
SPR['drowned'] = demons.SPR['drowned']
SPR['bellwraith'] = demons.SPR['bellwraith']
SPR['tome'] = ["......#.", "..#..###", ".###..#.", ".#.#.#..", ".###....", "######..", ".####...", "######.."]   # hooded caster, a spark at the hand
SPR['cavalier'] = ["...##..#", "...##..#", "..####.#", "##.##..#", "#######.", ".######.", ".#.#.#..", ".#.#.#.."]   # horse + lance
SPR['eagle'] = ["#.......", "##...##.", "###.####", ".######.", "..####..", "..#####.", "..#.#...", ".##.##.."]   # wings up, a rider on its back
SPR['monk'] = ["...##...", "..####..", "..#..#..", ".######.", ".#.##.#.", ".######.", ".######.", "##.##.##"]    # bald head, a bell on the robe
SPR['staff'] = ["......#.", "..##.###", "..##..#.", ".####.#.", ".####.#.", ".####.#.", "######.#", "......#."]


def sprite_px(name, enemy, spent, promoted=False):
    pat = [list(r) for r in SPR[name]]
    if promoted:                                                    # a pennant in the top corner
        for (x, y) in ((0, 0), (1, 0), (2, 0), (0, 1), (1, 1), (0, 2)): pat[y][x] = '#'
    px = [[False] * 8 for _ in range(8)]
    for y in range(8):
        for x in range(8):
            sx = 7 - x if enemy else x
            on = pat[y][sx] == '#'
            if on and enemy: on = (x - y) % 3 != 0 or y == 0        # hatched, faces left
            if on and spent: on = (x + y) % 2 == 0                     # dithered once it has acted
            px[y][x] = on
    return px


def to_bytes(px):
    return [sum(1 << y for y in range(8) if px[y][x]) for x in range(8)]


# 3x5 font: printable ASCII 32..90, variable width
FONT_CHARS = ''.join(chr(c) for c in range(32, 91))


def font_table():
    out = []
    for ch in FONT_CHARS:
        gph = FONT.get(ch, FONT.get('?') if ch not in FONT else None)
        if ch not in FONT: gph = None
        if gph is None:
            out.append((0, [0, 0, 0], ch)); continue
        w = len(gph[0])
        cols = [sum(1 << j for j in range(5) if gph[j][i] == '#') for i in range(w)]
        out.append((w, cols + [0] * (3 - w), ch))
    return out


def px_canvas(px):
    c = C(8, 8)
    for y in range(8):
        for x in range(8): c.px(x, y, 1 if px[y][x] else 0)
    return c


def map_art():
    """FX strips: tiles (TILES order) and unit sprites (player, enemy, spent x every class)."""
    tiles = [px_canvas(g(rows)) for _, rows in TILES]
    units = [px_canvas(sprite_px(n, enemy, spent, promo))
             for enemy, spent in ((False, False), (True, False), (False, True)) for n, promo in CLASS_SPR]
    return tiles, units


def write_font_bin():
    """3x5 font, chars 32..90: width, then 3 column bytes (bit0 = top); missing glyphs become '?'"""
    tab = font_table()
    q = next(t for t in tab if t[2] == '?')
    out = bytearray()
    for w, cols, ch in tab:
        if not w: w, cols = q[0], q[1]
        out += bytes([w] + cols)
    with open(os.path.join(ROOT, 'fxdata', 'font.bin'), 'wb') as f: f.write(out)


# ======================================================== FX: battle backdrop
def backdrop():
    c = C(128, 56)
    # sun: disc with petal rays (top centre)
    sx, sy = 64, 9
    fill(c, ellipse(sx, sy, 4.5, 4.5))
    for k in range(8):
        a = math.radians(k * 45 + 22)
        petal = ellipse(sx + 8 * math.cos(a), sy + 8 * math.sin(a), 1.6, 1.6)
        fill(c, petal)
    # far ridge with a spiky tree line, carved as dotted contours
    def ridge(x): return 31 + 3 * math.sin(x / 13.0) + 1.5 * math.sin(x / 5.1)
    for x in range(128):
        r = int(ridge(x))
        if x % 2 == 0: c.px(x, r)
        if (x // 3) % 2 == 0 and x % 2 == 1: c.px(x, r + 5)
    for tx in range(3, 128, 9):
        base = int(ridge(tx)) - 1
        hgt = 4 + (tx * 7) % 4
        for k in range(hgt):
            for d in range(-(hgt - k) // 2, (hgt - k) // 2 + 1): c.px(tx + d, base - k)
    # near field: dotted rows receding
    # near field kept sparse so the fighters stay readable
    for x in range(44, 84, 6): c.px(x, 41)
    # ground band: white with carved dashes (figures stand on y=46)
    c.rect(0, 47, 128, 9, 1)
    for y in range(49, 56, 3):
        for x in range((y * 5) % 11, 128, 11):
            c.rect(x, y, 5 if (x + y) % 3 else 3, 1, 0)
    return c


def ground(c):
    """the shared ground band: white with carved dashes (figures stand on y=46)"""
    c.rect(0, 47, 128, 9, 1)
    for y in range(49, 56, 3):
        for x in range((y * 5) % 11, 128, 11):
            c.rect(x, y, 5 if (x + y) % 3 else 3, 1, 0)


def spruce(c, x, base, h, solid=True):
    for k in range(h):
        half = (h - k) // 2 if k % 3 else (h - k) // 2 + 1
        for d in range(-half, half + 1):
            if solid or abs(d) == half or k % 3 == 0: c.px(x + d, base - k)
    c.rect(x, base, 1, 3, 1)


def back_forest():
    c = C(128, 56)
    for x in range(0, 128, 2): c.px(x, 44)                                  # forest floor
    for k, x in enumerate(range(4, 128, 9)):                                 # far rank: small, solid
        spruce(c, x, 29 + (k * 5) % 3, 8 + (k * 7) % 4)
    for x in range(0, 128, 2): c.px(x, 33 + (x // 2) % 2)                   # the wood's edge
    for x, h in ((5, 34), (122, 30), (60, 14)):                             # a few near trunks, framing the fight
        spruce(c, x, 43, h, solid=False)
    return c


def back_hills():
    c = C(128, 56)
    for i, (amp, base, per, ph) in enumerate(((6, 18, 23.0, 0), (5, 27, 17.0, 2), (4, 36, 13.0, 4))):
        for x in range(128):
            y = int(base + amp * math.sin(x / per + ph))
            c.px(x, y)
            if i == 0 and x % 4 == 0: c.px(x, y + 2)                        # far hill: a second, dotted line
            elif x % (3 + i) == 0: c.px(x, y + 2)
    for x in (14, 96, 117):                                                 # a few cairns
        y = int(36 + 4 * math.sin(x / 13.0 + 4)) - 1
        c.rect(x, y - 3, 3, 3, 1); c.px(x + 1, y - 4)
    return c


def back_river():
    c = C(128, 56)
    for x in range(128):                                                    # far bank
        y = int(24 + 2 * math.sin(x / 9.0))
        c.px(x, y)
        if x % 3 == 0: c.px(x, y - 1)
    for tx in range(4, 128, 13): spruce(c, tx, int(24 + 2 * math.sin(tx / 9.0)) - 1, 6, solid=False)
    for y in range(29, 45, 5):                                              # current: dashes drifting
        for x in range((y * 7) % 19, 128, 19):
            c.rect(x, y, 3 + (x + y) % 3, 1, 1)
    for x in range(128): c.px(x, 46 if x % 2 else 45)                      # near bank lip
    return c


def back_stronghold():
    c = C(128, 56)
    for x in range(0, 128, 6):                                              # the stakes, carved in outline
        h = 18 + (x * 7) % 5
        top = 44 - h
        for y in range(top + 2, 44): c.px(x + 1, y); c.px(x + 4, y)
        c.px(x + 2, top + 1); c.px(x + 3, top + 1); c.px(x + 2, top); 
    for x in range(0, 128, 2): c.px(x, 31)                                  # a dotted cross rail
    tower = rectm(104, 6, 18, 38)
    c.rect(102, 4, 22, 41, 0)
    fill(c, tower, lambda x, y: (x - y) % 3 == 0 or x in (104, 121))
    for x in range(104, 122, 4): c.rect(x, 3, 2, 3, 1)                      # crenels
    c.rect(110, 14, 5, 7, 0)                                                # arrow slit window
    return c


def back_village():
    c = C(128, 56)
    for k, x in enumerate((2, 34, 92)):
        w, h = (24, 14) if k != 1 else (20, 12)
        base = 42
        roof = poly([(x - 2, base - h), (x + w // 2, base - h - 10), (x + w + 2, base - h)])
        fill(c, roof, lambda xx, yy: (xx + yy) % 3 != 0)
        for xx in range(x, x + w):
            for yy in range(base - h, base):
                if xx in (x, x + w - 1) or yy % 4 == 0: c.px(xx, yy)
        c.rect(x + w // 2 - 2, base - 6, 4, 6, 1)                           # door
        if k != 1:
            for j in range(6): c.px(x + w - 4 + (j % 2), base - h - 12 - j * 2)   # smoke
    for x in range(0, 128, 3): c.px(x, 44)                                   # fence line
    return c


def back_vault():
    """a vaulted hall: round arches on pillars, a brazier, the floor (halls, vaults, caves)"""
    c = C(128, 56)
    for k, x0 in enumerate(range(-8, 128, 34)):
        cx = x0 + 17
        for a in range(0, 181, 3):                                          # the arch, carved twice
            t = math.radians(a)
            for r, step in ((15, 1), (12, 2)):
                if a % (3 * step) == 0: c.px(round(cx + r * math.cos(t)), round(22 - r * math.sin(t) * 0.9))
        for x in (cx - 15, cx - 12, cx + 12, cx + 15):                      # pillars down to the floor
            for y in range(22, 46): c.px(x, y) if (y + x) % 2 == 0 or x in (cx - 15, cx + 15) else None
        for y in range(25, 46, 4): c.px(cx - 14, y); c.px(cx + 13, y)       # their masonry joints
        if k % 2 == 1:                                                      # a far doorway, solid dark
            for y in range(30, 46): c.px(cx - 3, y); c.px(cx + 3, y)
            for x in range(cx - 3, cx + 4): c.px(x, 29)
    bx = 64                                                                 # brazier: bowl, flame
    for x in range(bx - 4, bx + 5): c.px(x, 38)
    for x in range(bx - 3, bx + 4): c.px(x, 39)
    c.rect(bx, 40, 1, 6, 1)
    for j, w in enumerate((1, 2, 3, 2, 3, 2)):
        for d in range(-w // 2, w // 2 + 1):
            if (j + d) % 2 == 0: c.px(bx + d + (j % 2), 37 - j)
    return c


def back_ship():
    """on deck: open sea to the horizon, a mast and its rigging, the rail in front"""
    c = C(128, 56)
    for x in range(128): c.px(x, 24)                                        # the horizon
    for y in range(28, 42, 4):                                              # swell, in dashes
        for x in range((y * 5) % 13, 128, 13): c.rect(x, y, 4 + (x + y) % 3, 1, 1)
    mx = 64
    c.rect(mx, 0, 2, 44, 1)                                                 # the mast
    for k in range(2):                                                      # a furled sail on its yard
        y = 6 + k * 14
        c.rect(mx - 20, y, 42, 1, 1)
        for x in range(mx - 18, mx + 20, 2): c.px(x, y + 2 + (x // 2) % 2)
    for sx in (mx - 20, mx + 21):                                           # stays down to the rail
        for t in range(0, 40):
            x = sx + (t if sx > mx else -t) * 0.9; y = 6 + t
            if t % 2 == 0: c.px(round(x), round(y))
    for x in range(128): c.px(x, 44); c.px(x, 45 if x % 6 else 44)          # the rail
    for x in range(0, 128, 6): c.rect(x, 41, 1, 4, 1)
    return c


def backdrops():
    frames = [backdrop(), back_forest(), back_hills(), back_river(), back_stronghold(), back_village(), back_vault(), back_ship()]
    for f in frames[1:]: ground(f)
    return frames


# ======================================================== FX: forecast shields
CHARGES = {
 'horse': ["......#.#...", ".....#####..", "....###.###.", "....########", "...#########", "...####..###", "..####......",
           "..#####.....", ".#######....", ".########...", "..########..", ".##########."],
 'lance': ["..........#.", ".........###", "..........#.", ".........#..", "........#...", ".......#....", "......#.....",
           ".....#......", "....#.......", "...#........", "..#.........", ".#.........."],
 'axe':   ["....####....", "...######...", "..#######...", "..####.#....", "...##..#....", "......#.....", ".....#......",
           "....#.......", "...#........", "..#.........", ".#..........", "#..........."],
 'bow':   ["...#........", "....#.......", "....##......", "....#.#.....", "....#..#....", "....#...#...", "....#...#...",
           "....#..#....", "....#.#.....", "....##......", "....#.......", "...#........"],
 'boss':  ["....####....", "...######...", "..########..", "..#......#..", "..########..", "..##.##.##..", "..########..",
           "...######...", "....####....", "...#.##.#...", "..#..##..#..", "....####...."],
 'drowned': ["#..#..#.....", "#..#..#..#..", "#..#..#..#..", "##.##.##.#..", ".#######.#..", ".########...", "..######....",
             "...####.....", "...####.....", "............", "#.##.##.##.#", ".##.##.##.##"],   # a hand out of the water
 'sword': ["...........#", "..........#.", ".........#..", "........#...", ".......#....", "..#...#.....", "...#.#......",
           "....#.......", "...#.#......", "..#...#.....", ".#..........", "#..........."],
 'flame': [".....#......", ".....##.....", "....###.....", "....####..#.", "...#####.##.", "..##.####.#.", "..#..#####..",
           ".##..##.###.", ".##.#.#.###.", ".###....###.", "..###..###..", "...######..."],
 'eagle': ["#..........#", "##........##", "###......###", ".####..####.", "..###..###..", "...######...", "....####....",
           "....####....", ".....##.....", "....####....", "...#.##.#...", "..#..##..#.."],   # an eagle displayed
 'bell':  [".....##.....", "....####....", "....#..#....", "...######...", "...######...", "..########..", "..########..",
           "..########..", ".##########.", "############", ".....##.....", ".....##....."],
}


def shield_img(charge, enemy, promoted=False):
    c = C(32, 40)
    outer = shield_mask(1, 1, 30, 38)
    inner = shield_mask(3, 3, 26, 34)
    fill(c, outer)
    fill(c, outer - inner, lambda x, y: True)
    ring = shield_mask(2, 2, 28, 36)
    fill(c, ring - inner, lambda x, y: False)
    if enemy:
        fill(c, inner, lambda x, y: (x - y) % 3 == 0)              # hatched field
    else:
        fill(c, inner, lambda x, y: not (x > 21 and (x - y) % 3 == 0))   # carved shadow side
    if promoted:                                                    # a second, dotted rim
        rim2 = shield_mask(5, 5, 22, 30)
        for (x, y) in edge(rim2):
            if (x + y) % 2 == 0: c.px(x, y, 0 if not enemy else 1)
    pat = CHARGES[charge]
    S = 2
    ox, oy = 16 - len(pat[0]) * S // 2, 7
    m = set()
    for j, row in enumerate(pat):
        for i, ch in enumerate(row):
            if ch == '#':
                ii = len(row) - 1 - i if enemy else i
                for a in range(S):
                    for b in range(S): m.add((ox + ii * S + a, oy + j * S + b))
    halo(c, m, 1)
    fill(c, m, lambda x, y: 0 if not enemy else 1)
    return c


# ======================================================== FX: portraits
def portrait(spec):
    """56x64 carved bust on a sunburst (same recipe as the dialogue mockup)."""
    c = C(56, 64)
    ox, oy, W, Hh = 0, 0, 56, 64
    box = (0, 0, 56, 64)
    cx, cy = 27, 27
    head = ellipse(cx, cy, 10.5, 13.5)
    neck = rectm(cx - 5, cy + 9, 10, 9)
    sh = spec.get('build', 0)
    body = poly([(ox + 1 - sh, oy + Hh), (ox + 5 - sh, oy + 50), (ox + 15, oy + 44), (ox + 39, oy + 44), (ox + 49 + sh, oy + 50), (ox + 54 + sh, oy + Hh)])
    body = {(x, y) for (x, y) in body if 0 <= x < 56 and y < 64}
    helm = set()
    if spec.get('helm'):
        helm = {(x, y) for (x, y) in ellipse(cx, cy - 3, 12.5, 14) if y <= cy - 3}
        helm |= {(x, y) for (x, y) in ellipse(cx, cy - 3, 12.5, 14) if abs(x - cx) >= 8 and y <= cy + 6}
    hair = set()
    if spec.get('hair') in ('short', 'braid'):
        cap = ellipse(cx, cy - 2, 12, 15)
        for (x, y) in cap:
            fringe = cy - 6 + (2 if (x // 3) % 2 else 0)
            if y < fringe or (y < cy + 2 and abs(x - cx) > 9): hair.add((x, y))
    sil = head | neck | body | helm | hair
    # bold alternating wedges, like the approved Brannock portrait
    for y in range(64):
        for x in range(56):
            a = (math.degrees(math.atan2(y - cy, x - cx)) + 360) % 360
            if int(a // 15) % 2 == 0 and math.hypot(x - cx, y - cy) > 15: c.px(x, y)
    halo(c, sil, 2)
    fill(c, body, lambda x, y: (x - y) % 3 != 0 if x > cx + 8 else True)
    for k in range(7):
        c.px(cx - 7 + k, oy + 45 + k, 0); c.px(cx + 7 - k, oy + 45 + k, 0)
    if spec.get('armor'):
        for y in range(oy + 50, 64, 3):
            for x in range(0, 56):
                if (x, y) in body and (x + y // 3) % 4 != 0: c.px(x, y, 0)
    fill(c, neck, lambda x, y: x < cx + 3)
    # face: white mass, carved shadow on the right cheek
    fill(c, head, lambda x, y: not (x > cx + 6 and (x - y) % 2 == 0))
    # brows, eyes, nose, mouth
    by_ = cy - 3 + spec.get('brow', 0)
    for x in range(cx - 7, cx - 2): c.px(x, by_ - (1 if spec.get('frown') and x > cx - 4 else 0), 0)
    for x in range(cx + 2, cx + 7): c.px(x, by_ - (1 if spec.get('frown') and x < cx + 4 else 0), 0)
    for e in (cx - 5, cx + 4):
        c.rect(e, by_ + 2, 2, 2, 0)
    for y in range(cy, cy + 5): c.px(cx, y, 0)
    c.px(cx - 1, cy + 5, 0); c.px(cx + 1, cy + 5, 0)
    for x in range(cx - 3, cx + 4): c.px(x, cy + 8, 0)
    if spec.get('frown'): c.px(cx - 4, cy + 9, 0); c.px(cx + 4, cy + 9, 0)
    if spec.get('beard'):
        for (x, y) in head:
            if y > cy + 5 and abs(x - cx) < 9 and (x % 2 == 0 or y > cy + 11): c.px(x, y, 1 if (x + y) % 3 else 0)
        for x in range(cx - 3, cx + 4): c.px(x, cy + 8, 0)
    if spec.get('scar'):
        for k in range(6): c.px(cx + 3 + k // 2, cy - 6 + k, 0)
    if hair:
        dark = spec.get('dark', False)
        fill(c, hair, (lambda x, y: (x + y) % 3 == 0) if dark else (lambda x, y: (x + y // 2) % 3 != 0))
        if spec.get('hair') == 'braid':
            for k in range(14):
                yy = cy + 4 + k; xx = cx + 10 + (1 if (k // 2) % 2 else 0)
                for d in range(3): c.px(xx + d, yy, 1 if (k + d) % 3 else 0)
    if helm:
        fill(c, helm, lambda x, y: (x - cx) % 4 != 0 or y > cy - 6)
        for x in range(cx - 12, cx + 13): c.px(x, cy - 6, 0); c.px(x, cy - 4, 0)
        for y in range(cy - 5, cy + 5): c.px(cx, y, 1)
        for y in range(cy - 5, cy + 3): c.px(cx - 1, y, 0); c.px(cx + 1, y, 0)
        if spec.get('crest'):
            for k in range(9):
                c.line(cx - 4 + k, oy + 4 + abs(k - 4) // 2, cx - 4 + k, oy + 13, 1)
                if k % 2: c.px(cx - 4 + k, oy + 9, 0)
    return c


def crop_mockup_portrait():
    """Brannock's approved portrait, straight from the dialogue mockup."""
    p = os.path.join(HERE, 'ref', 'dialogue.png')
    if not os.path.exists(p): return None
    im = Image.open(p).convert('L')
    c = C(56, 64)
    for y in range(64):
        for x in range(56):
            c.px(x, y, 1 if im.getpixel((x, y)) > 128 else 0)
    return c


# ======================================================== FX: chapter card
def chapter_card(num, title, goal):
    c = C()
    sunburst(c, 64, 50, 9, 80, step=8, a0=184, a1=360)
    def ridge(x): return 44 + 5 * math.sin(x / 11.0) + 2 * math.sin(x / 4.3)
    land = {(x, y) for x in range(128) for y in range(64) if y >= ridge(x)}
    halo(c, land, 1)
    def landrule(x, y):
        d = y - ridge(x)
        if 2 <= d < 3: return False
        if 5 <= d < 6 and (x // 2) % 3: return False
        if d > 8: return (x - y) % 3 != 0
        return True
    fill(c, land, landrule)
    keep = rectm(98, 30, 12, 16) | rectm(96, 26, 4, 6) | rectm(108, 26, 4, 6) | rectm(101, 24, 6, 7)
    halo(c, keep, 1); fill(c, keep, lambda x, y: (x - y) % 3 != 0 if x > 106 else True)
    for x in (96, 98, 101, 103, 105, 108, 110): c.px(x, 24 if 100 < x < 107 else 26, 0)
    c.rect(102, 38, 4, 8, 0); c.px(103, 37, 0); c.px(104, 37, 0)
    c.rect(103, 28, 2, 3, 0)
    river = {(x, y) for x in range(52, 78) for y in range(46, 64) if abs(x - 64 - (y - 46) * 0.25) < 9 + (y - 46) * 0.6}
    river = {p for p in river if p in land}
    fill(c, river, lambda x, y: 0)
    for y in range(48, 64, 3):
        for x in range(50, 82):
            if (x, y) in river and ((x + y * 2) // 3) % 3 == 0: c.px(x, y)
    for (sx, sy) in [(60, 50), (65, 54), (61, 58)]:
        fill(c, ellipse(sx, sy, 2.2, 1.2))
    tw = c.tw(title, 2)
    c.rect(64 - tw // 2 - 4, 14, tw + 8, 15, 0)
    c.text(64 - tw // 2, 17, title, 1, 2)
    for x in range(64 - tw // 2, 64 + tw // 2, 2): c.px(x, 28)
    plank(c, 40, 5, 48, 7, f"CHAPTER {num}")
    border(c)
    c.rect(28, 54, 72, 9, 0)
    plank(c, 30, 55, 68, 7, goal)
    return c


# ======================================================== FX: title card
def title_card():
    """UNDERTOW over a whirlpool, a small bell going down the middle of it."""
    c = C()
    cx, cy = 64, 33
    for k, r in enumerate(range(6, 80, 5)):                      # broken rings: the gaps turn, so it spirals
        ry = r * 0.36
        for a in range(0, 360, 2):
            if (a + k * 37) % 120 < 26: continue                     # the gap in this ring
            t = math.radians(a)
            x, y = round(cx + r * math.cos(t)), round(cy + ry * math.sin(t))
            if 0 <= x < 128 and 22 <= y < 64: c.px(x, y)
    bell = ["...##...", "..####..", "..#..#..", ".######.", ".######.", "########", "...##...", "........"]
    for j, row in enumerate(bell):                              # the lost bell, tipping into the eye
        for i, ch in enumerate(row):
            if ch == '#':
                x, y = cx - 4 + i + j // 3, cy - 7 + j
                c.px(x, y)
    tw = c.tw("UNDERTOW", 3)
    c.rect(64 - tw // 2 - 4, 2, tw + 8, 19, 0)
    c.text(64 - tw // 2, 4, "UNDERTOW", 1, 3)
    for x in range(64 - tw // 2, 64 + tw // 2, 2): c.px(x, 20)
    border(c)
    return c


# ======================================================== FX: pages (victory, defeat, record) and the status card
# Everything fixed on these screens is drawn here, so the sketch only writes the numbers and names.
# The sketch's positions (Undertow.ino: drawEnd, drawEpilogue, drawStatus, ST_VAL) match these.
def end_frame():
    """the chapter-end / record border"""
    c = C(); c.frame(0, 0, 128, 64); c.frame(2, 2, 124, 60)
    for i in range(6, 124, 6): c.px(i, 2, 0); c.px(i, 61, 0)
    return c


def heading(c, y, t):
    tw = c.tw(t)
    plank(c, 64 - tw // 2 - 6, y, tw + 12, 9, t)
    for x in range(8, 64 - tw // 2 - 9, 2): c.px(x, y + 4)              # a dotted rule either side
    for x in range(64 + tw // 2 + 9, 120, 2): c.px(x, y + 4)


def big_title(c, y, t):
    tw = c.tw(t, 3); c.text(64 - tw // 2, y, t, 1, 3)


def victory_page():
    """VICTORY; the sketch adds the line under it, who fell, and ONWARD / RESTART"""
    c = end_frame(); big_title(c, 8, "VICTORY")
    return c


def defeat_page():
    """the whole defeat screen: nothing on it changes"""
    c = end_frame(); big_title(c, 8, "DEFEAT")
    t = "BRANNOCK HAS FALLEN"; c.text(64 - c.tw(t) // 2, 32, t)
    plank(c, 30, 47, 68, 7, "A  RETRY")
    return c


def record_page(heroes, sides):
    """THE RECORD: allies /N, lost, secrets /N, turns; the rank letter and title come from the sketch"""
    c = end_frame(); heading(c, 4, "THE RECORD")
    for r, lab in enumerate(("ALLIES", "LOST", "SECRETS", "TURNS")):
        y = 17 + r * 8
        c.text(8, y, lab)
        for x in range(30 if r == 3 else 36, 37 if r == 3 else 42, 2): c.px(x, y + 4)   # dots up to the number (turns: 3 digits)
        if r in (0, 2): c.text(52, y, f"/{heroes if r == 0 else sides}")
    c.frame(80, 15, 33, 31)                                                 # the rank, in a carved box
    for i in range(82, 112, 3): c.px(i, 15, 0)
    plank(c, 85, 13, 23, 7, "RANK")
    return c


def status_card():
    """72x64, drawn at x=56 beside the portrait: name plank, labels, the items rule"""
    c = C(72, 64)
    plank(c, 1, 0, 71, 8)
    c.text(52, 10, "LV")
    c.text(3, 17, "HP"); c.text(22, 17, "/"); c.text(42, 17, "MV")
    for i, lab in enumerate(("STR", "SKL", "SPD", "DEF")):
        c.text(3 + (i & 1) * 39, 24 + (i >> 1) * 7, lab)
    for x in range(2, 72, 2): c.px(x, 37)
    return c


# ======================================================== write it all
def save_strip(frames, w, h, name, transparent):
    im = Image.new('RGBA' if transparent else 'RGB', (w * len(frames), h))
    for i, c in enumerate(frames): im.paste(c.image(transparent), (i * w, 0))
    fn = f"{name}_{w}x{h}.png" if len(frames) > 1 or name == 'chapter' else f"{name}.png"
    im.save(os.path.join(GFX, fn))
    return fn


def main():
    write_font_bin()
    tiles, units = map_art()
    figs = figures.all_frames()
    figs += demons.all_frames()[:2]                     # figure 9: the Drowned
    figs += figures.later_frames()                      # 10-11 Oren, 12-13 Cass, 14 Dain rider, 15 Gerault, 16 Corvane
    figs += figures.act3_frames()                       # 17 Aelis, 18 cultist, 19 Mother Silt, 20 King Aldric
    figs += demons.all_frames()[2:4]                    # 21: a bellwraith
    figs += demons.all_frames()[4:6]                    # 22: the horned (a sea horror)
    figs += figures.ship_frames()                       # 23-24 Rook (+ mirrored), 25 Black Marl
    figs += figures.island_frames()                     # 26-27 Saoirse (+ mirrored), 28 Thane, 29-30 Hesk (+ mirrored), 31 Lir
    figs += figures.finale_frames()                     # 32 Oda, 33 Nhal
    files = {}
    files['tiles'] = save_strip(tiles, 8, 8, 'tiles', False)
    files['units'] = save_strip(units, 8, 8, 'units', False)
    files['figures'] = save_strip([c for _, c in figs], 64, 48, 'figures', True)
    files['backdrop'] = save_strip(backdrops(), 128, 56, 'backdrop', False)
    shields = []
    CHARGE_OF = {'horse': 'horse', 'lance': 'lance', 'axe': 'axe', 'bow': 'bow', 'boss': 'boss', 'staff': 'bell', 'drowned': 'drowned',
                 'cavalier': 'horse', 'sword': 'sword', 'tome': 'flame', 'bellwraith': 'bell', 'eagle': 'eagle', 'monk': 'bell'}
    for spr, promo in CLASS_SPR:
        shields += [shield_img(CHARGE_OF[spr], False, promo), shield_img(CHARGE_OF[spr], True, promo)]
    files['shields'] = save_strip(shields, 32, 40, 'shields', False)
    # heroes in the clean-line style, villains in light & shadow (see tools/woodcut/portraits.py)
    import portraits as P
    order = [('brannock', 'clean'), ('osk', 'notan'), ('idra', 'clean'), ('hale', 'clean'), ('wren', 'clean'),
             ('maren', 'clean'), ('tam', 'clean'), ('goody', 'clean'), ('varga', 'notan'),
             ('kell', 'notan'), ('voss', 'notan'), ('fenn', 'clean'),
             ('oren', 'clean'), ('cass', 'clean'), ('corvane', 'notan'), ('gerault', 'notan'),
             ('aelis', 'clean'), ('silt', 'notan'), ('aldric', 'notan'), ('hollow', 'notan'), ('prisoner', 'clean'),
             ('rook', 'clean'), ('marl', 'notan'), ('dockmaster', 'notan'), ('islander', 'clean'),
             ('saoirse', 'clean'), ('thane', 'clean'), ('hesk', 'clean'), ('lir', 'clean'), ('brigid', 'clean'),
             ('vey', 'notan'), ('gatekeeper', 'notan'), ('oda', 'clean'), ('nhal', 'notan')]   # 32 Oda, 33 Nhal
    files['portraits'] = save_strip([P.STYLES[st](P.CAST[n]) for n, st in order], 56, 64, 'portraits', False)
    # one chapter card per chapter, titles and goals from chapters/*.txt (via make_chapters.py)
    import json
    meta = json.load(open(os.path.join(ROOT, 'fxdata', 'chapters.json')))
    cards = [chapter_card(m['label'], m['title'], m['goal']) for m in meta]
    files['chapter'] = save_strip(cards, 128, 64, 'chapter', False)
    files['title'] = save_strip([title_card()], 128, 64, 'title', False)
    sides = sum(1 for m in meta if m.get('requires'))
    files['pages'] = save_strip([victory_page(), defeat_page(), record_page(GD.NUM_HEROES, sides)], 128, 64, 'pages', False)
    files['card'] = save_strip([status_card()], 72, 64, 'card', False)
    with open(os.path.join(ROOT, 'fxdata', 'fxdata.txt'), 'w') as f:
        f.write("// Generated by tools/woodcut/make_gfx.py\n")
        f.write("// Build:  python3 fxdata-build.py fxdata/fxdata.txt   (Arduboy-Python-Utilities)\n\n")
        f.write("namespace FX_GFX\n")
        for label in ('tiles', 'units', 'figures', 'backdrop', 'shields', 'portraits', 'chapter', 'title', 'pages', 'card'):
            f.write(f'image_t {label} = "gfx/{files[label]}"\n')
        f.write('raw_t font = "font.bin"\n')
        f.write('raw_t strings = "strings.bin"\n')
        f.write('raw_t tables = "tables.bin"\n')
        f.write('raw_t chapters = "chapters.bin"\n')
        f.write("namespace_end\n")
        # the save: a 4 KB block of FX flash of this game's own (see mirrorSave in the sketch)
        f.write("\nsavesection\nuint8_t 0xFF\n")
    print("figures:", [k for k, _ in figs])


if __name__ == '__main__':
    main()
